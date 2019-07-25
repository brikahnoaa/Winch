// s39.c - for working with s39 module
#include <main.h>

#define EOL "\r"
#define BAUD 9600L
#define EXEC "<Executed/>"

S39Info s39;

///
// turn on s39 module, wait until s39 responds
// sets: s39.mode .port
void s39Init(void) {
  static char *self="s39Init";
  DBG();
  s39.me="s39";
  antPort(&s39.port);
  s39.on = false;
  s39Start();
  utlReadExpect(s39.port, all.str, EXEC, 2);
  utlWrite(s39.port, "TxSampleNum=N", EOL);
  utlReadExpect(s39.port, all.str, EXEC, 2);
  utlWrite(s39.port, "txRealTime=n", EOL);
  utlReadExpect(s39.port, all.str, EXEC, 2);
  if (s39.initStr) {
    utlWrite(s39.port, s39.initStr, EOL);
    utlReadExpect(s39.port, all.str, EXEC, 2);
  }
  // just in case auton was left on
  utlWrite(s39.port, "stop", EOL);
  utlReadExpect(s39.port, all.str, EXEC, 2);
  s39Stop();
} // s39Init

///
// turn on, clean, set params, talk to sbe39
int s39Start(void) {
  static char *self="s39Start";
  if (s39.on) // verify
    if (s39Prompt()) {
      s39Sample();
      return 0;
    } else {
      flogf("\n%s(): ERR sbe39, expected prompt", self);
      return 1;
    }
  s39.on = true;
  flogf("\n === s39 module start %s", utlDateTime());
  // s39LogOpen();
  antDevice(cf2_dev);
  utlDelay(200);
  TURxFlush(s39.port);
  TUTxFlush(s39.port);
  // get cf2 startup message
  if (!utlReadExpect(s39.port, all.str, "ok", 6))
    flogf("\n%s(): expected ok, saw '%s'", self, all.str);
  DBG1("%s", all.str);
  if (s39.auton)
    s39Auton(false);
  sprintf(all.str, "datetime=%s", utlDateTimeS16());
  utlWrite(s39.port, all.str, EOL);
  if (!utlReadExpect(s39.port, all.str, EXEC, 5))
    flogf("\n%s(): ERR sbe39, datetime not executed", self);
  if (s39.startStr) {
    utlWrite(s39.port, s39.startStr, EOL);
    utlReadExpect(s39.port, all.str, EXEC, 2);
  }
  s39Sample();
  return 0;
} // s39Start

///
int s39Stop() {
  s39.on = false;
  flogf("\n === s39 module stop %s", utlDateTime());
  if (s39.auton) s39Auton(false);
  s39LogClose();
  return 0;
} // s39Stop

///
// open or reopen log file
int s39LogOpen(void) {
  static char *self="s39LogOpen";
  int r=0;
  if (!s39.on)
    s39Start();
  if (!s39.log)
    r = utlLogOpen(&s39.log, s39.me);
  else
    DBG2("%s: log already open", self);
  return r;
} // s39LogOpen

///
// open or reopen log file
int s39LogClose(void) {
  static char *self="s39LogClose";
  int r=0;
  if (s39.log)
    r = utlLogClose(&s39.log);
  else
    DBG2("%s: log already closed", self);
  return r;
} // s39LogClose

///
// rets: true==success (returns early)
bool s39Prompt() {
  DBG1("aP");
  if (s39Pending()) 
    s39DataWait();
  antDevice(cf2_dev);
  TURxFlush(s39.port);
  // if asleep, first EOL wakens but no response
  utlWrite(s39.port, "", EOL);
  if (utlReadExpect(s39.port, all.str, EXEC, 1))
    return true;
  utlWrite(s39.port, "", EOL);
  if (utlReadExpect(s39.port, all.str, EXEC, 1))
    return true;
  // try a third time after break
  s39Break();
  utlNap(2);
  TURxFlush(s39.port);
  utlWrite(s39.port, "", EOL);
  if (utlReadExpect(s39.port, all.str, EXEC, 1))
    return true;
  utlErr(s39_err, "s39Prompt() fail");
  return false;
} // s39Prompt

///
// reset or exit sync mode
void s39Break(void) {
  static char *self="s39Break";
  DBG();
  TUTxPutByte(s39.port, (ushort) 2,1);      // ^B  (blocking)
} // s39Break

///
// data waiting
bool s39Data() {
  int r;
  DBG2("aD");
  r=TURxQueuedCount(s39.port);
  if (r)
    tmrStop(s39_tmr);
  return r>0;
} // s39Data

///
// wait for data or not pending (timeout)
bool s39DataWait(void) {
  static char *self="aDW";
  DBG();
  do if (s39Data()) 
    return true;
  while (s39Pending());
  flogf(" %s:fail", self);
  return false;
} // s39DataWait

///
// flush; if !tmrOn request sample
// sets: s39_tmr
void s39Sample(void) {
  if (s39Pending()) return;
  DBG0("aSam");
  // flush old data, check for sleep message and prompt if needed
  if (s39Data()) {
    utlRead(s39.port, all.str);
    if (strstr(all.str, "sleep"))
      s39Prompt();      // wakeup
  } // s39Data()
  if (!s39.auton && s39.sampStore)
    utlWrite(s39.port, "TSSon", EOL);
  else
    utlWrite(s39.port, "TS", EOL);
  // catch echo - none in auton
  if (!s39.auton)
    utlReadWait(s39.port, all.str, 1);
  tmrStart(s39_tmr, s39.delay);
  s39.sampT = time(0);
} // s39Sample

///
// TS   ->' 20.1000,    1.287, 18 Sep 1914, 12:40:30\\<Executed/>\\' 56
// TSSon->' 20.1000,    1.287, 18 Sep 1914, 12:40:30, 126\\<Executed/>\\' 61
//  - note: now TS=TSSon due to TxSampleNum=N
// sets: s39.temp .depth 
// note: s39.sampT set in s39Sample()
bool s39Read(void) {
  char *p0, *p1, *p2;
  static char *self="s39Read";
  DBG0("aRd");
  if (!s39Data()) return false;
  // data waiting
  // with auton there is no Executed, so look for #
  if (s39.auton)
    p0 = utlReadExpect(s39.port, all.str, "# ", 2);
  else
    p0 = utlReadExpect(s39.port, all.str, EXEC, 2);
  if (!p0) {
    utlErr(s39_err, "s39Read: no data");
    return false;
  } // not data
  if (s39.log)
    write(s39.log, all.str, strlen(all.str));
  // read temp, depth // parse two numeric csv
  p1 = strtok(all.str, "\r\n#, ");
  p2 = strtok(NULL, ", ");
  if (!p1 || !p2) {
    utlErr(s39_err, "s39Read: garbage");
    return false;
  }
  // sampT was set in s39Sample
  // new values
  s39.temp = atof(p1);
  s39.depth = atof(p2);
  if (s39.temp==0.0 && s39.depth==0.0) {
    utlErr(s39_err, "s39Read: null values");
    return false;
  }
  DBG2("= %4.2f, %4.2f", s39.temp, s39.depth);
  s39Sample();
  return true;
} // s39Read

///
// tmr not expired and on
bool s39Pending(void) {
  return (tmrOn(s39_tmr));
}
    
///
// if data then read
// retn: .depth
float s39Depth(void) {
  DBG1("aDep");
  if (s39Data())
    s39Read();
  return s39.depth;
} // s39Depth

float s39Temp(void) {
  s39Depth();
  return s39.temp;
} // s39Temp

///
// turn autonomous on/off, with no output. Fetch with s39GetSamples()
// sets: .auton
// rets: 0=good 1=off 2=badResponse
int s39Auton(bool auton) {
  int r=0;
  flogf("\ns39Auton(%s)", auton?"true":"false");
  if (!s39.on) {
    r = 1;
    s39Start();
  }
  s39Prompt();
  if (auton) {
    sprintf(all.str, "sampleInterval=%d", s39.sampInter);
    utlWrite(s39.port, all.str, EOL);
    utlReadExpect(s39.port, all.str, EXEC, 2);
    utlWrite(s39.port, "startnow", EOL);
    if (!utlReadExpect(s39.port, all.str, "-->", 2)) {
      flogf("\t| startnow fail, retry ...");
      utlWrite(s39.port, "startnow", EOL);
      if (!utlReadExpect(s39.port, all.str, "-->", 2)) 
        flogf(" startnow failed");
    } // if -->
  } else {
    utlWrite(s39.port, "stop", EOL);
    if (!utlReadExpect(s39.port, all.str, "-->", 2)) {
      r = 2;
      flogf("\t| stop fail, retry ...");
      utlWrite(s39.port, "stop", EOL);
      if (!utlReadExpect(s39.port, all.str, "-->", 2)) 
        flogf(" stop failed");
    } // if -->
    utlNap(1);
    TURxFlush(s39.port);
  } // if auton
  s39.auton = auton;
  return r;
}

///
// write stored sample to a file
void s39GetSamples(void) {
  static char *self="s39GetSamples";
  int len1=sizeof(all.str);
  int len2=len1, len3=len1;
  int total=0;
  int log;
  DBG();
  len1 = len2 = len3 = sizeof(all.buf);
  s39Auton(false);
  if (s39.log)
    log = s39.log;
  else
    if (utlLogOpen(&log, s39.me)) {
      flogf("%s() failed", self);
      return;
    }
  s39Prompt();          // wakeup
  utlWrite(s39.port, "GetSamples:", EOL);
  while (len1==len3) {
    // repeat until less than a full buf
    len2 = (int) TURxGetBlock(s39.port, all.buf, (long) len1, (short) 1000);
    len3 = write(log, all.buf, len2);
    if (len2!=len3)
      flogf("\t| ERR fail write to log");
    total += len3;
  } // while ==
  flogf(": %d bytes to %s", total, s39.me);
  if (s39.sampClear) {
    utlWrite(s39.port, "initLogging", EOL);
    utlReadExpect(s39.port, all.str, "-->", 2);
    utlWrite(s39.port, "initLogging", EOL);
    utlReadExpect(s39.port, all.str, "-->", 2);
  }
} // s39GetSamples
