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
  mpcPamPulse(WISPR_PWR_OFF);
  utlDelay(100);
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
int wspStart(int pam) {
  DBG0("wspStart()")
  // select, power on
  mpcPamDev(pam);
  wsp.pam =  pam;
  mpcPamPulse(WISPR_PWR_ON);
  utlDelay(100);
  // expect df output
  utlExpect(wsp.port, utlBuf, "/mnt", 40);
  flogf("\n%s\n", utlBuf);
  return 0;
} // wspStart

///
// stop current card
void wspStop(void) {
  mpcPamPulse(WISPR_PWR_OFF);
  utlDelay(100);
  wsp.pam = null_pam;
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
// data waiting
int wspDetect(void) {
  float disk;
  char *s;
  int i;
  for (i=0; i<10; i++) {
    utlWrite(wsp.port, "$DFP*", EOL);
    if (utlReadWait(wsp.port, utlBuf, 2)) break;
    utlNap(3);
  } // try 10 times
  DBG2("%s", utlBuf)
  s = strtok(utlBuf, ",");
  s = strtok(NULL, "*");
  if (!s) {
    utlErr(wsp_err, "no wispr commun");
    return 1;
  }
  disk = atof(s);
  flogf("\nwspStart() %4.1f%% free", disk);
  if (disk<wsp.freeMin) 
    return 2;
  utlWrite(wsp.port, "$EXI*", EOL);
  utlExpect(wsp.port, utlBuf, "FIN", 5);
  return 0;
} // wspData

