// wsp.c 
#include <utl.h>
#include <pwr.h>
#include <boy.h>
#include <wsp.h>
#include <mpc.h>
#include <tmr.h>

#define EOL "\r"

WspInfo wsp;

///
// sets: wsp.port .wspPending
void wspInit(void) {
  DBG0("wspInit()")
  wsp.port = mpcPamPort();
  PIOClear(PAM_12);
  PIOClear(PAM_34);
  tmrStop(wsp_tmr);
  mpcPamDev(wsp1_pam);
  mpcPamPulse(WISPR_PWR_OFF);
  mpcPamDev(wsp2_pam);
  mpcPamPulse(WISPR_PWR_OFF);
  mpcPamDev(wsp3_pam);
  mpcPamPulse(WISPR_PWR_OFF);
  mpcPamDev(null_pam);
  wsp.on = false;
} // wspInit

///
// turn on, disk free > wsp.freeMin
// ?? needs more checks
// sets: wsp.on .card
// rets: 0=success >=nextCard
int wspStart(int card) {
  DBG0("wspStart()")
  if (strlen(wsp.logFile))
    wsp.log = utlLogFile(wsp.logFile);
  // select, power on
  mpcPamDev(card);
  mpcPamPulse(WISPR_PWR_ON);
  // expect df output
  // utlExpect(wsp.port, utlBuf, "/mnt", 40);
  // ?? check free disk, maybe increment card
  // flogf("\n%s\n", utlBuf);
  wsp.on = true;
  return 0;
} // wspStart

///
// stop current card
void wspStop(void) {
  int i;
  // try for graceful shutdown, 3x
  if (wsp.on) {
    for (i=0;i<3;i++) {
      utlWrite(wsp.port, "$EXI*", EOL);
      if (utlExpect(wsp.port, utlBuf, "FIN", 5))
        break;
    }
    if (wsp.storm) {
      wspStorm(utlBuf);
      wspLog(utlBuf);
    }
    if (!utlExpect(wsp.port, utlBuf, "done", 10))
      flogf("\nwspStop\t| err: expected 'done', not seen...");
    wsp.on = false;
  } // wisp.on
  mpcPamPulse(WISPR_PWR_OFF);
  // wsp.card = null_pam;
  // mpcPamDev(null_pam);
  if (wsp.log) {
    close(wsp.log);
    wsp.log = 0;
  }
} // wspStop

///
// send gps, gain to wispr
int wspSetup(char *gps, int gain) {
// ??
  if (*gps) return gain;
  return 0;
} // wspSetup

///
// wsp storm check started. interact.
int wspStorm(char *buf) {
  DBG0("wspStorm()")
  if (!utlExpect(wsp.port, buf, "RDY", 200)) return 1;
  utlWrite(wsp.port, "$WS?*", EOL);
  utlReadWait(wsp.port, buf, 10);
  flogf("\nwspStorm prediction: %s", buf);
  return 0;
} // wspStorm

///
// ?? replace with log to wsp.log
void wspLog(char *str) {
  if (wsp.log) {
    sprintf(utlStr, "%s\n", str);
    write(wsp.log, utlStr, strlen(utlStr));
  } else {
    flogf("\nwspLog(%s)", str);
  }
} // wspLog

///
// test routine, run detection for minutes
int wspDetectMin(int minutes, int *detect) {
  int r;
  tmrStart(minute_tmr, minutes*60);
  while (!tmrExp(minute_tmr)) {}
  r = wspQuery(detect);
  wspStop();
  return r;
} // wspDetectMin

///
// run detection for each hour until witching hour
// rets: 0=success 1=WatchDog 11=hour.WD 12=hour.startFail 13=hour.minimum
int wspDetectDay(int *detections) {
  struct tm *tim;
  time_t secs;
  int i, r, currHr, doneHr, waitHr;
  DBG0("wspDetectDay()")
  doneHr = wsp.hour;
  // tim->tm_hour tim->tm_min tim->tm_sec
  time(&secs);
  tim = gmtime(&secs);
  currHr = tim->tm_hour;
  // wrap around math, 24hr diff
  waitHr = (24+doneHr-currHr)%24;
  flogf("\n starting wispr detection; end in hour %d", doneHr);
  flogf(", %d hours to run", waitHr);
  utlLogTime();
  tmrStart(day_tmr, waitHr*60*60);
  for (i=0;i<waitHr;i++) {
    r = wspDetectHour(detections);
    if (r) return 10+r;
    // watchdog
    if (tmrExp(day_tmr)) return 1;
  }
  return 0;
} // wspDetectDay

///
// run detection for wsp.duty minutes at end of hour
// rets: 0=success 1=watchdog 2=startFail 3=minimum
int wspDetectHour(int *detections) {
  struct tm *tim;
  time_t secs;
  int min, hour, remains, rest, duty, detect;
  DBG0("wspDetectHour()")
  min = wsp.minute;   // when not testing, min=60
  duty = wsp.duty; 
  tmrStart(hour_tmr, 60*60+60);   // hour and a minute
  // tim->tm_hour tim->tm_min tim->tm_sec
  time(&secs);
  tim = gmtime(&secs);
  hour = tim->tm_hour;
  remains = 60 - tim->tm_min;
  // need at least wsp.minimum to start/stop
  if (remains<=wsp.minimum) return 3;
  // rest first
  if (remains>duty) {
    rest = remains-duty;
    flogf("\nwispr:\t| rest for %d min", rest);
    utlLogTime();
    utlNap(rest*min);
  } else {
    // no time for full duty
    duty = remains;
  }
  // duty calls
  flogf("\nwispr:\t| run for %d min", duty);
  utlLogTime();
  if (wspStart(wsp.card)) return 2;
  while (duty>0) {
    if (duty>wsp.detInt) 
      utlNap(wsp.detInt*min);
    else
      utlNap(duty*min);
    duty -= wsp.detInt;
    wspQuery(&detect);
    flogf(" [%d]", detect);
    *detections += detect;
    // watchdog - returns err below
    if (tmrExp(hour_tmr)) break;
  } // duty
  wspStop();
  // wait until we are on to next hour
  while (hour==tim->tm_hour) {
    time(&secs);
    tim = gmtime(&secs);
    utlNap(5);
    // watchdog
    if (tmrExp(hour_tmr)) return 1;
  }
  // need to capture stats ??
  return 0;
} // wspDetectHour

///
// query detections
// rets: 0=success 1=badData
int wspQuery(int *det) {
  char *s, query[32];
  TURxFlush(wsp.port);
  *det = 0;
  sprintf(query, "$DX?,%d*", wsp.detMax);
  utlWrite(wsp.port, query, EOL);
  utlReadWait(wsp.port, utlBuf, 16);
  wspLog(utlBuf);
  // total det
  s = strtok(utlBuf, "$,");
  if (!strstr(s, "DXN")) return 1;
  s = strtok(NULL, ",");
  *det = atoi(s);
  DBG1("detected %d", det)
  return 0;
} // wspQuery

///
// wispr detection program, query disk space
int wspSpace(float *free) {
  char *s;
  *free = 0.0;
  utlWrite(wsp.port, "$DFP*", EOL);
  if (!utlReadWait(wsp.port, utlBuf, 2)) return 2;
  DBG2("%s", utlBuf)
  s = strstr(utlBuf, "DFP");
  if (!s) return 1;
  strtok(s, ",");
  s = strtok(NULL, "*");
  *free = atof(s);
  return 0;
} // wspChkCF

