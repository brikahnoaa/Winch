// wspTst.c
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
  wspStart(wsp2_pam);
  wspMinutes(&detect, 4);
  flogf("wspDetect(%d)", detect);
  utlNap(15);
  wspStop();
  utlStop("clean");
}
