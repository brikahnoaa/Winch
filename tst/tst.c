// irid.c
#include <utl.h>
#include <gps.h>
#include <mpc.h>
#include <ant.h>
#include <sys.h>
#include <tmr.h>
#include <boy.h>
#include <cfg.h>

extern GpsInfo gps;
extern BoyInfo boy;
extern SysInfo sys;

void main(void){
  int i, r=0;
  sysInit();
  mpcInit();
  // antInit();
  i=boyEngLog();
  exit(r);
}
