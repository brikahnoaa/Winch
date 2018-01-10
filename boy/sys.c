// sys.c
#include <common.h>
#include <sys.h>

#include <ant.h>
#include <boy.h>
#include <ctd.h>
#include <dat.h>
#include <mpc.h>
#include <ngk.h>

systemData sys = {
  true,
  "c:0000.log", "LR01", "LARA", "QUEH",
  0, 50,
}

IEV_C_PROTO(ExtFinishPulseRuptHandler);


/*
 * initHW and SW structures. loop over phase 1-4
 * sets: boy.com1 wis.com2 wis.com3 ngk.com4
 */
void main(void) {
  preRun(10);
  sysInit(&boy.com1, &wis.com2, &wis.com3, &ngk.com4);
  setConfFile(&ant, &boy, &ctd, &dat, &mpc, &ngk, &sys);
  boyInit();
  ctdInit();

  boyMain(sys.starts);
} // main

void preRun(int delay) {
  char c;
  TickleSWSR();
  ciflush();
  cprintf("\n press ' ' ([space]) to exit into picoDOS \n");
  flogf("You have %d seconds ", delay);
  while(--delay) {
    c = SCIRxGetCharWithTimeout(1000); // 1 second
    if (c == ' ')
      BIOSResetToPicoDOS();
    // any other char, run
    if (c != -1) // -1 for timeout
      return;
    cprintf(" %d ", delay);
  }
} // preRun

/*
 * init pins, ports, power
 */
void sysInit() {
  // global boy .port .device
  logInit(sys.logfile);
  Initflog(sys.logfile, true);
  sysStarts(&sys.starts);

  flogf("\nProgram: %s  Project ID: %s   Platform ID: %s  Boot ups: %d",
        MPC.PROGNAME, MPC.PROJID, MPC.PLTFRMID, MPC.STARTUPS);
  flogf("\nProgram Start time: %s", TimeDate(NULL));

  PZCacheSetup('C' - 'A', calloc, free);
  TUInit(calloc, free);  // enable TUAlloc for serial ports

  mpcVoltage( &mpc.volts );
  flogf("\n\t|Check Startup Voltage: %5.2fV", mpc.volts);

  // Safety Check. Minimum Voltage
  if (mpc.volts < mpc.voltMin) {
    flogf("\n\t|Battery Voltage Below Minimum. Activate Hibernation Mode");
    SleepUntilWoken();
    BIOSReset();
  }
} // sysInit()

/*
 * check STARTS_VEE>STARTSMAX_VEE to see if we are rebooting wildly
 * these two settings are in veeprom to allow check before startup
 */
void sysStarts(long *starts, long *startsMax) {
  starts = VEEFetchLong(STARTS_VEE, 0L);
  startsMax = VEEFetchLong(STARTSMAX_VEE, 0L);
  if (starts>startsMax) {
    flogf("\nsysStarts(): startups>startmax");
    shutdown();
  }
  VEEStoreLong(STARTS_VEE, starts+1L);
} // sysStarts


