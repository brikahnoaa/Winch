// pwr.h
#ifndef H_PWR
#define H_PWR

typedef struct PwrInfo {
  bool on;              // track and log battery capacity
  bool sampleReady;     // sample should be saved, until written
  char *me;
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
