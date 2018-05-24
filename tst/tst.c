// gpsTst.c
#include <utl.h>
#include <gps.h>
#include <mpc.h>
#include <ant.h>
#include <sys.h>


void main(void){
  sysInit();
  mpcInit();
  antInit();
  gpsInit();
  //
  antStart();
  gpsStart();
  gpsStats();
  iridSig();
  gpsStop();
}
