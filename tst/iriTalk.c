// gpsTst.c
#include <main.h>

extern IriData iri;

void main(void){
  Serial port;
  char c;
  sysInit();
  mpcInit();
  antInit();
  iriInit();
  //
  antStart();
  iriStart();
  iriSats();
  iriStats();
  if (iridSig())
    flogf("\niridSig\t| fail");
  /**/
  port = iri.port;
  flogf("\nPress Q to exit, C:cf2, A:a3la; antenna:: G:gps, I:irid\n");
  while (true) {
    if (TURxQueuedCount(port)) {
      c=TURxGetByte(port,false);
      cputc(c);
    }
    if (cgetq()) {
      c=cgetc();
      if (c=='Q') break;
      if (c=='C') {
        antDevice(cf2_dev);
        continue;
      }
      if (c=='A') {
        antDevice(a3la_dev);
        continue;
      }
      if (c=='G') {
        antSwitch(gps_ant);
        continue;
      }
      if (c=='I') {
        antSwitch(irid_ant);
        continue;
      }
      cputc(c);
      TUTxPutByte(port,c,false);
    }
  }
  /**/

  iriStop();
  antStop();
}
