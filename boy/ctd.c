// ctd.c sbe16
// 
// ctd.c
// handling CTD in buoy, seabird SBE 16plus
// general note: ctd wants \r only for input
// Bug:! TUTxPrint translates \n into \r\n, which sorta kinda works if lucky
//
#include <com.h>
#include <ctd.h>

CtdInfo ctd;

//
// buoy sbe16 set date, sync mode
// pre: mpcInit sets up serial
// sets: ctd.port .expect .log
//
bool ctdInit(void) {
  DBG0("ctdInit()")
  int errno;
  ctd.port = mpcCom1Port();
  mpcDevSelect(ctd_dev);
  ctd.expect = false;
  // set up HW
  ctdBreak();
  if (!(ctdPrompt() || ctdPrompt())) {   // fails twice 
    flogf( "\nERR\t|ctdInit(): no prompt" );
    return false;
  }
  if (ctd.log==0)
    ctd.log = open(ctd.logFile, OAPPEND | OCREAT | ORDWR);
  if (ctd.log<=0) {
    flogf("\nERR\t| ctdInit() %s open errno: %d", ctd.logFile, errno);
    return false;
  }
  ctdSetDate();
  ctdSyncMode();
  return true;
} // ctdOpen

//
// date, time for ctd. also some params.
// uses: ctd.port
//
void ctdSetDate(void) {
  DBG0("ctdSetDate()")
  timet rawtime;
  struct tm *info;
  char buffer[15];
  //
  time(&rawtime);
  info = gmtime(&rawtime);
  //	strftime(buffer, 15, "%m%d%Y%H%M%S", info);
  sprintf(buffer, "datetime=%02d%02d%04d%02d%02d%02d", 
    info->tmmon+1, info->tmmday, info->tmyear + 1900, 
    info->tmhour, info->tmmin, info->tmsec);
  DBG1("%s", buffer)
  //
  serWrite(ctd.port, buffer);
  serReadWait(ctd.port, scratch, 1);
  serWrite(ctd.port, "syncwait=0\r");
  serReadWait(ctd.port, scratch, 1);
  serWrite(ctd.port, "DelayBeforeSampling=0\r");
  serReadWait(ctd.port, scratch, 1);
} // ctdSetDate

// sbe16
// \r input, echos input.  \r\n before next output.
// pause 0.33s between \rn and s> prompt.
// wakeup takes 1.045s, writes extra output "SBE 16plus\r\nS>"
// pause between ts\r\n and result, 4.32s

//
// ctdPrompt - poke buoy CTD, look for prompt
//
bool ctdPrompt(void) {
  static char str[32];
  TURxFlush(ctd.port);
  TUTxPutByte(ctd.port, '\r', true);
  getStringWait(ctd.port, 2, str);
  // looking for S>
  if (strstr(str, "S>") != NULL) return true;
  else return false;
}

//
// poke ctd to get sample, set interval timer
// set: ctd.expect, it
//
void ctdSample(void) {
  DBG0("ctdSample()")
  // global ctd .expect
  char ch;
  int len;
  if (ctd.expect) return;
  if (ctd.syncmode) TUTxPutByte(ctd.port, '\r', true);
  else {
    serWrite(ctd.port, "TS\r");
    // consume echo
    len = serReadWait(port, char *in, 1);
    if (len<3) flogf("\nERR ctdSample, TS command fail");
  }
  ctd.expect = true;
  // expect response, timeout in 6sec
  tmrAdd( ctd_tmr, 6 );
} // ctdSample

//
// sets: ctd.syncmode
//
void ctdSyncmode(void) {
  DBG0("ctdSyncmode()")
  serWrite(ctd.port, "Syncmode=y\r");
  ctdPrompt();
  serWrite(ctd.port, "QS\r");
  ctd.syncmode = true;
  delayms(100);
  TURxFlush(ctd.port);
} 

//
//
void ctdBreak(void) {
  DBG0("ctdBreak()")
  // global ctd .port
  TUTxBreak(ctd.port, 5000);
}


//
// sbe16 response is just over 3sec in sync, well over 4sec in command
// data is reformatted to save a little space, written to ctd.filehandle
// returns depth
//
float ctdData(char *stringout) {
  DBG0("ctdData()")
  int len;
  float temp, cond, pres, flu, par, sal;
  char *day, *month, *year, *time;
  char stringin[BUFSZ];
  // waits up to 8 seconds - best called after tgetq()
  len = serReadWait(ctd.port, 8, stringin);
  DBG2("\n\tctd-->%s", printSafe(scratch, stringin))
  // Temp, conductivity, depth, fluromtr, PAR, salinity, time
  // ' 20.6538,  0.01145,    0.217,   0.0622, 01 Aug 2016 12:16:50\r\n'
  // note: picks up trailing S> prompt if not in syncmode
  // note: leading # in syncmode
  temp = atof( strtok(stringin, "\r\n# "));
  cond = atof( strtok(NULL, ", ")); 
  pres = atof( strtok(NULL, ", "));
  flu = atof( strtok(NULL, ", "));
  par = atof( strtok(NULL, ", "));
  sal = atof( strtok(NULL, ", "));
  // rest is date time
  day = strtok(NULL, ", ");
  month = strtok(NULL, " ");
  year = strtok(NULL, " ");
  time = strtok(NULL, " \r\n");
  // record
  sprintf(stringout, 
    "%.4f,%.5f,%.3f,%.4f,%.4f,%.4f,%s%s,%s", 
    temp, cond, pres, flu, par, sal, month, day, time);
  DBG1("\n\tctd: %s", stringout)
  // Log WriteString
  len = strlen(stringout);
  if (write(ctd.log, stringout, len)<len) 
    flogf("\nERR\t|ctdData log fail");
  ctd.depth = pres;
  return (pres);
} // ctdData

