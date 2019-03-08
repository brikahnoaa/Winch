// wispr.c
#include <utl.h>
#include <wsp.h>
#include <mpc.h>
#include <sys.h>

extern WspInfo wsp;

void main(void){
  int i;
  char c;
  bool b=false;
  Serial port;
  sysInit();
  mpcInit();
  // mpcPamDev(wsp2_pam);
  port = mpcPamPort();
  flogf("\nPress [tab] q=quit i=wInit w=wStart x=wStop s=wStorm ?=wQuery\n");
  while (true) {
    if (cgetq()) {
      c=cgetc();
      if (c=='\t') {
        b=true;
        continue;
      }
      if (b==false) {
        TUTxPutByte(port,c,false);
      } else {
        b=false;
        switch (c) {
        case 'q':
          mpcPamPulse(WISPR_PWR_OFF);
          return;
        case 'i':
          wspInit();
          break;
        case 'w':
          wspStart(wsp.cardUse);
          break;
        case 's':
          wspStorm(utlBuf);
          utlNonPrint(utlBuf);
          break;
        case 'x':
          wspStop();
          break;
        case '?':
          wspQuery(&i);
          flogf("%d detections\n", i);
          break;
        } // case
      } // if true
    }
    if (TURxQueuedCount(port)) {
      c=TURxGetByte(port,false);
      cputc(c);
    }
  }
}
