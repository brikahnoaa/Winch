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
static void ctdSetDate(void);

float ctdDepth();             // query, wait for response
float ctdData(char *out);
void ctdSample(void);         // ctd .pending
void ctdSyncmode(void); 
void ctdBreak(void);
void ctdInit(void);
void ctdStop(void);
void ctdFlush(void);
