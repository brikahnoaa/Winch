// boy.h

typedef enum {
  deploy_pha=0, reboot_pha, error_pha,
  data_pha, rise_pha, call_pha, drop_pha
} PhaseType;

// boy
typedef struct BoyInfo {
  char logFile[16];       // log file
  float boy2ant;        // meters from buoy ctd to ant ctd under still water
  float currChkD;         // stop at this depth to check ocean current
  float currMax;          // too much ocean current
  float dockD;            // Depth when docked in winch
  float firstDropV;     // Velocity meters/min of the first drop (descent)
  float firstRiseV;     // Velocity meters/min of the first rise (ascent)
  float lastDropV;      // Velocity meters/min of the most recent drop 
  float lastRiseV;      // Velocity meters/min of the most recent rise 
  int callFreq;           // number of times per day to call (1)
  int callHour;           // 0-23 (midnight-11pm) hour to call home 
  int fileNum;            // current number for filename ####.dat ####.log
  int log;                // log filehandle
  PhaseType phase;        // deploy, data, rise, call, drop, error
  Serial port;            // sbe16 or ant mod
  time_t deployT;         // startup time
  time_t phaseT;          // time this phase started
} BoyInfo;

static bool oceanCurrChk(void);
static float oceanCurr(void);
static void boyStat(char *buffer);
static void callPhase(void);
static void callPhase(void);
static void dataPhase(void);
static void deployPhase(void);
static void dropPhase(void);
static void rebootPhase(void);
static void risePhase(void);
static void transferFiles(void);
static void flushBuffers(void);
static void errorPhase(void);
static void flushBuffers(void);

void boyInit(void);
void boyMain(int starts);
void boyStop(void);
