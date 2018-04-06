// ctd.c sbe16
// 
// ctd.c
// handling CTD in buoy, seabird SBE 16plus
// general note: ctd wants \r only for input
// Bug:! TUTxPrint translates \n into \r\n, which sorta kinda works if lucky
//
// ctd shares com1 with antmod cf2.
//
#include <utl.h>
#include <ctd.h>
#include <mpc.h>
#include <tmr.h>

#define EOL "\r"

CtdInfo ctd;

// \r input, echos input.  \r\n before next output.
// pause 0.33s between \rn and s> prompt.
// wakeup takes 1.045s, writes extra output "SBE 16plus\r\nS>"
// sbe16 - response time for TS during auton?
// sbe16 - response time for getlastsamples during auton?
// sbe16 - response to auton/getlastsamples before first sample stored?

///
// buoy sbe16 set date
// pre: mpcInit sets up serial
// sets: ctd.port .pending .log
void ctdInit(void) {
  DBG0("ctdInit()")
  mpcPam(sbe_pam);
  ctd.port = mpcPort();
  ctd.pending = false;
  ctdAuton(false);
  if (!ctdPrompt())
    utlStop("ERR\t| ctdInit(): no prompt from ctd");
  // sbe16 gets quirky when logging, best to STOP even if it flags error
  utlWrite(ctd.port, "stop", EOL);
  utlWrite(ctd.port, "DelayBeforeSampling=0", EOL);
  sprintf(utlStr, "datetime=%s", utlDateTimeBrief());
  utlWrite(ant.port, utlStr, EOL);
  utlRead(ctd.port,utlBuf);
  DBG2("\nctd>>%s", utlBuf)
} // ctdInit

///
void ctdStop(void){
  mpcPam(null_pam);
  if (ctd.logging) 
    ctdLog(false);
}

///
// sbe16
// ctdPrompt - poke buoy CTD, look for prompt
bool ctdPrompt(void) {
  DBG2("ctdPrompt()")
  TURxFlush(ctd.port);
  utlWrite(ctd.port, "", EOL);
  utlReadWait(ctd.port, utlBuf, 2*ctd.delay);
  // looking for S> at end
  if (strstr(utlBuf, "S>"))
    return true;
  // try again after break
  ctdBreak();
  utlWrite(ctd.port, "", EOL);
  utlReadWait(ctd.port, utlBuf, 2*ctd.delay);
  if (strstr(utlBuf, "S>"))
    return true;
  return false;
} // ctdPrompt

///
// reset, exit sync mode
void ctdBreak(void) {
  DBG0("ctdBreak()")
  TUTxBreak(ctd.port, 5000);
} // ctdBreak

///
// data waiting
bool ctdData() {
  return TURxQueuedCount(ctd.port);
} // ctdData

///
// poke ctd to get sample, set interval timer (ignore ctd.auton)
// pause between ts\r\n and result = 4.32s
// sets: ctd.pending ctd_tmr
void ctdSample(void) {
  int len;
  DBG0("ctdSample()")
  if (ctd.auton || ctd.pending) return;
  ctdPrompt();
  utlWrite(ctd.port, "TS", EOL);
  len = utlReadWait(ctd.port, utlBuf, 1);
  if (len<3) 
    flogf("\nERR ctdSample, TS command fail");
  ctd.pending = true;
  // pending response, timeout in 5sec, checked by ctdReady()
  tmrStart( ctd_tmr, ctd.delay );
} // ctdSample

///
// sbe16 response is just over 3sec in sync, well over 4sec in command
// waits up to ctd.delay+1 seconds - good to call after tgetq()
// data is reformatted to save a little space, written to ctd.log
// logs: reformatted data string
// sets: ctd.depth .pending 
void ctdRead() {
  int len;
  float temp, cond, pres, flu, par, sal;
  DBG0("ctdRead()")
  if (ctd.pending) 
    ctd.pending = false;
  else
    return;           // error
  // waits up to max+1 seconds - best called after tgetq()
  len = utlReadWait(ctd.port, utlBuf, ctd.delay+1);
  if (len==0) {       // error
    flogf("\nERR\t| ctdRead() no response in %d sec", ctd.delay+1);
    ctd.depth = 0;
    return;
  }
  DBG2("\n\tctd-->%s", utlNonPrint(utlBuf))
  // Temp, conductivity, depth, fluromtr, PAR, salinity, time
  // ' 20.6538,  0.01145,    0.217,   0.0622, 01 Aug 2016 12:16:50\r\n'
  // note: leading # in syncmode '# 20.6...'
  // note: picks up trailing S> prompt if not in syncmode
  temp = atof( strtok(utlBuf, "\r\n#, "));
  cond = atof( strtok(NULL, ", ")); 
  pres = atof( strtok(NULL, ", "));
  flu = atof( strtok(NULL, ", "));
  par = atof( strtok(NULL, ", "));
  sal = atof( strtok(NULL, ", "));
  // rest is date time
  // day = strtok(NULL, ", ");
  // month = strtok(NULL, " ");
  // year = strtok(NULL, " ");
  // time = strtok(NULL, " \r\n");
  // record
  flogf("\nSBE16:\t%.4f %.5f %.3f %.4f %.4f %.4f %s",
    temp, cond, pres, flu, par, sal, utlDateTime());
  // ctd.log is for autonomous getsample, see ctdAuto
  // len = strlen(utlBuf);
  // if (write(ctd.log, utlBuf, len)<len) 
  //   flogf("\nERR\t| ctdRead log fail");
  ctd.depth = pres;
  return;
} // ctdRead

///
// sample if not pending, wait for data, return depth
float ctdDepth() {
  if (!ctd.pending)
    ctdSample();
  ctdRead();
  return ctd.depth;
} // ctdDepth

///
// turn autonomous on/off. see ctdLog
void ctdAuton(bool auton) {
  if (ctd.auton==auton) return;
  DBG0("ctdAuton(%d)", auton)
  if (auton) {
    // note - initlogging done at end of ctdGetSamples
    ctdPrompt();
    utlWrite(ctd.port, "sampleInterval=10", EOL);
    utlWrite(ctd.port, "txRealTime=y", EOL);
    utlWrite(ctd.port, "startnow", EOL);
    utlRead(ctd.port,utlBuf);
    DBG2("\nctd>>%s", utlBuf)
  } else {
    utlWrite(ctd.port, "stop", EOL);
    // pause and flush, some sample output after "stop"
    utlNap(2*ctd.delay);
    utlRead(ctd.port, utlBuf);
    DBG2("\nctd>>%s", utlBuf)
  } // if auton
  ctd.auton = auton;
} // ctdAuton

///
// get science, clear log
void ctdGetSamples(void) {
  int len1=BUFSZ;
  int len2=len1, len3=len1;
  DBG0("ctdLog(%s)", ctd.logFile)
  ctd.log = utlLogFile(ctd.logFile);
  ctdPrompt();          // wakeup
  utlWrite(ctd.port, "GetSamples:", EOL);
  while (len1==len3) {
    // repeat until less than a full buf
    len2 = (int) TURxGetBlock(ctd.port, utlBuf, (long) len1, (short) 1000);
    len3 = write(ctd.log, utlBuf, len2);
    if (len2!=len3) 
      flogf("\nERR\t| ctdLog() could not write ctd.log");
  } // while ==
  close(ctd.log);
  utlWrite(ctd.port, "initLogging", EOL);
  utlWrite(ctd.port, "initLogging", EOL);
  utlRead(ctd.port,utlBuf);
  DBG2("\nctd>>%s", utlBuf)
} // ctdLog

///
// start logging ctdDepth() calls
void ctdLog(bool on) {
  DBG0("ctdLog(%s)", ctd.logFile)
  if (on) {
    ctd.log = utlLogFile(ctd.logFile);
    ctd.logging = true;
  } else {
    close(ctd.log);
    ctd.logging = false;
  }
}

