// antTst.c
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
  ctdInit();
  antStart();
  flogf("\nPress any to talk, q to exit\n");
  while (!cgetq()) {
    flogf("antDepth() -> %f\n", antDepth());
    flogf("antMoving() -> %f\n", antMoving());
  }
  flogf("connected to ant\n");
  while (true) {
    if (cgetq()) {
      c=cgetc();
      if (c=='q') break;
      TUTxPutByte(ant.port,c,false);
    }
    if (TURxQueuedCount(ant.port)) {
      c=TURxGetByte(ant.port,false);
      cputc(c);
    }
  }
}
