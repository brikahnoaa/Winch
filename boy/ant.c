// ant.c - for working with antenna module
#include <utl.h>
#include <ant.h>
#include <mpc.h>
#include <tmr.h>

#define EOL "\r"

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
  if (ant.logging)
    strcpy(ant.sample, "TSSon");
  else
    strcpy(ant.sample, "TS");
} // antInit

///
// turn on, clean, set params, talk to sbe39
void antStart(void) {
  DBG0("antStart() %s", utlDateTime())
  antDevice(cf2_dev);
  PIOClear(ANT_PWR);
  utlDelay(200);
  PIOSet(ANT_PWR);
  PIOSet(ANT_SEL);
  utlNap(4);                        // uMPC has countdown exit = 3
  // state
  ant.auton = false;
  tmrStop(ant_tmr);
  // get cf2 startup message
  utlReadWait(ant.port, utlBuf, 4);
  if (!strstr(utlBuf, "Program:"))
    flogf("\nErr\t| expected ant startup message, got '%s'", utlBuf);
  DBG2("-> %s", utlBuf)
  antPrompt();
  // utlWrite(ant.port, "OutputFormat=1", EOL);
  sprintf(utlStr, "datetime=%s", utlDateTimeBrief());
  utlWrite(ant.port, utlStr, EOL);
  utlReadWait(ant.port, utlBuf, 1);
} // antStart

///
// turn off power to antmod 
void antStop() {
  if (ant.log)
    close(ant.log);
  ant.log = 0;
  PIOClear(ANT_PWR);
} // antStop

///
// if asleep, first EOL wakens but no response
bool antPrompt() {
  DBG1("antPrompt()")
  TURxFlush(ant.port);
  utlWrite(ant.port, "", EOL);
  utlReadWait(ant.port, utlBuf, 2+ant.delay);
  if (strstr(utlBuf, "Exec"))
    return true;
  // try again after break
  antBreak();
  TURxFlush(ant.port);
  utlWrite(ant.port, "", EOL);
  utlNap(2+ant.delay);
  utlReadWait(ant.port, utlBuf, 1);
  if (strstr(utlBuf, "Exec"))
    return true;
  utlErr(ant_err, "antPrompt: fail");
  return false;
} // antPrompt

///
// reset or exit sync mode
void antBreak(void) {
  DBG0("antBreak()")
  TUTxPutByte(ant.port, (ushort) 2,1);      // ^B  (blocking)
} // antBreak

///
// data waiting
bool antData() {
  DBG1("aD")
  return TURxQueuedCount(ant.port);
} // antData

///
// if !tmrOn request sample
// sets: ant_tmr
void antSample(void) {
  int len;
  DBG1("antSample()")
  if (antPending()) return;
  antPrompt();
  utlWrite(ant.port, ant.sample, EOL);
  len = utlReadWait(ant.port, utlBuf, 1);
  // get echo 
  if (!strstr(utlBuf, ant.sample))
    utlErr(ant_err, "antSample: TS command fail");
  else
    tmrStart( ant_tmr, ant.delay );
} // antSample

///
// antRead processes one or more lines of data, stores samples if auton
// sets: ant.depth .temp .samples[]
bool antRead(void) {
  int i;
  char *p0, *p1, *p2;
  if (!antData()) return false;
  DBG1("antRead()");
  utlRead(ant.port, utlBuf);
  // ?? sanity check
  // could be multiple lines, ending crlf, len 32 < char < 64
  // read temp, depth and scan ahead for line end
  p0 = utlBuf;
  while (p0) {              // p0 will be null when no more lines
    p1 = strtok(p0, "\r\n#, ");
    if (!p1) continue;
    p2 = strtok(NULL, ", ");
    if (!p2) continue;
    // samples[] for antMoving - shift samples in array
    for (i=0; i<ant.sampleCnt; i++) 
      ant.samples[i+1] = ant.samples[i];
    ant.samples[0] = ant.depth;
    ant.temp = atof(p1);
    ant.depth = atof(p2);
    DBG1("= %4.2f, %4.2f", ant.temp, ant.depth)
    // ?? range check
    // loop until no more line ends in buf
    p0 = strstr(p0, "\r");
  } // while 
  tmrStop(ant_tmr);
  ant.time = time(0);
  return true;
}

///
// data read recently
bool antFresh(void) {
  bool fresh = (time(0)-ant.time)<ant.fresh;
  DBG1("antFresh()->%d", fresh)
  return fresh;
}

///
// tmrOn ? pending. tmrExp ? err
bool antPending(void) {
  if (ant.auton)
    return true;
  else if (tmrOn(ant_tmr)) 
    return true;
  else if (tmrExp(ant_tmr)) 
    utlErr(ant_err, "ant: timer expired");
  return false;
}
    
///
// if !data&&fresh, return. if !pending, sample. wait for data. read.
float antDepth(void) {
  DBG1("antDepth()")
  if (!antData() && antFresh()) 
    return ant.depth;
  if (!antPending())
    antSample();
  // err if timeout ?? count?
  while (antPending())
    if (antData()) 
      if (antRead())
        return ant.depth;
  // timeout
  return 0.0;
} // antDepth

float antTemp(void) {
  antDepth();
  return ant.temp;
} // antTemp

///
// turn autonomous on/off. idle_ant clears samples
void antAuton(bool auton) {
  int i;
  DBG0("antAuto(%d)", auton)
  if (auton) {
    utlWrite(ant.port, "SampleInterval=0.5", EOL);
    utlReadWait(ant.port, utlBuf, 1);
    utlWrite(ant.port, "txRealTime=y", EOL);
    utlReadWait(ant.port, utlBuf, 1);
    utlWrite(ant.port, "initlogging", EOL);
    utlReadWait(ant.port, utlBuf, 1);
    utlWrite(ant.port, "initlogging", EOL);
    utlReadWait(ant.port, utlBuf, 1);
    utlWrite(ant.port, "startnow", EOL);
    // swallow header
    utlReadWait(ant.port, utlBuf, 5);
    if (!strstr(utlBuf, "Start logging"))
      utlErr(ant_err, "antAuto: expected 'Start logging' header");
    // clear samples used for antMoving()
    for (i=0; i<=ant.sampleCnt; i++) 
      ant.samples[i] = 0;
  } else {
    utlWrite(ant.port, "stop", EOL);
    // swallow tail
    utlNap(2);
    TURxFlush(ant.port);
  } // if auton
  tmrStop(ant_tmr);
  ant.auton = auton;
}

///
// checks recent depth against previous
// returns delta change in depth (0 if not enough samples)
// uses: ant.sampleCnt .sampleRes .depth
// rets: - | + | 0.0
float antMoving(void) {
  float d;
  DBG1("antMoving()")
  // got samples?
  d = ant.samples[ant.sampleCnt];
  if (d==0.0) 
    return 0.0;
  // got delta?
  d = ant.depth-d;
  if (abs(d)<ant.sampleRes) 
    return 0.0;
  // got depth? wave motion
  if (ant.depth<(ant.surfD+ant.sampleRes))
    return 0.0;
  // delta
  return d;
} // antMoving

///
// antmod uMPC cf2 and iridium A3LA
// switch between devices on com1, clear pipe
void antDevice(DevType dev) {
  if (dev==ant.dev) return;
  DBG0("antDevice(%s)",(dev==cf2_dev)?"cf2":"a3la")
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
  DBG0("antSwitch(%s)", (antenna==gps_ant)?"gps":"irid")
  TUTxPutByte(ant.port, 1, false);        // ^A
  if (antenna==gps_ant) 
    TUTxPutByte(ant.port, 'G', false);
  else
    TUTxPutByte(ant.port, 'I', false);
  ant.antenna = antenna;
} // antSwitch
    
void antGetSamples(void) {
  int len1=sizeof(utlBuf);
  int len2=len1, len3=len1;
  int total=0;
  DBG0("antGetSamples()")
  ant.log = utlLogFile(ant.logFile);
  antPrompt();          // wakeup
  utlWrite(ant.port, "GetSamples:", EOL);
  while (len1==len3) {
    // repeat until less than a full buf
    len2 = (int) TURxGetBlock(ant.port, utlBuf, (long) len1, (short) 1000);
    len3 = write(ant.log, utlBuf, len2);
    if (len2!=len3)
      flogf("\nERR\t| antGetSamples() could not write ant.log");
    total += len3;
  } // while ==
  close(ant.log);
  utlWrite(ant.port, "initLogging", EOL);
  utlWrite(ant.port, "initLogging", EOL);
  utlReadWait(ant.port, utlBuf, 1);
  flogf("\nantGetSamples(): %d bytes to %s", total, ant.logFile);
} // antGetSamples
