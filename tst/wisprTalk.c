// wispr.c
#include <utl.h>
#include <wsp.h>
#include <mpc.h>
#include <sys.h>

extern WspInfo wsp;

void main(void){
  char c;
  Serial port;
  sysInit();
  mpcInit();
  // mpcPamDev(wsp2_pam);
  port = mpcPamPort();
  flogf("\nPress Q=quit I=wspInit W=wspStart S=wspStop X=poweroff\n");
  while (true) {
    if (cgetq()) {
      c=cgetc();
      if (c=='Q') return;
      if (c=='I') wspInit();
      if (c=='W') wspStart(wsp.cardUse);
      if (c=='S') wspStop();
      if (c=='X') mpcPamPulse(WISPR_PWR_OFF);
      cputc(c);
      TUTxPutByte(port,c,false);
    }
    if (TURxQueuedCount(port)) {
      c=TURxGetByte(port,false);
      cputc(c);
    }
  }
}
