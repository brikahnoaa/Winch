// ctd.c sbe16
// 
// ctd.c
// handling CTD in buoy, seabird SBE 16plus
// general note: ctd wants \r only for input
// Bug:! TUTxPrint translates \n into \r\n, which sorta kinda works if lucky
//
#include <com.h>
#include <ctd.h>
#include <mpc.h>
#include <tmr.h>

#define EOL "\r"

CtdInfo ctd;

///
// buoy sbe16 set date, sync mode
// pre: mpcInit sets up serial
// sets: ctd.port .pending .log
void ctdInit(void) {
  DBG0("ctdInit()")
  ctd.port = mpcCom1Port();
  mpcDevice(ctd_dev);
  ctd.pending = false;
  DBG1("%d", ctd.delay)
  // set up HW
  ctdBreak();
  if (!(ctdPrompt() || ctdPrompt()))   // fails twice 
    utlShutdown("\nERR\t|ctdInit(): no prompt from ctd");
  if (ctd.log==0)
    ctd.log = open(ctd.logFile, O_APPEND | O_CREAT | O_RDWR);
  if (ctd.log<=0) 
    utlShutdown("\nERR\t| ctdInit(): logfile open fail");
  // ctdSetDate();
  ctdSyncmode();
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
  char buffer[16];
  DBG0("ctdSetDate()")
  //
  time(&rawtime);
  info = gmtime(&rawtime);
  //	strftime(buffer, 15, "%m%d%Y%H%M%S", info);
  sprintf(buffer, "datetime=%02d%02d%04d%02d%02d%02d", 
    info->tm_mon+1, info->tm_mday, info->tm_year + 1900, 
    info->tm_hour, info->tm_min, info->tm_sec);
  DBG1("%s", buffer)
  //
  utlWrite(ctd.port, buffer, EOL);
  utlReadWait(ctd.port, scratch, 1);
  utlWrite(ctd.port, "syncwait=0", EOL);
  utlReadWait(ctd.port, scratch, 1);
  utlWrite(ctd.port, "DelayBeforeSampling=0", EOL);
  utlReadWait(ctd.port, scratch, 1);
} // ctdSetDate


///
// sbe16
// \r input, echos input.  \r\n before next output.
// pause 0.33s between \rn and s> prompt.
// wakeup takes 1.045s, writes extra output "SBE 16plus\r\nS>"
// ctdPrompt - poke buoy CTD, look for prompt
bool ctdPrompt(void) {
  static char str[32];
  DBG0("ctdPrompt()")
  if (ctd.syncmode) 
    ctdBreak();
  TURxFlush(ctd.port);
  TUTxPutByte(ctd.port, '\r', true);
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
// sets: ctd.syncmode
void ctdSyncmode(void) {
  DBG0("ctdSyncmode()")
  utlWrite(ctd.port, "Syncmode=y", EOL);
  ctdPrompt();
  utlWrite(ctd.port, "QS", EOL);
  utlDelay(100);
  TURxFlush(ctd.port);
  ctd.syncmode = true;
} 

///
// how to exit sync mode
void ctdBreak(void) {
  DBG0("ctdBreak()")
  TUTxBreak(ctd.port, 5000);
  ctd.syncmode = false;
} // ctdBreak

///
// false if not pending, true if Q(), false and retry if timeout
// sets: ctd.pending
bool ctdReady() {
  if (!ctd.pending)
    return false;
  if (TURxQueuedCount(ctd.port))
    return true;
  // retry if timeout
  if (tmrExp(ctd_tmr)) {
    flogf("\nWARN\t|ctdReady() timeout, retry");
    ctd.pending = false;      // ctdSample needs to see pending false
    ctdSample();
  }
  return false;
} // ctdReady

///
// poke ctd to get sample, set interval timer
// pause between ts\r\n and result = 4.32s
// sets: ctd.pending ctd_tmr
void ctdSample(void) {
  int len;
  DBG0("ctdSample()")
  if (ctd.pending) return;
  if (ctd.syncmode) TUTxPutByte(ctd.port, '\r', true);
  else {
    utlWrite(ctd.port, "TS", EOL);
    // consume echo
    len = utlReadWait(ctd.port, scratch, 1);
    if (len<3) 
      flogf("\nERR ctdSample, TS command fail");
  }
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
  char stringin[BUFSZ], stringout[BUFSZ];
  DBG0("ctdData()")
  stringout[0] = 0;   // in case of error return
  if (ctd.pending) 
    ctd.pending = false;
  else
    return;           // error
  // waits up to max+1 seconds - best called after tgetq()
  len = utlReadWait(ctd.port, stringin, ctd.delay+1);
  if (len==0) {       // error
    ctd.depth = 0;
    return;
  }
  DBG2("\n\tctd-->%s", utlNonPrint(stringin))
  // Temp, conductivity, depth, fluromtr, PAR, salinity, time
  // ' 20.6538,  0.01145,    0.217,   0.0622, 01 Aug 2016 12:16:50\r\n'
  // note: leading # in syncmode '# 20.6...'
  // note: picks up trailing S> prompt if not in syncmode
  temp = atof( strtok(stringin, "\r\n#, "));
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
  sprintf(stringout, 
    "%.4f %.5f %.3f %.4f %.4f %.4f %s",
    temp, cond, pres, flu, par, sal, utlTimeDate());
  DBG1("\n\tctd: %s", stringout)
  // Log WriteString
  len = strlen(stringout);
  if (write(ctd.log, stringout, len)<len) 
    flogf("\nERR\t|ctdData log fail");
  ctd.depth = pres;
  if (ctd.pending)
    ctd.pending = false;
  return;
} // ctdData

///
// ?? turn off auton mode if on
void ctdStop(void){
  close(ctd.log);
  ctd.pending = false;
  ctd.depth = 0;
}
