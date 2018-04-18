// ctdTst.c
#include <utl.h>
#include <ctd.h>
#include <mpc.h>
#include <sys.h>

extern CtdInfo ctd;

void main(void){
  char c;
  sysInit();
  mpcInit();
  ctdInit();
  wspInit();
  cprintf("\n+dataPhase()@%s", utlDateTime());
  wspStart();
  wspStop();
}
