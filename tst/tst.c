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
  ctdStart();
  flogf("\nctdDepth %4.2f\n", ctdDepth());
  flogf("\nctdDepth %4.2f\n", ctdDepth());
  flogf(" press a key to move on, Q to quit\n");
  while (!cgetq()) {}
  c=cgetc();
  if (c=='Q') exit(0);
  ctdAuton(true);
  flogf("\nautonomous mode:");
  flogf(" press a key to move on, Q to quit\n");
  while (!cgetq()) {}
  c=cgetc();
  if (c=='Q') exit(0);
  ctdAuton(false);
  ctdGetSamples();
  flogf("\nsbe16 prompt:");
  flogf("Press Q to exit\n");
  while (true) {
    if (cgetq()) {
      c=cgetc();
      if (c=='Q') break;
      TUTxPutByte(ctd.port,c,false);
    }
    if (TURxQueuedCount(ctd.port)) {
      c=TURxGetByte(ctd.port,false);
      cputc(c);
    }
  }
}
