// ctdTst.c
#include <utl.h>
#include <ctd.h>
#include <mpc.h>
#include <sys.h>

void main(void){
  sysInit();
  mpcInit();
  ctdInit();
  flogf("\nPress a key to exit\n");
  flogf(" %2.1f", ctdDepth());
  ctdAuton(true);
  while (!cgetq()) {
  }
  flogf(" %2.1f", ctdDepth());
  ctdAuton(false);
  ctdLog();
}
