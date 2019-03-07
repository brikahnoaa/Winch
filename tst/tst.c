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
  flogf("\nPress Q=quit I=wspInit W=wspStart X=wspStop S=storm\n");
  while (true) {
    if (cgetq()) {
      c=cgetc();
      switch (c) {
      case 'Q':
        return;
      case 'I':
        wspInit();
        break;
      case 'W':
        wspStart(wsp.cardUse);
        break;
      case 'S':
        utlWrite(port, "/bin/spectrogram -T10 -C12 -v1 -n512 -o256 -g0 -t20 -l noise.log", NULL);
        utlRead(port, utlBuf);
        flogf("-> %s \n", utlBuf);
        utlWrite(port, "", "\n");
        utlRead(port, utlBuf);
        wspStorm(utlBuf);
        utlNonPrint(utlBuf);
        break;
      case 'X':
        wspStop();
        mpcPamPulse(WISPR_PWR_OFF);
        break;
      default:
        TUTxPutByte(port,c,false);
      }
    }
    if (TURxQueuedCount(port)) {
      c=TURxGetByte(port,false);
      cputc(c);
    }
  }
}
