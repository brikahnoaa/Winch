// wispr.c
#include <utl.h>
#include <wsp.h>
#include <mpc.h>
#include <sys.h>

extern WspInfo wsp;

void main(void){
  int i, r;
  char c;
  bool tab=false;
  char *name="wspDetectM", *rets="1=start 9=stop 10=!wspQuery 20=!wspStop";
  Serial port;

  sysInit();
  mpcInit();
  wspInit();
  port = mpcPamPort();
  flogf("\n%s\n", utlDateTime());
  flogf("[tab] q=quit s=wStorm m=detectM(.duty) h=detH d=detD t=time\n");
  while (true) {
    if (cgetq()) {
      c=cgetc();
      if (c=='\t') {
        tab=true;
      } else if (tab==false) {
        TUTxPutByte(port,c,false);
        cputc(c);
      } else {
        tab=false;
        flogf("\n%s\n", utlTime());
        switch (c) {
        case 'q':
          wspInit();
          exit(0);
        case 't':
          flogf("time\n");
          wspDateTime();
          break;
        case 's':
          flogf("storm\n");
          wspStorm(all.buf);
          utlNonPrint(all.buf);
          break;
        case 'd':
          flogf("detectD()\n");
          r=wspDetectD(&i);
          flogf("%d detections", i);
          if (r) flogf(", %d err", r);
          break;
        case 'h':
          flogf("detectH()\n");
          r=wspDetectH(&i);
          flogf("%d detections", i);
          if (r) flogf(", %d err", r);
          break;
        case 'm':
          flogf("detectM(%d)\n", wsp.dutyM);
          r=wspDetectM(&i, wsp.dutyM);
          flogf("%d detections", i);
          if (r) flogf(", %d err", r);
          break;
        } // case
        flogf("\n... %s\n", utlTime());
      } // if tab
    }
    // if (TURxQueuedCount(port)) {
    //   c=TURxGetByte(port,false);
    //   cputc(c);
    // }
  }
}
