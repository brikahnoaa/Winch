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
  ant.on = false;
  // alloc bloc to store depth values for moving/velo
  ant.ring = (RingNode *) calloc(ant.ringSize, sizeof(RingNode));
  // to string the nodes into a ring, access like an array
  for (i=0; i<ant.ringSize-1; i++) {
    ant.ring[i].next = &ant.ring[i+1];
  }
  ant.ring[i].next = ant.ring;
  ant.ringFresh = ant.fresh * ant.ringSize;
} // antInit

///
// turn on, clean, set params, talk to sbe39
void antStart(void) {
  DBG0("antStart() %s", utlDateTime())
  if (ant.on) return;
  ant.on = true;
  antDevice(cf2_dev);
  PIOClear(ANT_PWR);
  utlDelay(200);
  TURxFlush(ant.port);
  TUTxFlush(ant.port);
  PIOSet(ANT_PWR);
  // get cf2 startup message
  utlExpect(ant.port, utlBuf, "ok", 6);
  DBG1("%s", utlBuf)
  // state
  ant.auton = false;
  tmrStop(ant_tmr);
  antRingReset();                   // flush sample buffer
  antPrompt();
  sprintf(utlStr, "datetime=%s", utlDateTimeBrief());
  utlWrite(ant.port, utlStr, EOL);
  utlExpect(ant.port, utlBuf, "Executed", 1);
} // antStart

///
// turn off power to antmod 
void antStop() {
  if (!ant.on) return;
  ant.on = false;
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
  DBG2("aDa=%d", r)
  return r;
} // antData

///
// wait for data or not pending (timeout)
bool antDataWait(void) {
  DBG2("aDW")
  while (antPending())
    if (antData()) 
      return true;
  return false;
} // antDataWait

///
// if !tmrOn request sample
// sets: ant_tmr
void antSample(void) {
  if (antPending()) return;
  DBG2("aSam")
  // flush old data, check for sleep message and prompt if needed
  antRead();
  utlWrite(ant.port, ant.samCmd, EOL);
  // get echo of command
  utlReadWait(ant.port, utlBuf, 1);
  if (!strstr(utlBuf, ant.samCmd))
    utlErr(ant_err, "antSample: TS command fail");
  else
    tmrStart( ant_tmr, ant.delay );
  ant.sampT = time(0);
} // antSample

///
// antRead processes most recent sample, could be multiple lines
// TS   ->' 20.1000,    1.287, 18 Sep 1914, 12:40:30\r\n<Executed/>\r\n'
// TSSon->' 20.1000,    1.287, 18 Sep 1914, 12:40:30, 126\r\n<Executed/>\r\n'
// after: if ant.autoSample then antSample(), else not pending after read
// sets: ant.temp .depth .ring->depth .ring->sampT
// note: ant.sampT set in antSample()
bool antRead(void) {
  char *p0, *p1, *p2;
  if (!antData()) return false;
  DBG2("antRead()");
  if (!(p1 = utlExpect(ant.port, utlBuf, "<Executed/>", 1)))
    return false;
  p0 = utlBuf;
  // terminate 
  *p1 = 0;
  // sleeping?
  if (strstr(utlBuf, "sleep")) {
    antPrompt();      // wakeup
    return false;
  }
  while (strlen(p0)>64)
    p0 = strchr(p0, '\r')+1;
  if (strlen(p0)<45) return false;
  // read temp, depth // parse two numeric csv
  p1 = strtok(p0, "\r\n#, ");
  if (!p1) 
    return false;
  p2 = strtok(NULL, ", ");
  if (!p2) 
    return false;
  // save last samples
  ringSamp();
  // new values
  ant.temp = atof(p1);
  ant.depth = atof(p2);
  // sampT set in antSample
  DBG2("= %4.2f, %4.2f", ant.temp, ant.depth)
  tmrStop(ant_tmr);
  if (ant.autoSample)
    antSample();
  return true;
} // antRead

///
// data read recently
bool antFresh(void) {
  int fresh;
  fresh = time(0)-ant.sampT;
  if (fresh<0) return false;
  DBG4("aFr=%d", fresh)
  return fresh<ant.fresh;
}

///
// tmr not expired and on
bool antPending(void) {
  bool r=false;
  if (!tmrExp(ant_tmr) && tmrOn(ant_tmr))
    r = true;
  DBG2("aPe=%d", r)
  return r;
}
    
///
// if data, read. if !pending, sample. if !fresh, wait.
// antRead may fail, e.g. sleep mode; if so, !antFresh
float antDepth(void) {
  DBG2("aDep")
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
  int dir;
  float v;
  // got samples? 
  if (ant.sampT - ant.ring->sampT > ant.ringFresh) return false;
  // candidate velo
  v = (ant.depth - ant.ring->depth) / (ant.sampT - ant.ring->sampT);
  DBG3("candiV:%4.2f", v)
  dir = ringDir(v);
  if (dir) *velo = v;
  else *velo = 0.0;
  DBG2("aV=%4.2f", *velo)
  return true;
} // antVelo

///
// sets: ring ring.depth ring.sampT
void ringSamp(void) {
  ant.ring->depth = ant.depth;
  ant.ring->sampT = ant.sampT;
  ant.ring = ant.ring->next;
} // ringSamp

///
// is this ring consistent increasing (1) or decreasing (-1)
int ringDir(float v) {
  RingNode *n;
  int dir;
  // direction - check all samples for consistent direction
  dir = (v>0) ? 1 : -1;
  for (n=ant.ring; n->next!=ant.ring; n=n->next) {
    DBG3("rd:%3.1f", n->depth)
    if (dir==1) // fall
      if (n->depth > n->next->depth)
        dir = 0;
    if (dir==-1) // rise
      if (n->depth < n->next->depth)
        dir = 0;
    if (dir==0) break;
  }
  return dir;
} // ringDir

/// debug
// print out values of ring
void ringPrint(void) {
  RingNode *r = ant.ring;
  time_t now = time(0);
  int i;
  printf("ring\n");
  for (i=0; i<ant.ringSize; i++) {
    printf("[%d]%3.1f,%d\n", i, r->depth, (int)(r->sampT-now));
    r = r->next;
  }
}

///
// clear sample times used by antVelo, call this when winch stops
// could be replaced with free(), calloc()
// sets: ant.ring->sampT;
void antRingReset(void) {
  RingNode *n;
  DBG0("antRingReset()")
  n = ant.ring; 
  while (true) {
    n->sampT = 0;
    n = n->next;
    if (n==ant.ring) break;
  } // while
} // antRingReset

///
// antmod uMPC cf2 and iridium A3LA
// switch between devices on com1, clear pipe
void antDevice(DevType dev) {
  if (dev==ant.dev) return;
  utlDelay(SETTLE);
  DBG1("antDevice(%s)",(dev==cf2_dev)?"cf2":"a3la")
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
  DBG1("antSwitch(%s)", (antenna==gps_ant)?"gps":"irid")
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
} // antAuton

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

float antSurfD(void) {
  return ant.surfD;
}
