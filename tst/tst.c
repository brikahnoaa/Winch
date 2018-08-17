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
  mpcPamDev(wsp1_pam);
  wspExit();
  wspStop();
}
