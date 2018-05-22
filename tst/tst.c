// ctdTst.c
#include <utl.h>
#include <wsp.h>
#include <mpc.h>
#include <sys.h>


void main(void){
  int detect=0;
  Serial port;
  sysInit();
  mpcInit();
  port = mpcPamPort();
  wspInit();
  // mpcPamDev(wsp2_pam);
  wspStart(wsp2_pam);
  wspDetect(&detect);
  flogf("\n wspDetect(): %d", detect);
  wspStorm(utlBuf);
  flogf("\nstorm: %s", utlBuf);
  wspStop();
}
