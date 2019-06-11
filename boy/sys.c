// sys.c
#include <main.h>

#define STARTS "0"
#define STARTS_MAX "50"
#define SYS_LOG "SYS.LOG"
#define C_DRV ('C'-'A')

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
// uses: sys.logfile
// sets: all.starts
int sysInit(void) {
  TUChParams *params;
  short qsize = 64*1024;
  preRun(10);
  all.starts = startCheck();
  time(&all.startProg);     // program start time, global
  time(&all.startCycle);    // cycle start time, global
  // need utlInit before logInit
  utlInit();                // malloc global all.str
  logInit(sys.logFile);     // stores flogf filename, found in VEE.sys_log
  dbgInit();                // common init: dbg0,1,2
  cfgInit();
  // make serial queues larger = 64K (rudics max block size)
  params = TUGetDefaultParams();
  params->rxqsz = qsize;
  params->txqsz = qsize;
  TUSetDefaultParams( params );
  // enable TUAlloc for serial ports
  TUInit(calloc, free);   
  return all.starts;
} // sysInit

///
// short delay for operator abort
void preRun(int delay) {
  char c;
  utlPet();
  cprintf("\n press ' ' ([space]) to exit into picoDOS \n");
  ciflush();
  // start
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
// note - dbgInit, sysInit called after this
// returns: starts
int startCheck(void) {
  int cnt, max;
  char str[128];
  max = atoi(VEEFetchStr("STARTS_MAX", STARTS_MAX));
  cnt = atoi(VEEFetchStr("STARTS", STARTS)) + 1;
  sprintf(str, "%d", cnt);
  VEEStoreStr("STARTS", str);
  // log file is not open yet
  cprintf("\nstartCheck(): starts %d, max %d", cnt, max);
  if (cnt>max) {
    sysStop("starts>max");
    BIOSResetToPicoDOS();
  }
  return cnt;
} // startCheck

///
// opens logfile named in pico var SYS_LOG, or defaults to sys.log
// copies logfile and deletes it before opening
// sets: (*file)
void logInit(char *file) {
  static char *self="logInit";
  char *dt, cmd[64];
  struct stat finfo;
  DBG0("logInit(%s)", file);
  utlPet();
  PZCacheSetup(C_DRV, calloc, free);
  strcpy(file, VEEFetchStr( "SYS_LOG", SYS_LOG ));
  // copy to log\MMDDHHMM.sys
  if (stat(file, &finfo)>=0) { // sys.log exists
    dt=utlDateTimeS16();
    sprintf(cmd, "copy %s log\\%.4s%.4s.sys", file, dt, dt+8);
    fprintf("\n%s: '%s'", self, cmd);
    execstr(cmd);
    sprintf(cmd, "del %s", file);
    fprintf("\n%s: '%s'", self, cmd);
    execstr(cmd);
  }
  //
  Initflog(file, true);
  flogf("\n---   ---");
  flogf("\nProgram: %s,  Build: %s %s", __FILE__, __DATE__, __TIME__);
  flogf("\nSystem Parameters: CF2 SN %05ld, PicoDOS %d.%02d, BIOS %d.%02d",
        BIOSGVT.CF1SerNum, BIOSGVT.PICOVersion, BIOSGVT.PICORelease,
        BIOSGVT.BIOSVersion, BIOSGVT.BIOSRelease);
  flogf("\nProgram: %s  Version: %s  Starts: %d",
    sys.program, sys.version, all.starts);
  flogf("\nStarted: %s", utlDateTime());
  flogf("\n---   ---");
  fflush(NULL);               // ??
  cdrain();
  ciflush();
  coflush();
} // logInit

///
// close files, turn off devices, power off
void sysStop(char *out) {
  VEEStoreStr("SHUTDOWN", out);
  antStop();
  boyStop();
  s16Stop();
  ngkStop();
  pwrStop();
  wspStop();
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
  pwrFlush();
  // antFlush();
  // s16Flush();
} // sysFlush
