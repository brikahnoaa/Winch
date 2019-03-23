// ctdTst.c
#include <utl.h>
#include <ant.h>
#include <ctd.h>
#include <gps.h>
#include <mpc.h>
#include <sys.h>

extern CtdInfo ctd;

void main(void){
  char c;
  sysInit();
  mpcInit();
  ctdInit();
  antInit();
  ctdStart();
  antStart();
  ctdDataWait();
  if (ctdPrompt())
    flogf("\n ++sbe16@%3.1f sbe39@%3.1f", ctdDepth(), antDepth());
  else
    flogf("\n err sbe16 no prompt");
  gpsStart();
  gpsStop();
  if (ctdPrompt())
    flogf("\n ++sbe16@%3.1f sbe39@%3.1f", ctdDepth(), antDepth());
  else
    flogf("\n err sbe16 no prompt");
  flogf("\nTalk to sbe16 - Press Q to exit\n");
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
