// ctd.h

#define CTD_H

typedef struct CtdInfo {
  bool auton;
  bool autoSample;
  bool logging;
  bool on;
  char logFile[32];
  char sample[32];
  float cond;
  float depth;
  float temp;
  int delay;                  // Delay seconds expected between polled samples
  int log;                    // log fileid
  time_t time;
  Serial port;                // same as mpc.port, ant.port
} CtdInfo;

static void ctdBreak(void);
static void ctdFlush(void);

bool ctdData(void);
bool ctdPending(void);
bool ctdPrompt(void);
bool ctdRead(void);
float ctdDepth(void);
void ctdAuton(bool auton);
void ctdGetSamples(void);
void ctdInit(void);
void ctdSample(void);
void ctdStart(void);
void ctdStop(void);
