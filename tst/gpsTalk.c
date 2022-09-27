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
  gpsSats();
  gpsStats();
  if (iridSig())
    flogf("\niridSig\t| fail");
  /**/
  port = gps.port;
  flogf("\nPress Q to exit, C:cf2, A:a3la; antenna:: G:gps, I:irid\n");
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
      if (c=='G') {
        antSwitch(gps_ant);
        continue;
      }
      if (c=='I') {
        antSwitch(irid_ant);
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
