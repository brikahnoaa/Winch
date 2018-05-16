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
  mpcPamPulse(WISPR_PWR_OFF);
  tmrStop(wsp_tmr);
  if (strlen(wsp.logFile))
    wsp.log = utlLogFile(wsp.logFile);
} // wspInit

///
// turn on, disk free > wsp.freeMin
// sets: wsp.card
int wspStart(int pam) {
  int i;
  float disk;
  char *s;
  DBG0("wspStart()")
  // select, power on
  mpcPamDev(pam);
  mpcPamPulse(WISPR_PWR_ON);
  wsp.pam =  pam;
  utlExpect(wsp.port, utlBuf, "/mnt", 40);
  flogf("\ndf\n%s\n", utlBuf);
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
  wsp.pam =  pam;
  return 0;
} // wspStart

///
// stop current card
void wspStop(void) {
  utlWrite(wsp.port, "$EXI*", EOL);
  utlExpect(wsp.port, utlBuf, "FIN", 20);
  mpcPamPulse(WISPR_PWR_OFF);
  wsp.pam = null_pam;
  mpcPamDev(null_pam);
  if (wsp.log) {
    close(wsp.log);
    wsp.log = 0;
  }
} // wspStop

///
// wsp started. interact.
int wspRead(char *buf) {
  int i, len = 0;
  flogf("\nWISPR storm detection");
  for (i=0; i<40; i++) {
    cprintf(".");
    utlNap(5);
    len = utlReadWait(wsp.port, buf, 1);
    if (len) {
      flogf(" %d->'%s'", len, buf);
      if (strstr(buf, "RDY"))
        utlWrite(wsp.port, "$WS?*", EOL);
      utlNap(5);
      len = utlReadWait(wsp.port, buf, 1);
    }
    if (len>8) {
      flogf(" %d->'%s'", len, buf);
      return len;
    }
  }
  return len;
} // wspRead

///
// data waiting
bool wspData() {
  DBG1("wD")
  return TURxQueuedCount(wsp.port);
} // wspData

