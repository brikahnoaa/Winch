// hps.c
#include <utl.h>
#include <gps.h>
#include <mpc.h>
#include <ant.h>
#include <sys.h>
#include <tmr.h>
#include <boy.h>
#include <cfg.h>
#include <hps.h>

extern GpsInfo gps;
extern BoyInfo boy;
extern SysInfo sys;

void main(void){
  sysInit();
  mpcInit();
  hpsStats();
  flogf("\nclear pin 21");
  PIOClear(21);
  hpsStats();
  flogf("\nclear pin 22");
  PIOClear(22);
  hpsStats();
  utlStop("hps");
}
