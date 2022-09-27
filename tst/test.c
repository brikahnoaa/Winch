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
  if (!ctdPrompt()) flogf("ctdPrompt fail\n");
  ctdSample();
  ctdDataWait();
  // ctdRead();
  flogf("ctdDepth %2.1f", ctdDepth());
  if (!ctdPrompt()) flogf("ctdPrompt fail\n");
  flogf("\nPress Q to exit\n");
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
  ctdStop();
}
