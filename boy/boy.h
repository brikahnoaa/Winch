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
  bool off;
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
  Time onStart;    // startup time
  Time phaseStart; // time this phase started
} BuoyInfo;
extern BuoyInfo boy;

void shutdown();
static bool currentWarning();
static int incomingData();
static ulong writeFile(ulong);
static void sleepUntilWoken();
static void devSwitch(int *devID);
static void deploy();
static void phase1();
static void phase2();
static void phase3();
static void phase4();
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
