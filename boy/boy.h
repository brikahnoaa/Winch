// boy.h
#define BOY_H

typedef enum {
  deploy_pha=0, 
  rise_pha, irid_pha, fall_pha, data_pha,
  reboot_pha, error_pha
} PhaseType;
typedef enum { free_ris, run_ris } RiseType;

// boy
typedef struct BoyInfo {
  bool reset;             // remote reset (false)
  bool testing;
  bool iridAuton;         // record depth during irid transfer
  bool noData;            // for test (false)
  bool noDeploy;          // for test (false)
  bool noIrid;            // for test (false)
  bool noRise;            // for test (false)
  char logFile[32];       // log file
  float ant2tip;          // meters from antmod ctd to antenna tip
  float boy2ant;          // meters from buoy ctd to ant ctd under still water
  float currChkD;         // stop at this depth to check ocean current
  float currMax;          // too much ocean current
  float dockD;            // Depth when docked in winch
  float fallVFirst;       // meters/s of the first fall
  float fallVLast;        // meters/s of the most recent fall 
  float fallVTest;        // m/s from tests
  float rateAccu;         // accuracy of rise/fall rate estimates (1.5)
  float riseVFirst;       // meters/min of the first rise 
  float riseVLast;        // meters/min of the most recent rise 
  float riseVTest;        // meters/min of the most recent rise 
  float surfD;            // depth floating at surface
  int cycle;
  int cycleMax;           // limit number of cycles, i.e. test deployment
  int depWait;            // wait until deployed after start (240min)
  int depSettle;          // time to let deploy settle (120)
  int fallOp;             // operation timeout minutes (30)
  int fallRetry;          // fall fails, retry times
  int iridFreq;           // number of times per day to call (1)
  int iridHour;           // 0-23 (midnight-11pm) hour to first call home (1)
  int iridOp;             // phase minutes
  int log;                // log filehandle
  int minute;             // set smaller to speed up test cycle (60)
  int ngkDelay;           // delay sec to wait on acoustic modem, one way (7)
  int riseOp;             // operation timeout minutes (30)
  int riseRetry;          // rise fails, retry times
  int startPh;            // phase to start in (0)
  int stayDown;           // stay down for days, expecting storm (0)
  int testCnt;            // counter used in testing (3)
  int testSize;           // send test file
  PhaseType phase;        // deploy, data, rise, irid, fall, error
  PhaseType phasePrev;    // deploy, data, rise, irid, fall, error
  Serial port;            // sbe16 or ant mod
  time_t deployT;         // startup time
  time_t phaseT;          // time this phase started
  time_t riseStart;
  time_t riseDone;
} BoyInfo;

static PhaseType iridPhase(void);
static PhaseType dataPhase(void);
static PhaseType deployPhase(void);
static PhaseType fallPhase(void);
static PhaseType errorPhase(void);
static PhaseType rebootPhase(void);
static PhaseType risePhase(void);
static int riseUp(float targetD, int try);
static int rise(float targetD, int try);
static int fall(float targetD, int try);
static int oceanCurrChk(void);
static int oceanCurr(float *curr);
static void boyStat(char *buffer);

bool boyDocked(float depth);
int boyCycle(void);
void boyEngLog(void);
void boyInit(void);
void boyFlush(void);
void boyMain(void);
void boyStop(void);
