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

///
// buoy sbe16 set date
// pre: mpcInit sets up serial
// sets: ctd.port .pending .log
void ctdInit(void) {
  DBG0("ctdInit()")
  ctd.port = mpcCom1();
  mpcDevice(ctd_dev);
  // set up HW
  ctdBreak();
  if (!(ctdPrompt() || ctdPrompt()))   // fails twice 
    utlStop("ERR\t| ctdInit(): no prompt from ctd");
  ctdSetDate();
} // ctdInit

void ctdFlush(void){
  if (ctd.pending)
    ctdData();
  PZCacheFlush(C_DRV);
} // ctdFlush

///
// date, time for ctd. also some params.
// uses: ctd.port
void ctdSetDate(void) {
  time_t rawtime;
  struct tm *info;
  DBG0("ctdSetDate()")
  //
  time(&rawtime);
  info = gmtime(&rawtime);
  //	strftime(utlBuf, 15, "%m%d%Y%H%M%S", info);
  sprintf(utlBuf, "datetime=%02d%02d%04d%02d%02d%02d", 
    info->tm_mon+1, info->tm_mday, info->tm_year + 1900, 
    info->tm_hour, info->tm_min, info->tm_sec);
  DBG1("%s", utlBuf)
  //
  utlWrite(ctd.port, utlBuf, EOL);
  utlReadWait(ctd.port, utlBuf, 1);
  utlWrite(ctd.port, "DelayBeforeSampling=0", EOL);
  utlReadWait(ctd.port, utlBuf, 1);
} // ctdSetDate

///
// turn autonomous on/off
CtdModeType ctdMode(CtdModeType mode) {
  char *out;
  int len1=BUFSZ;
  int len2=len1, len3=len1;
  if (ctd.mode==mode) return mode;
  DBG0("ctdMode(%d)", mode)
  mpcDevice(ctd_dev);
  switch (mode) {
  case auto_ctd:
    out = "\n initlogging \n initlogging \n"
          "sampleInterval=0 \n txRealTime=n \n startnow \n";
    utlWriteLines(ctd.port, out, EOL);
    break;
  case idle_ctd:
    utlWrite(ctd.port, "stop", EOL);
    // ?? pause and flush
    utlNap(2);
    TURxFlush(ctd.port);
    // science log
    ctd.log = utlLogFile(ctd.logFile);
    utlWrite(ctd.port, "getSamples", EOL);
    while (len1==len3) {
      // repeat until less than a full buf
      len2 = (int) TURxGetBlock(ctd.port, utlBuf, (long) len1, (short) 100);
      len3 = write(ctd.log, utlBuf, len2);
      if (len2!=len3) 
        flogf("\nERR\t| ctdMode() could not write ctd.log");
    } // while ==
    close(ctd.log);
    break;
  } // switch
  ctd.mode = mode;
  mpcDevice(ant_dev);
  return ctd.mode;
}

///
// sbe16
// \r input, echos input.  \r\n before next output.
// pause 0.33s between \rn and s> prompt.
// wakeup takes 1.045s, writes extra output "SBE 16plus\r\nS>"
// ctdPrompt - poke buoy CTD, look for prompt
bool ctdPrompt(void) {
  static char str[32];
  DBG0("ctdPrompt()")
  TURxFlush(ctd.port);
  utlWrite(ctd.port, "", EOL);
  utlReadWait(ctd.port, str, 2);
  // looking for S>
  if (strstr(str, "S>") == NULL) {
    // try again after break
    ctdBreak();
    TURxFlush(ctd.port);
    TUTxPutByte(ctd.port, '\r', true);
    utlReadWait(ctd.port, str, 2);
    // looking for S>
    if (strstr(str, "S>") == NULL) {
      flogf("\nERR\t| ctdPrompt fail");
      return false;
    }
  }
  return true;
}

///
// reset, exit sync mode
void ctdBreak(void) {
  DBG0("ctdBreak()")
  TUTxBreak(ctd.port, 5000);
} // ctdBreak

///
// poke ctd to get sample, set interval timer
// pause between ts\r\n and result = 4.32s
// sets: ctd.pending ctd_tmr
void ctdSample(void) {
  int len;
  DBG0("ctdSample()")
  if (ctd.pending) return;
  if (ctd.mode==auto_ctd)
    utlWrite(ctd.port, "getlastsamples", EOL);
  else {
    ctdPrompt();
    utlWrite(ctd.port, "TS", EOL);
  }
  // consume echo
  len = utlReadWait(ctd.port, utlBuf, 1);
  if (len<3) 
    flogf("\nERR ctdSample, TS command fail");
  ctd.pending = true;
  // pending response, timeout in 5sec, checked by ctdReady()
  tmrStart( ctd_tmr, ctd.delay );
} // ctdSample

///
// sample if not pending, wait for data, return depth
float ctdDepth() {
  if (!ctd.pending)
    ctdSample();
  ctdData();
  return ctd.depth;
} // ctdDepth

///
// sbe16 response is just over 3sec in sync, well over 4sec in command
// waits up to ctd.delay+1 seconds - good to call after tgetq()
// data is reformatted to save a little space, written to ctd.log
// logs: reformatted data string
// sets: ctd.depth .pending 
void ctdData() {
  int len;
  float temp, cond, pres, flu, par, sal;
  DBG0("ctdData()")
  if (ctd.pending) 
    ctd.pending = false;
  else
    return;           // error
  // waits up to max+1 seconds - best called after tgetq()
  len = utlReadWait(ctd.port, utlBuf, ctd.delay+1);
  if (len==0) {       // error
    flogf("\nERR\t| ctdData() no response in %d sec", ctd.delay+1);
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
    temp, cond, pres, flu, par, sal, utlTimeDate());
  // ctd.log is for autonomous getsample, see ctdMode
  // len = strlen(utlBuf);
  // if (write(ctd.log, utlBuf, len)<len) 
  //   flogf("\nERR\t| ctdData log fail");
  ctd.depth = pres;
  return;
} // ctdData

///
// close log
void ctdStop(void){
  if (ctd.log)
    close(ctd.log);
}
