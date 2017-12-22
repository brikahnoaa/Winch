// boy.h

#define CTDBAUD 9600L
#define ANTBAUD 9600L
#define STARTS_VEE "STARTS"
#define STARTSMAX_VEE "STARTSMAX"
#ifdef DEBUGWISPR
#define WISPRNUMBER 0
#else
#define WISPRNUMBER 4
#endif

// boy
typedef struct BuoyInfo {
  bool on;
  char platformID[6];  // rudicsland
  char programName[20]; // added HM
  char projectID[6];    // rudicsland
  float avgVel;
  float depth;     // Most recent depth measurement from sbe16
  float moorDepth; // Depth at beginning of LARA.PHASE==1. Averaged Samples
  int dataInt; // VEE:DATAXINTERVAL_NAME
  int deviceID;           // DEVA=1 = antenna, DEVB=2 = buoy ctd
  int logFile;
  int phase; // 1=WISPR, 2=Ascent, 3=Surface, 4=Descent, 5=deployment
  int phaseInitial; // normal start in this phase (2)
  long filenumber; // current number for filename ####.dat ####.log
  long starts;
  long startsMax;
  short callTime;     // 24hr time description, e.g. 1330 = 1:30pm local
  short callFreq;     // number of times per day to call
  time_t runStart;    // startup time
  time_t phaseStart; // time this phase started
} BuoyInfo;
extern BuoyInfo boy;

void shutdown(void);
static bool currentWarning(void);
static int incomingData(void);
static ulong writeFile(ulong);
static void sleepUntilWoken(void);
static void devSwitch(int *devID);
static void deploy(void);
static void phase1(void);
static void phase2(void);
static void phase3(void);
static void phase4(void);
static void reboot(int *phase);
static void restartCheck(long *starts);
static void Console(char);
static void IRQ2_ISR(void);
static void IRQ3_ISR(void);
static void IRQ4_ISR(void);
static void IRQ5_ISR(void);
static void WaitForWinch(short);
static void boyInit(Serial antPort, ctdPort, winchPort, wisprPort);
static void systemStatus(char *stringout);
