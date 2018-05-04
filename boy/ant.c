// ant.c - for working with antenna module
#include <utl.h>
#include <ant.h>
#include <tmr.h>

#define EOL "\r"
#define SBE_SLEEP 20

AntInfo ant;

///
// turn on antenna module, wait until ant responds
// sets: ant.mode .port
void antInit(void) {
  short rx, tx, i;
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
  // alloc bloc to store depth values for moving/velo
  ant.ring = (AntNode *) calloc(ant.ringSize, sizeof(AntNode));
  // to string the nodes into a ring, access like an array
  ant.ring[ant.ringSize-1].next = ant.ring;
  for (i=0; i<ant.ringSize-1; i++) 
    ant.ring[i].next = &ant.ring[i+1];
  ant.ringFresh = ant.fresh * ant.ringSize;
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
  antRingClear();                      // flush sample buffer
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
  if (ant.sampT+SBE_SLEEP<time(0))
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
// antRead processes most recent sample, could be multiple
// after: if ant.autoSample then antSample(), else not pending after read
// sets: ant.temp .depth .sampT .ring->depth .ring->sampT
bool antRead(void) {
  char *p0, *p1, *p2;
  if (!antData()) return false;
  DBG1("antRead()");
  utlRead(ant.port, utlBuf);
  // ?? sanity check
  // should be multiple lines, ending crlf
  // data line len 32 < char < 64
  // read temp, depth and scan ahead for line end
  p0 = utlBuf;
  // trim expected trailing EOL<Executed/>EOL
  p1 = strrchr(p0, '\r');
  if (p1)
    *p1 = 0;
  p1 = strrchr(p0, '\r');
  if (!strstr(p1, "<Exec"))
    return false;
  if (p1)
    *p1 = 0;
  // skip any leading lines
  p1 = strrchr(p0, '\r');
  if (p1)
    p0 = p1;
  // parse two numeric csv
  p1 = strtok(p0, "\r\n#, ");
  if (!p1) 
    return false;
  p2 = strtok(NULL, ", ");
  if (!p2) 
    return false;
  // save samples
  ant.ring->depth = ant.depth;
  ant.ring->sampT = ant.sampT;
  // new values
  ant.temp = atof(p1);
  ant.depth = atof(p2);
  ant.sampT = time(0);
  //
  DBG1("= %4.2f, %4.2f", ant.temp, ant.depth)
  tmrStop(ant_tmr);
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
  bool fresh = (time(0)-ant.sampT)<ant.fresh;
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
// checks recent depth against previous, computes velocity m/s
// false if oldest recent depth is very old
// sets *velo 0.0 if depths go up and down
// uses: ant.ring ringFresh
// sets: *velo ant.ring->depth ->time returns bool
bool antVelo(float *velo) {
  short dir;
  float v;
  AntNode *n;
  DBG1("antVelo()")
  // got samples? 
  if (ant.sampT - ant.ring->sampT > ant.ringFresh) return false;
  // candidate velo
  v = (ant.depth - ant.ring->depth) / (ant.sampT - ant.ring->sampT);
  // direction - check all samples for consistent direction
  dir = (v>0) ? 1 : -1;
  n = ant.ring; 
  while (true) {
    if (dir==1) // fall
      if (n->depth < n->next->depth)
        dir = 0;
    if (dir==-1) // rise
      if (n->depth > n->next->depth)
        dir = 0;
    if (dir==0) break;
    n=n->next;
    if (n==ant.ring) break;
  }
  if (dir) *velo = v;
  else *velo = 0.0;
  DBG2("aM=%4.2f", *velo)
  return true;
} // antVelo

///
// clear sample times used by antVelo, call this when winch stops
// could be replaced with free(), calloc()
// sets: ant.ring->sampT;
void antRingClear(void) {
  AntNode *n;
  n = ant.ring; 
  while (true) {
    n->sampT = 0;
    n = n->next;
    if (n==ant.ring) break;
  } // while
} // antRingClear

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
  utlDelay(SETTLE);
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

bool antSurf(void) {
  return (ant.depth<(ant.surfD+2));
}
