// gpsTst.c
#include <utl.h>
#include <gps.h>
#include <mpc.h>
#include <ant.h>
#include <sys.h>

extern GpsInfo gps;

void main(void){
  Serial port;
  char c;
  sysInit();
  mpcInit();
  antInit();
  gpsInit();
  //
  antStart();
  gpsStart();
  // gpsStats();
  flogf("\nstart %s", utlTime());
  iridSig();
  iridSendTest(gps.testSize);
  iridSendTest(gps.testSize);
  iridHup();
  flogf("\nstop %s", utlTime());
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
