// ctd.h

typedef struct CtdInfo {
  bool on;        // initialized, syncmode
  bool pending;   // request pending, expect response
  char *filename;     // 
  float depth;        // most recent
  short delay;        // Delay in seconds between polled samples
  int filehandle;     // log file
} CtdInfo;
extern CtdInfo ctd;

float ctdDepth();               // query, wait for response
float ctdData(char *out);
bool ctdOpen(void);     // ctd .off .port .pending .filehandle
void ctdClose(void);    // ctd .off .port .pending .filehandle
bool ctdPrompt(void);
static void ctdSetDate(void);
void ctdSample(void);   // ctd .pending
void ctdSyncmode(void); 
void ctdBreak(void);
