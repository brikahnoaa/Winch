// ctd.h

#define CTD_H

typedef struct CtdInfo {
  bool auton;
  bool pending;
  char logFile[32];
  float depth;
  int delay;                  // Delay seconds expected between polled samples
  int log;
  Serial port;                // same as mpc.port, ant.port
} CtdInfo;

static bool ctdPrompt(void);
static void ctdBreak(void);
static void ctdRead(void);
static void ctdSample(void);

bool ctdData(void);
float ctdDepth(void);
void ctdAuton(bool auton);
void ctdInit(void);
void ctdLog(void);
void ctdStop(void);
