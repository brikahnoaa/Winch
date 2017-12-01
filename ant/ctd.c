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

bool ctdInit() {
  // global ctd .off .port .pending
  DBG0("\n\t|. ctdInit")
  if (ctd.off) {
    ctd.port = TUOpen(DEVICERX, DEVICETX, CTDBAUD, 0);
    if (ctd.port) ctd.off = false;
    else flogf( "\nERR ctdInit(): fail TUOpen" );
  }
  ctdBreak(ctd.port);
  if (!ctdPrompt(stringin)) {
    DBG2(" fail start getprompt")
    ctdBreak();
    DevSelect(sbe);
    DelayADLog(3);
    if (!ctdPrompt(stringin)) {
      DBG2(" fail start1 getprompt")
      if (sbe==DEVA) DevSelect(DEVX);
      DelayADLog(1);
      DevSelect(sbe);
      ctdBreak();
      DelayADLog(3);
      if (!ctdPrompt(stringin)) {
        DBG2(" fail start2 getprompt")
      }
    } 
  }
  ctdSetDate();
  // ctdSyncmode();
  return true;
} // ctdInit



/*
 */
void ctdSetDate() {

  timet rawtime;
  struct tm *info;
  char buffer[15];
  DBG1("\n\t|ctdSetDate()")
  time(&rawtime);

  info = gmtime(&rawtime);

  //	strftime(buffer, 15, "%m%d%Y%H%M%S", info);
  sprintf(buffer, "%02d%02d%04d%02d%02d%02d", info->tmmon+1, info->tmmday,
          info->tmyear + 1900, info->tmhour, info->tmmin, info->tmsec);
  printf("\nctdSetDate(): %s\n");

  TUTxPrintf(ctd.port, "DATETIME=%s\r", buffer);
  Delayms(250);
  while (tgetq(ctd.port))
    cprintf("%c", TURxGetByte(ctd.port, true));

} // ctdSetDate() 

// sbe16
// \r input, echos input.  \r\n before next output.
// pause 0.33s between \rn and s> prompt.
// wakeup takes 1.045s, writes extra output "SBE 16plus\r\nS>"
// pause between ts\r\n and result, 4.32s

/*
 * ctdPrompt - poke buoy CTD, look for prompt
 */
bool ctdPrompt() {
  static char str[32];
  TURxFlush(ctd.port);
  TUTxPutByte(ctd.port, '\r', true);
  getStringWait(ctd.port, 2, str);
  // looking for S>
  if (strstr(str, "S>") != NULL) return true;
  else return false;
}

/*
 */
void ctdSample() {
  char ch;
  DBG1("\n . ctdSample")
  //if (SyncMode) {
    //TUTxPrintf(ctd.port, "+\r");
    //Delayms(20);
    //TURxFlush(ctd.port);
  //} else {
  // TUTxPrintf(ctd.port, "TS\r");
  TUTxPutByte(ctd.port, 'T', true);
  ch=TURxGetByteWithTimeout(ctd.port, 10);
  TUTxPutByte(ctd.port, 'S', true);
  ch=TURxGetByteWithTimeout(ctd.port, 10);
  TUTxPutByte(ctd.port, '\r', true);
  ch=TURxGetByteWithTimeout(ctd.port, 10);
  ch=TURxGetByteWithTimeout(ctd.port, 10);
  //}
} // ctdSample

/*
 */
void ctdSyncmode() {
  // global ctd .port .syncmode
  DBG0("\n\t|ctdSyncmode()")
  ctdBreak();
  TUTxPrintf(ctd.port, "Syncmode=y\r");
  ctdPrompt();
  TUTxPrintf(ctd.port, "QS\r");
  ctd.syncmode = true;
  Delayms(100);
  TURxFlush(ctd.port);
} 

/*
 */
void ctdBreak() {
  // global ctd .port .syncmode
  TUTxBreak(ctd.port, 5000);
  ctd.syncmode = false;
}


/*
 * bool ctdData() 
 * Temp, conductivity, depth, fluromtr, PAR, salinity, time
 * 16.7301,  0.00832,    0.243, 0.0098, 0.0106,   0.0495, 14 May 2017 23:18:20
 * The response is approximately 2 sec.
 * waits up to 8 seconds - best called after tgetq()
 */
bool ctdData() {
  // global stringin CTDLogFile
  char *strin;  // pointer into stringin
  int filehandle;
  int i=0;
  int byteswritten, month;
  long len;
  char *splittemp, *splitcond, *splitpres;
  char *splitflu, *splitpar, *splitsal;
  float temp, cond, pres, flu, par, sal;
  char *splitdate, *mon;
  struct tm info;
  timet secs = 0;
  DBG0("\n. ctdData()")

  memset(stringin, 0, BUFSZ);

  // waits up to 8 seconds - best called after tgetq()
  len = getStringWait(stringin, (short) 8000);
  DBG( printsafe(len, stringin);)

  TURxFlush(ctd.port);
  strin=stringin;

  memset(stringout, 0, BUFSZ);
  // Split data string up into separate values
  // Example: # 20.6538,  0.01145,    0.217,   0.0622, 01 Aug 2016 12:16:50
  // Example: 20.6538,  0.01145,    01 Aug 2016, 12:16:50
  splittemp = strtok(strin, "\r\n# ");
  splitcond = strtok(NULL, ", "); 
  splitpres = strtok(NULL, ", ");
  splitflu = strtok(NULL, ", ");
  splitpar = strtok(NULL, ", ");
  splitsal = strtok(NULL, ", ");
  splitdate = strtok(NULL, "\r\n"); 

  temp = atof(splittemp);
  cond = atof(splitcond);
  pres = atof(splitpres);
  flu = atof(splitflu);
  par = atof(splitpar);
  sal = atof(splitsal);
  sprintf(stringout, "%.4f,%.5f,%.3f,%.4f,%.4f,%.4f", 
    temp, cond, pres, flu, par, sal);
  } else { // antMod sbe
    // Example: # 20.6538,  0.217,   01 Aug 2016 12:16:50
    splittemp = strtok(strin, "\r\n#, ");
    splitpres = strtok(NULL, ", ");
    splitdate = strtok(NULL, "\r\n"); 

    temp = atof(splittemp);
    pres = atof(splitpres);
    sprintf(stringout, "%.4f,%.3f", temp, pres);
  }
  LARA.DEPTH = pres;
  // LARA.TEMP = temp; //??
  DBG1("\nctd->%s", stringout)

  // buoy sbe vvvvv
  // convert date time to secs
  info.tmmday = atoi(strtok(splitdate, ", "));
  mon = strtok(NULL, " ");
  info.tmyear = (atoi(strtok(NULL, " ,")) - 1900);
  info.tmhour = atoi(strtok(NULL, ":"));
  info.tmmin = atoi(strtok(NULL, ":"));
  info.tmsec = atoi(strtok(NULL, " "));

  info.tmmon = -1;
  if (strstr(mon, "Jan") != NULL)
    info.tmmon = 0;
  else if (strstr(mon, "Feb") != NULL)
    info.tmmon = 1;
  else if (strstr(mon, "Mar") != NULL)
    info.tmmon = 2;
  else if (strstr(mon, "Apr") != NULL)
    info.tmmon = 3;
  else if (strstr(mon, "May") != NULL)
    info.tmmon = 4;
  else if (strstr(mon, "Jun") != NULL)
    info.tmmon = 5;
  else if (strstr(mon, "Jul") != NULL)
    info.tmmon = 6;
  else if (strstr(mon, "Aug") != NULL)
    info.tmmon = 7;
  else if (strstr(mon, "Sep") != NULL)
    info.tmmon = 8;
  else if (strstr(mon, "Oct") != NULL)
    info.tmmon = 9;
  else if (strstr(mon, "Nov") != NULL)
    info.tmmon = 10;
  else if (strstr(mon, "Dec") != NULL)
    info.tmmon = 11;

  // need better sanity checks
  if (info.tmmon == -1) {
    flogf("\nERROR|ctdData(): month %s incorrect. ", mon);
    flogf("\n\t|''%s''", stringin);
    return false;
  } 
  else month = info.tmmon + 2;

  memset(stringin, 0, 32);
  sprintf(stringin, ",%d/%d/%d,%02d:%02d:%02d", month, info.tmmday,
          info.tmyear - 100, info.tmhour, info.tmmin, info.tmsec);
  strcat( stringout, stringin );
  DBG1("%s", stringin)

  secs = mktime(&info);

  // if antMod DEVA, we are done, no log
  if (sbeID==DEVA) { return true; }

  // Log WriteString
  filehandle = open(CTDLogFile, OAPPEND | OCREAT | ORDWR);
  if (filehandle <= 0) {
    flogf("\nERROR  |ctdlogfile '%s' fd %d", CTDLogFile, filehandle);
    flogf("\nERROR  |ctdLogger() %s open errno: %d", CTDLogFile, errno);
    return false;
  }
  byteswritten = write(filehandle, stringout, strlen(stringout));
  DBG2("\nBytes Written: %d", byteswritten)
  if (close(filehandle) != 0) {
    flogf("\nERROR  |ctdLogger: File Close error: %d", errno);
    return false;
  }
  // this incr looks strange, but lara.ctd is not ctdsamples, not part of averaging
  LARA.CTDSAMPLES++;
  if (LARA.BUOYMODE != 0) ctdVertVel(secs); // ??
  return true;
} // ctdData

