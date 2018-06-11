// wsp.c 
#include <utl.h>
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
  float free;
  int phaseM, dutyM, phaseS, cycleS, dutyS, queryS;
  int cycles=0, cycleCnt=1, detTotal=0, det=0, r=0;  // r==0 means no err
  enum {phase_tmr, cycle_tmr, duty_tmr, query_tmr};
  // ?? nasty hack
  if (boyCycle()==0) return 0;
  if (boyCycle()==1) 
    cycles = wsp.cycle1;
  else
    cycles = wsp.cycles;
  phaseM = cycles * wsp.cycle;
  dutyM = (int) wsp.cycle*wsp.duty/100; // (60, 50)
  phaseS = phaseM*60;
  cycleS = wsp.cycle*60;
  dutyS = dutyM*60;
  queryS = wsp.detInt*60;
  flogf("\nwspDetect()\t| phase cycles=%d, cycle=%dm, duty=%d%%, detInt=%dm",
    cycles, wsp.cycle, wsp.duty, wsp.detInt);
  flogf("\nsecs %d %d %d %d\n", 
    phaseS, cycleS, dutyS, queryS);
  // while no err and tmr
  tmrStart(phase_tmr, phaseS);
  // phase
  while (!r) {
    if (tmrQuery(phase_tmr)<cycleS) continue;
    tmrStart(cycle_tmr, cycleS);
    tmrStart(duty_tmr, dutyS);
    flogf("\nwspDetect\t| cycle %d", cycleCnt++);
    // cycle
    while (!r) {
      // duty
      while (!r) {
        if (tmrQuery(duty_tmr)<queryS) continue;
        tmrStart(query_tmr, queryS);
        // query
        while (!r) {
          utlNap(5);
          if (tmrExp(query_tmr)) break;
        } // while det_tmr
        // detections
        r = wspQuery(&det);
        detTotal += det;
        flogf("\nwspDetect\t| detected %d", det);
        // short naps avoids extra loops
        if (tmrExp(duty_tmr)) break;
      } // while duty
      flogf("\nwspDetect\t| duty cycle");
      // wait until cycle ends
      while (!tmrExp(cycle_tmr)) utlNap(5);
      break;
    } // while cycle
    // check disk space
    if (wspSpace(&free)) r = 2;     // fail
    if (free*wsp.cfSize<wsp.freeMin) r = 1;
    DBG2("\nfree: %3.1f GB: %3.1f err: %d\n", free, free*wsp.cfSize, r)
    if (tmrExp(phase_tmr)) break;
  } // while phase
  if (r) tmrStopAll();       // err
  utlWrite(wsp.port, "$EXI*", EOL);
  utlExpect(wsp.port, utlBuf, "FIN", 5);
  *detections = detTotal;
  flogf("\nwspDetect\t| total detections %d", detTotal);
  return r;
} // wspDetect

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

