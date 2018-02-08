// boy.h

typedef enum {
  null_pha=0,
  init_pha,
  data_pha,
  rise_pha,
  call_pha,
  drop_pha,
  error_pha,
  sizeof_pha
} PhaseType;

// boy
typedef struct BuoyInfo {
  char logFile[16];       // log file
  float currChkD;         // stop at this depth to check ocean current
  float currMax;          // too much ocean current
  float dockD;            // Depth when docked in winch
  int callFreq;           // number of times per day to call (1)
  int callHour;           // 0-23 (midnight-11pm) hour to call home 
  int fileNum;            // current number for filename ####.dat ####.log
  int log;                // log filehandle
  PhaseType phase;        // deploy, data, rise, call, drop, error
  PhaseType firstPhase;   // start in this phase (deploy)
  Serial port;            // sbe16 or ant mod
  time_t deployT;         // startup time
  time_t phaseT;          // time this phase started
} BuoyInfo;

static void dataFiles(void);
static void deployPhase(void);
static void dataPhase(void);
static void risePhase(void);
static void callPhase(void);
static void dropPhase(void);
static float oceanCurr(void);
static bool oceanCurrChk(void);

void boyInit(void);
void boyStat(char *buffer);
void boyMain(int starts);
