// ctd.h

#define CTD_H

typedef struct CtdInfo {
  bool auton;
  bool logging;
  char logFile[32];
  char sample[32];
  float depth;
  int delay;                  // Delay seconds expected between polled samples
  int fresh;
  int log;                    // log fileid
  time_t time;
  Serial port;                // same as mpc.port, ant.port
} CtdInfo;

static bool ctdPrompt(void);
static bool ctdPending(void);
static bool ctdFresh(void);
static void ctdBreak(void);
static bool ctdRead(void);
static void ctdSample(void);

bool ctdData(void);
float ctdDepth(void);
void ctdAuton(bool auton);
void ctdGetSamples(void);
void ctdInit(void);
void ctdLog(bool on);
void ctdStart(void);
void ctdStop(void);
