// boy.h
#ifndef H_BOY
#define H_BOY

#include <hps.h>
#include <iri.h>

typedef enum {
  deploy_pha=0, 
  rise_pha, irid_pha, fall_pha, data_pha,
  reboot_pha, error_pha
} PhaseType;
typedef enum { free_ris, run_ris } RiseType;

// boy data - aka engineering data
typedef struct BoyData {
  GpsStats gpsBgn;        // initial stats, just surfaced
  GpsStats gpsEnd;        // final stats, irid done
  HpsStats physical;      // float curr, volt, pres, humi;
  Serial port;            // sbe16 or ant mod
  float dockD;            // Depth when docked in winch
  float fallV1st;         // meters/s of the first fall
  float fallVNow;         // meters/s of the most recent fall 
  float iceTemp;          // check temp while looking for ice
  float oceanCurr;
  float riseV1st;         // meters/min of the first rise 
  float riseVNow;         // meters/min of the most recent rise 
  float surfD;            // depth floating at surface
  int log;                // log filehandle
  time_t fallBgn;
  time_t fallEnd;
  time_t riseBgn;
  time_t riseEnd;
} BoyData;

// boy params
typedef struct BoyInfo {
  bool reset;             // remote reset (false)
  bool stop;              // remote stop (false)
  bool useBrake;          // false->no brake, true->riseDo(antSurfD())
  float ant2tip;          // meters from antmod s16 to antenna tip
  float boy2ant;          // meters from buoy s16 to ant s16 under still water
  float currChkD;         // stop at this depth to check ocean current
  float currMax;          // too much ocean current
  float iceTemper;        // ice danger at this temp (-1.2)
  float predFallV;        // predicted fall velo (.2)
  float predRiseV;        // predicted rise velo (.33)
  int callFreq;           // number of times per day to call !callHour (0)
  int callHour;           // 0-23 (midnight-11pm) hour to call home (1)
  int depSettle;          // time to let deploy settle (60)
  int depWait;            // wait until deployed after start (240min)
  int fallOpM;            // operation timeout minutes (30)
  int filePause;          // pause between sending files
  int iridOpM;            // phase minutes
  int ngkDelay;           // delay sec to wait on acoustic modem, one way (7)
  int riseOpM;            // operation timeout minutes (30)
  int startPh;            // phase to start in (0)
  int stayDown;           // stay down for days, expecting storm (0)
} BoyInfo;

static PhaseType dataPhase(void);
static PhaseType deployPhase(void);
static PhaseType errorPhase(void);
static PhaseType fallPhase(void);
static PhaseType iridPhase(void);
static PhaseType risePhase(void);
static bool docked(float depth);
static int fallDo(float targetD);
static int iridDo(void);
static int nextCycle(void);
static int reboot(void);
static int oceanCurr(float *curr);
static int riseDo(float targetD);
static void riseTime(time_t *riseT);

int boyEngLog(void);
int boySafeChk(float *curr, float *temp);
void boyFlush(void);
void boyInit(void);
void boyMain(void);
void boyStop(void);

#endif
