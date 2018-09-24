// ctdTst.c
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
  port = mpcPamPort();
  wspInit();
  // mpcPamDev(wsp2_pam);
  flogf("\nPress q=exit, w=wspStart s=wspStop x=poweroff\n");
  while (true) {
    if (cgetq()) {
      c=cgetc();
      if (c=='q') return;
      if (c=='w') wspStart(wsp.card);
      if (c=='s') wspStop();
      if (c=='x') mpcPamPulse(WISPR_PWR_OFF);
      cputc(c);
      TUTxPutByte(port,c,false);
    }
    if (TURxQueuedCount(port)) {
      c=TURxGetByte(port,false);
      cputc(c);
    }
  }
}
