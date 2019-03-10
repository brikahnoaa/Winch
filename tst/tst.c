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
  flogf("%s\n", utlDateTime());
  flogf("\nPress [tab] x=exit q=wStop i=wInit w=wStart s=wStorm d=wQuery\n");
  while (true) {
    if (cgetq()) {
      c=cgetc();
      if (c=='\t') {
        b=true;
        continue;
      }
      if (b==false) {
        TUTxPutByte(port,c,false);
        cputc(c);
      } else {
        b=false;
        switch (c) {
        case 'x':
          mpcPamPulse(WISPR_PWR_OFF);
          return;
        case 'i':
          wspInit();
          break;
        case 'w':
          wspStart();
          break;
        case 's':
          wspStorm(utlBuf);
          utlNonPrint(utlBuf);
          break;
        case 'q':
          wspStop();
          break;
        case 'd':
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
