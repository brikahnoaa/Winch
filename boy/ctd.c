// ctd.c sbe16
#include <utl.h>
#include <ctd.h>
#include <mpc.h>
#include <tmr.h>

#define EOL "\r"
#define EXEC "S>"

CtdInfo ctd;

// general note: ctd wants \r only for input, outputs \r\n
// Bug:! TUTxPrint translates \n into \r\n, which sorta kinda works if lucky
//
// \r input, echos input.  \r\n before next output.
// pause 0.33s between \rn and s> prompt.
// wakeup takes 1.045s, writes extra output "SBE 16plus\r\nS>"
// pause between ts\r\n and result = 4.32s
// sbe16 response is just over 3sec in sync, well over 4sec in command
// NOTE - sbe16 does not echo while logging, must get prompt before STOP

///
// sets: ctd.port .ctdPending
void ctdInit(void) {
  static char *self="ctdInit";
  DBG()
  ctd.port = mpcPamPort();
  ctdStart();
  utlWrite(ctd.port, "DelayBeforeSampling=0", EOL);
  utlReadWait(ctd.port, all.buf, 1);   // echo
  utlWrite(ctd.port, "stop", EOL);
  utlReadWait(ctd.port, all.buf, 1);   // echo
  ctdStop();
} // ctdInit

///
int ctdStart(void) {
  static char *self="ctdStart";
  if (ctd.on) // verify
    if (ctdPrompt()) {
      return 0;
    } else {
      flogf("\n%s(): ERR sbe39, expected prompt", self);
      return 1;
    }
  ctd.on = true;
  flogf("\n === buoy sbe16 start %s", utlDateTime());
  mpcPamDev(sbe16_pam);
  tmrStop(s16_tmr);
  if (!ctd.log && strlen(ctd.logFile))
    utlLogFile(&ctd.log, ctd.logFile);
  if (!ctdPrompt())
    utlErr(ctd_err, "ctd: no prompt");
  sprintf(all.str, "datetime=%s", utlDateTimeCtd());
  utlWrite(ctd.port, all.str, EOL);
  utlReadWait(ctd.port, all.buf, 2);   // echo
  return 0;
} // ctdStart

///
int ctdStop(void){
  static char *self="ctdStop";
  flogf("\n === buoy sbe16 stop %s", utlDateTime());
  utlCloseFile(&ctd.log);
  if (ctd.auton)
    ctdAuton(false);
  mpcPamDev(null_pam);
  ctd.on = false;
  return 0;
} // ctdStop

///
// sbe16
// ctdPrompt - poke buoy CTD, look for prompt
bool ctdPrompt(void) {
  DBG1("cPt")
  if (ctdPending()) 
    ctdDataWait();
  ctdFlush();
  utlWrite(ctd.port, "", EOL);
  // looking for S> at end
  if (utlExpect(ctd.port, all.buf, EXEC, 5))
    return true;
  // try again after break
  ctdBreak();
  utlWrite(ctd.port, "", EOL);
  if (utlExpect(ctd.port, all.buf, EXEC, 5))
    return true;
  return false;
} // ctdPrompt

///
// reset, exit sync mode
void ctdBreak(void) {
  static char *self="ctdBreak";
  DBG()
  TUTxBreak(ctd.port, 5000);
} // ctdBreak

///
// data waiting
bool ctdData() {
  int r;
  DBG2("cDa")
  r=TURxQueuedCount(ctd.port);
  if (r)
    tmrStop(s16_tmr);
  return r>0;
} // ctdData

///
// wait for data or not pending (timeout)
bool ctdDataWait(void) {
  static char *self="cDW";
  DBG()
  while (ctdPending())
    if (ctdData()) 
      return true;
  // should not fail
  flogf(" %s:fail", self);
  return false;
} // ctdDataWait

///
// poke ctd to get sample, set interval timer (ignore ctd.auton)
// sets: s16_tmr
void ctdSample(void) {
  if (ctdPending()) return;
  DBG1("cSam")
  // flush old data, check for sleep message and prompt if needed
  if (ctdData()) {
    utlRead(ctd.port, all.buf);
    if (strstr(all.buf, "time out"))
      ctdPrompt();      // wakeup
  } // ctdData()
  if (!ctd.auton && ctd.sampStore)
    utlWrite(ctd.port, "TSSon", EOL);
  else
    utlWrite(ctd.port, "TS", EOL);
  // get echo // NOTE - sbe16 does not echo while auton
  if (!ctd.auton)
    utlReadWait(ctd.port, all.buf, 1);
  tmrStart(s16_tmr, ctd.delay);
} // ctdSample

///
// sample, read data, log to file
// sets: .temp .cond .depth 
bool ctdRead(void) {
  char *p0, *p1, *p2, *p3;
  static char *self="ctdRead";
  DBG()
  if (!ctdData()) return false;
  // utlRead(ctd.port, all.buf);
  p0 = utlExpect(ctd.port, all.buf, EXEC, 2);
  if (!p0) {
    utlErr(ctd_err, "ctdRead: no S>");
    return false;
  } // not data
  if (ctd.log) 
    write(ctd.log, all.buf, strlen(all.buf)-2); // no S>
  // Temp, conductivity, depth, fluromtr, PAR, salinity, time
  // ' 20.6538,  0.01145,    0.217,   0.0622, 01 Aug 2016 12:16:50\r\n'
  // note: leading # in syncmode '# 20.6...'
  // note: picks up trailing S> prompt if not in syncmode
  p0 = all.buf;
  p1 = strtok(p0, "\r\n#, ");
  if (!p1) return false;
  ctd.temp = atof( p1 );
  p2 = strtok(NULL, ", "); 
  if (!p2) return false;
  ctd.cond = atof( p2 );
  p3 = strtok(NULL, ", ");
  if (!p3) return false;
  ctd.depth = atof( p3 );
  if (ctd.temp==0.0 && ctd.depth==0.0) {
    utlErr(ant_err, "antRead: null values");
    return false;
  }
  DBG1("= %4.2", ctd.depth)
  ctd.time = time(0);
  ctdSample();
  return true;
} // ctdRead

///
// tmrOn ? ctdPending. tmrExp ? err
bool ctdPending(void) {
  return (tmrOn(s16_tmr));
}

///
float ctdDepth(void) {
  DBG1("cDep")
  if (ctdData())
      ctdRead();
  return ctd.depth;
} // ctdDepth

///
// NOTE - sbe16 does not echo while logging, but it does S> prompt
// must get prompt after log starts, before STOP 
// "start logging at = 08 Jul 2018 05:28:29, sample interval = 10 seconds\r\n"
// sets: .auton
// rets: 0=good 1=off 2=badResponse
int ctdAuton(bool auton) {
  int r=0;
  flogf("\nctdAuton(%s)", auton?"true":"false");
  if (!ctd.on) {
    r = 1;
    ctdStart();
  }
  if (auton) {
    // note - initlogging may be done at end of ctdGetSamples
    if (ctdPending())
      ctdDataWait();
    ctdPrompt();
    sprintf(all.str, "sampleInterval=%d", ctd.sampleInt);
    utlWrite(ctd.port, all.str, EOL);
    utlExpect(ctd.port, all.str, EXEC, 2);
    utlWrite(ctd.port, "txRealTime=n", EOL);
    utlExpect(ctd.port, all.str, EXEC, 2);
    utlWrite(ctd.port, "startnow", EOL);
    if (!utlExpect(ctd.port, all.str, "start logging", 4)) {
      r = 1;
      utlErr(ctd_err, "ctdAuton: expected 'start logging'");
    }
    // ctdPrompt();
  } else {
    // turn off
    ctdPrompt();
    // utlWrite(ctd.port, "stop", EOL);
    // utlExpect(ctd.port, all.str, EXEC, 2);
    utlWrite(ctd.port, "stop", EOL);
    if (!utlExpect(ctd.port, all.buf, "logging stopped", 4)) {
      flogf("\nERR\t| expected 'logging stopped', retry...");
      utlWrite(ctd.port, "stop", EOL);
      if (!utlExpect(ctd.port, all.buf, "logging stopped", 4)) {
        r=2;
        flogf("\nERR\t| got '%s'", all.buf);
        utlErr(ctd_err, "expected 'logging stopped'");
      }
    }
  } // if auton
  ctd.auton = auton;
  return r;
} // ctdAuton

///
// get science, clear log
void ctdGetSamples(void) {
  int len1=sizeof(all.buf);
  int len2=len1, len3=len1;
  int total=0;
  if (!ctd.log && strlen(ctd.logFile))
    if (utlLogFile(&ctd.log, ctd.logFile)) return;
  flogf("\n+ctdGetSamples()");
  ctdPrompt();          // wakeup
  utlWrite(ctd.port, "GetSamples:", EOL);
  while (len1==len3) {
    // repeat until less than a full buf
    len2 = (int) TURxGetBlock(ctd.port, all.buf, (long) len1, (short) 1000);
    len3 = write(ctd.log, all.buf, len2);
    if (len2!=len3) 
      flogf("\nERR\t| ctdGetSamples() could not write %s.log", ctd.logFile);
    flogf("+[%d]", len3);
    total += len3;
  } // while ==
  utlCloseFile(&ctd.log);
  if (ctd.clearSamp) {
    utlWrite(ctd.port, "initLogging", EOL);
    utlExpect(ctd.port, all.buf, "verify", 2);
    utlWrite(ctd.port, "initLogging", EOL);
    utlExpect(ctd.port, all.buf, EXEC, 2);
  }
  flogf(" = %d bytes to %s", total, ctd.logFile);
} // ctdGetSamples

///
// flush input - clears timeout
void ctdFlush(void) {
  TURxFlush(ctd.port);
} // ctdFlush
