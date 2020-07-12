// ant.c
#include <main.h>

//extern IriInfo iri;
//extern IriData irid;

void main(void){
  Serial port;
  char c;
  sysInit();
  mpcInit();
  flogf("ant.c\n");
  antInit();
  // iriInit();
  //
  antStart();
  // iriStart();
  port = antPort();
  flogf("\nPress Q=exit C=cf2(s39) A=a3la G=gps I=irid \n");
  while (true) {
    utlPet(10);
    while (TURxQueuedCount(port)) {
      c=TURxGetByte(port,false);
      cputc(c);
    }
    if (cgetq()) {
      c=cgetc();
      if (c=='Q') break; // breaks from while
      switch (c) {
      case 'A': 
        printf("a3la_dev\n");
        antDevice(a3la_dev);
        utlWrite(port, "ate1", "\r");
        utlRead(port, all.str);
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

  // iriStop();
  antStop();
}
