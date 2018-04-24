// ant.c - for working with antenna module
#include <utl.h>
#include <ant.h>
#include <mpc.h>
#include <tmr.h>

#define EOL "\r"
#define SBE_SLEEP 20

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
    strcpy(ant.samCmd, "TSSon");
  else
    strcpy(ant.samCmd, "TS");
  antDevice(null_dev);
} // antInit

///
// turn on, clean, set params, talk to sbe39
void antStart(void) {
  DBG0("antStart() %s", utlDateTime())
  antDevice(cf2_dev);
  PIOClear(ANT_PWR);
  utlDelay(200);
  TURxFlush(ant.port);
  TUTxFlush(ant.port);
  PIOSet(ANT_PWR);
  utlNap(4);                        // uMPC has countdown exit = 3
  // get cf2 startup message
  if (utlReadWait(ant.port, utlBuf, 1))
    flogf(" %s", utlBuf);
  // if (!strstr(utlBuf, "Program:"))
    // flogf("\nErr\t| expected ant startup message, got '%s'", utlBuf);
  // DBG2("-> %s", utlBuf)
  // state
  ant.auton = false;
  tmrStop(ant_tmr);
  antFlush();                      // flush sample buffer
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
bool antPrompt() {
  DBG1("antPrompt()")
  TURxFlush(ant.port);
  // if asleep, first EOL wakens but no response
  utlWrite(ant.port, "", EOL);
  utlReadWait(ant.port, utlBuf, 1);
  if (strstr(utlBuf, "Exec"))
    return true;
  utlWrite(ant.port, "", EOL);
  utlReadWait(ant.port, utlBuf, ant.delay);
  if (strstr(utlBuf, "Exec"))
    return true;
  // try a third time after break
  antBreak();
  TURxFlush(ant.port);
  utlNap(2);
  utlWrite(ant.port, "", EOL);
  utlReadWait(ant.port, utlBuf, 1);
  if (strstr(utlBuf, "Exec"))
    return true;
  utlErr(ant_err, "antPrompt() fail");
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
int antData() {
  int r=TURxQueuedCount(ant.port);
  DBG1("aDa=%d", r)
  // ?? ?? fails without this, why?
  utlDelay(100);
  return r;
} // antData

///
// if !tmrOn request sample
// sets: ant_tmr
void antSample(void) {
  DBG1("aSam")
  if (antPending()) return;
  TURxFlush(ant.port);
  // sleeping?
  if (ant.lastT+SBE_SLEEP<time(0))
    antPrompt();
  utlWrite(ant.port, ant.samCmd, EOL);
  // get echo of command
  utlReadWait(ant.port, utlBuf, 1);
  if (!strstr(utlBuf, ant.samCmd))
    utlErr(ant_err, "antSample: TS command fail");
  else
    tmrStart( ant_tmr, ant.delay );
} // antSample

///
// antRead processes one or more lines of data, stores samples if auton
// if ant.autoSample then sample, else not pending after read
// sets: ant.depth .temp 
bool antRead(void) {
  char *p0, *p1, *p2;
  if (!antData()) return false;
  DBG1("antRead()");
  utlRead(ant.port, utlBuf);
  // ?? sanity check
  // should be multiple lines, ending crlf; ignore <Executed/>
  // data line len 32 < char < 64
  // read temp, depth and scan ahead for line end
  p0 = utlBuf;
  while (p0) {              // p0 will be null when no more lines
    p1 = strtok(p0, "\r\n#, ");
    if (!p1) continue;
    p2 = strtok(NULL, ", ");
    if (!p2) continue;
    antMovSam();
    ant.temp = atof(p1);
    ant.depth = atof(p2);
    DBG1("= %4.2f, %4.2f", ant.temp, ant.depth)
    // ?? range check
    // loop until no more line ends in buf
    p0 = strstr(p0, "\r");
  } // while 
  tmrStop(ant_tmr);
  ant.lastT = time(0);
  if (ant.autoSample)
    antSample();
  return true;
} // antRead

///
// wait for data or not pending (timeout)
bool antDataWait(void) {
  DBG1("aDW")
  // err if timeout ?? count?
  while (antPending())
    if (antData()) 
      return true;
    utlX;
  // error, prob timeout
  return false;
} // antDataWait

///
// data read recently
bool antFresh(void) {
  bool fresh = (time(0)-ant.lastT)<ant.fresh;
  DBG1("aFr=%d", fresh)
  return fresh;
}

///
// tmrOn ? pending. tmrExp ? err
bool antPending(void) {
  bool r=false;
  if (ant.auton)
    r = true;
  else if (tmrOff(ant_tmr))
    r = false;
  else if (tmrOn(ant_tmr)) 
    r = true;
  else if (tmrExp(ant_tmr)) {
    utlErr(ant_err, "ant: timer expired");
    r = false;
  }
  DBG1("aPe=%d", r)
  return r;
}
    
///
// if !data&&fresh, return. if !pending, sample. wait for data. read.
// if data, read. if !pending, sample. if !fresh, wait.
float antDepth(void) {
  DBG1("aDep")
  if (antData())
    antRead();
  if (!antPending())
    antSample();
  if (!antFresh()) {
    antDataWait();
    antRead();
  }
  return ant.depth;
} // antDepth

float antTemp(void) {
  antDepth();
  return ant.temp;
} // antTemp

///
// checks recent depth against previous
// returns delta change in depth 
// fills sample buffer if not enough samples, takes several seconds
// uses: ant.samLen .samRes .depth
// rets: -rise | +drop | 0.0stop
float antMoving(void) {
  float d;
  DBG1("antMoving()")
  // got samples? fill FIFO buffer  ?? needs timeout
  while (ant.samCnt<ant.samLen) {
    antSample();
    antDataWait();
    antRead();
  }
  // got delta?
  d = ant.depth-ant.samQue[ant.samCnt];
  DBG4("aM=%4.2f", d)
  if (abs(d)<ant.samRes) 
    return 0.0;
  // got depth? wave motion
  if (ant.depth<antSurfMaxD())
    return 0.0;
  // delta
  return d;
} // antMoving

///
// shift sample in samQue, used by antMoving
// sets: ant.samQue[] .samCnt
void antMovSam(void) {
  int i;
  // samples[] for antMoving - shift samples in array
  for (i=0; i<ant.samCnt; i++)
    ant.samQue[i+1] = ant.samQue[i];
  if (ant.samCnt<ant.samLen)
    ant.samCnt++;
  ant.samQue[0] = ant.depth;
} // antMovSam

///
// clear samples used by antMoving, call this when winch stops
void antFlush(void) {
  ant.samCnt = 0;
}

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

///
// tell antmod to power dev on/off
void antDevPwr(char c, bool on) {
  DevType currDev=ant.dev;
  DBG0("antDevPwr(%c, %d)", c, on)
  antDevice(cf2_dev);
  if (on)
    TUTxPutByte(ant.port, 3, false);
  else
    TUTxPutByte(ant.port, 4, false);
  TUTxPutByte(ant.port, c, false);
  antDevice(currDev);
} // antDevPwr

Serial antPort(void) {
  return ant.port;
} // antPort

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
    
///
void antAutoSample(bool autos) {
  DBG1("antAutoSample")
  ant.autoSample = autos;
} // antAutoSample

///
// turn autonomous on/off. idle_ant clears samples
void antAuton(bool auton) {
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
  } else {
    utlWrite(ant.port, "stop", EOL);
    // swallow tail
    utlNap(2);
    TURxFlush(ant.port);
  } // if auton
  tmrStop(ant_tmr);
  ant.auton = auton;
}

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

float antSurfMaxD(void) {
  return ant.surfD+ant.samRes;
} // antSurfMaxD
