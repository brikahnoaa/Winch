// sleep.c
#include <test.h>

void main(void){
  int i=0, delayms=100, sleepsec=2;
  // char *buff;
  // int i, j, r;
  // int blk, len, fil, del;
  // blk = 4; len = 1024; fil = 1; del = 0;
  sysInit();
  mpcInit();
  //
  //
  cprintf("\nsleep 6 sec\n");
  if (dbg.t1) delayms = dbg.t1;
  if (dbg.t2) sleepsec = dbg.t2;
  cprintf(" delayms:t1=%d, sleepsec:t2=%d\n", 
    delayms, sleepsec);
  //if (dbg.t2) len = dbg.t2;
  //if (dbg.t3) fil = dbg.t3;
  //if (dbg.t4) del = dbg.t4;
  //cprintf("\n block.t1=%d   length.t2=%d   files.t3=%d   delay.t4=%d\n", 
      //blk, len, fil, del);
  //
  cprintf("%s\n", utlDateTime());
  cprintf("to sleep,");
  cdrain();
  PWRSuspendSecs(sleepsec, true, WakeOnTimeout);
  cputc(0); utlDelay(delayms);   
  // cprintf("-"); utlDelay(delayms);   
  cprintf("\n perchance \n");
  PWRSuspendSecs(sleepsec, true, WakeOnTimeout);
  cputc(0); utlDelay(delayms);   
  cprintf("to dream\n");
  PWRSuspendSecs(sleepsec, true, WakeOnTimeout);
  cputc(0); utlDelay(delayms);   
  cprintf("%s\n", utlDateTime());
  exit(0);
}
