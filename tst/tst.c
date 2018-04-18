// antTst.c
#include <utl.h>
#include <ctd.h>
#include <mpc.h>
#include <sys.h>
#include <ant.h>
#include <boy.h>

extern AntInfo ant;
extern CtdInfo ctd;

void main(void){
  sysInit();
  mpcInit();
  antInit();
  ctdInit();
  wspInit();
  cprintf("\n+dataPhase()@%s", utlDateTime());
  wspStart();
  wspStop();
}
