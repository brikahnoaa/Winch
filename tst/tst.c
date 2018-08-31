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
  Serial port;
  int len=boy.iridTest;
  char c, *buff;
  buff = malloc(len);
  memset(buff, 0, len);
  sprintf(buff, "Test String");
  sysInit();
  mpcInit();
  antInit();
  gpsInit();
  //
  antStart();
  gpsStart();
  // gpsStats();
  // flogf("\n%s\n", utlTime());
  // gpsStats();
  // flogf("\n%s\n", utlTime());
  if (iridSig()) exit ;
  if (iridDial()) exit ;
  iridSendBlock(buff, len, 1, 1);
  utlReadWait(gps.port, utlBuf, gps.rudResp);
  if (strstr(utlBuf, "cmds"))
    len = iridLandCmds(buff);
  iridHup();
  flogf("\n%s\n", utlTime());
  /**/
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
  /**/

  gpsStop();
  antStop();
}
