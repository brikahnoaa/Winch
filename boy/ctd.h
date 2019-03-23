// ctd.h
#ifndef H_CTD
#define H_CTD

typedef struct CtdInfo {
  bool auton;
  bool clearSamp;             // clear stored samples
  bool sampStore;             // store on device with TSSon - except auton
  bool on;
  bool sbe39;                 // init and use sbe39
  char logFile[32];
  char samCmd[16];
  float cond;
  float depth;
  float temp;
  int delay;                  // Delay seconds expected between polled samples
  int log;                    // log fileid
  int sampleInt;              // sample Interval for auton
  time_t time;
  Serial port;                // same as mpc.port, ant.port
} CtdInfo;

static void ctdBreak(void);
static void ctdFlush(void);

bool ctdData(void);
bool ctdDataWait(void);
bool ctdPending(void);
bool ctdPrompt(void);
bool ctdRead(void);
float ctdDepth(void);
int ctdAuton(bool auton);
int ctdStart(void);
int ctdStop(void);
void ctdGetSamples(void);
void ctdInit(void);
void ctdSample(void);
void ctdTest(void);

#endif
