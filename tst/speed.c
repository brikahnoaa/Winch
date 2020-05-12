// speed.c
#include <test.h>

void main(void){
  int i=0, best=14720, low=160, delay=3, divisor=4, speed; // vs 16000
  // char *buff;
  // int i, j, r;
  // int blk, len, fil, del;
  // blk = 4; len = 1024; fil = 1; del = 0;
  sysInit();
  mpcInit();
  //
  //
  cprintf("\nclock speeds\n");
  if (dbg.t1) divisor = dbg.t1;
  if (dbg.t2) delay = dbg.t2;
  if (dbg.t3) low = dbg.t3;
  cprintf(" divisor.t1=%d  delay.t2=%d  low.t3=%d  \n", 
    divisor, delay, low);
  //if (dbg.t2) len = dbg.t2;
  //if (dbg.t3) fil = dbg.t3;
  //if (dbg.t4) del = dbg.t4;
  //cprintf("\n block.t1=%d   length.t2=%d   files.t3=%d   delay.t4=%d\n", 
      //blk, len, fil, del);
  // best/4
  cprintf("%s\n", utlDateTime());
  speed=best/(divisor);
  cprintf("[%5d] ", speed);
  TMGSetSpeed(speed);
  cprintf("to sleep, ");
  cprintf("%s\n", utlDateTime());
  utlNap(delay);
  // best/8
  speed=best/(divisor*2);
  cprintf("[%5d] ", speed);
  TMGSetSpeed(speed);
  cprintf(" perchance ");
  cprintf("%s\n", utlDateTime());
  utlNap(delay);
  // best/16
  speed=best/(divisor*4);
  cprintf("[%5d] ", speed);
  TMGSetSpeed(speed);
  cprintf("to dream ");
  cprintf("%s\n", utlDateTime());
  utlNap(delay);
  // low
  speed=low;
  cprintf("[%5d] ", speed);
  TMGSetSpeed(speed);
  cprintf("to dream ");
  cprintf("%s\n", utlDateTime());
  utlNap(delay);
  BIOSResetToPicoDOS();
}
