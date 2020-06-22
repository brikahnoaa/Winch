// wspCmd.c
#include <main.h>

extern WspInfo wsp;

int main(void){
  static char *self="tst/wspCmd";
  static char *rets="1=!start 2=!open 3=timeout 4=!close";
  char c;
  int r, x, run=12, rest=5, cmdTO=30;
  char *dfCmd="df -h /mnt > output";
  Serial port;

  sysInit();
  mpcInit();
  wspInit();
  port = mpcPamPort();
  if (dbg.t1) run = dbg.t1;
  if (dbg.t2) rest = dbg.t2;
  if (dbg.t3) cmdTO = dbg.t3;
  wspStart();
  flogf("  params are system vars, e.g.:  set dbg.t1=30 \n");
  flogf("run=%d (t1)  rest=%d (t2) cmdTO=%d (t3)\n", run, rest, cmdTO);
  flogf("\n%s\n", utlDateTime());
  flogf(" .. %s .. \n", dfCmd);
  wspCmd(all.buf, dfCmd, 66);
  flogf("\n%s\n", utlDateTime());
  flogf("press : to enter command line, or quick command as below\n");
  flogf("q=quit d=detect%d s=wStorm t=setTime \n", run);
  while (true) 
  {
    if (cgetq())
    { // input: cmd or "command line"
      c = cgetc();
      cputc(c);
      switch (c)
      { // 
      case ':':
        cgets(all.str, 100);
        cprintf("\n%s (y/n)?");
        c = cgetc();
        cprintf("%c\n", c);
        if (c=='y')
        { 
          r = wspCmd(all.buf, all.str, cmdTO);
          if (r) raise(2);
          if (all.buf) 
            cprintf("\n%s\n", utlNonPrint(all.buf));
        }
        break;
      case 'q':
        wspStop();
        sysStop("test wispr");
        break;
      case 'd':
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
    if (SCIRxBreak(50))
      BIOSReset();
    utlNap(1);
  } // while true
} // main
