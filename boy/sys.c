// sys.c
#include <com.h>
#include <sys.h>

#include <ant.h>
#include <boy.h>
#include <cfg.h>
#include <ctd.h>
#include <mpc.h>
#include <ngk.h>
#include <pwr.h>
#include <wsp.h>

SysInfo sys;

///
// Before deploying, set CF2 time and SM2 time, format SD cards,
// replace SM2 and CF2 clock batteries
// erase activity.log, set status=0, set startups=0,
// verify boottime is correct, verify pwrondepth and pwroffdepth are correct,
// make sure you have the correct mA multiplier set in the mAh calculation in
// status 5 for the SM2 settings that you are using (compression, kHz, etc.)
// as power consumption varies between those settings
//

IEV_C_PROTO(ExtFinishPulseRuptHandler);


///
// pre, starts, config, log, pico
// uses: sys.starts
int sysInit(void) {
  preRun(10);
  sys.starts = startCheck();
  comInit();              // dbg0,1,2
  configFile();
  logInit(sys.logFile);
  TUInit(calloc, free);   // enable TUAlloc for serial ports
  flogf("\nProgram: %s  Version: %s  Project: %s  Platform: %s  Starts: %d",
    sys.program, sys.version, sys.project, sys.platform, sys.starts);
  flogf("\nStart at: %s", clockTimeDate(scratch));
  return sys.starts;
} // sysInit

///
// short delay for operator abort
void preRun(int delay) {
  char c;
  pet();
  ciflush();
  cprintf("\n press ' ' ([space]) to exit into picoDOS \n");
  cprintf("You have %d seconds ", delay);
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

///
// check STARTS>STARTSMAX to see if we are rebooting wildly
// returns: starts
int startCheck(void) {
  int cnt, max;
  max = atoi(VEEFetchStr("STARTS_MAX", STARTS_MAX));
  cnt = atoi(VEEFetchStr("STARTS", STARTS)) + 1;
  sprintf(scratch, "%d", cnt);
  VEEStoreStr("STARTS", scratch);
  // log file is not open yet
  cprintf("\nstartCheck(): starts %d, max %d", cnt, max);
  if (cnt>max)
    sysStop("starts>max");
  return cnt;
} // startCheck

///
// read config from CONFIG_FILE
void configFile(void) {
  // load cfg and log file names
  strcpy(sys.logFile, VEEFetchStr( "SYS_LOG", SYS_LOG ));
  strcpy(sys.cfgFile, VEEFetchStr( "SYS_CFG", SYS_CFG ));
  cfgRead(sys.cfgFile);
  if (sys.cfgWild) {
    // wildcard match for config files
    // ??
  }
} // configFile

void logInit(char *file) {
  pet();
  PZCacheSetup(C_DRV, calloc, free);
  Initflog(file, true);
  flogf("\n----------------------------------------------------------------");
  flogf("\nProgram: %s,  Build: %s %s", __FILE__, __DATE__, __TIME__);
  flogf("\nSystem Parameters: CF2 SN %05ld, PicoDOS %d.%02d, BIOS %d.%02d",
        BIOSGVT.CF1SerNum, BIOSGVT.PICOVersion, BIOSGVT.PICORelease,
        BIOSGVT.BIOSVersion, BIOSGVT.BIOSRelease);
  flogf("\n----------------------------------------------------------------");
  fflush(NULL);
  cdrain();
  ciflush();
  coflush();
} // logInit

///
// close files ??, Sleep until keypress
// 2nd release 6/24/2002 by HM -Changed to use ADS8344/45
void sysSleep(void) {
  mpcSleep();
} // sysSleep

///
// close files, turn off devices, power off
void sysStop(char *out) {
  VEEStoreStr("SHUTDOWN", out);
  antStop();
  boyStop();
  ngkStop();
  pwrStop();
  wspStop();
  sysSleep();
  BIOSReset();
} // sysStop

///
//
void sysAlarm(AlarmType alm) { 
  sys.alarm[alm] += 1; 
} // sysAlarm

///
// Voltage: checking the average
// 1- Check Absolute MIN volts
// 2- Check User min volts
// 3- Check Startups
// 4- Check WISPR freespace (RAOSBottom)
// 5- Check CF2 CF freespace
// return -1 on need to shutdown
// return 0 if System is fine
// return 1 if below absolute Min Bat Voltage
// return 2 if below MinVolt
// return 3 if Min Bat Capacity
// return 4 if MIN WISPR FREE Space
// return 5 if No CF2 Free Space
int checkVitals(void) {
  return 0;
} // checkVitals

///
// sysDiskFree Returns the free space in kBytes
long sysDiskFree(void) {
  sys.diskFree = DSDFreeSectors(C_DRV);
  sys.diskSize = DSDDataSectors(C_DRV);
  return sys.diskFree/2;
} // sysDiskFree

///
// call flush for each module with logging
void sysFlush(void) {
  boyFlush();
  ctdFlush();
  pwrFlush();
  wspFlush();
} // sysFlush
