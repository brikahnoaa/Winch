// sleeplite.c
#include <test.h>

int dog;
void MyChore(void); 
void MyChore(void) { if (! --dog) utlErr( watchdog_err, "woof" ); } 
static void Irq4RxISR(void);
static void Irq4RxISR(void) { PinIO(IRQ4RXD); RTE(); }

void main(void){
  time_t now, then;
  int i=0, watchdog=30, sleepsec=6, breakms=2;
  sysInit();
  mpcInit();
  //
  if (dbg.t1) watchdog = dbg.t1;
  if (dbg.t2) sleepsec = dbg.t2;
  if (dbg.t3) breakms = dbg.t3;
  cprintf("\nsleep lite\n");
  cprintf(" watchdog:t1=%d, sleepsec:t2=%d, breakms:t3=%d\n", 
    watchdog, sleepsec, breakms);
 
  // this goes in utlInit?
  dog=watchdog;
  IEVInsertAsmFunct(Irq4RxISR, level4InterruptAutovector);
  IEVInsertAsmFunct(Irq4RxISR, spuriousInterrupt);
  //
  PITInit(6); //At priority 6  
  PITSet51msPeriod(19);  // ~ 1sec 
  PITAddChore(MyChore,6); 
  //
  time(&now);
  then = now;
  cprintf("sleep %d @ %s\n", sleepsec, utlDateTimeFmt(now));
  cdrain();
  utlDelay(100);
  while(sleepsec--) 
  { // loop until console BREAK or PIT seconds
    PinBus(IRQ4RXD);
    LPStopCSE(FullStop);
    if (SCIRxBreak(breakms)) 
    { // consume BREAK, leave loop
      while (SCIRxBreak(breakms)) i++; 
      break;
    }
  } // loop
  if (i) 
  { // user BREAK
    cprintf("break %dms\n", ++i*breakms);
    cprintf("sleepsec remaining=%d\n", sleepsec);
  }
  PITSet51msPeriod(PITOff);  
  PITRemoveChore(MyChore); 
  time(&now);
  cprintf("slept %ld @ %s\n", now-then, utlDateTimeFmt(now));
  cdrain();
  BIOSResetToPicoDOS();
}

