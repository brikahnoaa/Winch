// pwr.h
// A-D SYSTEM CURRENT AND VOLTAGE LOGGING
// Changing parameters here will cause problems to program timing.

// #include <ADExamples.h>

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
  bool on;             // track and log battery capacity
  bool sampleReady;     // sample should be saved, until written
  char log[16];          // log file
  float charge;   // minimum system capacity to run
  float chargeMin;   // minimum system capacity to run
  float volts;     // minimum system voltage to run
  float voltsMin;     // minimum system voltage to run
  long batCap;          // for lithium
  short interval;       // deciSecs for PIT timer interrupt
} PwrInfo;
extern PwrInfo pwr;

void pwrLog(void);
void pwrDelay(short sec);
void pwrLogWrite(ushort *);
bool pwrCheck(void);
ushort pwrInit(bool, long, ushort);
float pwrMonitor(ulong, int, ulong *);
float pwrVolts(void);
void pwrOpenLog(long);
void pwrFileName(long);

//
// orig by alex - seems to be off, should be double. 1<<1==2^1
// 11:: PITRATE*PITPERIOD*1<<11 = 104.448
   10: 25.6seconds/file write 843.75 bytes/hour
   11: 51.2secs/file write    421.875bytes/hr
   12: 102.4secs/file         201.937bytes/hr
   13: 204.8secs/file         105.468
   14: 409.6                  52.734
   15: 819.2                  26.367
   16: 1638.4                 13.183
//
