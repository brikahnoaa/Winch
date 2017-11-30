// ctd.h

// ctd
typedef struct CtdData {
  bool off;
  bool syncMode;  // off = polled
  bool pending;   // polled request pending
  short delay;    // Delay in seconds between polled samples
  TUPort *port;    // same as ant.port
} CtdData;
extern CtdData ctd;

bool ctdInit();
bool ctdGetPrompt();
static void ctdDateTime();
void ctdSample();
void ctdSyncMode();
void ctdSampleBreak();
