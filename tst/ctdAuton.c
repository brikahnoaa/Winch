// ctdTst.c
#include <main.h>

extern CtdInfo ctd;

void main(void){
  char c;
  sysInit();
  mpcInit();
  ctdInit();
  ctdStart();
  ctdAuton(true);
  utlNap(ctd.sampleInt*3);
  // ctdSample();
  // ctdDataWait();
  // ctdRead();
  // flogf("\nctdDepth %2.1f", ctdDepth());
  // ctdDataWait();
  // ctdRead();
  // flogf("\nctdDepth %2.1f", ctdDepth());
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
  ctdAuton(false);
  ctdStop();
}
