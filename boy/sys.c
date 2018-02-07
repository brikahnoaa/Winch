// sys.c
#include <com.h>
#include <sys.h>

#include <ant.h>
#include <boy.h>
#include <cfg.h>
#include <ctd.h>
#include <mpc.h>
#include <ngk.h>
#include <../fun/winching.h> // ??

//
// Before deploying, set CF2 time and SM2 time, format SD cards,
// replace SM2 and CF2 clock batteries
// erase activity.log, set status=0, set startups=0,
// verify boottime is correct, verify pwrondepth and pwroffdepth are correct,
// make sure you have the correct mA multiplier set in the mAh calculation in
// status 5 for the SM2 settings that you are using (compression, kHz, etc.)
// as power consumption varies between those settings
//

systemData sys = {
  "c:sys.log", "LR01", "LARA", "QUEH", "4.0",
  0, 0, 50,
};

IEV_C_PROTO(ExtFinishPulseRuptHandler);


//
// initHW and SW structures. loop over phase 1-4
// sets: boy.com1 wis.com2 wis.com3 ngk.com4
//
void main(void) {
  preRun(10);
  startCheck();
  sysInit();

  mpcInit();
  //?? boyInit();
  //?? ctdInit();
  ngkInit();
  //?? boyMain(sys.starts);
  winchingMain();
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

void logInit() {
  Initflog(sys.log, true);
  flogf("\n----------------------------------------------------------------");
  flogf("\nProgram: %s,  Build: %s %s", __FILE__, __DATE__, __TIME__);
  flogf("\nSystem Parameters: CF2 SN %05ld, PicoDOS %d.%02d, BIOS %d.%02d",
        BIOSGVT.CF1SerNum, BIOSGVT.PICOVersion, BIOSGVT.PICORelease,
        BIOSGVT.BIOSVersion, BIOSGVT.BIOSRelease);
  flogf("\n----------------------------------------------------------------");
  fflush(NULL);
  TickleSWSR();
  cdrain();
  ciflush();
  coflush();
}

void sysInit() {
  logInit();
  configFile();
  PZCacheSetup('C' - 'A', calloc, free);
  TUInit(calloc, free);  // enable TUAlloc for serial ports
  flogf("\nProgram: %s  Version: %s  Project: %s  Platform: %s  Starts: %d",
    sys.program, sys.version, sys.project, sys.platform, sys.starts);
  flogf("\nStart at: %s", clockTimeDate(scratch));
} // sysInit

//
// close files ??, Sleep until keypress
// 2nd release 6/24/2002 by HM -Changed to use ADS8344/45
//
void sysSleepUntilWoken(void) {
  ciflush(); // flush any junk
  mpcSleep();
} // sysSleepUntilWoken

//
// close files, turn off devices, power off
//
void sysShutdown(void) {
} // sysShutdown

//
// read config from CONFIG_FILE
//
void configFile(void) {
} // configFile

//
// check STARTS>STARTSMAX to see if we are rebooting wildly
// sets: sys.cfg .log starts .startsMax
//
void startCheck(void) {
  sys.starts = atoi(VEEFetchStr("STARTS", STARTS)) + 1;
  sys.startsMax = atoi(VEEFetchStr("STARTS_MAX", STARTS_MAX));
  if (sys.starts>sys.startsMax) {
    // log file is not open yet, but still works as printf
    cprintf("\nstartCheck(): starts>startmax, so shutdown...\n");
    sysShutdown("starts>startmax");
  }
  // load cfg and log file names
  strcpy(sys.log, VEEFetchStr( "SYS_LOG", SYS_LOG ));
  strcpy(sys.cfg, VEEFetchStr( "SYS_CFG", SYS_CFG ));
  csprintf(scratch, "%d", sys.starts);
  VEEStoreStr("STARTS", scratch);
} // startCheck


//
// Setup directories for files not needing to be access anymore.
//
void dirSetup(char *path) {
  char DOSCommand[64];
  memset(DOSCommand, 0, 64);
  strncpy(DOSCommand, "mkdir ", 6);
  strncat(DOSCommand, path, 3);

  flogf("\n\t|MakeDirectory() %s", DOSCommand);
  putflush();
  CIOdrain();
  execstr(DOSCommand);
  delayms(1000);
} // sysDirSetup

//
//
int sysOSCmd(char *command, long filenum, char *ext, char *extt) {
  char Com[64];
  int r = 0;
  static char fname[] = "c:00000000.";
  memset(Com, 0, 64);

  if (strstr(command, "copy") != NULL) {
    strncpy(Com, "copy ", 5);
    sprintf(fname, "c:%08ld.", filenum);
    strncat(Com, fname, 11);
    strncat(Com, ext, 3);
    strncat(Com, " c:", 3);
    if (extt == NULL)
      strncat(Com, ext, 3);
    else
      strncat(Com, extt, 3);
    strncat(Com, "\\", 1);
    strncat(Com, &fname[2], 9);
    if (extt == NULL)
      strncat(Com, ext, 3);
    else
      strncat(Com, extt, 3);
  } else if (strstr(command, "del") != NULL) {
    strncpy(Com, "del ", 4);
    sprintf(fname, "c:%08ld.", filenum);
    strncat(Com, fname, 11);
    strncat(Com, ext, 3);
  } else if (strstr(command, "ren") != NULL) {
    strncpy(Com, "ren ", 4);
    sprintf(fname, "c:%08ld.", filenum);
    strncat(Com, fname, 11);
    strncat(Com, ext, 3);
    strncat(Com, " ", 1);
    strncat(Com, fname, 11);
    strncat(Com, extt, 3);
  }

  else if (strstr(command, "move") != NULL) {

    sysOSCmd("copy", filenum, ext, extt);
    sysOSCmd("del", filenum, ext, NULL);
    return r;
  }

  flogf("\n%s|COMDos() %s", clockTime(scratch), Com);
  putflush();
  CIOdrain();
  execstr(Com);
  delayms(250);
  return r;
} // sysOSCmd


//
//
void sysAlarm(AlarmType alm) { 
  sys.alarm[alm] += 1; 
}

//
// //Voltage: checking the average
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
//
int checkVitals(void) {
  return 0;
} // checkVitals

//
// void AppendFile
//
  /*
bool Append_Files(int Dest, const char *SourceFileName, bool erase,
                  long maxBytes) {
  int Source;
  int i;
  struct stat fileinfo;
  long BlockLength = 256;
  long NumBlks, LastBlkLength;
  char extension[] = "XXX";
  int byteswritten;
  // char filename[]="c:00000000.xxx";

  delayms(50);

  stat(SourceFileName, &fileinfo);
  if (fileinfo.st_size <= 0) {
    flogf("\nERROR  |Return upon no file to append");
    return false;
  }
  Source = open(SourceFileName, O_RDONLY);
  if (Source <= 0) {
    flogf("\nERROR  |AppendFiles() %s open errno: %d", SourceFileName, errno);
    return false;
  }
  DBG(else flogf("\n\t|Append_Files() %s opened", SourceFileName);)

  lseek(Source, 0, SEEK_SET);

  flogf("\n\t|Append_Files %s Size: %ld Bytes", SourceFileName,
        fileinfo.st_size);

  if (maxBytes <= 0)
    NumBlks = fileinfo.st_size / BlockLength;
  else
    NumBlks = maxBytes / BlockLength;

  LastBlkLength = fileinfo.st_size % BlockLength;

  for (i = 0; i <= NumBlks; i++) {

    if (i == NumBlks)
      BlockLength = LastBlkLength;
    memset(WriteBuffer, 0, 256 * sizeof(char));

    byteswritten = read(Source, WriteBuffer, BlockLength * sizeof(char));
    DBG1("\t|AppendFiles: bytes read: %d", byteswritten)
    if (i == NumBlks) {
      strcat(WriteBuffer, "\n");
      BlockLength++;
    }
    byteswritten = write(Dest, WriteBuffer, BlockLength * sizeof(char));
    DBG1("\t|AppendFiles: bytes written: %d", byteswritten)
  }

  if (erase) {
    if (close(Source) == 0) {
      DBG1("\t|Append_Files() %s Closed", SourceFileName)
      sprintf(extension, "%c%c%c", SourceFileName[11], SourceFileName[12],
              SourceFileName[13]);
      DOS_Com("del", MPC.FILENUM, extension, NULL);
    } else
      flogf("\nERROR  |AppendFiles() %s close errno: %d", SourceFileName,
            errno);
  } else {
    if (close(Source) != 0)
      flogf("\nERROR  |AppendFiles() %s close errno: %d", SourceFileName,
            errno);
    DBG(else flogf("\n\t|AppendFiles() %s Closed", SourceFileName);)
  }

  return true;

} // AppendFiles
  */ //??

