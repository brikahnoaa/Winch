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
  setConfFile(&ant, &boy, &ctd, &dat, &mpc, &ngk, &sys);
  // sysInit(&boy.com1, &wis.com2, &wis.com3, &ngk.com4);
  sysInit();
  mpcInit();
  boyInit();
  ctdInit();
  ngkInit();
  datInit();

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
 * init files, basic checks
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


/*
 * Setup directories for files not needing to be access anymore.
 */
void Make_Directory(char *path) {
  char DOSCommand[64];
  memset(DOSCommand, 0, 64);
  strncpy(DOSCommand, "mkdir ", 6);
  strncat(DOSCommand, path, 3);

  flogf("\n%s|MakeDirectory() %s", Time(NULL), DOSCommand);
  putflush();
  CIOdrain();
  execstr(DOSCommand);
  delayms(1000);
}

/*
 * MakeDirectory()
 * Setup directories for files not needing to be access anymore.
 * AT 7/13/2015
 */
void DOS_Com(char *command, long filenum, char *ext, char *extt) {
  char Com[64];
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

    DOS_Com("copy", filenum, ext, extt);
    DOS_Com("del", filenum, ext, NULL);
    return;
  }

  flogf("\n%s|COMDos() %s", Time(NULL), Com);
  putflush();
  CIOdrain();
  execstr(Com);
  delayms(250);
}


/*
 * Check_Vitals
 * //Voltage: checking the average
 * 1- Check Absolute MIN volts
 * 2- Check User min volts
 * 3- Check Startups
 * 4- Check WISPR freespace (RAOSBottom)
 * 5- Check CF2 CF freespace
 * return -1 on need to shutdown
 * return 0 if System is fine
 * return 1 if below absolute Min Bat Voltage
 * return 2 if below MinVolt
 * return 3 if Min Bat Capacity
 * return 4 if MIN WISPR FREE Space
 * return 5 if No CF2 Free Space
 */
short Check_Vitals(void) {
  short returnvalue = 0;
  float currentvoltage;
  float minvolt;

  minvolt = atof(ADS.MINVOLT); // Grab User defined minimum voltage.
  currentvoltage = Get_Voltage(); // Current Voltage

  if (currentvoltage == 0.00)
    returnvalue = 0;
  else if (currentvoltage <= minvolt) {
    flogf("\n%s|Check_Vitals():", Time(NULL));
    flogf("\n\t|Voltage: %4.2f", currentvoltage);
    if (currentvoltage <= MIN_BATTERY_VOLTAGE) {
      flogf("below absolute min.");
      returnvalue = 1; // System Shutdown
      return returnvalue;
    }
    else if (currentvoltage <= atof(ADS.MINVOLT)) {
      flogf("below user set min.");
      returnvalue = 2; // Hibernate? Stop WISPR?
    }
  }
  // Free_Disk_Space() somewhere else
  if (SystemFreeSpace < 100) {
    flogf("\nCheck_Vitals(): CF2 Free Space: %ldkB", SystemFreeSpace);
    returnvalue = 5; // Delete Files? Hibernate?
  }
  if (MPC.STARTUPS >= MPC.STARTMAX) {
    flogf("\n%s|Check_Vitals():", Time(NULL));
    flogf("\n\t|Startups surpassed maximum: %d", MPC.STARTUPS);
    returnvalue = 3; // System Shutdown.
  }
  return returnvalue;
} // Check_Vitals

/*
 * void AppendFile
 */
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

/*
 * Free_Disk_Space *   Returns the free space in kBytes
 */
long Free_Disk_Space(void) {
  long freeSpacekB;
  long freeSectors;
  long totalSectors;

  freeSectors = DSDFreeSectors('C' - 'A');
  totalSectors = DSDDataSectors('C' - 'A');
  freeSpacekB = freeSectors / 2;
  flogf("\n\t|FreeDiskSpace %ldkB", freeSpacekB);

  return freeSpacekB;
} 
