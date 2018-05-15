// veloTst.c
#include <utl.h>
#include <ctd.h>
#include <mpc.h>
#include <sys.h>
#include <ant.h>
#include <tmr.h>

extern AntInfo ant;
extern CtdInfo ctd;

void main(void){
  bool b=false;
  char c;
  float f;
  sysInit();
  mpcInit();
  antInit();
  ctdInit();
  antStart();
  tmrStart(ngk_tmr, 3);
  flogf("\nPress [space] to pause, Q to exit\n");
  while (true) {
    utlNap(1);
    if (cgetq()) {
      c=cgetc();
      if (c=='Q') exit(0);
      if (c==' ') {
        b = !b;
        ringPrint();
      }
    }
    if (b) continue; // pause
    antDepth();
    if (tmrExp(ngk_tmr)) {
      if (antVelo(&f)) 
        flogf("antVelo() -> %f\n", f);
      tmrStart(ngk_tmr, 3);
    }
  }
  flogf("connected to ant\n");
  while (true) {
    if (cgetq()) {
      c=cgetc();
      if (c=='Q') exit(0);
      TUTxPutByte(ant.port,c,false);
    }
    while (TURxQueuedCount(ant.port)) {
      c=TURxGetByte(ant.port,false);
      cputc(c);
    }
  }
}
