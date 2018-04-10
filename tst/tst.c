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
  /* 
  antInit();
  antDevice(cf2_dev);
  antStart();
  flogf("\nPress q to exit\n");
  antAuton(true);
  */
  ctdInit();
  flogf("\nPress q to exit\n");
  // ctdAuton(true);
  while (!cgetq()) {
    flogf(" %f", ctdDepth());
  }
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
