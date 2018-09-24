// wspTst.c
#include <utl.h>
#include <wsp.h>
#include <mpc.h>
#include <sys.h>


void main(void){
  int detect=0;
  int r;
  sysInit();
  mpcInit();
  wspInit();
  r = wspDetectDay(&detect);
  switch (r) {
  case 1: flogf("\nDay watchdog"); break;
  case 11: flogf("\nhour.watchdog"); break;
  case 12: flogf("\nhour.startFail"); break;
  case 13: flogf("\nhour.minimum"); break;
  }
  flogf("\nwspDetectHour() total (%d)", detect);
}
