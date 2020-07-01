// wispr.c
#include <main.h>

void main(void){
  int dq=0, r=0, run=2;
  float f=0.0;
  Serial port;

  sysInit();
  mpcInit();
  wspInit();
  if (dbg.t1) run = dbg.t1;
  cprintf("  params are system vars, e.g.:  set dbg.t1=30 \n");
  cprintf("run=%d (t1)  \n", run);
  cprintf("  hint: set wsp.wisprtest=1  for detection simul \n");
  port = mpcPamPort();
  cprintf("\n%s\n", utlDateTime());
  wspStart();
  r = wspDetectM(&dq, &f, run);
  printf("\nwspDetect() %5.2f free, %d detected -> returns %d \n", f, dq, r);
  utlNap(2);
  wspClose();
  wspStop();
}
