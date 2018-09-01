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
  printf("Using block len boy.iridTest=%d, block count boy.testCnt=%d \n",
    len, cnt);
  buff = malloc(len);
  memset(buff, 0, len);
  sprintf(buff, "Test String");
  // gpsStats();
  // flogf("\n%s\n", utlTime());
  // gpsStats();
  // flogf("\n%s\n", utlTime());
  if (iridSig()) exit ;
  if (iridDial()) exit ;
  for (i=1; i<=cnt; i++) {
    r = iridSendBlock(buff, len, i, cnt);
    printf("(%d)", r);
  }
  utlReadWait(gps.port, utlBuf, gps.rudResp);
  if (strstr(utlBuf, "cmds"))
    len = iridLandCmds(buff);
  iridHup();
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
