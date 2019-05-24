// ctdTst.c
#include <main.h>

extern CtdInfo ctd;

void main(void){
  int r;
  float fc, ft;
  sysInit();
  mpcInit();
  ctdInit();
  antInit();
  ctdStart();
  antStart();
  if (!(r=boySafeChk(&fc, &ft)))
    flogf("boySafeChk(%f, %f):>%d\n", fc, ft, r);
  utlNap(9);
  if (!(r=boySafeChk(&fc, &ft)))
    flogf("boySafeChk(%f, %f):>%d\n", fc, ft, r);
}
