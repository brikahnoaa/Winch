// ctdTst.c
#include <main.h>

extern CtdInfo ctd;

void main(void){
  sysInit();
  mpcInit();
  // ctdInit();
  wspInit();
  cprintf("\n+dataPhase()@%s", utlDateTime());
  wspStart();
  wspStop();
}
