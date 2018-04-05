// ctdTst.c
#include <utl.h>
#include <ctd.h>
#include <mpc.h>
#include <sys.h>

extern CtdInfo ctd;

void main(void){
  char c;
  sysInit();
  mpcInit();
  ctdInit();
  flogf("\nPress q to exit\n");
  flogf(" %2.1f", ctdDepth());
  ctdAuton(true);
  while (!cgetq()) {}
  flogf(" %2.1f", ctdDepth());
  ctdAuton(false);
  ctdGetSamples();
  while (true) {
    if (cgetq()) {
      c=cgetc();
      if (c=='q') break;
      TUTxPutByte(ctd.port,c,false);
    }
    if (TURxQueuedCount(ctd.port)) {
      c=TURxGetByte(ctd.port,false);
      cputc(c);
    }
  }
}
