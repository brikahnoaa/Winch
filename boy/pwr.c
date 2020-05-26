// pwr.c
#include <main.h>


PwrInfo pwr;
void pwrInit(void)
{ //
  // allow for BREAK on console to interrupt sleep
  IEVInsertAsmFunct(pwrIrq4RxISR, level4InterruptAutovector);
  IEVInsertAsmFunct(pwrIrq4RxISR, spuriousInterrupt);
} // pwrInit

void pwrStop(void){}  // TBD

void pwrIrq4RxISR(void) { PinIO(IRQ4RXD); RTE(); }

///
// power down as much as we can
// allow for operator BREAK
// ?? does it help to shut off CFcard rs232MAX
// rets: 0=sleep #=woke with sec remaining
int pwrSleep(long secs)
{ // using LPStop with wakeup every second from PIT watchdog
  time_t now, then;
  long sec;
  float tuning=1.02;
  int i;
  // adjust seconds using tuning percent, PIT chore is more than 1 sec
  sec = secs/tuning;
  time(&now);
  then = now;
  flogf("sleep %ld(%ld PITs) @ %s\n", secs, sec, utlDateTimeFmt(now));
  cdrain();
  utlDelay(2); // cdrain is not enough !?
  EIAForceOff(true);          // turn off the RS232 driver
  CFEnable(false);            // turn off the CompactFlash card
  while(sec--)
  { // loop once a second, or break on console BREAK
    utlPet(0);
    PinBus(IRQ4RXD);
    LPStopCSE(FullStop);
    // woke by PIT or console BREAK? interrupt handler sets pin to I/O
    if (PinTestIsItBus(IRQ4RXD)) continue; // PIT! continue loop
    if (SCIRxBreak(50)) // console! test for noise 
    { // BREAK!
      for (i=200; (SCIRxBreak(10)) && i>0; i--) {} // BREAK up to 2 sec
      break; // leave loop
    }
  } // sec--
  EIAForceOff(false);         // turn on the RS232 driver
  CFEnable(true);             // turn on the CompactFlash card
  utlDelay(2); // settle
  if (sec) // user break
    flogf("user break. sleepsec remaining=%d\n", sec);
  time(&now);
  flogf("slept %ld @ %s\n", now-then, utlDateTimeFmt(now));
  return(sec);
} // pwrSleep

void pwrNap(int sec) {
  utlDelay(sec*1000);
}

/*
// A-D SYSTEM CURRENT AND VOLTAGE LOGGING
// Changing parameters here will cause problems to program timing.
#define FCHAN 0   // first channel
#define NCHAN 2   // number of channels, just accumulating current in buffers.
#define PITRATE 1 // 1=51ms cycle, 2=102ms,etc..... @ 100us. 250=25ms.
#define PITPERIOD .051 // represents 51ms (about 20Hz)

#define POWERERROR 1.02
// volts or 11.0 for 15V Battery System of Seaglider
#define MIN_BATTERY_VOLTAGE 11.0
// kiloJoules
#define INITIAL_BATTERY_CAPACITY 5000
#define MINIMUM_BATTERY_CAPACITY INITIAL_BATTERY_CAPACITY * 0.1

// 104 second pwr cycle
#define BITSHIFT 11
 */
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
// to the pwr file.  Only when pwr monitor is called does the pwr
// file sum its "ushort" values and divided again ( non-bit shifted
// integer division) before it is converted to a floating point value with
// "CFxADRawToVolts(...)"  This makes for a very fast and efficient pwr
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
long pwrSum[2] = {0, 0};

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
PowerInfo pwr = {
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
    pwrSum[0] = currentSum;
    pwrSum[1] = voltsSum;
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

bool pwrCheck(void) {
  if (pwr.sampleReady == true && !pwr.off) {
    utlPet();
    pwrLog();
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
    pwrOpenLog(filecounter);
    flogf("\n%s|ADS(%s)", Time(NULL), ADAvgFileName);
    Setup_Acquisition(bitshift);

  } else {
    PITSet100usPeriod(PITOff); // Stop sampling
    PITRemoveChore(0);
    utlDelay(10);
    pwr.sampleReady = true;
    pwr.counter = 0;
  }
  return pwr.interval;
} // void SetUpADS
  
//
// Void OpenAvgFile()
//
void pwrOpenLog(long counter) {

  sprintf(&ADAvgFileName[2], "%08ld.pwr", counter);
  utlDelay(25);
  ADSFileHandle = creat(ADAvgFileName, 0);
  if (ADSFileHandle <= 0) {
    flogf("\nCouldn't Open %s file, errno%d", ADAvgFileName, errno);
    return;
  }
  if (close(ADSFileHandle) != 0)
    flogf("\nERROR  |pwrOpenLog() %s Close error: %d", ADAvgFileName, errno);

  utlDelay(10);

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

  utlDelay(20);

  pwrSum[0] = 0L;
  pwrSum[1] = 0L;

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

  DBG1("\t|Writing every %4.1fSeconds", pwr.interval / 10.0);
  utlDelay(1);

  // Set the Rate and start the PIT
  PITSet51msPeriod(PITRATE);
  pwr.sampleReady = false;
} // SetupAcquistion

//
// 1) Comes here when pwr.sampleReady == true
// 2) writes correct side of AD Buffer to file 
// sets: pwr.sampleReady=false
//
void pwrLog(void) {

  ushort AveragedEnergy[2] = {0, 0};
  float current = 0.0;

  if (pwr.sampleReady && !pwr.off) {
    AveragedEnergy[0] = (ushort)(pwrSum[0] >> bitshift);
    AveragedEnergy[1] = (ushort)(pwrSum[1] >> bitshift);
  }

  current = CFxADRawToVolts(ad, AveragedEnergy[0], VREF, true);
  voltage = CFxADRawToVolts(ad, AveragedEnergy[1], VREF, true) * 100;
  flogf("\n\t|POWER: %5.3fA, %5.2fV", current, voltage);

  pwr.sampleReady = false;
  pwrWrite(AveragedEnergy);
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
void pwrWrite(ushort *AveragedEnergy) {
  DBG0("pwrWrite");
  // global
  CLK(start_clock = clock();)
  pwr.filehdl = open(ADAvgFileName, O_RDWR | O_BINARY | O_APPEND);
  if (pwr.filehdl <= 0) {
    flogf("\nERROR|pwrWrite() %s open fail. errno: %d", ADAvgFileName, errno);
    return;
  }

  CLK(stop_clock = clock();
      print_clock_cycle_count(start_clock, stop_clock, "pwrWrite: open");)

  CLK(start_clock = clock();)

  write(pwr.filehdl, AveragedEnergy, 3 * sizeof(ushort));
  utlDelay(25);
  CLK(stop_clock = clock();
      print_clock_cycle_count(start_clock, stop_clock, "pwrWrite: write");)

  if (pwr.off) // SetupAD(false) from pwr monitor
    return;
  if (close(pwr.filehdl) < 0)
    flogf("\nERROR  |pwrWrite() %s Close error: %d", ADAvgFileName, errno);
  // DBG(   else      flogf("\n\t|pwrWrite() %s Closed", ADAvgFileName););
 
  utlDelay(10);

} // pwrWrite
//
// PowerMonitor
// This function is called when the WriteInterval (WRTINT) is met.
// With a FWT for the ADS of 32seconds and a WRTINT of ~60 minutes (really 64
minutes)
//
float pwrMonitor(ulong totaltime, int filehandle, ulong *LoggingTime) {
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

  // Normal enterance to pwrMonitor
  if (totaltime != 0) {
    pwrInit(false, NULL, NULL);
    if (pwr.interval < 1)
      pwr.interval = 1044;
    // Last AD Power Buffer size
    pwr.interval = ((10 * totaltime) % pwr.interval); 
    pwrLog();
    // opens adsfh
  }
  // Coming in after reboot // pwrInit(false), pwrLog also opens .pwr file
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
    DBG(else flogf("\n\t|PowerMonitor: ADSFile Closed"););
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
    DBG(else flogf("\n\t|PowerMonitor: ADSFile Closed"););

    utlDelay(25);
    if (DataCount != 0) {
      energy[0] = (ushort)(TotalAmp / DataCount);
      energy[1] = (ushort)(TotalVolts / DataCount);
    }

    amps = CFxADRawToVolts(ad, energy[0], VREF, true);
    utlDelay(10);
    voltage = CFxADRawToVolts(ad, energy[1], VREF, true) * 100;
    utlDelay(10);
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

  DBG1("%s", WriteBuffer);
  if (filehandle > 0)
    byteswritten = write(filehandle, WriteBuffer, strlen(WriteBuffer));

  utlDelay(150);

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
  DBG1("MINVOLT=%s (%s)", ADS.MINVOLT, p ? "vee" : "def");
  if (MIN_BATTERY_VOLTAGE > atof(ADS.MINVOLT)) {
    sprintf(ADS.MINVOLT, "%5.2f", MIN_BATTERY_VOLTAGE);
    VEEStoreStr(MINSYSVOLT_NAME, ADS.MINVOLT);
  }

  p = VEEFetchData(BATTERYCAPACITY_NAME).str;
  strncpy(ADS.BATCAP, p ? p : BATTERYCAPACITY_DEFAULT, sizeof(ADS.BATCAP));
  DBG1("BATCAP=%s (%s)", ADS.BATCAP, p ? "vee" : "def");

  //"t" 0 or 1
  p = VEEFetchData(BATTERYLOGGER_NAME).str;
  ADS.BATLOG = atoi(p ? p : BATTERYLOGGER_DEFAULT);
  DBG1("BATLOG=%u (%s)", ADS.BATLOG, p ? "vee" : "def");
}

//
// pwrDelay()
// AD function with time delay.  Do pwrLog at 5 sec incrment.
// number of seconds for delay while watching Power
// Logging & Tickling Watch Dog Timer
//
void pwrDelay(short Sec) {
  short i;
  long last, rem;
  DBG1(" {%d} ", Sec );
  cdrain();
  last = Sec / 5;
  rem = Sec - last * 5;

  utlPet(); // another reprieve
  for (i = 0; i < last; i++) {

    pwrCheck();
    utlDelay(5000);
  }
  pwrCheck();
  utlDelay(rem * 1000);
  utlPet();                         // another reprieve

} //pwrDelay()

*/


// here on was hps.c -------------------------------

/******************************************************************************\
** HPADMain.c
** Simple AD program for MPC board.  Not for CSACDAQ.
** MPC uses CF2 pin 19 to enable ADC and REF ICs, whereas CSACDAQ uses 28.
** MPC uses CF2 pin 26 to power ON. 1=ON, 0=OFF  
**
** Revised to set pin 28 (AD_REF_SHDN_PIN) on to enable AD on receipe card.(8/08/2011)
** Revised to control pin 35 to turn on/off pre-amp (rev 9b). (08/07/2011)
**	
**	Program to read A/D values of NOAADAQ2 board using ADS8344 chip.			
**	
**	Release:		2001/12/22
**  Version 2		2003/08/21
**					Updated mcp for Palm 8.0 Codewarrior and newer BIOS 3.00
**
*****************************************************************************
**	
**	COPYRIGHT (C) 1995-2001 PERSISTOR INSTRUMENTS INC., ALL RIGHTS RESERVED
**	
**	Developed by: John H. Godley for Persistor Instruments Inc.
**	254-J Shore Road, Bourne, MA 02532  USA
**	jhgodley@persistor.com - http://www.persistor.com
**	
**	Copyright (C) 1995-2001 Persistor Instruments Inc.
**	All rights reserved.
**	
*****************************************************************************
**	
**	Copyright and License Information
**	
**	Persistor Instruments Inc. (hereafter, PII) grants you (hereafter,
**	Licensee) a non-exclusive, non-transferable license to use the software
**	source code contained in this single source file with hardware products
**	sold by PII. Licensee may distribute binary derivative works using this
**	software and running on PII hardware products to third parties without
**	fee or other restrictions.
**	
**	PII MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
**	SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
**	IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
**	OR NON-INFRINGEMENT. PII SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY
**	LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING THE SOFTWARE OR
**	ITS DERIVATIVES.
**	
**	By using or copying this Software, Licensee agrees to abide by the
**	copyright law and all other applicable laws of the U.S. including, but
**	not limited to, export control laws, and the terms of this license. PII
**	shall have the right to terminate this license immediately by written
**	notice upon Licensee's breach of, or non-compliance with, any of its
**	terms. Licensee may be held legally responsible for any copyright
**	infringement or damages resulting from Licensee's failure to abide by
**	the terms of this license. 
**	
\******************************************************************************/
/*
//#define DEBUG			// comment this out to turn off diagnostics
//#include <cfxbios.h>		// Persistor BIOS and I/O Definitions
//#include <cfxpico.h>		// Persistor PicoDOS Definitions
//#include <cfxad.h>		// Generic SPI A-D QPB Driver for CFx
//#include "ADExamples.h"	// Common definitions for the A-D Examples
//#include <ADS8344.h>
//#include <stdio.h>
//#include	<stdlib.h>
//#include	<ctype.h>
//#include <dosdrive.h>

#define PICODEV 4
#ifndef NOT_R216_RECIPECARD
#ifndef NO_AD_REF_SHDN_PIN
#define     AD_REF_SHDN_PIN     28  // default /SHDN on Pii boards
#endif 
#endif
//#define ADTYPE ADisADS8344
//#ifdef DEBUG
  	//#define	DBG(X)	X	// template:	DBG( cprintf("\n"); )
	//#pragma message	("!!! "__FILE__ ": Don't ship with DEBUG compile flag set!")
//#else
  //#define	DBG(X)		// nothing
//#endif
#define ADREF_SHDN_PIN 19 //For MPC board 3/12/2018 HM
#define HPSENS_PWR_ON  26 //For HP sensor power ON/OFF 1=ON, 0=OFF 3/12/2018 HM
//#define ADREF_SHDN_PIN 28 //For CSACDAQ

//main()
	//{

///
// HP Sensor stats
int hpsRead (HpsStats *hps) {
  float ret;
  readStat( &ret, 0);
  readStat( &(hps->curr), 0);
  readStat( &(hps->volt), 1);
  readStat( &(hps->pres), 2);
  readStat( &(hps->humi), 3);
  // turn off
  readStat( &ret, 4);
  return 0;
} // hpsRead

///
// read one stat
int readStat (float *ret, int c) {
	///char		c, r;
	bool		uni = true;		// true for unipolar, false for bipolar
	bool		sgl = true;		// true for single-ended, false for differential
	short	   sample,  *samples;
	CFxAD		adbuf, *ad;
	float		vref = VREF;   //defined in ADExamples.h
  	ushort	entryclock = TMGGetSpeed();
  	float    output;

	samples = (ushort *) malloc(8* sizeof(ushort)); 
	
	//Identify the program and build
	///printf("\nProgram: %s: %s %s \n", __FILE__, __DATE__, __TIME__);
	///cprintf("ADREF PIN = %d, Sensor board PWR ON pin = %d\n",AD_REF_SHDN_PIN, HPSENS_PWR_ON);
	///cprintf("This program is for MPC.You must use a different prog for CSACDAQ. Proceed ? y/n ");
   ///r=cgetc();
   ///cprintf("%c\n",r);
   ///putflush();
   ///if(r=='y'||r=='Y')
   {
		//Power on
		PIOSet(ADREF_SHDN_PIN);  //ADC and Ref IC power ON
		PIOSet(HPSENS_PWR_ON);   //HP-Sensor board power on from DiFar Power port  (J14) 
		
		// Initialize QPB to accept our A-D with its QSPI connection.
		ad = CFxADInit(&adbuf, ADSLOT, ADInitFunction);

		///cprintf("\nKey in 0 to 3 for a single channel ADC. '.' to quite\n");
		///cprintf("0 = current, 1 = Bat voltage, 2 = Pressure, 3 = Humidity\n");
		///cprintf("also Unipolar, Bipolar, Single-ended, Differential, Powerdown\n");
	 	///cprintf("\n%c%c> ", uni ? 'U' : 'B', sgl ? 'S' : 'D');
	   ///fflush(stdout);  
		
		///while (true)
			{
			///c = cgetc();
			///switch (c = toupper(c))
                        switch (c)
				{
				///case '0' : 
				///case '1' : 
				///case '2' : 
				///case '3' : 
				case 0 : 
				case 1 : 
				case 2 : 
				case 3 : 
					///sample = CFxADSample(ad, c - '0', uni, sgl, false);
					sample = CFxADSample(ad, c, uni, sgl, false);
					output=CFxADRawToVolts(ad, sample, vref, uni);
					///if (c=='0') 
					if (c==0) 
						{
						output=output*1000.;  //Current in mA
						///printf("Current  = %7.1f mA\n", output);
						}
					///if (c=='1') 
					if (c==1) 
						{
						output= output*100.;  //Voltage in V
						///printf("Bat Volt = %7.1f V\n", output);
						}

					///if (c=='2') 
					if (c==2) 
						{
						//output=(output*0.4+0.040)*2439.0;//pressure
						output=(output*0.4+0.040)*2494.0;//pressure
						///printf("Pressure = %7.1f mBar\n", output);
						}
					///if (c=='3') 
					if (c==3) 
						{
						output=(output*2-0.831)/0.029;//humidity
						///printf("Humidity = %7.1f %\n", output);
						}
                    *ret = output;
					break;
	   		///putflush();				
				//case 'U' :	uni = true;		break;
				//case 'B' :	uni = false;	break;
				//case 'S' :	sgl = true;		break;
				//case 'D' :	sgl = false;	break;
				//case 'P' :	CFxADPowerDown(ad, true);	break;
				///case '.' :	PIOClear(ADREF_SHDN_PIN); PIOClear(HPSENS_PWR_ON); return 0;
				case 4 :	PIOClear(ADREF_SHDN_PIN); PIOClear(HPSENS_PWR_ON); return 0;
				}
			}
        }
	return 0;

	}	// readStat

  */
