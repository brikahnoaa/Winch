// global data structures define ant, boy, ctd, irid, power, winch, wisp

#include <stdio.h>
#include <stdlib.h>
typedef char bool;              // pico thing
typedef int TUPort;
#define false 0
#define true  1
#include <string.h>
#include <global.h>
#include <settings.h>
char scratch[BUFSZ], stringin[BUFSZ], stringout[BUFSZ];
char WriteBuffer[BUFSZ];

void testSettings();

// surfaced ctdPos gpsLong[20] gpsLat[20] port
AntennaData ant = {
  false, 1.5, "123:45.6789 W", "45:67.8900 N", NULL
};
//
// progname[20] projID[6] pltfrmID[6] filenum starts maxStarts
// detInt dataInt phase phaseStart on 
// depth moorDepth avgVel port
BuoyData boy = {
  "LARA", "QUEH", "LR01", 0, 0, 50, 
  0, 0, 0, 2, 1,
  0, 0, 0, NULL
};
//
// syncMode pending delay port
CtdData ctd = {
  0, 0, 3.5, NULL
};
//
// phone[14] minSigQ maxCalls maxUpl
// warmup offset rest callHour callMode
IridiumData irid = {
  "0088160000519", 3, 5, 30000,
  30, 4, 30, 20, 0
};
//
// delay rrate frate port
WinchData winch = {
  6.5, 0, 0, NULL
};
//
// gain num detMax detNum dutycycl port
WisprData wisp = {
  2, 4, 10, 0, 360, NULL
};


/*
 * used during devo test
 */
void testSettings(){
  // global boy
  char str[32];
  int r;
  printf ( "%d, %hd, %.1f, %s\n", 
    boy.on, boy.phase, boy.depth, boy.projID );
  // update
  r = settingString(strcpy( str, "boy.phase=22"));
  if ( r ) printf( "error boy.phase\n");
  r = settingString(strcpy( str, "pD=abc def"));
  if ( r ) printf( "error boy.projID\n");
  printf ( "%d, %hd, %.1f, %s\n", 
    boy.on, boy.phase, boy.depth, boy.projID );
}


int main(){ testSettings(); }