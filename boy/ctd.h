// ctd.h

typedef struct CtdInfo {
  bool expect;                // request pending, expect response
  bool syncmode;
  char logFile[16];
  float depth;                // most recent
  int delay;                  // Delay in seconds between polled samples
  int log;                    // log filehandle
  Serial port;                // same as mpc.port, ant.port
} CtdInfo;

static bool ctdPrompt(void);
static float ctdData(char *out);
static void ctdBreak(void);
static void ctdSample(void);
static void ctdSetDate(void);
static void ctdSyncmode(void); 

float ctdDepth(void);
void ctdFlush(void);
void ctdInit(void);
void ctdStop(void);
