// wspTst.c
#include <utl.h>
#include <mpc.h>
#include <sys.h>
#include <wsp.h>

void main(void){
  sysInit();
  mpcInit();
  wspInit();
  wspStart(wsp2_pam);
  wspStorm(utlBuf);
  flogf("storm: %s\n", utlBuf);
  wspStop();
}
