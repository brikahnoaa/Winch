// boy.h
#define BOY_H

typedef enum {
  deploy_pha=0, 
  rise_pha, irid_pha, fall_pha, data_pha,
  reboot_pha, error_pha
} PhaseType;
typedef enum { free_ris, run_ris } RiseType;

typedef struct EngGrp {
  char gpsInitLat[64];
  char gpsInitLng[64];
  char gpsDriftLat[64];
  char gpsDriftLng[64];
  char riseInit[64];
  char riseDone[64];
  float dockD;
  float oceanCurr;
  float surfD;
  float fallVFirst;       // meters/s of the first fall
  float fallVLast;        // meters/s of the most recent fall 
  float fallVTest;        // m/s from tests
  float rateAccu;         // accuracy of rise/fall rate estimates (1.5)
  float riseVFirst;       // meters/min of the first rise 
  float riseVLast;        // meters/min of the most recent rise 
  float riseVTest;        // meters/min of the most recent rise 
} EngGrp;

// boy
typedef struct BoyInfo {
  bool reset;             // remote reset (false)
  bool stop;              // remote stop (false)
  bool test;              // test mode - activate tst.*
  bool iridAuton;         // record depth during irid transfer
  char logFile[32];       // log file
  float ant2tip;          // meters from antmod ctd to antenna tip
  float boy2ant;          // meters from buoy ctd to ant ctd under still water
  float currChkD;         // stop at this depth to check ocean current
  float currMax;          // too much ocean current
  float dockD;            // Depth when docked in winch
  float surfD;            // depth floating at surface
  int cycleMax;           // max # of cycles or days
  int depWait;            // wait until deployed after start (240min)
  int depSettle;          // time to let deploy settle (120)
  int detect;
  int fallOp;             // operation timeout minutes (30)
  int fallRetry;          // fall fails, retry times
  int filePause;          // pause between sending files
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
  time_t fallStart;
  time_t fallDone;
  time_t riseStart;
  time_t riseDone;
  Serial port;            // sbe16 or ant mod
  EngGrp eng;             // engineering data group
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
