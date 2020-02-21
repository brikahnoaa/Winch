// ant.c - for working with antenna module
#include <main.h>

#define EOL "\r"
#define BAUD 9600L

AntInfo ant;

///
// turn on antenna module, wait until ant responds
// sets: ant.mode .port
void antInit(void) {
  short rx, tx, i;
  static char *self="antInit";
  DBG();
  ant.me="ant";
  // port
  rx = TPUChanFromPin(ANT_RX);
  tx = TPUChanFromPin(ANT_TX);
  ant.port = TUOpen(rx, tx, BAUD, 0);
  if (ant.port==NULL)
    utlStop("antInit() com1 open fail");
  antDevice(null_dev);
  ant.on = false;
  antStart();
  utlReadExpect(ant.port, all.str, EXEC, 2);
  utlWrite(ant.port, "TxSampleNum=N", EOL);
  utlReadExpect(ant.port, all.str, EXEC, 2);
  utlWrite(ant.port, "txRealTime=n", EOL);
  utlReadExpect(ant.port, all.str, EXEC, 2);
  if (ant.initStr) {
    utlWrite(ant.port, ant.initStr, EOL);
    utlReadExpect(ant.port, all.str, EXEC, 2);
  }
  // just in case auton was left on
  utlWrite(ant.port, "stop", EOL);
  utlReadExpect(ant.port, all.str, EXEC, 2);
  antStop();
} // antInit

///
// turn on, clean, set params, talk to sbe39
int antStart(void) {
  static char *self="antStart";
  if (ant.on) // already on, verify
    if (antPrompt()) {
      return 0;
    } else {
      utlErr(ant_err, "ant: no prompt");
      return 1;
    }
  ant.on = true;
  flogf("\n === ant module start %s", utlDateTime());
  antLogOpen();
  antDevice(cf2_dev);
  PIOClear(ANT_PWR);
  utlDelay(200);
  TURxFlush(ant.port);
  TUTxFlush(ant.port);
  PIOSet(ANT_PWR);
  // get cf2 startup message
  if (!utlReadExpect(ant.port, all.str, "ok", 6))
    flogf("\n%s(): expected ok, saw '%s'", self, all.str);
  if (ant.startStr) {
    utlWrite(ant.port, ant.startStr, EOL);
    utlReadExpect(ant.port, all.str, EXEC, 2);
  }
  s39Start();
  return 0;
} // antStart

///
// turn off power to antmod 
int antStop() {
  s39Stop();
  ant.on = false;
  flogf("\n === ant module stop %s", utlDateTime());
  if (ant.auton) antAuton(false);
  antLogClose();
  antDevice(null_dev);
  PIOClear(ANT_PWR);
  return 0;
} // antStop

///
// open or reopen log file
int antLogOpen(void) {
  static char *self="antLogOpen";
  int r=0;
  if (!ant.on)
    antStart();
  if (!ant.log)
    r = utlLogOpen(&ant.log, ant.me);
  else
    DBG2("%s: log already open", self);
  return r;
} // antLogOpen

///
///
// open or reopen log file
int antLogClose(void) {
  static char *self="antLogClose";
  int r=0;
  if (ant.log)
    r = utlLogClose(&ant.log);
  else
    DBG2("%s: log already closed", self);
  return r;
} // antLogClose

///
// rets: true==success (returns early)
bool antPrompt() {
  DBG1("aP");
  if (antPending()) 
    antDataWait();
  antDevice(cf2_dev);
  TURxFlush(ant.port);
  // if asleep, first EOL wakens but no response
  utlWrite(ant.port, "", EOL);
  if (utlReadExpect(ant.port, all.str, EXEC, 1))
    return true;
  utlWrite(ant.port, "", EOL);
  if (utlReadExpect(ant.port, all.str, EXEC, 1))
    return true;
  // try a third time after break
  antBreak();
  utlNap(2);
  TURxFlush(ant.port);
  utlWrite(ant.port, "", EOL);
  if (utlReadExpect(ant.port, all.str, EXEC, 1))
    return true;
  utlErr(ant_err, "antPrompt() fail");
  return false;
} // antPrompt

///
// reset or exit sync mode
void antBreak(void) {
  static char *self="antBreak";
  DBG();
  TUTxPutByte(ant.port, (ushort) 2,1);      // ^B  (blocking)
} // antBreak

///
// data waiting
bool antData() {
  int r;
  DBG2("aD");
  r=TURxQueuedCount(ant.port);
  if (r)
    tmrStop(s39_tmr);
  return r>0;
} // antData

///
// wait for data or not pending (timeout)
bool antDataWait(void) {
  static char *self="aDW";
  DBG();
  do if (antData()) 
    return true;
  while (antPending());
  flogf(" %s:fail", self);
  return false;
} // antDataWait

///
// flush; if !tmrOn request sample
// sets: s39_tmr
void antSample(void) {
  if (antPending()) return;
  DBG0("aSam");
  // flush old data, check for sleep message and prompt if needed
  if (antData()) {
    utlRead(ant.port, all.str);
    if (strstr(all.str, "sleep"))
      antPrompt();      // wakeup
  } // antData()
  if (!ant.auton && ant.sampStore)
    utlWrite(ant.port, "TSSon", EOL);
  else
    utlWrite(ant.port, "TS", EOL);
  // catch echo - none in auton
  if (!ant.auton)
    utlReadWait(ant.port, all.str, 1);
  tmrStart(s39_tmr, ant.delay);
  ant.sampT = time(0);
} // antSample

///
// TS   ->' 20.1000,    1.287, 18 Sep 1914, 12:40:30\\<Executed/>\\' 56
// TSSon->' 20.1000,    1.287, 18 Sep 1914, 12:40:30, 126\\<Executed/>\\' 61
//  - note: now TS=TSSon due to TxSampleNum=N
// sets: ant.temp .depth 
// note: ant.sampT set in antSample()
bool antRead(void) {
  char *p0, *p1, *p2;
  static char *self="antRead";
  DBG0("aRd");
  if (!antData()) return false;
  // data waiting
  // with auton there is no Executed, so look for #
  if (ant.auton)
    p0 = utlReadExpect(ant.port, all.str, "# ", 2);
  else
    p0 = utlReadExpect(ant.port, all.str, EXEC, 2);
  if (!p0) {
    utlErr(ant_err, "antRead: no data");
    return false;
  } // not data
  if (ant.log)
    write(ant.log, all.str, strlen(all.str));
  // read temp, depth // parse two numeric csv
  p1 = strtok(all.str, "\r\n#, ");
  p2 = strtok(NULL, ", ");
  if (!p1 || !p2) {
    utlErr(ant_err, "antRead: garbage");
    return false;
  }
  // sampT was set in antSample
  // new values
  ant.temp = atof(p1);
  ant.depth = atof(p2);
  if (ant.temp==0.0 && ant.depth==0.0) {
    utlErr(ant_err, "antRead: null values");
    return false;
  }
  DBG2("= %4.2f, %4.2f", ant.temp, ant.depth);
  antSample();
  return true;
} // antRead

///
// tmr not expired and on
bool antPending(void) {
  return (tmrOn(s39_tmr));
}
    
///
// if data then read
// retn: .depth
float antDepth(void) {
  DBG1("aDep");
  if (antData())
    antRead();
  return ant.depth;
} // antDepth

float antTemp(void) {
  antDepth();
  return ant.temp;
} // antTemp

///
// antmod uMPC cf2 and iridium A3LA
// switch between devices on com1, clear pipe
void antDevice(DevType dev) {
  DBG1("antDevice(%s)",(dev==cf2_dev)?"cf2":"a3la");
  if (dev==ant.dev) return;
  utlDelay(SETTLE);
  if (dev==cf2_dev)
    PIOSet(ANT_SEL);
  else if (dev==a3la_dev)
    PIOClear(ANT_SEL);
  else
    return;
  utlDelay(SETTLE);
  TUTxFlush(ant.port);
  TURxFlush(ant.port);
  ant.dev = dev;
  return;
} // antDevice

///
// gps.port = antPort()
Serial antPort(void) {
  return ant.port;
} // antPort

///
// tell antmod to power dev on/off
// should be in gps.c??
void antDevPwr(char c, bool on) {
  DevType currDev=ant.dev;
  DBG0("antDevPwr(%c, %d)", c, on);
  antDevice(cf2_dev);
  if (on)
    TUTxPutByte(ant.port, 3, false);
  else
    TUTxPutByte(ant.port, 4, false);
  TUTxPutByte(ant.port, c, false);
  antDevice(currDev);
} // antDevPwr

///
// should be in gps.c??
void antSwitch(AntType antenna) {
  DevType dev;
  if (antenna==ant.antenna) return;
  DBG1("antSwitch(%s)", (antenna==gps_ant)?"gps":"irid");
  dev = ant.dev;
  antDevice(cf2_dev);
  TUTxPutByte(ant.port, 1, false);        // ^A
  if (antenna==gps_ant) 
    TUTxPutByte(ant.port, 'G', false);
  else
    TUTxPutByte(ant.port, 'I', false);
  antDevice(dev);
  ant.antenna = antenna;
} // antSwitch
    
///
// turn autonomous on/off, with no output. Fetch with antGetSamples()
// sets: .auton
// rets: 0=good 1=off 2=badResponse
int antAuton(bool auton) {
  int r=0;
  flogf("\nantAuton(%s)", auton?"true":"false");
  if (!ant.on) {
    r = 1;
    antStart();
  }
  antPrompt();
  if (auton) {
    sprintf(all.str, "sampleInterval=%d", ant.sampInter);
    utlWrite(ant.port, all.str, EOL);
    utlReadExpect(ant.port, all.str, EXEC, 2);
    utlWrite(ant.port, "startnow", EOL);
    if (!utlReadExpect(ant.port, all.str, "-->", 2)) {
      flogf("\t| startnow fail, retry ...");
      utlWrite(ant.port, "startnow", EOL);
      if (!utlReadExpect(ant.port, all.str, "-->", 2)) 
        flogf(" startnow failed");
    } // if -->
  } else {
    utlWrite(ant.port, "stop", EOL);
    if (!utlReadExpect(ant.port, all.str, "-->", 2)) {
      r = 2;
      flogf("\t| stop fail, retry ...");
      utlWrite(ant.port, "stop", EOL);
      if (!utlReadExpect(ant.port, all.str, "-->", 2)) 
        flogf(" stop failed");
    } // if -->
    utlNap(1);
    TURxFlush(ant.port);
  } // if auton
  ant.auton = auton;
  return r;
}

///
// write stored sample to a file
void antGetSamples(void) {
  static char *self="antGetSamples";
  int len1=sizeof(all.str);
  int len2=len1, len3=len1;
  int total=0;
  int log;
  DBG();
  len1 = len2 = len3 = sizeof(all.buf);
  antAuton(false);
  if (ant.log)
    log = ant.log;
  else
    if (utlLogOpen(&log, ant.me)) {
      flogf("%s() failed", self);
      return;
    }
  antPrompt();          // wakeup
  utlWrite(ant.port, "GetSamples:", EOL);
  while (len1==len3) {
    // repeat until less than a full buf
    len2 = (int) TURxGetBlock(ant.port, all.buf, (long) len1, (short) 1000);
    len3 = write(log, all.buf, len2);
    if (len2!=len3)
      flogf("\t| ERR fail write to log");
    total += len3;
  } // while ==
  flogf(": %d bytes to %s", total, ant.me);
  if (ant.sampClear) {
    utlWrite(ant.port, "initLogging", EOL);
    utlReadExpect(ant.port, all.str, "-->", 2);
    utlWrite(ant.port, "initLogging", EOL);
    utlReadExpect(ant.port, all.str, "-->", 2);
  }
} // antGetSamples

bool antSurf(void) {
  return (ant.depth<(ant.surfD+2));
}

float antSurfD(void) {
  return ant.surfD;
}

AntType antAntenna(void) {
  return ant.antenna;
}
