// gpsTst.c
#include <utl.h>
#include <gps.h>
#include <mpc.h>
#include <ant.h>
#include <sys.h>
#include <tmr.h>
#include <boy.h>

extern GpsInfo gps;
extern BoyInfo boy;

void main(void){
  // Serial port;
  // char c;
  char *buff;
  int len, cnt;
  int i, r;
  sysInit();
  mpcInit();
  antInit();
  gpsInit();
  //
  antStart();
  gpsStart();
  //
  len = boy.testSize;
  cnt = boy.testCnt;
  cprintf("\nlength boy.testSize=%d, count boy.testCnt=%d ", len, cnt);
  cprintf("\nbaud gps.rudBaud=%d", gps.rudBaud);
  buff = malloc(len);
  // gpsStats();
  // flogf("\n%s\n", utlTime());
  // gpsStats();
  // flogf("\n%s\n", utlTime());
  if (iridSig()) return;
  if (iridDial()) return;
  for (i=1; i<=cnt; i++) {
    memset(buff, 0, len);
    sprintf(buff, "%d of %d =%d @%d", i, cnt, len, gps.rudBaud);
    r = iridSendBlock(buff, len, i, cnt);
    cprintf("(%d)\n", r);
  }
  // r = TURxGetBlock(gps.port, utlBuf, 5, gps.rudResp*1000);
  tmrStart(gps_tmr, gps.rudResp);
  tmrStart(sec_tmr, 3);
  memset(utlBuf, 0, 9);
  for (r=0; r<5; r++) {
    if (TURxQueuedCount(gps.port)) 
      utlBuf[r] = TURxGetByte(gps.port, false);
    if (tmrExp(gps_tmr)) {
      flogf("\nbad land, %d bytes", r);
      break;
    }
  }
  flogf("land (%d) ''%s''", r, utlNonPrintBlock(utlBuf,r));
  if (strstr(utlBuf, "cmds"))
    len = iridLandCmds(buff);
  flogf("landcmds (%d)", len);
  utlNap(1);
  utlRead(gps.port, utlBuf);
  flogf("\n land also said '%s'", utlBuf);
  iridHup();
  iridSig();
  flogf("\n%s\n", utlTime());
  /*
  port = gps.port;
  flogf("\nPress Q to exit, C:cf2, A:a3la\n");
  while (true) {
    if (TURxQueuedCount(port)) {
      c=TURxGetByte(port,false);
      cputc(c);
    }
    if (cgetq()) {
      c=cgetc();
      if (c=='Q') break;
      if (c=='C') {
        antDevice(cf2_dev);
        continue;
      }
      if (c=='A') {
        antDevice(a3la_dev);
        continue;
      }
      cputc(c);
      TUTxPutByte(port,c,false);
    }
  }
  */

  gpsStop();
  antStop();
}
