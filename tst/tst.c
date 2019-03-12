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
  wspInit();
  // mpcPamDev(wsp2_pam);
  port = mpcPamPort();
  flogf("%s\n", utlDateTime());
  flogf("\nPress [tab] q=quit x=off o=on s=wStorm m=detectM(t1) t=time\n");
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
          flogf("%s\n", utlTime());
          mpcPamPulse(WISPR_PWR_OFF);
          flogf("\nquit ");
          flogf(" %s\n", utlTime());
          return;
        case 't':
          flogf("%s\n", utlTime());
          wspDateTime();
          flogf("\ntime ");
          flogf(" %s\n", utlTime());
          break;
        case 'o':
          flogf("%s\n", utlTime());
          wspStart();
          flogf("\non ");
          flogf(" %s\n", utlTime());
          break;
        case 's':
          flogf("%s\n", utlTime());
          wspStorm(all.buf);
          utlNonPrint(all.buf);
          flogf("\nstorm ");
          flogf(" %s\n", utlTime());
          break;
        case 'x':
          flogf("%s\n", utlTime());
          wspStop();
          flogf("\noff ");
          flogf(" %s\n", utlTime());
          break;
        case 'm':
          flogf("%s\n", utlTime());
          if (tst.t1) {
            flogf("\ndetectM(%d)\n", tst.t1);
            r=wspDetectM(&i, tst.t1);
          } else {
            flogf("\ndetectM(%d)\n", 15);
            r=wspDetectM(&i, 15);
          }
          flogf("%d detections", i);
          if (r) flogf(", %d err", r);
          flogf(" %s\n", utlTime());
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
