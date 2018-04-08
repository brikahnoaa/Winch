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
    utlStop("ERR\t| ctdInit(): no prompt from ctd");
  // sbe16 gets quirky when logging, best to STOP even if it flags error
  utlWrite(ctd.port, "stop", EOL);
  utlWrite(ctd.port, "DelayBeforeSampling=0", EOL);
  sprintf(utlStr, "datetime=%s", utlDateTimeBrief());
  utlWrite(ctd.port, utlStr, EOL);
  utlRead(ctd.port,utlBuf);
  DBG2("\nctd>>%s", utlBuf)
  if (strlen(ctd.logFile))
    ctd.log = utlLogFile(ctd.logFile);
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
// sets: ctd.ctdPending ctd_tmr
void ctdSample(void) {
  int len;
  DBG0("ctdSample()")
  if (ctdPending()) return;
  ctdPrompt();
  utlWrite(ctd.port, "TS", EOL);
  len = utlReadWait(ctd.port, utlBuf, 1);
  // get echo ?? better check, and utlErr()
  if (len<8 || !strstr(utlBuf, "TS"))
    utlStop("\nERR ctdSample, TS command fail");
  tmrStart( ctd_tmr, ctd.delay );
} // ctdSample

///
// sets: ctd.depth .ctdPending 
void ctdRead() {
  float pres;
  char *p0, *p1;
  if (!ctdData()) return;
  DBG0("ctdRead()")
  utlRead(ctd.port, utlBuf);
  if (ctd.log) 
    write(ctd.log, utlBuf, strlen(utlBuf));
  DBG2("\n\tctd-->%s", utlNonPrint(utlBuf))
  // Temp, conductivity, depth, fluromtr, PAR, salinity, time
  // ' 20.6538,  0.01145,    0.217,   0.0622, 01 Aug 2016 12:16:50\r\n'
  // note: leading # in syncmode '# 20.6...'
  // note: picks up trailing S> prompt if not in syncmode
  p0 = strtok(utlBuf, "\r\n#");
  // p0 = {crlf#}...{crlf}  e.g. one line
  // skip to 3rd value. Xtemp,X Xcond,X pres
  p1 = strtok(p0, "#, ");
  if (!p1) return;
  p1 = strtok(NULL, ", "); 
  if (!p1) return;
  p1 = strtok(NULL, ", ");
  if (!p1) return;
  pres = atof( p1 );
  // ctd.log is for autonomous getsample, see ctdAuto
  // len = strlen(utlBuf);
  // if (write(ctd.log, utlBuf, len)<len) 
  //   flogf("\nERR\t| ctdRead log fail");
  ctd.depth = pres;
  return;
} // ctdRead

///
// data read recently
bool ctdFresh(void) {
  return (time(0)-ctd.time)<ctd.fresh;
}

///
// tmrOn ? ctdPending. tmrExp ? err
bool ctdPending(void) {
  if (ctd.auton || tmrOn(ctd_tmr))
    return true;
  if (tmrExp(ctd_tmr))
    utlErr(ctd_err, "ctd timer expired");
  return false;
}

///
// sample if not ctdPending, wait for data, return depth
float ctdDepth(void) {
  if (!ctdData() && ctdFresh())
    return ctd.depth;
  while (!ctdData())
    if (!ctdPending())
      ctdSample();
  ctdRead();
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
  tmrStop(ctd_tmr);
  ctd.auton = auton;
} // ctdAuton

///
// get science, clear log
void ctdGetSamples(void) {
  int len1=BUFSZ;
  int len2=len1, len3=len1;
  DBG0("ctdGetSamples(%s)", ctd.logFile)
  ctd.log = utlLogFile(ctd.logFile);
  ctdPrompt();          // wakeup
  utlWrite(ctd.port, "GetSamples:", EOL);
  while (len1==len3) {
    // repeat until less than a full buf
    len2 = (int) TURxGetBlock(ctd.port, utlBuf, (long) len1, (short) 1000);
    len3 = write(ctd.log, utlBuf, len2);
    if (len2!=len3) 
      flogf("\nERR\t| ctdGetSamples() could not write ctd.log");
  } // while ==
  close(ctd.log);
  utlWrite(ctd.port, "initLogging", EOL);
  utlWrite(ctd.port, "initLogging", EOL);
  utlRead(ctd.port,utlBuf);
  DBG2("\nctd>>%s", utlBuf)
} // ctdGetSamples

