// iridFile.c
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
  // Serial port;
  char *buff;
  // int l, r;
  sysInit();
  mpcInit();
  antInit();
  gpsInit();
  //
  antStart();
  gpsStart();
  //
  buff = malloc(1024);
  // antSwitch(gps_ant);
  // gpsStats();
  antSwitch(irid_ant);
  tmrStart(minute_tmr, 5*60);
  while (!tmrExp(minute_tmr)) {
    if (iridSig()) continue;
    if (iridDial()) continue;
    if (iridProjHdr()) continue;
    if (iridSendFile("log\\001eng.log")) continue;
    if (iridSendFile("log\\001s16.log")) continue;
    iridHup();
    break;
  }
  utlDelay(500);
    flogf("\nsys.program = %s", sys.program);
  gpsStop();
  antStop();
}
