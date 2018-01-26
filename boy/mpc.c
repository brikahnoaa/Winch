// MPC.c - hardware, mpc specific
#include <common.h>
#include <mpc.h>

MpcData mpc = {
  1500.0, 15.0, 12.5,
  NULL, ctd_dev,
};

// Enable watch dog  HM 3/6/2014
short CustomSYPCR = WDT105s | HaltMonEnable | BusMonEnable | BMT32;

/*
 * Set IO pins, set SYSCLK
 */
void mpcInit(void) {
  short waitsFlash, waitsRAM, waitsCF;
  ushort nsRAM, nsFlash, nsCF;
  short nsBusAdj;

  // Define unused pins here ?? 1-14 20 27 28 34 38-41 43-50
  uchar mirrorpins[] = {15, 16, 17, 18, 19, 26, 36, 0};
  PIOMirrorList(mirrorpins);

  CSSetSysAccessSpeeds(nsFlashStd, nsRAMStd, nsCFStd, WTMODE);
  TMGSetSpeed(SYSCLK);

  flogf("\n----------------------------------------------------------------");
  flogf("\nProgram: %s,  Version: %3.2f,  Build: %s %s", __FILE__, PROG_VERSION,
        __DATE__, __TIME__);
  CSGetSysAccessSpeeds(&nsFlash, &nsRAM, &nsCF, &nsBusAdj);
  flogf("\nSystem Parameters: CF2 SN %05ld, PicoDOS %d.%02d, BIOS %d.%02d",
        BIOSGVT.CF1SerNum, BIOSGVT.PICOVersion, BIOSGVT.PICORelease,
        BIOSGVT.BIOSVersion, BIOSGVT.BIOSRelease);
  CSGetSysWaits(&waitsFlash, &waitsRAM, &waitsCF); // auto-adjusted
  cprintf(
      "\n%ukHz nsF:%d nsR:%d nsC:%d adj:%d WF:%-2d WR:%-2d WC:%-2d SYPCR:%02d",
      TMGGetSpeed(), nsFlash, nsRAM, nsCF, nsBusAdj, waitsFlash, waitsRAM,
      waitsCF, *(uchar *)0xFFFFFA21);
  fflush(NULL);
  TickleSWSR();
  cdrain();
  ciflush();
  coflush();
} // mpcInit
  

/*
 * VEEStoreShort(char*)
 */
void VEEStoreShort(char *veename, short value) {
  char string[sizeof "00000"];
  memset(string, 0, 5);
  sprintf(string, "%d", value);
  VEEStoreStr(veename, string);
} // VEEStoreShort


