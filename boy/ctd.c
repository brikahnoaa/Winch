// ctd.c sbe16
#include <utl.h>
#include <ctd.h>
#include <mpc.h>
#include <tmr.h>

#define EOL "\r"

CtdInfo ctd;

// general note: ctd wants \r only for input, outputs \r\n
// Bug:! TUTxPrint translates \n into \r\n, which sorta kinda works if lucky
//
// \r input, echos input.  \r\n before next output.
// pause 0.33s between \rn and s> prompt.
// wakeup takes 1.045s, writes extra output "SBE 16plus\r\nS>"
// pause between ts\r\n and result = 4.32s
// sbe16 response is just over 3sec in sync, well over 4sec in command

///
// sets: ctd.port .ctdPending
void ctdInit(void) {
  DBG0("ctdInit()")
  mpcPam(sbe_pam);
  ctd.port = mpcPort();
  ctdAuton(false);
  tmrStop(ctd_tmr);
  if (!ctdPrompt())
    utlErr(ctd_err, "ctd: no prompt");
  // sbe16 gets quirky when logging, best to STOP even if it flags error
  utlWrite(ctd.port, "stop", EOL);
  utlWrite(ctd.port, "DelayBeforeSampling=0", EOL);
  sprintf(utlStr, "datetime=%s", utlDateTimeBrief());
  utlWrite(ctd.port, utlStr, EOL);
  utlRead(ctd.port,utlBuf);
  DBG2("\nctd>>%s", utlBuf)
  if (strlen(ctd.logFile))
    ctd.log = utlLogFile(ctd.logFile);
  if (ctd.logging)
    strcpy(ctd.sample, "TSSon");
  else
    strcpy(ctd.sample, "TS");
} // ctdInit

///
void ctdStop(void){
  mpcPam(null_pam);
  if (ctd.log) 
    close(ctd.log);
  ctd.log = 0;
} // ctdStop

///
// sbe16
// ctdPrompt - poke buoy CTD, look for prompt
bool ctdPrompt(void) {
  DBG1("ctdPrompt()")
  TURxFlush(ctd.port);
  utlWrite(ctd.port, "", EOL);
  utlReadWait(ctd.port, utlBuf, 2+ctd.delay);
  // looking for S> at end
  if (strstr(utlBuf, "S>"))
    return true;
  // try again after break
  ctdBreak();
  utlWrite(ctd.port, "", EOL);
  utlReadWait(ctd.port, utlBuf, 2+ctd.delay);
  if (strstr(utlBuf, "S>"))
    return true;
  return false;
} // ctdPrompt

///
// reset, exit sync mode
void ctdBreak(void) {
  DBG1("ctdBreak()")
  TUTxBreak(ctd.port, 5000);
} // ctdBreak

///
// data waiting
bool ctdData() {
  DBG1("cD")
  return TURxQueuedCount(ctd.port);
} // ctdData

///
// poke ctd to get sample, set interval timer (ignore ctd.auton)
// sets: ctd.ctdPending ctd_tmr
void ctdSample(void) {
  int len;
  DBG1("ctdSample()")
  if (ctdPending()) return;
  ctdPrompt();
  utlWrite(ctd.port, ctd.sample, EOL);
  len = utlReadWait(ctd.port, utlBuf, 1);
  // get echo ?? better check, and utlErr()
  if (!strstr(utlBuf, ctd.sample))
    utlErr(ctd_err, "ctd: TS command fail");
  tmrStart( ctd_tmr, ctd.delay );
} // ctdSample

///
// sets: ctd.depth .ctdPending 
bool ctdRead(void) {
  char *p0, *p1, *p2, *p3;
  if (!ctdData()) return false;
  DBG1("ctdRead()")
  utlRead(ctd.port, utlBuf);
  if (ctd.log) 
    write(ctd.log, utlBuf, strlen(utlBuf));
  // Temp, conductivity, depth, fluromtr, PAR, salinity, time
  // ' 20.6538,  0.01145,    0.217,   0.0622, 01 Aug 2016 12:16:50\r\n'
  // note: leading # in syncmode '# 20.6...'
  // note: picks up trailing S> prompt if not in syncmode
  p0 = utlBuf;
  p1 = strtok(p0, "\r\n#, ");
  if (!p1) return false;
  p2 = strtok(NULL, ", "); 
  if (!p2) return false;
  p3 = strtok(NULL, ", ");
  if (!p3) return false;
  ctd.depth = atof( p3 );
  DBG1("= %4.2", ctd.depth)
  tmrStop(ctd_tmr);
  ctd.time = time(0);
  return true;
} // ctdRead

///
// data read recently
bool ctdFresh(void) {
  DBG1("ctdFresh()")
  return (time(0)-ctd.time)<ctd.fresh;
}

///
// tmrOn ? ctdPending. tmrExp ? err
bool ctdPending(void) {
  if (ctd.auton || tmrOn(ctd_tmr))
    return true;
  if (tmrExp(ctd_tmr))
    utlErr(ctd_err, "ctd: timer expired");
  return false;
}

///
// sample if not ctdPending, wait for data, return depth
float ctdDepth(void) {
  DBG1("ctdDepth()")
  if (!ctdData() && ctdFresh())
    return ctd.depth;
  if (!ctdPending())
      ctdSample();
  // err if timeout ?? count?
  while (ctdPending())
    if (ctdData())
      if (ctdRead())
        return ctd.depth;
  // timeout
  return ctd.depth;
} // ctdDepth

///
// turn autonomous on/off.
void ctdAuton(bool auton) {
  DBG0("ctdAuton(%d)", auton)
  if (auton) {
    // note - initlogging done at end of ctdGetSamples
    ctdPrompt();
    utlWrite(ctd.port, "sampleInterval=10", EOL);
    utlReadWait(ctd.port, utlBuf, 1);
    utlWrite(ctd.port, "txRealTime=y", EOL);
    utlReadWait(ctd.port, utlBuf, 1);
    utlWrite(ctd.port, "startnow", EOL);
    utlReadWait(ctd.port, utlBuf, 1);
    if (!strstr(utlBuf, "Start logging"))
      utlErr(ctd_err, "ctdAuton: expected 'Start logging' header");
  } else {
    utlWrite(ctd.port, "stop", EOL);
    // utlNap(2+ctd.delay);
    utlReadWait(ctd.port, utlBuf, 1);
  } // if auton
  tmrStop(ctd_tmr);
  ctd.auton = auton;
} // ctdAuton

///
// get science, clear log
void ctdGetSamples(void) {
  int len1=sizeof(utlBuf);
  int len2=len1, len3=len1;
  int total=0;
  ctd.log = utlLogFile(ctd.logFile);
  flogf("\n+ctdGetSamples()");
  ctdPrompt();          // wakeup
  utlWrite(ctd.port, "GetSamples:", EOL);
  while (len1==len3) {
    // repeat until less than a full buf
    len2 = (int) TURxGetBlock(ctd.port, utlBuf, (long) len1, (short) 1000);
    len3 = write(ctd.log, utlBuf, len2);
    if (len2!=len3) 
      flogf("\nERR\t| ctdGetSamples() could not write ctd.log");
    flogf("+[%d]", len3);
    total += len3;
  } // while ==
  close(ctd.log);
  utlWrite(ctd.port, "initLogging", EOL);
  utlWrite(ctd.port, "initLogging", EOL);
  utlReadWait(ctd.port, utlBuf, 1);
  flogf(" = %d bytes to %s", total, ctd.logFile);
} // ctdGetSamples

