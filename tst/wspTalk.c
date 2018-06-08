// ctdTst.c
#include <utl.h>
#include <wsp.h>
#include <mpc.h>
#include <sys.h>


void main(void){
  char c;
  Serial port;
  sysInit();
  mpcInit();
  port = mpcPamPort();
  wspInit();
  // mpcPamDev(wsp2_pam);
  wspStart(wsp1_pam);
  flogf("\nPress Q to exit\n");
  while (true) {
    if (cgetq()) {
      c=cgetc();
      if (c=='Q') return;
      cputc(c);
      TUTxPutByte(port,c,false);
    }
    if (TURxQueuedCount(port)) {
      c=TURxGetByte(port,false);
      cputc(c);
    }
  }
}
