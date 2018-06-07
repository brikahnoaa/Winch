// boy.h
#define BOY_H

typedef enum {
  deploy_pha=0, reboot_pha, error_pha,
  data_pha, rise_pha, irid_pha, fall_pha
} PhaseType;

// boy
typedef struct BoyInfo {
  bool gpsAvail;          // use gps module, for testing
  char logFile[32];       // log file
  float ant2tip;          // meters from antmod ctd to antenna tip
  float boy2ant;          // meters from buoy ctd to ant ctd under still water
  float currChkD;         // stop at this depth to check ocean current
  float currMax;          // too much ocean current
  float dockD;            // Depth when docked in winch
  float fallVFirst;       // meters/min of the first fall
  float fallVLast;        // meters/min of the most recent fall 
  float riseAccu;         // accuracy of rise estimate (1.5)
  float riseOrig;         // original rise rate based on tests
  float riseRate;         // riseOrig modified by measures
  float riseVFirst;       // meters/min of the first rise 
  float riseVLast;        // meters/min of the most recent rise 
  int iridFreq;           // number of times per day to call (1)
  int iridHour;           // 0-23 (midnight-11pm) hour to first call home (1)
  int cycleMax;           // limit number of cycles, i.e. test deployment
  int fileNum;            // current number for filename ####.dat ####.log
  int log;                // log filehandle
  int ngkDelay;           // delay sec to wait on acoustic modem, one way (7)
  int riseRetry;          // rise fails, retry times
  int settleT;            // time to let deploy settle (120)
  PhaseType phase;        // deploy, data, rise, irid, fall, error
  PhaseType phasePrev;    // deploy, data, rise, irid, fall, error
  Serial port;            // sbe16 or ant mod
  time_t deployT;         // startup time
  time_t phaseT;          // time this phase started
} BoyInfo;

static PhaseType iridPhase(void);
static PhaseType dataPhase(void);
static PhaseType deployPhase(void);
static PhaseType fallPhase(void);
static PhaseType errorPhase(void);
static PhaseType rebootPhase(void);
static PhaseType risePhase(void);
static bool riseUp(float targetD, int errMax, int delay);
static bool riseToSurf(void);
static int rise(float targetD, int try);
bool oceanCurrChk(void);
float oceanCurr(void);
static void boyStat(char *buffer);

bool boyDocked(float depth);
void boyInit(void);
void boyFlush(void);
void boyMain(void);
void boyStop(void);
