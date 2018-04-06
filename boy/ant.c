// ant.c - for working with antenna module
#include <utl.h>
#include <ant.h>
#include <mpc.h>
#include <tmr.h>

#define EOL "\n"

AntInfo ant;

///
// turn on antenna module, wait until ant responds
// sets: ant.mode .port
void antInit(void) {
  short rx, tx;
  DBG0("antInit()")
  // port
  rx = TPUChanFromPin(ANT_RX);
  tx = TPUChanFromPin(ANT_TX);
  ant.port = TUOpen(rx, tx, ANT_BAUD, 0);
  if (ant.port==NULL)
    utlStop("antInit() com1 open fail");
} // antInit

///
// turn on, clean, set params
void antStart(void) {
  DBG0("antStart() %s", utlDateTime())
  PIOSet(ANT_PWR);
  utlDelay(RS232_SETTLE); // to settle rs232
  TURxFlush(ant.port);
  TUTxFlush(ant.port);
  // state
  ant.pending = false;
  antAuton(false);
  // get cf2 startup message
  utlNap(3);
  if ( utlReadWait(ant.port, utlBuf, 9)==0 )
    utlStop("FATAL\t| antInit() startup fail");
  DBG1("-> %s", utlBuf)
  // sbe39
  if (!(antPrompt())   
    utlStop("ERR\t| antInit(): no prompt from ant");
  // utlWrite(ant.port, "OutputFormat=1", EOL);
  // stop in case hw is autonomous
  utlWrite(ant.port, "stop", EOL);
  utlWrite(ant.port, "SampleInterval=0.5", EOL);
  utlWrite(ant.port, "DelayBeforeSampling=0", EOL);
  sprintf(utlStr, "datetime=%s", utlDateTimeBrief());
  utlWrite(ant.port, utlStr, EOL);
  utlRead(ant.port, utlBuf);
} // antStart

///
// turn off power to antmod 
void antStop() {
  PIOClear(ANT_PWR);
} // antStop

///
// if asleep, first EOL wakens but no response
bool antPrompt() {
  DBG2("antPrompt()")
  TURxFlush(ant.port);
  utlWrite(ant.port, "", EOL);
  utlReadWait(ant.port, utlBuf, 2*ant.delay);
  if (strstr(utlBuf, "Exec"))
    return true;
  // try again after break
  antBreak();
  utlWrite(ant.port, "", EOL);
  utlReadWait(ant.port, utlBuf, 2*ant.delay);
  if (strstr(utlBuf, "Exec"))
    return true;
  return false;
} // antPrompt

///
// reset or exit sync mode
void antBreak(void) {
  DBG0("antBreak()")
  TUTxBreak(ant.port, 5000);
} // antBreak

///
// data waiting
bool antData() {
  return TURxQueuedCount(ant.port);
} // antData

///
// request sample
void antSample(void) {
  int len;
  DBG0("antSample()")
  if (ant.auton || ant.pending) return;
  antPrompt();
  utlWrite(ant.port, "TS", EOL);
  len = utlReadWait(ant.port, utlBuf, 1);
  if (len<3)
    utlShut("\nERR antSample, TS command fail");
  ant.pending = true;
  // pending response, checked by antRead()
  tmrStart( ant_tmr, ant.delay );
} // antSample

// get depth from antmod
// if (!data !pending) if (auto) return else sample
//  while (!data) check timeout
//  read
//
// sets: ant.depth .temp .samples[]
// returns: depth
void antRead(void) {
  int i;
  char *p0, *p1;
  if (!ant.pending && !antData()) 
    // no data here or expected from sbe39
    if (ant.auton) 
      // most common case, use the last ant.depth or ant.temp 
      return;
    else 
      // poke sbe, set pending 
      antSample();
  // pending, so wait for data
  while (!antData()) {
    // error if timeout
    if (tmrExp(ctd_tmr)) {
      flogf("\nERR\t| antRead(): tmr expired");
      return; // ?? reset lara
    }
  } // while !data
  // data ready
  utlRead(ant.port, utlBuf);
  // ?? sanity check
  // could be multiple lines, ending crlf, len 32 < char < 64
  p0 = utlBuf;
  while ((p1 = strstr(p0, "\r\n"))) {
    // found line, zero terminate and copy
    *p1 = 0;
    strcpy(utlStr, p0);
    if (ant.auton) {
      // shift samples in array
      for (i=0; i<ant.sampleCnt; i++) 
        ant.samples[i+1] = ant.samples[i];
      ant.samples[0] = ant.depth;
    } // if auton
    // ?? range check
    ant.temp = atof(strtok(utlStr, "#, "));
    ant.depth = atof(strtok(NULL, ", "));
    // continue scan for lines after crlf
    p0 = p1 + 2;
  } // while crlf
}

float antDepth(void) {
  antSample();
  antRead();
  return ant.depth;
} // antDepth

float antTemp(void) {
  antSample();
  antRead();
  return ant.temp;
} // antTemp

///
// turn autonomous on/off. idle_ant clears samples
void antAuton(bool auton) {
  int i;
  if (ant.auton==auton) return;
  DBG0("antAuto(%d)", auton)
  if (auton) {
    utlWrite(ant.port, "initlogging", EOL);
    utlWrite(ant.port, "initlogging", EOL);
    utlWrite(ant.port, "sampleInterval=0", EOL);
    utlWrite(ant.port, "txRealTime=y", EOL);
    utlWrite(ant.port, "startnow", EOL);
    // clear samples
    for (i=0; i<=ant.sampleCnt; i++) 
      ant.samples[i] = 0;
    // ant.pending tells antRead() to wait for first sample
    ant.pending = true;
    // swallow header
    utlReadWait(ant.port, utlBuf, 5);
    if (!strstr(utlBuf, "Start logging"))
      flogf("\nERR\t| antAuto - didn't get 'Start logging' header");
  } else {
    utlWrite(ant.port, "stop", EOL);
    // swallow tail
    utlNap(2);
    TURxFlush(ant.port);
  } // if auton
  ant.auton = auton;
}

///
// if auton_ant, checks recent depth against previous
// returns delta change in depth (0 if not enough samples)
// uses: ant.sampleCnt .sampleRes .depth
// rets: - | + | 0.0
float antMoving(void) {
  float d;
  // got samples?
  d = ant.samples[ant.sampleCnt];
  if (d==0.0) 
    return 0.0;
  // got delta?
  d = ant.depth-d;
  if (abs(d)<ant.sampleRes) 
    return 0.0;
  // got depth?
  if (ant.depth>(ant.surfD-ant.sampleRes))
    return 0.0;
  // delta
  return d;
} // antMoving

///
// antmod uMPC cf2 and iridium A3LA
// switch between devices on com1, clear pipe
void antDevice(DevType dev) {
  if (dev==ant.dev) return;
  if (dev==cf2_dev)
    PIOSet(ANT_SEL);
  else if (dev==a3la_dev)
    PIOClear(ANT_SEL);
  else
    return;
  utlDelay(RS232_SETTLE);
  TUTxFlush(ant.port);
  TURxFlush(ant.port);
  utlPet();
  return;
} // antDevice

void antSwitch(AntType antenna) {
  if (antenna==ant.antenna) return;
  TUTxPutByte(ant.port, 1, false);        // ^A
  if (antenna==gps_ant) 
    TUTxPutByte(ant.port, 'G', false);
  else
    TUTxPutByte(ant.port, 'I', false);
  ant.antenna = antenna;
} // antSwitch
    
