// mpc.c - hardware, mpc specific
#include <utl.h>
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


// Enable watch dog  HM 3/6/2014
short CustomSYPCR = WDT105s | HaltMonEnable | BusMonEnable | BMT32;

//
// Set IO pins, set SYSCLK
// ?? walk thru to verify all actions
//
void mpcInit(void) {
  ushort nsRAM, nsFlash, nsCF;
  short waitsFlash, waitsRAM, waitsCF, nsBusAdj;
  short rx, tx;
  uchar unusedpins[] = {15, 16, 17, 18, 19, 36, 0};
  uchar outputpins[] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 0};
  PIOMirrorList(unusedpins);
  PIOMirrorList(outputpins);
  // setup pam port, shared by wispr and science ctd sbe16
  rx = TPUChanFromPin(PAM_RX);
  tx = TPUChanFromPin(PAM_TX);
  mpc.pamDev = null_pam;
  mpc.pamPort = TUOpen(rx, tx, PAM_BAUD, 0);
  if (mpc.port==NULL)
    utlStop("mpcInit() pam open fail");
  utlDelay(RS232_SETTLE); // to settle rs232
  TUTxFlush(mpc.port);
  TURxFlush(mpc.port);

  TMGSetSpeed(SYSCLK);
  CSSetSysAccessSpeeds(nsFlashStd, nsRAMStd, nsCFStd, WTMODE);
  CSGetSysAccessSpeeds(&nsFlash, &nsRAM, &nsCF, &nsBusAdj);
  CSGetSysWaits(&waitsFlash, &waitsRAM, &waitsCF); // auto-adjusted
  flogf(
      "\n%ukHz nsF:%d nsR:%d nsC:%d adj:%d WF:%-2d WR:%-2d WC:%-2d SYPCR:%02d",
      TMGGetSpeed(), nsFlash, nsRAM, nsCF, nsBusAdj, waitsFlash, waitsRAM,
      waitsCF, *(uchar *)0xFFFFFA21);
} // mpcInit

///
// pam port shares rx/tx between com3, com4
// switch between devices on pam port, clear 
void mpcPamDev(PamType pam) {
  if (pam==mpc.pam) return;
  DBG0("mpcPam(%d)", pam)
  switch (mpc.pamDev) {
  case wsp1_pam;
  case wsp2_pam;
    mpcPamPulse(WISPR_PWR_OFF);
    break;
  } // switch
  switch (pam) {
  case wsp1_pam;
    PIOClear(SBE_PAM);
    PIOClear(SBE_16);
    PIOSet(WSP_PAM);
    PIOClear(WSP_12);
    mpcPamPulse(WISPR_PWR_ON);
    break;
  case wsp2_pam;
    PIOClear(SBE_PAM);
    PIOClear(SBE_16);
    PIOSet(WSP_PAM);
    PIOSet(WSP_12);
    mpcPamPulse(WISPR_PWR_ON);
    break;
  case sbe16_pam:
    PIOClear(WSP_PAM);
    PIOClear(WSP_12);
    PIOSet(SBE_PAM);
    PIOSet(SBE_16);
    break;
  case null_pam
    PIOClear(WSP_PAM);
    PIOClear(WSP_12);
    PIOClear(SBE_PAM);
    PIOClear(SBE_16);
    break;
  } // switch
  TUTxFlush(mpc.pamPort);
  TURxFlush(mpc.pamPort);
  utlPet();
  mpc.pamDev = pam;
  return;
} // mpcPam

///
void mpcPamPulse(int pin) {
  PIOSet(pin);
  utlDelay(1);
  PIOClear(pin);
} // mpcPamPulse

///
// pam port is shared
Serial mpcPamPort(void) {
  return mpc.pamPort;
 } // mpcPort

///
// capture interrupt, used to wake up
static void IRQ4_ISR(void) {
  PIORead(IRQ4RXD);     // console
  RTE();
} // IRQ4_ISR

///
static void IRQ5_ISR(void) {
  PIORead(IRQ5);        // pam ??
  RTE();
} // IRQ5_ISR

///
// spurious interrupt, ignore
static void spur_ISR(void) {
  RTE();
} // spur_ISR

///
// Sleep until keypress or wispr
void mpcSleep(void) {
  ciflush(); // flush any junk
  flogf("\nmpcSleep() at %s", utlDateTime());
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

  utlPet();      // another reprieve
  TMGSetSpeed(1600);
  while (PinTestIsItBus(IRQ4RXD) && PinTestIsItBus(IRQ5)) {
    // we loop here on spurious interrupt
    //*HM050613 added to reduce current when Silicon System CF card is used
    //*(ushort *)0xffffe00c=0xF000; //force CF card into Card-1 active mode

    LPStopCSE(FullStop); // we will be here until interrupted
    utlPet();
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
  flogf(", wakeup at %s", utlTime());
  putflush(); 
} // mpcSleep

void mpcStop(){}
