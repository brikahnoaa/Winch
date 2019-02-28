// boy.h
#ifndef H_BOY
#define H_BOY
#include <gps.h>

typedef enum {
  deploy_pha=0, 
  rise_pha, irid_pha, fall_pha, data_pha,
  reboot_pha, error_pha
} PhaseType;
typedef enum { free_ris, run_ris } RiseType;

// boy data
typedef struct BoyData {
  float dockD;            // Depth when docked in winch
  float surfD;            // depth floating at surface
  float oceanCurr;
  float fallV1st;         // meters/s of the first fall
  float fallVNow;         // meters/s of the most recent fall 
  float riseV1st;         // meters/min of the first rise 
  float riseVNow;         // meters/min of the most recent rise 
  int detect;
  int log;                // log filehandle
  time_t fallBgn;
  time_t fallEnd;
  time_t riseBgn;
  time_t riseEnd;
  Serial port;            // sbe16 or ant mod
  GpsStats gpsBgn;        // initial stats, just surfaced
  GpsStats gpsEnd;        // final stats, irid done
} BoyData;

// boy params
typedef struct BoyInfo {
  bool iridAuton;         // record depth during irid transfer
  bool reset;             // remote reset (false)
  bool stop;              // remote stop (false)
  bool test;              // test mode - activate tst.*
  char logFile[32];       // log file
  float ant2tip;          // meters from antmod ctd to antenna tip
  float boy2ant;          // meters from buoy ctd to ant ctd under still water
  float currChkD;         // stop at this depth to check ocean current
  float currMax;          // too much ocean current
  float fallVPred;        // predicted fall velo
  float riseVPred;        // predicted rise velo
  int cycleMax;           // max # of cycles or days
  int depSettle;          // time to let deploy settle (120)
  int depWait;            // wait until deployed after start (240min)
  int fallOp;             // operation timeout minutes (30)
  int fallRetry;          // fall fails, retry times
  int filePause;          // pause between sending files
  int iridFreq;           // number of times per day to call (1)
  int iridHour;           // 0-23 (midnight-11pm) hour to first call home (1)
  int iridOp;             // phase minutes
  int minute;             // set smaller to speed up test cycle (60)
  int ngkDelay;           // delay sec to wait on acoustic modem, one way (7)
  int riseOp;             // operation timeout minutes (30)
  int riseRetry;          // rise fails, retry times
  int startPh;            // phase to start in (0)
  int stayDown;           // stay down for days, expecting storm (0)
} BoyInfo;

static PhaseType dataPhase(void);
static PhaseType deployPhase(void);
static PhaseType errorPhase(void);
static PhaseType fallPhase(void);
static PhaseType iridPhase(void);
static PhaseType rebootPhase(void);
static PhaseType risePhase(void);
static int fall(float targetD, int try);
static int iridPhaseDo(void);
static int oceanCurr(float *curr);
static int oceanCurrChk(void);
static int rise(float targetD, int try);
static int riseUp(float targetD, int try);
static void boyStat(char *buffer);

bool boyDocked(float depth);
int boyCycle(void);
void boyEngLog(void);
void boyFlush(void);
void boyInit(void);
void boyMain(void);
void boyStop(void);

#endif
