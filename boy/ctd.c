/* 
 * ctd.c
 * handling CTD in buoy, seabird SBE 16plus
 * general note: ctd wants \r only for input
 * Bug:! TUTxPrint translates \n into \r\n, which sorta kinda works if lucky
 */
#include <common.h>
#include <ctd.h>

// syncMode pending delay port
CtdInfo ctd = {
  0, 0, 3.5, NULL
};

bool ctdOpen(void) {
  DBG0("ctdOpen()")
  // global ctd .on .port .pending
  if (!ctd.on) {
    ctd.port = TUOpen(DEVICERX, DEVICETX, CTDBAUD, 0);
    if (ctd.port==NULL) { 
      flogf( "\nERR ctdOpen(): fail TUOpen" );
      return false;
    }
  }
  ctdBreak(ctd.port);
  if (!ctdPrompt() && !ctdPrompt()) {   // try twice
    flogf( "\nERR ctdOpen(): no prompt" );
    return false;
  }
  
  ctd.filehandle = open(ctd.filename, OAPPEND | OCREAT | ORDWR);
  if (ctd.filehandle <= 0) {
    flogf("\nERR %s open errno: %d", ctd.filename, errno);
    return false;
  }
  ctd.on = true;
  ctd.pending = false;
  ctdSetDate();
  ctdSyncMode();
  return true;
} // ctdOpen

/*
 * date, time for ctd. also some params.
 */
void ctdSetDate(void) {
  DBG0("ctdSetDate()")
  // global ctd .port
  timet rawtime;
  struct tm *info;
  char buffer[15];
  
  time(&rawtime);
  info = gmtime(&rawtime);
  //	strftime(buffer, 15, "%m%d%Y%H%M%S", info);
  sprintf(buffer, "datetime=%02d%02d%04d%02d%02d%02d", 
    info->tmmon+1, info->tmmday, info->tmyear + 1900, 
    info->tmhour, info->tmmin, info->tmsec);
  flogf("\n\t|ctdSetDate(): %s\n", buffer);

  serWrite(ctd.port, buffer);
  serReadWait(ctd.port, scratch, 1);
  serWrite(ctd.port, "syncwait=0\r");
  serReadWait(ctd.port, scratch, 1);
  serWrite(ctd.port, "DelayBeforeSampling=0\r");
  serReadWait(ctd.port, scratch, 1);

} // ctdSetDate() 

// sbe16
// \r input, echos input.  \r\n before next output.
// pause 0.33s between \rn and s> prompt.
// wakeup takes 1.045s, writes extra output "SBE 16plus\r\nS>"
// pause between ts\r\n and result, 4.32s

/*
 * ctdPrompt - poke buoy CTD, look for prompt
 */
bool ctdPrompt(void) {
  static char str[32];
  TURxFlush(ctd.port);
  TUTxPutByte(ctd.port, '\r', true);
  getStringWait(ctd.port, 2, str);
  // looking for S>
  if (strstr(str, "S>") != NULL) return true;
  else return false;
}

/*
 * poke ctd to get sample, set interval timer
 * set: ctd.pending, it
 */
void ctdSample(void) {
  DBG0("ctdSample()")
  // global ctd .pending
  char ch;
  int len;

  if (ctd.syncmode) TUTxPutByte(ctd.port, '\r', true);
  else {
    serWrite(ctd.port, "TS\r");
    // consume echo
    len = serReadWait(port, char *in, 1);
    if (len<3) flogf("\nERR ctdSample, TS command fail");
  }
  ctd.pending = true;
  // expect response in 6sec
  itAdd( Ctd_it, 6 );
} // ctdSample

/*
 */
void ctdSyncmode(void) {
  DBG0("ctdSyncmode()")
  // global ctd .port .syncmode
  serWrite(ctd.port, "Syncmode=y\r");
  ctdPrompt();
  serWrite(ctd.port, "QS\r");
  ctd.syncmode = true;
  Delayms(100);
  TURxFlush(ctd.port);
} 

/*
 */
void ctdBreak(void) {
  DBG0("ctdBreak()")
  // global ctd .port
  TUTxBreak(ctd.port, 5000);
}


/*
 * sbe16 response is just over 3sec in sync, well over 4sec in command
 * data is reformatted to save a little space, written to ctd.filehandle
 * returns depth
 */
float ctdData(char *stringout) {
  DBG0("ctdData()")
  // global scratch, ctd .depth .pending .filehandle, boy .port
  int len;
  float temp, cond, pres, flu, par, sal;
  char *day, *month, *year, *time;
  char stringin[BUFSZ];

  if (ctd.off) ctdOpen();

  // waits up to 8 seconds - best called after tgetq()
  len = serReadWait(boy.port, 8, stringin);
  DBG2("ctd-->%s", printSafe(scratch, stringin))

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

  sprintf(stringout, 
    "%.4f,%.5f,%.3f,%.4f,%.4f,%.4f,%s%s,%s", 
    temp, cond, pres, flu, par, sal, month, day, time);
  flogf("ctd: %s\n", stringout);

  // Log WriteString
  len = strlen(stringout);
  if (write(ctd.filehandle, stringout, len)<len) {
    flogf("\nERR ctdData log fail");
  }

  ctd.depth = pres;
  return (pres);
} // ctdData

