// wspTst.c
#include <utl.h>
#include <mpc.h>
#include <sys.h>

void main(void){
  sysInit();
  mpcInit();
  wspInit();
  wspStart(wsp1_pam);
  wspStop();
}
