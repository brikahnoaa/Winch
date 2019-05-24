// pwr.h
#ifndef H_PWR
#define H_PWR

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

// 104 second power cycle
#define BITSHIFT 11

typedef struct PwrInfo {
  bool on;              // track and log battery capacity
  bool sampleReady;     // sample should be saved, until written
  char logFile[32];     // log file
  float amps;
  float charge;         // minimum system capacity to run
  float chargeMin;      // minimum system capacity to run
  float volts;          // minimum system voltage to run
  float voltsMin;       // minimum system voltage to run
  int log;              // log filehandle
  long batCap;          // for lithium
  short interval;       // deciSecs for PIT timer interrupt
} PwrInfo;

bool pwrCheck(void);
float pwrMonitor(ulong, int, ulong *);
float pwrVolts(void);
void pwrNap(int sec);
void pwrFlush(void);
static void pwrLog(void);
static void pwrLogWrite(ushort *);
static void pwrOpenLog(long);
void pwrInit(void);
void pwrStop(void);

#endif
