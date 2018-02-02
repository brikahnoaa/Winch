// boy.h

typedef enum { 
  null_alm=0,
  bottomCurrent_alm, 
  midwayCurrent_alm, 
  ice_alm,
  ngkTimeout_alm,
  sizeof_alm
} AlarmType;

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
typedef struct BuoyData {
  float currCheckD;     // stop at this depth to check ocean current
  float currMax;        // too much ocean current
  float dockD;          // Depth when docked in winch
  int alarm[sizeof_alm];
  int callHour;         // 0-23 (midnight-11pm) hour to call home 
  int callFreq;         // number of times per day to call (1)
  int fileNum;          // current number for filename ####.dat ####.log
  PhaseType phase;      // 0=deploy, 1=data, 2=rise, 3=call, 4=drop, 5=error
  PhaseType firstPhase; // start in this phase (deploy)
  Serial port;          // sbe16 / ant mod
  time_t deployT;       // startup time
  time_t phaseT;        // time this phase started
} BuoyData;
extern BuoyData boy;

int boyAlarm(AlarmType alarm);
void boyStop(void);
void boyInit(void);
void boyStat(char *buffer);
void boyMain(int starts);
static void deployPhase(void);
static void dataPhase(void);
static void risePhase(void);
static void callPhase(void);
static void dropPhase(void);
