// gpsTst.c
#include <utl.h>
#include <gps.h>
#include <mpc.h>
#include <ant.h>
#include <sys.h>
#include <tmr.h>

extern GpsInfo gps;

void main(void){
  Serial port;
  char c;
  int i;
  sysInit();
  mpcInit();
  antInit();
  gpsInit();
  //
  antStart();
  antAuton(true);
  flogf("\n%s ===\n", utlTime());
  tmrStart(phase_tmr, 6*60);
  gpsStart();
  gpsStats();
  for (i=0; i<3; i++) {
    flogf("\n%s ===\n", utlTime());
    iridSig();
    iridDial();
    iridSendTest(100);
    iridHup();
  }
  flogf("\n%s ===\n", utlTime());
  gpsSats();
  while (!tmrExp(phase_tmr)) {
    gpsStats();
    antDevice(cf2_dev);
    antSample();
    antDataWait();
    flogf("\nantDepth()->%3.1f", antDepth());
    antDevice(a3la_dev);
  }
  antAuton(false);
  flogf("\n%s ===\n", utlTime());
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
