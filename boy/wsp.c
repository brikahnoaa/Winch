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
  if (strlen(wsp.logFile))
    wsp.log = utlLogFile(wsp.logFile);
} // wspInit

///
// turn on, disk free > wsp.freeMin
// sets: wsp.card
int wspStart(int card) {
  DBG0("wspStart()")
  mpcPamDev(wsp1_pam);
  mpcPamPulse(WISPR_PWR_OFF);
  mpcPamDev(wsp2_pam);
  mpcPamPulse(WISPR_PWR_OFF);
  mpcPamDev(wsp3_pam);
  mpcPamPulse(WISPR_PWR_OFF);
  // select, power on
  mpcPamDev(card);
  mpcPamPulse(WISPR_PWR_ON);
  wsp.card =  card;
  // expect df output
  // utlExpect(wsp.port, utlBuf, "/mnt", 40);
  // ?? check free disk, maybe increment card
  // flogf("\n%s\n", utlBuf);
  return card;
} // wspStart

///
// stop current card
void wspStop(void) {
  mpcPamPulse(WISPR_PWR_OFF);
  utlDelay(100);
  wsp.card = null_pam;
  mpcPamDev(null_pam);
  if (wsp.log) {
    close(wsp.log);
    wsp.log = 0;
  }
} // wspStop

///
// wsp storm check started. interact.
int wspStorm(char *buf) {
  DBG0("wspStorm()")
  utlExpect(wsp.port, buf, "RDY", 200);
  utlWrite(wsp.port, "$WS?*", EOL);
  utlReadWait(wsp.port, buf, 10);
  flogf("\nwspStorm prediction: %s", buf);
  return 0;
} // wspStorm

///
// ?? replace with log to wsp.log
void wspLog(char *str) {
  flogf("%s\n", str);
} // wspLog

///
// test routine, run detection for minutes
int wspDetectMin(int minutes, int *detect) {
  int r;
  tmrStart(minute_tmr, minutes*60);
  while (!tmrExp(minute_tmr)) {}
  r = wspQuery(detect);
  wspExit();
  return r;
} // wspDetectMin

///
// run detection for %wsp.dutyC of remainder of this hour
int wspDetectHour(int *detect) {
  struct tm *tim;
  time_t secs;
  int min, hour=60;
  //
  time(&secs);
  tim = localtime(&secs);
  // tim->tm_hour tim->tm_min tim->tm_sec
  // rest until duty*minutes in 2nd part of hour
  min = hour-wsp.duty-tm_min;
  utlNap(min*60);
  min=wsp.duty; 
  while (min>0) {
    if (min>wsp.detInt) 
      utlNap(detInt*60);
    else
      utlNap(min*60);
    wspQuery(&detections);
    wspLog(detections);
    min-=wsp.detInt;
  } // duty
} // wspDetectHour

///
// log up to .detMax detections every .query minutes
// while .duty% * .hour minutes
// return: 0 no err, 1 disk space, 2 no response, 3 bad DXN
// uses: .day .day1 .duty .hour .detInt
// sets: *detections
int wspDetectDay(int *detections) {
  // float free;
  long dayM, dutyM, dayS, hourS, dutyS, queryS;
  int day=0, cycleCnt=1, detTotal=0, det=0, r=0;  // r==0 means no err
  enum {day_tmr, hour_tmr, duty_tmr, query_tmr};
  if (boyCycle()==1) 
    day = wsp.day1;
  else
    day = wsp.day;
  dayM = day * wsp.hour;
  dutyM = (int) wsp.hour*wsp.duty/100; // (60, 50)
  dayS = dayM*60;
  hourS = wsp.hour*60;
  dutyS = dutyM*60;
  queryS = wsp.detInt*60;
  flogf("\nwspDetect()\t| day=%dh, hour=%dm, duty=%d%%, detInt=%dm",
    day, wsp.hour, wsp.duty, wsp.detInt);
  flogf("\nsecs %ld %ld %ld %ld %s", 
    dayS, hourS, dutyS, queryS, utlTime());
  // check disk space
  // if (wspSpace(&free)) r = 2;     // fail
  // if (free*wsp.cfSize<wsp.freeMin) r = 1;
  // DBG2("\nfree: %3.1f GB: %3.1f err: %d\n", free, free*wsp.cfSize, r)
  // while no err and tmr
  tmrStart(day_tmr, dayS);
  // day
  while (!r) {
    if (cgetq() && cgetc()=='q') { r = 5; continue; }
    if (tmrQuery(day_tmr)<hourS) continue;
    tmrStart(hour_tmr, hourS);
    tmrStart(duty_tmr, dutyS);
    flogf("\nwspDetect\t| hour %d %s", cycleCnt++, utlTime());
    // hour
    while (!r) {
      // duty
      while (!r) {
        if (cgetq() && cgetc()=='q') { r = 5; continue; }
        if (tmrQuery(duty_tmr)<queryS) continue;
        tmrStart(query_tmr, queryS);
        // query
        while (!r) {
          if (cgetq() && cgetc()=='q') { r = 5; continue; }
          pwrNap(5);
          if (tmrExp(query_tmr)) break;
        } // while det_tmr
        // detections
        if (r) break; // ??
        r = wspQuery(&det);
        detTotal += det;
        flogf("\nwspDetect\t| detected %d %s", det, utlTime());
        // short naps avoids extra loops
        if (tmrExp(duty_tmr)) break;
      } // while duty
      flogf("\nwspDetect\t| duty cycle %s", utlTime());
      // wait until hour ends
      while (!r && !tmrExp(hour_tmr)) {
        if (cgetq() && cgetc()=='q') { r = 5; continue; }
        pwrNap(5);
      }
      break;
    } // while hour
    if (tmrExp(day_tmr)) break;
  } // while day
  if (r) tmrStopAll();       // err
  wspExit();
  *detections = detTotal;
  flogf("\nwspDetect\t| total detections %d %s", detTotal, utlTime());
  return r;
} // wspDetect

void wspExit(void) {
  utlWrite(wsp.port, "$EXI*", EOL);
  utlExpect(wsp.port, utlBuf, "FIN", 5);
} // wspEnd

///
// query detections
int wspQuery(int *det) {
  int r=0;
  char *s, query[32];
  TURxFlush(wsp.port);
  sprintf(query, "$DX?,%d*", wsp.detMax);
  utlWrite(wsp.port, query, EOL);
  utlReadWait(wsp.port, utlBuf, 16);
  wspLog(utlBuf);
  // total det
  s = strtok(utlBuf, "$,");
  if (!strstr(s, "DXN")) r = 3;
  s = strtok(NULL, ",");
  *det = atoi(s);
  DBG1("detected %d", det)
  return r;
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

