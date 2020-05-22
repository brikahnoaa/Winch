// sleeplite.c
#include <test.h>

int dog;
void MyChore(void); 
void MyChore(void) { if (! --dog) utlErr( watchdog_err, "woof" ); } 
static void Irq4RxISR(void);
static void Irq4RxISR(void) { PinIO(IRQ4RXD); RTE(); }

void main(void){
  time_t now, then;
  int cons=0, watchdog=30, sleepsec=6, tuning=27;
  sysInit();
  mpcInit();
  //
  if (dbg.t1) watchdog = dbg.t1;
  if (dbg.t2) sleepsec = dbg.t2;
  if (dbg.t3) tuning = dbg.t3;
  cprintf("\nsleep lite\n");
  cprintf(" watchdog:t1=%d, sleepsec:t2=%d, tuning:t3=%d\n", 
    watchdog, sleepsec, tuning);
 
  sleepsec = 1.027 * sleepsec;
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
    // console or PIT?
    if (!PinTestIsItBus(IRQ4RXD))
      if (SCIRxBreak(50)) 
      { // not noise
        cons = 1;
        // consume BREAK
        while (SCIRxBreak(10)) {}
        break;
      }
  } // loop
  //
  if (cons)
    cprintf("user break. sleepsec remaining=%d\n", sleepsec);
  PITSet51msPeriod(PITOff);  
  PITRemoveChore(MyChore); 
  time(&now);
  cprintf("slept %ld @ %s\n", now-then, utlDateTimeFmt(now));
  cdrain();
  BIOSResetToPicoDOS();
}

