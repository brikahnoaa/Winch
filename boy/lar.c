// lar.c
#include <com.h>

#include <ant.h>
#include <boy.h>
#include <ctd.h>
#include <mpc.h>
#include <ngk.h>
#include <pwr.h>
#include <sys.h>
#include <wsp.h>

///
// initHW and SW structures. call boyMain()
void main(void) {
  int starts;
  starts = sysInit();
  mpcInit();
  antInit();
  boyInit();
  ctdInit();
  ngkInit();
  pwrInit();
  boyMain(starts);
} // main
