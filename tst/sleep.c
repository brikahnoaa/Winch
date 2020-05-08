// sleep.c
#include <test.h>

void main(void){
  time_t now;
  // char *buff;
  // int i, j, r;
  // int blk, len, fil, del;
  // blk = 4; len = 1024; fil = 1; del = 0;
  sysInit();
  mpcInit();
  //
  //
  cprintf("\nsleep 6 sec\n");
  //if (dbg.t1) blk = dbg.t1;
  //if (dbg.t2) len = dbg.t2;
  //if (dbg.t3) fil = dbg.t3;
  //if (dbg.t4) del = dbg.t4;
  //cprintf("\n block.t1=%d   length.t2=%d   files.t3=%d   delay.t4=%d\n", 
      //blk, len, fil, del);
  //
  time(&now);
  cprintf("to sleep,");
  PWRSuspendUntil(now+2, true, WakeOnTimeout);
  cprintf(" perchance ");
  PWRSuspendUntil(now+4, true, WakeOnTimeout);
  cprintf("to dream");
  PWRSuspendUntil(now+6, true, WakeOnTimeout);
  exit(0);
}
