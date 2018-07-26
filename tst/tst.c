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
  sysInit();
  mpcInit();
  antInit();
  gpsInit();
  flogf("iridPhase()");
  antStart();
  antAuton(true);
  // ctdAuton(true);
  tmrStart(phase_tmr, 5*60);
  {
    gpsStart();
    flogf("\n%s ===\n", utlTime());
    gpsSats();
    gpsStats();
    while (!tmrExp(phase_tmr)) {
      flogf("\n%s ===\n", utlTime());
      // 0=success
      if (iridSig()) continue;
      if (iridDial()) continue;
      iridSendTest(100);
      iridHup();
    }
    gpsSats();
    flogf("\n%s ===\n", utlTime());
    tmrStart(minute_tmr, 60);
    while (!tmrExp(minute_tmr)) {
      gpsStats();
      antDevice(cf2_dev);
      antSample();
      antDataWait();
      flogf("\nantDepth()->%4.2f", antDepth());
      antDevice(a3la_dev);
    }
    flogf("\n%s ===\n", utlTime());
    gpsStop();
  } // irid
  antAuton(false);
  // ctdAuton(false);
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
