// sleeplite.c
#include <test.h>

int dog;
time_t watch;
static void Irq4RxISR(void);
static void Irq4RxISR(void) { PinIO(IRQ4RXD); RTE(); }
void MyChore(void); 
void MyChore(void) { ++watch; if (--dog<1) BIOSResetToPicoDOS(); } 

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
  time(&watch);
  //
  PITInit(6); //At priority 6  
  PITSet51msPeriod(19);  // ~ 1sec 
  PITAddChore(MyChore,6); 
  //
  cprintf("%s  %s\n", utlDateTimeFmt(watch), utlDateTime());
  cdrain();
  utlDelay(100);
  time(&now);
  then=now+sleepsec;
  while(true) {
    PinBus(IRQ4RXD);
    LPStopCSE(FullStop);
    if (SCIRxBreak(breakms)) break;
    if (watch>=then) break;
  }
  while (SCIRxBreak(breakms)) i++; 
  if (i) cprintf("break %dms\n", i*breakms+breakms);
  cprintf("watchdog=%d\n", dog);
  cdrain();
  PITSet51msPeriod(PITOff);  
  PITRemoveChore(MyChore); 
  cprintf("%s  %s\n", utlDateTimeFmt(watch), utlDateTime());
  cdrain();
  BIOSResetToPicoDOS();
}

