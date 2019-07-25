// a3la.c
#include <main.h>

extern AntInfo ant;

void main(void){
  char c;
  short rx, tx;
  Serial port;
  sysInit();
  mpcInit();
  // antInit();
  rx = TPUChanFromPin(ANT_RX);
  tx = TPUChanFromPin(ANT_TX);
  ant.port = TUOpen(rx, tx, 9600L, 0);
  // ctdInit();
  antStart();
  utlNap(5);
  // TUTxPutByte(ant.port, 3, false);
  // TUTxPutByte(ant.port, 'I', false);
  // TUClose(ant.port);
  // antDevice(a3la_dev);
  // port = TUOpen(rx, tx, 19200L, 0);

  flogf("\nQ to a3la\n");
  TUTxPutByte(ant.port, 3, false);
  TUTxPutByte(ant.port, 'I', false);
  while (true) {
    if (cgetq()) {
      c=cgetc();
      if (c=='Q') break;
      TUTxPutByte(ant.port,c,false);
    }
    if (TURxQueuedCount(ant.port)) {
      c=TURxGetByte(ant.port,false);
      cputc(c);
    }
  }
  flogf("\nQ to exit\n");
  flogf("connected to a3la at 19200\n");
  TUClose(ant.port);
  antDevice(a3la_dev);
  port = TUOpen(rx, tx, 19200L, 0);
  // port = ant.port;
  while (true) {
    if (cgetq()) {
      c=cgetc();
      if (c=='Q') break;
      TUTxPutByte(port,c,false);
    }
    if (TURxQueuedCount(port)) {
      c=TURxGetByte(port,false);
      cputc(c);
    }
  }
  flogf("\nQ to exit\n");
  flogf("connected to a3la at 9600\n");
  TUClose(port);
  port = TUOpen(rx, tx, 9600L, 0);
  // port = ant.port;
  while (true) {
    if (cgetq()) {
      c=cgetc();
      if (c=='Q') break;
      TUTxPutByte(port,c,false);
    }
    if (TURxQueuedCount(port)) {
      c=TURxGetByte(port,false);
      cputc(c);
    }
  }
}
