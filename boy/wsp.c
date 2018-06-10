// wsp.c 
#include <utl.h>
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
  utlExpect(wsp.port, utlBuf, "/mnt", 40);
  // ?? check free disk, maybe increment card
  flogf("\n%s\n", utlBuf);
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
// wsp started. interact.
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
// log up to .detMax detections every .query minutes
// while .duty% * .cycle minutes
// return: 0 no err, 1 disk space, 2 no response, 3 bad DXN
// uses: .duty .cycle .detInt
// sets: *detections
int wspDetect(int *detections) {
  char *s, query[32];
  float free;
  int dc, det=0, r=0;  // r==0 means no err
  DBG0("wspDetect()")
  dc = (int) 60*wsp.duty/100*wsp.cycle; // (50, 60)
  DBG1("\ndutycycle %d %s", dc, utlTime())
  tmrStart(minute_tmr, dc);
  // while no err and duty cycle
  while (!r && !tmrExp(minute_tmr)) {
    // nap for a time
    utlNap(60*wsp.detInt);         // (10)
    // detections
    TURxFlush(wsp.port);
    sprintf(query, "$DX?,%d*", wsp.detMax);
    utlWrite(wsp.port, query, EOL);
    utlReadWait(wsp.port, utlBuf, 16);
    wspLog(utlBuf);
    // total det
    s = strtok(utlBuf, "$,");
    if (!strstr(s, "DXN")) r = 3;
    s = strtok(NULL, ",");
    det += atoi(s);
    DBG1("detected %d", det)
    // check disk space
    if (wspSpace(&free)) r = 2;     // fail
    if (free*wsp.cfSize<wsp.freeMin) r = 1;
    DBG2("\nfree: %3.1f GB: %3.1f err: %d\n", free, free*wsp.cfSize, r)
  } // while duty cycle
  if (r) tmrStop(minute_tmr);       // err
  utlWrite(wsp.port, "$EXI*", EOL);
  utlExpect(wsp.port, utlBuf, "FIN", 5);
  *detections = det;
  return r;
} // wspDetect

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

