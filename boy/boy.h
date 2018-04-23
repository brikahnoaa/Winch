// boy.h
#define BOY_H

typedef enum {
  deploy_pha=0, reboot_pha, error_pha,
  data_pha, rise_pha, call_pha, drop_pha
} PhaseType;

// boy
typedef struct BoyInfo {
  char logFile[32];       // log file
  float ant2tip;          // meters from antmod ctd to antenna tip
  float boy2ant;          // meters from buoy ctd to ant ctd under still water
  float currChkD;         // stop at this depth to check ocean current
  float currMax;          // too much ocean current
  float dockD;            // Depth when docked in winch
  float dropVFirst;       // Velocity meters/min of the first drop (descent)
  float dropVLast;        // Velocity meters/min of the most recent drop 
  float riseVFirst;       // Velocity meters/min of the first rise (ascent)
  float riseVLast;        // Velocity meters/min of the most recent rise 
  int callFreq;           // number of times per day to call (1)
  int callHour;           // 0-23 (midnight-11pm) hour to call home 
  int cycles;             // limit number of cycles, i.e. test deployment
  int fileNum;            // current number for filename ####.dat ####.log
  int log;                // log filehandle
  PhaseType phase;        // deploy, data, rise, call, drop, error
  PhaseType phasePrev;    // deploy, data, rise, call, drop, error
  Serial port;            // sbe16 or ant mod
  time_t deployT;         // startup time
  time_t phaseT;          // time this phase started
} BoyInfo;

static PhaseType callPhase(void);
static PhaseType dataPhase(void);
static PhaseType deployPhase(void);
static PhaseType dropPhase(void);
static PhaseType errorPhase(void);
static PhaseType rebootPhase(void);
static PhaseType risePhase(void);
static bool riseUp(float targetD, int errMax, int delay);
static bool riseToSurf(void);
bool oceanCurrChk(void);
float oceanCurr(void);
static void boyStat(char *buffer);

bool boyDocked(float depth);
void boyInit(void);
void boyFlush(void);
void boyMain(void);
void boyStop(void);
