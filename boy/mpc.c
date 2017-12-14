// MPC.c - pre-run and hardware
#include <common.h>
#include <mpc.h>

/*
 * "Only pin should not be mirrored is pin 20. It is connected to the
 * output of AD. Pin 20 is input and should not be mirrored. Also making pin
 * 19 high turns on A/D, which increases the power usage by a few milliamps.
 * Only pin safe to mirror is pin 15 (/IRQ7).  Alex code uses the A/D to
 * monitor the voltage and current, it probably does not matter." - haru
 */

char time_chr[21];

typedef enum { 
  Nada_it=0,
  Watch_it=1,
  Power_it=1<<1,
  Winch_it=1<<2,
  Wispr_it=1<<3,
  Data_it=1<<4,
  Call_it=1<<5,
  } IntervalTimer;

typdef struct ITnode {
  long time;
  IntervalTimer it;
  ITnode *prev, *next;
  } = {0L, Nada_it, NULL, NULL} ITnode;

/*
 * interval timer, abstracted for general purpose
 * call often. triggers watchdog reset and power monitor (144s)
 * fast check for next timer, so low overhead
 * auto regen for some types, but must be seeded during init
 * 
 * Returns: enum IntervalType * use bitshifted values i.e. flags
 * Nada_it
 * Watch_it - emergency watchdog
 * Power_it - record power stats
 * Winch_it - expected response
 * Wispr_it - detection duty cycle
 * Data_it - surface and phone home
 * Call_it - give up call and descend
 */
IntervalTimer addIT(void) {
  debug0("addIT()")
  static long nextTime;
  static ITnode *next;
  static ITNode head;
} // mpcTimer

int System_Timer(void) {
  int remainder;
  int adcount = 0;
  time_t rawtime;
  struct tm *info;
  static int DataTimer;
  float vel, depth;

  // Counting the number of ADS file writes, Timing is a function of this number
  // ?? use time instead
  adcount = power.counter;

  // To avoid Division by zero: we can just return here.
  if (adcount == 0)
    return 0;
  remainder = adcount % DetectionInt;

  // Intermittent check for WISPR Duty Cycle.
  if (DutyCycleTicks > 0)
    if (DetectionInt > DutyCycleTicks && !WISPR_Status())
      if ((adcount % (DetectionInt - DutyCycleTicks)) == 0)
        WISPRPower(true);
      // If DUTYCYCL is zero
      else if (WISPR_Status())
        WISPRPower(false);

  // Regular Detection Interval
  if (remainder == 0) {
    RTS(DataTimer++;)
    flogf("\n%s|System_Timer(%d/%d):", Time(NULL), DataTimer, DataInterval);

    // If WISPR ON
    if (WISPR_Status()) {
      flogf("\n\t|Detection Call: Requesting %d DTX", WISP.DETMAX);
      WISPRDet(WISP.DETMAX);
      Delayms(100);
      WISPRExpectedReturn(3, false);
      if (WISP.DUTYCYCL < 100) {
        flogf("\n\t|WISPR Duty Cycle");
        WISPRSafeShutdown();
      } else {
        WISPRDFP();
        WISPRExpectedReturn(2, true);
      }
    }

    // CTD Measurements
    // Get an average of CTD Depth and calculate velocity
    depth = CTD_AverageDepth(9, &vel);
    // If initial average is less than target depth... why?
    if (depth <= NIGK.TDEPTH) {
      // Average more CTD Depth
      depth = CTD_AverageDepth(20, &vel);
      // Still Less than target depth? Set in recovery mode. Set calling
      // interval to 30 minutes.
      if (depth < NIGK.TDEPTH) {
        flogf(
            "\nERROR|System_Timer(CTD): DEPTH LESS THAN TARGET DEPTH!"); 
        // Set NIGK to recovery?
        NIGK.RECOVERY = 1;
        VEEStoreShort(NIGKRECOVERY_NAME, NIGK.RECOVERY);
        // Call every 30 minutes at surface.
        if (MPC.DATAXINT != 30) { 
          MPC.DATAXINT = 30;
          VEEStoreShort(DATAXINTERVAL_NAME, MPC.DATAXINT); 
        }
        return 2;
      }
    }

    // Reset AD Counter for next Detection Interval
    resetPowerCounter();
  }

  return 1;
} // System_Timer

/*
 * PreRun		Exit opportunity before we start the program
 */
void PreRun(void) {
  short ndelay = 10;
  short i;
  char c;
  time_t nowsecs;
  char strbuf[64];
  struct tm t;
  ushort Ticks;
  char *ProgramDescription = { "\n" "press '.' to exit into DOS" "\n"};

  nowsecs = RTCGetTime(NULL, &Ticks); // get RTC clock right now
  t = *localtime(&nowsecs);
  strftime(strbuf, sizeof(strbuf), "%m/%d/%Y  %H:%M:%S", &t);

  flogf("\nProgram start time: %s.%.3d [ctime: %lu]\n", strbuf, Ticks / 40,
        nowsecs);
  cprintf(ProgramDescription);
  flogf("You have %d seconds to launch\n", ndelay);

  TickleSWSR();

  for (i = ndelay; i > 0; i--) {
    cprintf("%u..", i);
    c = SCIRxGetCharWithTimeout(1000); // 1 second
    if (c == '.')
      BIOSResetToPicoDOS();
    // any other char, run
    if (c != -1) // !timeout
      return;
  }
} // PreRun

/*
 * Set IO pins, set SYSCLK
 */
void initMPC(void) {
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
} // initMPC
  
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
  Delayms(1000);
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
  Delayms(250);
}

/*
 * Time String
 * push the RTC seconds into (*seconds) and return HH:MM:SS global *time_chr
 */
char *Time(ulong *seconds) {
  // global time_chr
  RTCtm *rtc_time;
  ulong secs = NULL;

  ushort ticks;

  RTCGetTime(&secs, &ticks);
  rtc_time = RTClocaltime(&secs);
  *seconds = secs;
  sprintf(time_chr, "%02d:%02d:%02d", 
          rtc_time->tm_hour, rtc_time->tm_min, rtc_time->tm_sec);
  return time_chr;

} // Time
/*
 * Time & Date String
 * Get the RTC time seconds since 1970 and convert it 
 */
char *TimeDate(ulong *seconds) {

  RTCtm *rtc_time;
  ulong secs = NULL;

  ushort ticks;

  RTCGetTime(&secs, &ticks);
  rtc_time = RTClocaltime(&secs);
  *seconds = secs;
  sprintf(time_chr, "%02d/%02d/%04d %02d:%02d:%02d", rtc_time->tm_mon + 1,
          rtc_time->tm_mday, rtc_time->tm_year + 1900, rtc_time->tm_hour,
          rtc_time->tm_min, rtc_time->tm_sec);
  return time_chr;

} // TimeDate

/*
 * Check the input values from cmdfile() and makes sure  they are divisible by
SysTimeInt. If not, rounds them up.
 * PLI: Power Log Interval
 */
float Check_Timers(ushort PLI) {
  DBG0("Check_Timers()")
  float callrate;
  ulong rawtime;
  struct tm *info;
  int minutes, hours, hour;
  // PLI = adstime = 1044

  // Rounding of our Detection Timer with the Power Logging Cycle "SysTimeInt"
  // ??
  DetectionInt = (((long)(MPC.DETINT * 600L) + (int)(PLI - 1)) / PLI);
  callrate = DetectionInt * (PLI / 600.0);

  flogf("\n\t|Recording Interval: %4.2f minutes", callrate);
  DBG1("\t|Detection Int: %d", DetectionInt)

  if (IRID.CALLMODE == 0 || NIGK.RECOVERY) {
    DataInterval = (MPC.DATAXINT + (MPC.DETINT - 1)) / MPC.DETINT;
    callrate = callrate * DataInterval;
    if (NIGK.RECOVERY)
      flogf("\n\t|RECOVERY MODE");
    flogf("\n\t|Data Interval %4.2f minutes", callrate);
    DBG1("\t|DataInterval: %d", DataInterval)
  } else if (IRID.CALLMODE == 1) {
    time(&rawtime);
    info = gmtime(&rawtime);
    flogf("\n\t|CallTime: %d, Hour Now; %d", IRID.CALLHOUR, info->tm_hour);
    if ((int)IRID.CALLHOUR <= info->tm_hour) {
      hour = IRID.CALLHOUR + 23;
      hours = hour - info->tm_hour;
    } else {
      hour = info->tm_hour;
      hours = IRID.CALLHOUR - hour - 1;
    }
    flogf("\n\t|Hours: %d", hours);
    minutes = 60 - info->tm_min;
    flogf("Minutes: %d", minutes);
    minutes = minutes + (60 * hours);
    flogf("\n\t|Time until call: %d minutes", minutes);
  }


  // Calculate Duty Cycle
  DutyCycleTicks = (int)(DetectionInt * ((float)(WISP.DUTYCYCL / 100.0)));
  flogf("\n\t|WISPR Duty Cycle: %d\%", WISP.DUTYCYCL);
  DBG1("\t|DutyCycleTicks: %d", DutyCycleTicks)
  // If some kind of Duty Cycle, keep Wispr Off until duty cycle begins
  // Reset SystemTimer by zeroing out ADCounter
  resetPowerCounter();
  return callrate;
} // Check_Timers

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

  Delayms(50);

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

/*
 * VEEStoreShort(char*)
 */
void VEEStoreShort(char *veename, short value) {

  char string[sizeof "00000"];

  memset(string, 0, 5);
  sprintf(string, "%d", value);
  VEEStoreStr(veename, string);

} // VEEStoreShort

void print_clock_cycle_count(clock_t start, clock_t stop, char *label) {

  flogf("\n%f seconds for %s",
        ((double)(stop - start)) / (double)CLOCKS_PER_SEC, label);
}
