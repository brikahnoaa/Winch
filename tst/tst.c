// veloTst.c
#include <utl.h>
#include <ctd.h>
#include <mpc.h>
#include <sys.h>
#include <ant.h>

extern AntInfo ant;
extern CtdInfo ctd;

void main(void){
  char c;
  float v;
  sysInit();
  mpcInit();
  antInit();
  ctdInit();
  antStart();
  flogf("\nPress any to talk, Q to exit\n");
  while (true) {
    if (cgetq()) {
      c=cgetc();
      if (c=='Q') exit(0);
      else break;
    }
    utlDelay(3);
    flogf("antDepth() -> %f\n", antDepth());
    if (antVelo(&v))
      flogf("antVelo() -> %f\n", v);
  }
  flogf("connected to ant\n");
  while (true) {
    if (cgetq()) {
      c=cgetc();
      if (c=='Q') exit(0);
      TUTxPutByte(ant.port,c,false);
    }
    while (TURxQueuedCount(ant.port)) {
      c=TURxGetByte(ant.port,false);
      cputc(c);
    }
  }
}
