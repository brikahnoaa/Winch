// cfgTst.c
#include <com.h>
#include <ctd.h>
#include <mpc.h>
#include <sys.h>

void main(void){
  sysInit();
  mpcInit();
  ctdInit();
  flogf("\nPress a key to exit\n");
  ctdSample();
  while (!cgetq()) {
    if (ctdReady()) {
      flogf(" %2.1f", ctdDepth());
      ctdSample();
    }
  }
}
