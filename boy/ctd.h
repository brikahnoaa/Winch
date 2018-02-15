// ctd.h

typedef struct CtdInfo {
  bool pending;                // request pending, expect response
  bool syncmode;              // currently in syncmode
  char logFile[16];
  float depth;                // most recent
  int delay;                  // Delay seconds between polled samples
  int log;
  Serial port;                // same as mpc.port, ant.port
} CtdInfo;

static bool ctdPrompt(void);
static void ctdBreak(void);
static void ctdSetDate(void);
static void ctdSyncmode(void); 

float ctdData(char *out);
float ctdDepth(void);
void ctdFlush(void);
void ctdInit(void);
void ctdSample(void);
void ctdStop(void);
