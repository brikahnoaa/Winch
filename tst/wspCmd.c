// wspCmd.c
#include <main.h>

extern WspInfo wsp;

void main(void){
  int r, x, run=12, rest=5;
  Serial port;

  sysInit();
  mpcInit();
  wspInit();
  port = mpcPamPort();
  if (dbg.t1) run = dbg.t1;
  if (dbg.t2) rest = dbg.t2;
  flogf("  params are system vars, e.g.:  set dbg.t1=30 \n");
  flogf("run=%d (t1)  rest=%d (t2)\n", run, rest);
  flogf("\n%s\n", utlDateTime());
  flogf("press @ to exec commands below, or type a full command\n");
  flogf("@q=quit @r=run%d @s=wStorm @t=setTime \n", run);
  while (true) 
  {
    if (cgets(all.str, 100)) 
    { // input: cmd or "command line"
      if (all.str[0]=='@')
      { // cmd
        switch (all.str[1]) 
        { // 
        case 'q':
          wspStop();
          mpcStop();
          sysStop("test wispr");
        case 'r':
          flogf("run detection for %d minutes \n", run);
          r = wspDetect(&x, run);
          flogf("wspr detected %d, returned %d \n", x, r);
          break;
        case 's':
          flogf("storm\n");
          r = wspStorm(all.buf);
          utlNonPrintBlock(all.buf, r);
          break;
        case 't':
          flogf("time\n");
          wspDateTime();
          break;
        } // switch
        flogf("\n... %s\n", utlTime());
      } // cmd
      else
      { // "command line"
      } // "command line"
    } // if input
    if (SCIRxBreak(50))
      BIOSReset();
    utlNap(1);
  } // while true
} // main
