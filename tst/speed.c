// speed.c
#include <test.h>

void idle(int secs);

void main(void){
  int i, best=14720, good=3680, slow=1840, secs=3, loop=2, speed; 
  sysInit();
  mpcInit();
  //
  cprintf("\nclock speeds\n");
  if (dbg.t1) best = dbg.t1;
  if (dbg.t2) good = dbg.t2;
  if (dbg.t3) slow = dbg.t3;
  if (dbg.t4) secs = dbg.t4;
  cprintf(" best.t1=%d  good.t2=%d  slow.t3=%d  secs.t4=%d \n", 
    best, good, slow, secs);
  //
  cprintf("%s\n", utlDateTime());
  speed=best;
  cprintf("[%5d] ", speed);
  TMGSetSpeed(speed);
  cprintf("to sleep, ");
  cprintf("%s\n", utlDateTime());
  idle(secs);
  speed=good;
  cprintf("[%5d] ", speed);
  TMGSetSpeed(speed);
  cprintf(" perchance ");
  cprintf("%s\n", utlDateTime());
  idle(secs);
  for (i=0; i<loop; i++) {
    speed=best;
    cprintf("[%5d] ", speed);
    TMGSetSpeed(speed);
    speed=slow;
    cprintf("[%5d] ", speed);
    TMGSetSpeed(speed);
    cprintf("to dream ");
    cprintf("%s\n", utlDateTime());
    idle(secs);
  }
  exit(0);
}

void idle(int secs){
  time_t now, then;
  struct tm *tim;
  time(&now); 
  then=now+secs; 
  while (now<then) {
    time(&now); 
    if (cgetq()) {
      tim = gmtime(&now);
      cprintf("\n %02d:%02d:%02d\n", 
        tim->tm_hour, tim->tm_min, tim->tm_sec);
      if (cgetc()=='q') exit(1);
    }
  }
}
