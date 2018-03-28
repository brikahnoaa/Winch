// ctd.h

typedef enum {idle_ctd, auto_ctd} CtdModeType;

typedef struct CtdInfo {
  char logFile[32];
  int delay;                  // Delay seconds expected between polled samples
  int log;
  CtdModeType mode;
  Serial port;                // same as mpc.port, ant.port
} CtdInfo;

static bool ctdPrompt(void);
static void ctdBreak(void);
static void ctdData(void);
static void ctdFlush(void);
static void ctdSample(void);
static void ctdSetDate(void);

float ctdDepth(void);
void ctdInit(void);
void ctdStop(void);
CtdModeType ctdMode(CtdModeType mode);
