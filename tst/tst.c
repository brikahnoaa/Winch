// s39Tst.c
#define TEST
#include <com.h>
#include <ant.h>
#include <ctd.h>
#include <mpc.h>
#include <sys.h>

void main(void){
  sysInit();
  mpcInit();
  ctdInit();
  antInit();
  flogf("\nPress a key to exit\n");
  antAuto(true);
  while (!cgetq()) {
    flogf(" %2.1f", antDepth());
    utlNap(2);
  }
}
