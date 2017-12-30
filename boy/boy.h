// boy.h

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
  char platformID[6];   // rudicsland
  char programName[20]; // added HM
  char projectID[6];    // rudicsland
  float avgVel;
  float dockDepth;      // Depth when docked in winch
  float sidewaysMax;    // 
  int callHour;         // 0-23 (midnight-11pm) hour to call home 
  int callFreq;         // number of times per day to call, expect 1
  int fileNum;          // current number for filename ####.dat ####.log
  int logFile;
  int phase;            // 0=deploy, 1=WISPR, 2=Ascent, 3=Surface, 4=Descent
  int phaseInitial;     // normal start in this phase (2)
  int starts;
  int startsMax;
  time_t deployT;       // startup time
  time_t phaseStartT;   // time this phase started
} BuoyInfo;
extern BuoyInfo boy;

void boyShut(void);
static int incomingData(void);
static ulong writeFile(ulong);
static void sleepUntilWoken(void);
static void phase0(void);
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
static void boyStatus(char *stringout);
