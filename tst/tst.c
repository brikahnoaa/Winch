// antTst.c
#include <utl.h>
#include <ctd.h>
#include <mpc.h>
#include <sys.h>
#include <ant.h>

extern AntInfo ant;

void main(void){
  char c;
  sysInit();
  mpcInit();
  antInit();
  antDevice(cf2_dev);
  antStart();
  flogf("\nPress q to exit\n");
  while (!cgetq()) {
    flogf(" %2.1f", antDepth());
  }
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
