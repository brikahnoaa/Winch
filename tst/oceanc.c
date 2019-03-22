// ctdTst.c
#include <utl.h>
#include <ctd.h>
#include <mpc.h>
#include <sys.h>
#include <boy.h>
#include <ant.h>

extern CtdInfo ctd;

void main(void){
  int r;
  float f;
  sysInit();
  mpcInit();
  ctdInit();
  antInit();
  ctdStart();
  antStart();
  if (!(r=oceanCurr(&f)))
    flogf("oceanCurr(%f):>%d\n", f, r);
}
