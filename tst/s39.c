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
  sysInit();
  mpcInit();
  antInit();
  antStart();
  flogf("\n");
  antSample();
  antDataWait();
  if (!antRead())
    flogf("read fails\n");
  flogf("antDepth() -> %f\n", antDepth());
  flogf("antTemp() -> %f\n", antTemp());
  // antAuton(true);
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
  antAuton(false);
  antGetSamples();
  utlStop("normal");
}
