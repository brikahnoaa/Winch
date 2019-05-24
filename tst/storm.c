// wspTst.c
#include <main.h>

void main(void){
  sysInit();
  mpcInit();
  wspInit();
  wspStart(wsp2_pam);
  wspStorm(all.buf);
  flogf("storm: %s\n", all.buf);
  wspStop();
}
