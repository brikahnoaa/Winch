// gpsTst.c
#include <utl.h>
#include <gps.h>
#include <mpc.h>
#include <ant.h>
#include <sys.h>

extern GpsInfo gps;

void main(void){
  Serial port;
  char c;
  sysInit();
  mpcInit();
  antInit();
  gpsInit();
  //
  antStart();
  gpsStart();
  port = gps.port;
  flogf("\nPress Q=exit A=a3la C=cf2 G=gps I=irid \n");
  while (true) {
    if (TURxQueuedCount(port)) {
      c=TURxGetByte(port,false);
      cputc(c);
    }
    if (cgetq()) {
      c=cgetc();
      // breaks from while
      if (c=='Q') break;
      switch (c) {
      case 'A': 
        printf("a3la_dev\n");
        antDevice(a3la_dev);
        utlWrite(port, "ate1", "\r");
        utlRead(port);
        break;
      case 'C': 
        printf("cf2_dev\n");
        antDevice(cf2_dev);
        break;
      case 'G': 
        printf("gps_ant\n");
        antSwitch(gps_ant);
        break;
      case 'I': 
        printf("irid_ant\n");
        antSwitch(irid_ant);
        break;
      default:
        TUTxPutByte(port,c,false);
      }
    }
  }

  gpsStop();
  antStop();
}
