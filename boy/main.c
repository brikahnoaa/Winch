// main.c
#include <utl.h>
#include <ant.h>
#include <ctd.h>
#include <gps.h>
#include <mpc.h>
#include <ngk.h>
#include <pwr.h>
#include <sys.h>
#include <tmr.h>
#include <wsp.h>
#include <boy.h>
void main(void) {
  all.starts = sysInit();
  mpcInit();
  pwrInit();
  antInit();
  boyInit();
  ctdInit();
  gpsInit();
  ngkInit();
  wspInit();

  boyMain();
}

