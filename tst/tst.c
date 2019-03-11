// wispr.c
#include <utl.h>
#include <wsp.h>
#include <mpc.h>
#include <sys.h>

extern WspInfo wsp;

void main(void){
  int i, r;
  char c;
  bool b=false;
  Serial port;
  sysInit();
  mpcInit();
  // mpcPamDev(wsp2_pam);
  port = mpcPamPort();
  flogf("%s\n", utlDateTime());
  flogf("\nPress [tab] q=quit x=off i=init o=on s=wStorm m=detectM(t1) \n");
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
        case 'q':
          flogf("\nquit\n");
          mpcPamPulse(WISPR_PWR_OFF);
          return;
        case 'i':
          flogf("\ninit\n");
          wspInit();
          break;
        case 'o':
          flogf("\noff\n");
          wspStart();
          break;
        case 's':
          flogf("\nstorm\n");
          wspStorm(all.buf);
          utlNonPrint(all.buf);
          break;
        case 'x':
          flogf("\noff\n");
          wspStop();
          break;
        case 'm':
          if (tst.t1) {
            flogf("\ndetectM(%d)\n", tst.t1);
            r=wspDetectM(&i, tst.t1);
          } else {
            flogf("\ndetectM(%d)\n", 15);
            r=wspDetectM(&i, 15);
          }
          flogf("%d detections", i);
          if (r) flogf(", %d err", r);
          flogf("\n");
          break;
        } // case
      } // if true
    }
    // if (TURxQueuedCount(port)) {
    //   c=TURxGetByte(port,false);
    //   cputc(c);
    // }
  }
}
