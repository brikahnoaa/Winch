// ctd.h

// ctd
typedef struct CtdInfo {
  bool on;
  bool syncmode;  // use syncmode
  bool pending;   // request pending, expect response
  float depth;
  short delay;    // Delay in seconds between polled samples
  int filehandle; // log file
  Serial port;    // same as ant.port
} CtdInfo;
extern CtdInfo ctd;

float ctdData(char *stringout);
bool ctdOpen(void);     // ctd .off .port .pending .filehandle
void ctdClose(void);    // ctd .off .port .pending .filehandle
bool ctdPrompt(void);
static void ctdSetDate(void);
void ctdSample(void);   // ctd .pending
void ctdSyncmode(void); 
void ctdBreak(void);
