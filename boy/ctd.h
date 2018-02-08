      // ctd.h

typedef struct CtdInfo {
  bool on;                    // initialized, syncmode
  bool expect;                // request pending, expect response
  char logFile[16];
  float depth;                // most recent
  int delay;                  // Delay in seconds between polled samples
  int log;                    // log filehandle
} CtdInfo;

float ctdDepth();             // query, wait for response
float ctdData(char *out);
bool ctdOpen(void);           // ctd .off .port .pending .filehandle
void ctdClose(void);          // ctd .off .port .pending .filehandle
bool ctdPrompt(void);
static void ctdSetDate(void);
void ctdSample(void);         // ctd .pending
void ctdSyncmode(void); 
void ctdBreak(void);
void ctdInit(void);
