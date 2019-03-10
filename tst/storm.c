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
  wspStorm(all.buf);
  flogf("storm: %s\n", all.buf);
  wspStop();
}
