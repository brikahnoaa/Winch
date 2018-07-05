// s39.c
#include <utl.h>
#include <ctd.h>
#include <mpc.h>
#include <sys.h>
#include <ant.h>

extern AntInfo ant;
extern CtdInfo ctd;

void main(void){
  char c;
  int i, ac, vc;
  float avg, velo;
  time_t t;
  sysInit();
  mpcInit();
  antInit();
  antStart();
  ringPrint();
  t = (time_t) 0;
  flogf("\n");
  flogf("t=%ld, 0=%ld\n", t, 0L);
  flogf("antDepth() -> %f\n", antDepth());
  flogf("antTemp() -> %f\n", antTemp());
  for (i=0; i<ant.ringSize+2; i++) {
    utlNap(2);
    ringSamp((float)i, time(0));
    ac = antAvg(&avg);
    vc = antVelo(&velo);
    flogf("\nvelo[ %d ]%f, avg[ %d ]%f", vc, velo, ac, avg);
    ringPrint();
  }
  antStop();
  sysStop("user stop");
  ///
  flogf("\nPress any to talk, Q to exit\n");
  flogf("connected to ant\n");
  while (true) {
    if (cgetq()) {
      c=cgetc();
      if (c=='Q') break;
      TUTxPutByte(ant.port,c,false);
    }
    while (TURxQueuedCount(ant.port)) {
      c=TURxGetByte(ant.port,false);
      cputc(c);
    }
  }
}
