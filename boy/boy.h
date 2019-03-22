// boy.h
#ifndef H_BOY
#define H_BOY
#include <gps.h>
#include <hps.h>

typedef enum {
  deploy_pha=0, 
  rise_pha, irid_pha, fall_pha, data_pha,
  reboot_pha, error_pha
} PhaseType;
typedef enum { free_ris, run_ris } RiseType;

// boy data
typedef struct BoyData {
  GpsStats gpsBgn;        // initial stats, just surfaced
  GpsStats gpsEnd;        // final stats, irid done
  HpsStats physical;    // float curr, volt, pres, humi;
  Serial port;            // sbe16 or ant mod
  float dockD;            // Depth when docked in winch
  float fallV1st;         // meters/s of the first fall
  float fallVNow;         // meters/s of the most recent fall 
  float iceTemp;          // check temp while looking for ice
  float oceanCurr;
  float riseV1st;         // meters/min of the first rise 
  float riseVNow;         // meters/min of the most recent rise 
  float surfD;            // depth floating at surface
  int detections;
  int log;                // log filehandle
  time_t fallBgn;
  time_t fallEnd;
  time_t riseBgn;
  time_t riseEnd;
} BoyData;

// boy params
typedef struct BoyInfo {
  bool iridAuton;         // record depth during irid transfer
  bool reset;             // remote reset (false)
  bool stop;              // remote stop (false)
  char logFile[32];       // log file
  float ant2tip;          // meters from antmod ctd to antenna tip
  float boy2ant;          // meters from buoy ctd to ant ctd under still water
  float currChkD;         // stop at this depth to check ocean current
  float currMax;          // too much ocean current
  float iceDanger;        // ice danger at this temp (-1.2)
  float predFallV;        // predicted fall velo (.2)
  float predRiseV;        // predicted rise velo (.33)
  int cycleMax;           // max # of cycles or days
  int depSettle;          // time to let deploy settle (60)
  int depWait;            // wait until deployed after start (240min)
  int fallOp;             // operation timeout minutes (30)
  int fallRetry;          // fall fails, retry times
  int filePause;          // pause between sending files
  int iridFreq;           // number of times per day to call !iridHour (0)
  int iridHour;           // 0-23 (midnight-11pm) hour to call home (1)
  int iridOp;             // phase minutes
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
static bool boyDocked(float depth);
static int fall(float targetD, int try);
static int iridPhaseDo(void);
static int nextCycle(void);
int oceanCurr(float *curr);
static int safetyChk(float *curr, float *temp);
static int rise(float targetD, int try);
static int riseUp(float targetD, int try);

int boyEngLog(void);
void boyFlush(void);
void boyInit(void);
void boyMain(void);
void boyStat(char *buffer);
void boyStop(void);

#endif
