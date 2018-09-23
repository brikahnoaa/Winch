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
  case 2: flogf("\nHour watchdog"); break;
  }
  /*
  utlNap(100);
  for (i=0;i<5;i++){
    wspQuery(&detect);
    flogf("\ndetected %d", detect);
    utlLogTime();
    utlNap(60);
  }
  */
  flogf("\nwspDetectHour() total (%d)", detect);
}
