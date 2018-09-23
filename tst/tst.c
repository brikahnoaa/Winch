// wspTst.c
#include <utl.h>
#include <wsp.h>
#include <mpc.h>
#include <sys.h>


void main(void){
  int detect=0;
  sysInit();
  mpcInit();
  wspInit();
  wspDetectHour(&detect);
  flogf("wspDetectHour() total (%d)", detect);
}
