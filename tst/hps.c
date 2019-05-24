// hps.c
#include <main.h>

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
