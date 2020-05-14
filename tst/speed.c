// speed.c
#include <test.h>
#define EOL "\r\n"

void idle(int secs);
void tpuinit(void);
Serial tpuport;

void main(void){
  int i, j=0, best=14720, good=3680, slow=1840, secs=3, loop=2, speed;
  speed=TMGGetSpeed(); 
  sysInit();
  mpcInit();
  tpuinit();
  sprintf(all.str, "hello %d", speed); utlWrite(tpuport, all.str, EOL);
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
  sprintf(all.str, "hello %d", speed); utlWrite(tpuport, all.str, EOL);
  cprintf("to sleep, ");
  cprintf("%s\n", utlDateTime());
  idle(secs);
  speed=good;
  cprintf("[%5d] ", speed);
  TMGSetSpeed(speed);
  sprintf(all.str, "hello %d", speed); utlWrite(tpuport, all.str, EOL);
  cprintf(" perchance ");
  cprintf("%s\n", utlDateTime());
  idle(secs);
  for (i=0; i<loop; i++) {
    speed=best;
    cprintf("[%5d] ", speed);
    TMGSetSpeed(speed);
  sprintf(all.str, "hello %d", speed); utlWrite(tpuport, all.str, EOL);
    speed=slow;
    cprintf("[%5d] ", speed);
    TMGSetSpeed(speed);
  sprintf(all.str, "hello %d", speed); utlWrite(tpuport, all.str, EOL);
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

void tpuinit(void){
  tpuport=mpcPamPort();
  if (tpuport==NULL)
    utlStop("com1 open fail");
  mpcPamPwr(sbe16_pam, true);
}
