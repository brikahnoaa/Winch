// boy.h
#ifndef H_BOY
#define H_BOY

#include <iri.h>  // ?? remove this?

typedef enum {
  deploy_pha=0, 
  rise_pha, irid_pha, fall_pha, 
  data_pha, reboot_pha, error_pha,
  sizeof_pha
} PhaseType;
typedef enum { free_ris, run_ris } RiseType;

// boy data 
typedef struct BoyData {
  char *buff;             // malloc buffer to pass into modules
  float dockD;            // Depth when docked in winch
  float fallV1st;         // meters/s of the first fall
  float fallVNow;         // meters/s of the most recent fall 
  float iceTemp;          // check temp while looking for ice
  float oceanCurr;
  float riseV1st;         // meters/min of the first rise 
  float riseVNow;         // meters/min of the most recent rise 
  float surfD;            // depth floating at surface
  int log;                // log filehandle
  time_t cycleBgnT;
  time_t cycleEndT;
  time_t fallBgnT;
  time_t fallEndT;
  time_t riseBgnT;
  time_t riseEndT;
  GpsStats gpsBgn;        // initial stats, just surfaced
  GpsStats gpsEnd;        // final stats, irid done
} BoyData;

// boy params
typedef struct BoyInfo {
  // char * first for initialization // phase name
  char * phaName[sizeof_pha];
  //
  bool reset;             // remote reset (false)
  bool stop;              // remote stop (false)
  float ant2tip;          // meters from antmod s16 to antenna tip
  float boy2ant;          // meters from buoy s16 to ant s16 under still water
  float fallVpred;        // predicted fall velo (.2)
  float iceDanger;        // ice danger at this temp (-1.2)
  float riseVpred;        // predicted rise velo (.33)
  int cycleDays;          // number of days in a cycle (1)
  int cycleHour;          // 0-23 (midnight-11pm) hour to start cycle (1)
  int cycleMint;          // cycle in Minutes, overrides cD cH
  int dataRestM;          // Minutes to sleep (20)
  int dataRunM;           // Minutes to run detection (10)
  int depSetlM;           // time to let deploy settle (60)
  int depWaitM;           // wait until deployed after start (240)
  int fallOpM;            // operation timeout minutes (30)
  int filePause;          // pause between sending files
  int iridOpM;            // phase minutes - on the phone (15)
  int ngkDelay;           // delay sec to wait on acoustic modem, one way (7)
  int riseOpM;            // operation timeout minutes (30)
  int startPh;            // phase to start in (0)
  int stayDown;           // stay down for days, expecting storm (0)
} BoyInfo;

static bool docked(float depth);
static int cycleSetup(void);
static int fallDn(float targetD);
static int iridCall(void);
static int nextCycle(void);
static int reboot(void);
static int riseUp(float targetD);
static void riseTime(time_t *riseT);
static PhaseType dataPhase(void);
static PhaseType deployPhase(void);
static PhaseType errorPhase(void);
static PhaseType fallPhase(void);
static PhaseType iridPhase(void);
static PhaseType risePhase(void);

int boyEngLog(void);
void boyFlush(void);
void boyInit(void);
void boyMain(void);
void boyStop(void);

#endif
