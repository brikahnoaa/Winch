// pow.c
#include <com.h>
#include <pwr.h>

PwrInfo pwr;

void pwrInit(void){}

void pwrStop(void){}

/*
//
// 12.2.2015 - Received many errno=0 when fopen return NULL. 
// No idea why. waiting to hear back from JG 
//
// Summing "shorts" for giving sampling interval which lasts
// #seconds=(2^#bits*PITRATE*PITINT)
// Here we can average a large number of samples at a quick rate due to
// bit shift division. At the end of every average, we write the three
// different "ushort" values (current, voltage, and time of sampling)
// to the power file.  Only when power monitor is called does the power
// file sum its "ushort" values and divided again ( non-bit shifted
// integer division) before it is converted to a floating point value with
// "CFxADRawToVolts(...)"  This makes for a very fast and efficient power
// logging process.
// 
// 1) BITSHIFT of 10 results in 25.6 secon buffers, 11: 51.2, 12: 102.4 etc.
// len(short)=2B, len(int)=4B, so room for at least 2^15 += short
// 16? Too big? only if unsigned short and signed int
// 
// 2) pwr.interval will be saved as a "ushort" in decisecs
//

IEV_C_PROTO(ADTimingRuptHandler);
IEV_C_PROTO(ADSamplingRuptHandler); // ??

extern volatile clock_t start_clock;
extern volatile clock_t stop_clock;

ushort intervalSamples;
ushort bitshift;

// File to save all values for Power Logging;
CFxAD *ad, adbuf;

// ADSample is ptr returned by CFxADQueueToArray(), alloc'd by magic
short *ADSample;
long powerSum[2] = {0, 0};

// Parameters Summed for calculation of ADS
static long voltsSum = 0L; // Summation of channel 1 from QSPI sampling function
static long currentSum = 0L; // Summation of channel 0 from QSPI...
static long sampleCnt = 0L;  // if sampleCnt == intervalSamples, p.sampleReady=true

ushort pwr.voltsMin = 0;
ushort pwr.currentMax = 0;

float voltage = 0.0;
static char ADAvgFileName[] = "c:00000000.pwr";

// bool off; bool sampleReady; float minBatCharge; float minBatVolt;
// int counter; int filehdl; long batCap; short interval;
// ushort currentMax; ushort voltsMin;
PowerInfo power = {
  true, false, 150.0, 12.5,
  0, 0, 123000, 1440, 
  99, 99,
  }

void resetPowerCounter(void) { pwr.counter = 0; }
float getVoltage(void) { return voltage; } 
void ADSFileName(long id) { sprintf(&ADAvgFileName[2], "%08ld.pwr", id); }

//
// ADTimingRuptHandler Chore		Initiate conversion
// Makes sure QSM is running and repeats previous synchronization
//
IEV_C_FUNCT(ADTimingRuptHandler) {
// implied (IEVStack *ievstack:__a0) parameter
#pragma unused(ievstack)
  static long currentSum, voltsSum;     // these could be int
  static short sampleCnt;
  QSMRun();
  QPBRepeatAsync(); // starts the QSPI running with previous parameters

  // QSPI driver function to sample '2' channels from
  // 'ad' and save into ushort array 'ADSample'
  ADSample = (short *)CFxADQueueToArray( ad, (void *)QRR, 2); 
  // function does magic alloc, or has static store

  currentSum += (long)(ADSample[0]); // Here we sum the voltage and current
  voltsSum += (long)(ADSample[1]);
  sampleCnt++;

  if (ADSample[0] > pwr.currentMax) {
    pwr.currentMax = ADSample[0];
    pwr.voltsMin = ADSample[1];
  }

  if (sampleCnt >= intervalSamples) {
    powerSum[0] = currentSum;
    powerSum[1] = voltsSum;
    currentSum = 0;
    voltsSum = 0;
    pwr.sampleReady = true;
    sampleCnt = 0;
  }
} // ADTimingRuptHandler

//
// Move raw QPSI data to main buffer
//
IEV_C_FUNCT(ADSamplingRuptHandler) {
// implied (IEVStack *ievstack:__a0) parameter
#pragma unused(ievstack)
  // #define SIM_PITR_DEF_IPL        6 
  CPUWriteInterruptMask(SIM_PITR_DEF_IPL);
  QPBClearInterrupt();
} // ADSamplingRuptHandler

bool powCheck(void) {
  if (pwr.sampleReady == true && !pwr.off) {
    pet();
    powLog();
    return true;
  } else
    return false;
}

//
// Set up AD to sample voltage and current usage.
// Name the file name with 8-digit numeral as a counter
// No need to calculate current upon Power off
//
  pwr.off = !ads_on;
  if (!pwr.off) {
    bitshift = val;
    powOpenLog(filecounter);
    flogf("\n%s|ADS(%s)", Time(NULL), ADAvgFileName);
    Setup_Acquisition(bitshift);

  } else {
    PITSet100usPeriod(PITOff); // Stop sampling
    PITRemoveChore(0);
    delayms(10);
    pwr.sampleReady = true;
    pwr.counter = 0;
  }
  return pwr.interval;
} // void SetUpADS
  
//
// Void OpenAvgFile()
//
void powOpenLog(long counter) {

  sprintf(&ADAvgFileName[2], "%08ld.pwr", counter);
  delayms(25);
  ADSFileHandle = creat(ADAvgFileName, 0);
  if (ADSFileHandle <= 0) {
    flogf("\nCouldn't Open %s file, errno%d", ADAvgFileName, errno);
    return;
  }
  if (close(ADSFileHandle) != 0)
    flogf("\nERROR  |powOpenLog() %s Close error: %d", ADAvgFileName, errno);

  delayms(10);

} // void OpenAvgFile

//
// Setup Acquisition
//
void Setup_Acquisition(ushort bitshift) {
  // global ADSample ad 
  double vref = VREF;
  bool uni = true; // true for unipolar, false for bipolar
  bool sgl = true; // true for single-ended, false for differential

  PITInit(6);               // PIT Priority Level
  PITSet51msPeriod(PITOff); // Assert Both PIT Timers off at this point.
  PITSet100usPeriod(PITOff);

  delayms(20);

  powerSum[0] = 0L;
  powerSum[1] = 0L;

  // Initialize AD Slot and Lock
  ad = CFxADInit(&adbuf, ADSLOT, ADInitFunction);
  if (!CFxADLock(ad)) {
    flogf("\nCouldn't lock and own A-D with QSPI\n");
    return;
  } // Lock in the ADDataBuf

  // for first dummy reading to set things up
  CFxADSampleBlock(ad, FCHAN, NCHAN, &ADSamplingRuptHandler, uni, sgl, false);

  // Make sure PIT is off
  PITSet51msPeriod(PITOff); // disable timer
  PITRemoveChore(0);        // get rid of all current chores?

  IEVInsertCFunct(&ADTimingRuptHandler, pitVector); // replacement fast routine

  // Current and voltage samples per pwr.interval interval
  intervalSamples = (ushort)(1 << bitshift);
  pwr.interval = (10 * intervalSamples * (PITRATE * PITPERIOD));

  DBG1("\t|Writing every %4.1fSeconds", pwr.interval / 10.0)
  delayms(1);

  // Set the Rate and start the PIT
  PITSet51msPeriod(PITRATE);
  pwr.sampleReady = false;
} // SetupAcquistion

//
// 1) Comes here when pwr.sampleReady == true
// 2) writes correct side of AD Buffer to file 
// sets: pwr.sampleReady=false
//
void powLog(void) {

  ushort AveragedEnergy[2] = {0, 0};
  float current = 0.0;

  if (pwr.sampleReady && !pwr.off) {
    AveragedEnergy[0] = (ushort)(powerSum[0] >> bitshift);
    AveragedEnergy[1] = (ushort)(powerSum[1] >> bitshift);
  }

  current = CFxADRawToVolts(ad, AveragedEnergy[0], VREF, true);
  voltage = CFxADRawToVolts(ad, AveragedEnergy[1], VREF, true) * 100;
  flogf("\n\t|POWER: %5.3fA, %5.2fV", current, voltage);

  pwr.sampleReady = false;
  powerWrite(AveragedEnergy);
} // ADLog

//
// Voltage Now()
//
float Voltage_Now(void) {
  float volts = 0.0;

  QPBRepeatAsync(); // starts the QSPI running with previous parameters

  ADSample = (short *)CFxADQueueToArray(ad, (void *)QRR, 2);
  volts = CFxADRawToVolts(ad, (ushort)ADSample[1], VREF, true) * 100;
  voltage = volts;
  return volts;
} // Voltage_Now

//
// AD Write
// Open file of Current averages, go to end of file and grab last averaged
reading.
// This function will increment the variable pwr.counter==FWT ~5minutes
// 
//
void powerWrite(ushort *AveragedEnergy) {
  DBG0("powerWrite")
  // global
  CLK(start_clock = clock();)
  pwr.filehdl = open(ADAvgFileName, O_RDWR | O_BINARY | O_APPEND);
  if (pwr.filehdl <= 0) {
    flogf("\nERROR|powerWrite() %s open fail. errno: %d", ADAvgFileName, errno);
    return;
  }

  CLK(stop_clock = clock();
      print_clock_cycle_count(start_clock, stop_clock, "powerWrite: open");)

  CLK(start_clock = clock();)

  write(pwr.filehdl, AveragedEnergy, 3 * sizeof(ushort));
  delayms(25);
  CLK(stop_clock = clock();
      print_clock_cycle_count(start_clock, stop_clock, "powerWrite: write");)

  if (pwr.off) // SetupAD(false) from power monitor
    return;
  if (close(pwr.filehdl) < 0)
    flogf("\nERROR  |powerWrite() %s Close error: %d", ADAvgFileName, errno);
  // DBG(   else      flogf("\n\t|powerWrite() %s Closed", ADAvgFileName);)
 
  delayms(10);

} // powerWrite
//
// PowerMonitor
// This function is called when the WriteInterval (WRTINT) is met.
// With a FWT for the ADS of 32seconds and a WRTINT of ~60 minutes (really 64
minutes)
//
float powMonitor(ulong totaltime, int filehandle, ulong *LoggingTime) {
  struct stat fileinfo;
  ulong DataCount = 0;
  ulong filelength = 0;
  ulong TotalAmp = 0;
  ulong TotalVolts = 0;
  ulong TotalTime = 0;
  ulong calculatedtime = 0L;
  ushort energy[3] = {0, 0, 0};
  float MaxCurrent = 0.0, MinVoltage = 0.0;
  float kjoules;
  float floater = 0.0;
  int byteswritten;
  float voltage = 0.0, amps = 0.0;

  // Normal enterance to powMonitor
  if (totaltime != 0) {
    powInit(false, NULL, NULL);
    if (pwr.interval < 1)
      pwr.interval = 1044;
    // Last AD Power Buffer size
    pwr.interval = ((10 * totaltime) % pwr.interval); 
    powLog();
    // opens adsfh
  }
  // Coming in after reboot // powInit(false), powLog also opens .pwr file
  else {
    pwr.filehdl = open(ADAvgFileName, O_RDWR | O_BINARY | O_APPEND);
    ad = CFxADInit(&adbuf, ADSLOT, ADInitFunction);
    if (!CFxADLock(ad)) {
      flogf("\nCouldn't lock and own A-D with QSPI\n");
      // close
      return 0.0;
    }
  }
  // adsfh is open

  flogf("\n\t|POWERMonitor(%s)", ADAvgFileName);

  // Reset Global Write Buffer
  memset(WriteBuffer, 0, BUFSZ);

  // Get file status
  stat(ADAvgFileName, &fileinfo);
  filelength = fileinfo.st_size;

  // if file unwritten to
  if (filelength < 6) {
    if (close(pwr.filehdl) < 0)
      flogf("\nERROR  |PowerMonitor: File Close error: %d", errno);
    DBG(else flogf("\n\t|PowerMonitor: ADSFile Closed");)
    return 0.0;
  }

  if (pwr.filehdl > 0) {
    // we maybe just wrote into file, so seek back to start
    lseek(pwr.filehdl, 0, SEEK_SET);
    // 6 is the number of bytes for the values of current, voltage, time.
    filelength = filelength / 6; 

    // Get the number of times file has been written to
    while (DataCount < filelength) {
      byteswritten = read(pwr.filehdl, energy, 3 * sizeof(ushort));
      TotalAmp += energy[0];
      TotalVolts += energy[1];
      TotalTime += (ulong)energy[2];
      DataCount++;
    }

    if (close(pwr.filehdl) < 0)
      flogf("\nERROR  |PowerMonitor: File Close error: %d", errno);
    DBG(else flogf("\n\t|PowerMonitor: ADSFile Closed");)

    delayms(25);
    if (DataCount != 0) {
      energy[0] = (ushort)(TotalAmp / DataCount);
      energy[1] = (ushort)(TotalVolts / DataCount);
    }

    amps = CFxADRawToVolts(ad, energy[0], VREF, true);
    delayms(10);
    voltage = CFxADRawToVolts(ad, energy[1], VREF, true) * 100;
    delayms(10);
    TotalTime = TotalTime / 10;
    kjoules = (amps * voltage * TotalTime) / 1000.0;
  }
  MaxCurrent = CFxADRawToVolts(ad, pwr.currentMax, VREF, true);
  MinVoltage = CFxADRawToVolts(ad, pwr.voltsMin, VREF, true) * 100;
  *LoggingTime = TotalTime;

  sprintf(WriteBuffer, "\n---POWER---\nTime: %lu\nEnergy:%.2fkJ\nAvg "
                       "Volt:%.2fV\nMin Volt:%.2fV\nMax Current:%.3fA\n",
          TotalTime, kjoules, voltage, MinVoltage, MaxCurrent);

  // Adjust and record Battery Capacity
  if (ADS.BATLOG) {
    flogf("\n\t|Current Bat Cap: %s", ADS.BATCAP);
    floater = atof(ADS.BATCAP);
    floater -= (kjoules * POWERERROR);
    sprintf(ADS.BATCAP, "%7.2f", floater);
    VEEStoreStr(BATTERYCAPACITY_NAME, ADS.BATCAP);
    flogf(", New Bat Cap: %s", ADS.BATCAP);
    sprintf(WriteBuffer, "\n---POWER---\nTime: %lu\nEnergy:%.2fkJ\nAvg "
                         "Volt:%.2fV\nMin Volt:%.2fV\nMax "
                         "Current:%.3fA\nBattery Capacity:%.2fkJ\n",
            TotalTime, kjoules, voltage, MinVoltage, MaxCurrent, floater);
  } else
    sprintf(WriteBuffer, "\n---POWER---\nTime: %lu\nEnergy:%.2fkJ\nAvg "
                         "Volt:%.2fV\nMin Volt:%.2fV\nMax Current:%.3fA\n",
            TotalTime, kjoules, voltage, MinVoltage, MaxCurrent);

  DBG1("%s", WriteBuffer)
  if (filehandle > 0)
    byteswritten = write(filehandle, WriteBuffer, strlen(WriteBuffer));

  delayms(150);

  return floater;

} // PowerMonitor


//
// 
//
void GetPowerSettings(void) {

  char *p;

  //"v" depending on battery type 11.5-14
  p = VEEFetchData(MINSYSVOLT_NAME).str;
  strncpy(ADS.MINVOLT, p ? p : MINSYSVOLT_DEFAULT, sizeof(ADS.MINVOLT));
  //	MPC.MINVOLT =p;
  DBG1("MINVOLT=%s (%s)", ADS.MINVOLT, p ? "vee" : "def")
  if (MIN_BATTERY_VOLTAGE > atof(ADS.MINVOLT)) {
    sprintf(ADS.MINVOLT, "%5.2f", MIN_BATTERY_VOLTAGE);
    VEEStoreStr(MINSYSVOLT_NAME, ADS.MINVOLT);
  }

  p = VEEFetchData(BATTERYCAPACITY_NAME).str;
  strncpy(ADS.BATCAP, p ? p : BATTERYCAPACITY_DEFAULT, sizeof(ADS.BATCAP));
  DBG1("BATCAP=%s (%s)", ADS.BATCAP, p ? "vee" : "def")

  //"t" 0 or 1
  p = VEEFetchData(BATTERYLOGGER_NAME).str;
  ADS.BATLOG = atoi(p ? p : BATTERYLOGGER_DEFAULT);
  DBG1("BATLOG=%u (%s)", ADS.BATLOG, p ? "vee" : "def")
}

//
// powDelay()
// AD function with time delay.  Do powLog at 5 sec incrment.
// number of seconds for delay while watching Power
// Logging & Tickling Watch Dog Timer
//
void powDelay(short Sec) {
  short i;
  long last, rem;
  DBG1(" {%d} ", Sec )
  cdrain();
  last = Sec / 5;
  rem = Sec - last * 5;

  pet(); // another reprieve
  for (i = 0; i < last; i++) {

    powCheck();
    delayms(5000);
  }
  powCheck();
  delayms(rem * 1000);
  pet();                         // another reprieve

} //powDelay()

*/
