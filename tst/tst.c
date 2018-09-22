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
  int l, len, cnt;
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
  antSwitch(irid_ant);
  if (iridSig()) return;
  if (iridDial()) return;
  if (iridProjHdr()) return;
  for (i=1; i<=cnt; i++) {
    memset(buff, 0, len);
    sprintf(buff, "%d of %d =%d @%d [%d]", 
      i, cnt, len, gps.rudBaud, gps.sendSz);
    buff[len-1] = 'Z';
    r = iridSendBlock(buff, len, i, cnt);
    cprintf("(%d)\n", r);
    // utlDelay(500);
  }
  iridLandResp(utlBuf);
  if (strstr(utlBuf, "cmds"))
    r = iridLandCmds(buff, &l);
  utlDelay(500);
  utlWrite(gps.port, "done", "");
  utlDelay(500);
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
