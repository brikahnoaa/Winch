// power.h
// A-D SYSTEM CURRENT AND VOLTAGE LOGGING
// Changing parameters here will cause problems to program timing.
#include <ADExamples.h>

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

// 104 second power cycle
#define BITSHIFT 11

typedef struct PowerInfo {
  bool on;             // track and log battery capacity
  bool sampleReady;     // sample should be saved, until written
  float minBatCharge;   // minimum system capacity to run
  float minBatVolt;     // minimum system voltage to run
  int counter;          // power monitor count
  int filehdl;          // log file
  long batCap;          // for lithium
  short interval;       // deciSecs for PIT timer interrupt
  ushort currentMax;
  ushort voltsMin;
} PowerInfo;
extern PowerInfo power;

void powLog(void);
void powLogDelay(short sec);
void Setup_Acquisition(ushort);
void powLogWrite(ushort *);
ushort GetSystemTimeInt(void);
bool powCheck(void);
ushort powInit(bool, long, ushort);
float powMonitor(ulong, int, ulong *);
float Get_Voltage(void);
float Voltage_Now(void);
void powOpenLog(long);
void GetPowerSettings(void);
void powFileName(long);

// Crucial to ADS Timing of Program. explained in ads power consumption
// calculation excel file
/*
 * orig alex - seems to be off, should be double. 1<<1==2^1
 * 11:: PITRATE*PITPERIOD*1<<11 = 104.448
   10: 25.6seconds/file write 843.75 bytes/hour
   11: 51.2secs/file write    421.875bytes/hr
   12: 102.4secs/file         201.937bytes/hr
   13: 204.8secs/file         105.468
   14: 409.6                  52.734
   15: 819.2                  26.367
   16: 1638.4                 13.183
 */
