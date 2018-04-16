// tst.c
//
#include <utl.h>
#include <ant.h>
#include <boy.h>
#include <ctd.h>
#include <mpc.h>
#include <sys.h>

// extern AntInfo ant;
// extern CtdInfo ctd;

void main(void){
  sysInit();
  mpcInit();
  antInit();
  ctdInit();
  antOn(true);
  cprintf("\n");
  printf("oceanCurr = %f\n", oceanCurr());
  printf("oceanCurrChk = %d\n", oceanCurrChk());
}
