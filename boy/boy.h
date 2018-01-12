// boy.h

#define STARTS_VEE "STARTS"
#define STARTSMAX_VEE "STARTSMAX"
#ifdef DEBUGWISPR
#define WISPRNUMBER 0
#else
#define WISPRNUMBER 4
#endif

typedef enum { 
  bottomCurrent_alm=0, 
  midwayCurrent_alm, 
  ice_alm,
  ngkTimeout_alm,
  sizeof_alm,
} AlarmType;

typedef enum {
  init_pha=0,
  data_pha,
  rise_pha,
  call_pha,
  drop_pha,
  trouble_pha,
  sizeof_pha,
} PhaseType;

// boy
typedef struct BuoyData {
  bool on;
  float currCheckD;     // stop at this depth to check ocean current
  float currMax;        // too much ocean current
  float dockD;          // Depth when docked in winch
  int alarm[sizeof_alm];
  int callHour;         // 0-23 (midnight-11pm) hour to call home 
  int callFreq;         // number of times per day to call (1)
  int fileNum;          // current number for filename ####.dat ####.log
  PhaseType phase;      // 0=deploy, 1=WISPR, 2=Ascent, 3=Surface, 4=Descent
  PhaseType startPhase; // start in this phase (0=deploy)
  Serial port;          // sbe16 / ant mod
  time_t deployT;       // startup time
  time_t phaseT;        // time this phase started
} BuoyData;
extern BuoyData boy;

int boyAlarm(AlarmType alarm);
void boyShut(void);
void boyInit(void);
void boyStatus(char *buffer);
static int incomingData(void);
static ulong writeFile(ulong);
static void sleepUntilWoken(void);
static void deployPhase(void);
static void dataPhase(void);
static void risePhase(void);
static void callPhase(void);
static void dropPhase(void);
static void reboot(int *phase);
static void restartCheck(long *starts);
static void Console(char);
static void IRQ2_ISR(void);
static void IRQ3_ISR(void);
static void IRQ4_ISR(void);
static void IRQ5_ISR(void);
