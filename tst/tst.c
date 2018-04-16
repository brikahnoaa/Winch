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
  antStart();
  cprintf("\n");
  printf("oceanCurr = %f\n", oceanCurr());
  printf("oceanCurrChk = %d\n", oceanCurrChk());
}
