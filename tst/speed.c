// speed.c
#include <test.h>

void idle(int sec);

void main(void){
  int i, best=14720, good=3680, slow=1840, repeat=3, delay=3, speed; 
  sysInit();
  mpcInit();
  //
  cprintf("\nclock speeds\n");
  if (dbg.t1) good = dbg.t1;
  if (dbg.t2) slow = dbg.t2;
  if (dbg.t3) repeat = dbg.t3;
  if (dbg.t4) delay = dbg.t4;
  cprintf(" good.t1=%d  slow.t2=%d  repeat.t3=%d  delay.t4=%d \n", 
    good, slow, repeat, delay);
  //
  cprintf("%s\n", utlDateTime());
  speed=best;
  cprintf("[%5d] ", speed);
  TMGSetSpeed(speed);
  cprintf("to sleep, ");
  cprintf("%s\n", utlDateTime());
  idle(delay);
  speed=good;
  cprintf("[%5d] ", speed);
  TMGSetSpeed(speed);
  cprintf(" perchance ");
  cprintf("%s\n", utlDateTime());
  idle(delay);
  for (i=0; i<repeat; i++) {
    speed=slow;
    cprintf("[%5d] ", speed);
    TMGSetSpeed(speed);
    cprintf("to dream ");
    cprintf("%s\n", utlDateTime());
    idle(delay);
    speed=good;
    cprintf("[%5d] ", speed);
    TMGSetSpeed(speed);
  }
  exit(0);
}

void idle(int sec){
  time_t now, then;
  struct tm *tim;
  time(&now); 
  then=now+sec; 
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
