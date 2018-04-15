// antTst.c
#include <utl.h>
#include <ctd.h>
#include <mpc.h>
#include <sys.h>
#include <ant.h>

extern AntInfo ant;
extern CtdInfo ctd;

void main(void){
  char c;
  sysInit();
  mpcInit();
  antInit();
  ctdInit();
  antStart();
  cprintf("oceanCurr = %f\n", oceanCurr());
  cprintf("oceanCurrChk = %d\n", oceanCurrChk());
}
