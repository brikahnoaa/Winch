// gpsTst.c
#include <utl.h>
#include <gps.h>
#include <mpc.h>
#include <ant.h>
#include <sys.h>
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
  len = boy.iridTest;
  cnt = boy.testCnt;
  cprintf("\nlength boy.iridTest=%d, count boy.testCnt=%d ", len, cnt);
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
    utlNap(1);
    cprintf("(%d)", r);
  }
  r = TURxGetBlock(gps.port, utlBuf, 4, gps.rudResp*1000);
  if (r!=4)
    flogf("\nbad land, %d bytes", r);
  uprintf("''%s''", utlNonPrintBlock(utlBuf,4));
  if (strstr(utlBuf, "cmds"))
    len = iridLandCmds(buff);
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
