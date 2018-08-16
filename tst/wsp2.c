// wspTst.c
#include <utl.h>
#include <tmr.h>
#include <wsp.h>
#include <mpc.h>
#include <sys.h>


void main(void){
  int r, detect=0;
  sysInit();
  mpcInit();
  wspInit();
  flogf("dataPhase()");
  wspStart(wsp2_pam);
  tmrStart(minute_tmr, 5*60);
  while (!tmrExp(minute_tmr)) {}
  r = wspQuery(&detect);
  wspExit();
  flogf("\ndataPhase detections: %d", detect);
  wspStorm(utlBuf);
  flogf("\nstorm: %s", utlBuf);
  wspStop();
}
