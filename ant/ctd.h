// ctd.h

// ctd
typedef struct CtdInfo {
  bool off;
  bool syncMode;  // off = polled
  bool pending;   // polled request pending
  short delay;    // Delay in seconds between polled samples
  Serial port;    // same as ant.port
} CtdInfo;
extern CtdInfo ctd;

bool ctdInit();     // global ctd .off .port .pending
bool ctdPrompt();
static void ctdSetDate();
void ctdSample();
void ctdSyncmode();
void ctdBreak();
