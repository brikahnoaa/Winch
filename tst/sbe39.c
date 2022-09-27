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
  antSample();
  if (!antDataWait())
    flogf("\ndata wait fail, no response from sbe39");
  if (!antRead())
    flogf("\nread fails");
  flogf("\nantDepth() -> %f", antDepth());
  flogf("\nantTemp() -> %f", antTemp());
  antDataWait();
  antRead();
  flogf("\nantDepth() -> %f", antDepth());
  flogf("\nantTemp() -> %f", antTemp());
  // antAuton(true);
  flogf("\n\nPress any to talk, Q to exit");
  flogf("\nconnected to ant");
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
  // antAuton(false);
  // antGetSamples();
  utlStop("\nnormal");
}
