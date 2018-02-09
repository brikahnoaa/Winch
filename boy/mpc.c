// mpc.c - hardware, mpc specific
#include <com.h>
#include <mpc.h>
#include <sys.h>

MpcInfo mpc;

//
// Interrupts:
//
// IRQ2 Wakes up from the sleep mode while waiting for ASC string from COM2
// IRQ3
// IRQ5 Interrupt to stop the program
//


MpcData mpc = {
  1500.0, 15.0, 11.0,
  NULL, ctd_dev
};

// Enable watch dog  HM 3/6/2014
short CustomSYPCR = WDT105s | HaltMonEnable | BusMonEnable | BMT32;

//
// Set IO pins, set SYSCLK
// ?? walk thru to verify all actions
//
void mpcInit(void) {
  ushort nsRAM, nsFlash, nsCF;
  short waitsFlash, waitsRAM, waitsCF, nsBusAdj;
  short deviceRX, deviceTX;

  // Define unused pins here ?? 1-14 20 27 28 34 38-41 43-50
  uchar mirrorpins[] = {15, 16, 17, 18, 19, 26, 36, 0};
  PIOMirrorList(mirrorpins);

  TMGSetSpeed(SYSCLK);
  CSSetSysAccessSpeeds(nsFlashStd, nsRAMStd, nsCFStd, WTMODE);
  CSGetSysAccessSpeeds(&nsFlash, &nsRAM, &nsCF, &nsBusAdj);
  CSGetSysWaits(&waitsFlash, &waitsRAM, &waitsCF); // auto-adjusted
  flogf(
      "\n%ukHz nsF:%d nsR:%d nsC:%d adj:%d WF:%-2d WR:%-2d WC:%-2d SYPCR:%02d",
      TMGGetSpeed(), nsFlash, nsRAM, nsCF, nsBusAdj, waitsFlash, waitsRAM,
      waitsCF, *(uchar *)0xFFFFFA21);
    mpcVoltage( &mpc.volts );
  flogf("\n\t|Check Startup Voltage: %5.2fV", mpc.volts);
  // com1
  deviceRX = TPUChanFromPin(COM1_RX);
  deviceTX = TPUChanFromPin(COM1_TX);
  mpc.com1 = TUOpen(deviceRX, deviceTX, COM1_BAUD, 0);
  if (mpc.com1==NULL) 
    sysShutdown("mpcInit() com1 open fail");
  delayms(RS232_SETTLE); // to settle rs232
  // Safety Check. Minimum Voltage
  if (mpc.volts < mpc.voltsMin) {
    flogf("\n\t|Battery Voltage Below Minimum. Activate Hibernation Mode");
    sysSleepUntilWoken();
    BIOSReset();
  }
} // mpcInit

//
// capture interrupt, used to wake up
//
static void IRQ4_ISR(void) {
  PIORead(IRQ4RXD);     // console
  RTE();
} // IRQ4_ISR
static void IRQ5_ISR(void) {
  PIORead(IRQ5);        // pam ??
  RTE();
} // IRQ5_ISR

//
// spurious interrupt, ignore
//
static void spur_ISR(void) {
  RTE();
} // spur_ISR

//
// Sleep until keypress or wispr
//
void mpcSleep(void) {
  ciflush(); // flush any junk
  clockTimeDate(scratch);
  flogf("\nmpcSleep() at %s", scratch);

  // Install the interrupt handlers that will break us out by "break signal"
  // ?? this should be a one time action, we toggle pins int|I/O
  IEVInsertAsmFunct(IRQ4_ISR, level4InterruptAutovector);
  IEVInsertAsmFunct(IRQ5_ISR, level5InterruptAutovector); // PAMPort ??
  IEVInsertAsmFunct(spur_ISR, spuriousInterrupt);

  PITSet51msPeriod(PITOff); // disable timer (drops power)
  CTMRun(false);            // turn off CTM6 module
  SCITxWaitCompletion();    // let any pending UART data finish
  EIAForceOff(true);        // turn off the RS232 driver
  QSMStop();                // shut down the QSM
  CFEnable(false);          // turn off the CompactFlash card

  // make it an interrupt pin
  PinBus(IRQ4RXD);          // console
  PinBus(IRQ5);             // wispr

  pet();      // another reprieve
  TMGSetSpeed(1600);
  while (PinTestIsItBus(IRQ4RXD) && PinTestIsItBus(IRQ5)) {
    // we loop here on spurious interrupt
    //*HM050613 added to reduce current when Silicon System CF card is used
    //*(ushort *)0xffffe00c=0xF000; //force CF card into Card-1 active mode

    LPStopCSE(FullStop); // we will be here until interrupted
    pet();
  }

  CSSetSysAccessSpeeds(nsFlashStd, nsRAMStd, nsCFStd, WTMODE);
  TMGSetSpeed(SYSCLK);

  // make it an I/O pin
  PIORead(IRQ4RXD);
  PIORead(IRQ5);

  EIAForceOff(false); // turn on the RS232 driver
  QSMRun();           // bring back the QSM
  CFEnable(true);     // turn on the CompactFlash card
  ciflush();          // discard any garbage characters
  clockTime(scratch);
  flogf(", wakeup at %s", scratch);
  putflush(); 
} // mpcSleep

float mpcVoltage(float *volts) {
  float v = 12.0;
  *volts = v;
  return v;
}

bool mpcDevSelect(DevType dev) {
  if (dev==mpc.device) return true;
  if (dev==ant_dev)
    PIOSet(COM1SELECT);
  else if (dev==ctd_dev)
    PIOClear(COM1SELECT);
  else
    return false;
  delayms(10);
  TUTxFlush(mpc.port);
  TURxFlush(mpc.port);
  pet();
  return true;
}

void mpcStop(){}

DevType mpcCom1Port(void) { return mpc.port; }

Serial mpcCom1Dev(void) { return mpc.device; }
