// ctd.h

#define CTD_H

typedef struct CtdInfo {
  bool auton;
  char logFile[32];
  float depth;
  int delay;                  // Delay seconds expected between polled samples
  int log;                    // log fileid
  Serial port;                // same as mpc.port, ant.port
} CtdInfo;

static bool ctdPrompt(void);
static bool ctdPending(void);
static bool ctdFresh(void);
static void ctdBreak(void);
static void ctdRead(void);
static void ctdSample(void);

bool ctdData(void);
float ctdDepth(void);
void ctdAuton(bool auton);
void ctdGetSamples(void);
void ctdInit(void);
void ctdLog(bool on);
void ctdStop(void);
