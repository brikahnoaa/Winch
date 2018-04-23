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
  antDevice(a3la_dev);
  flogf("\nq to exit\n");
  flogf("connected to a3la\n");
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
