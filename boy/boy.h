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

void SleepUntilWoken();
void shutdown();
void devSwitch(int *devID);
static void deploy();
static void phase1();
static void phase2();
static void phase3();
static void phase4();
static void reboot(int *phase);
static void restartCheck(long *starts);
static void Console(char);
static ulong WriteFile(ulong);
static void IRQ2_ISR(void);
static void IRQ3_ISR(void);
static void IRQ4_ISR(void);
static void IRQ5_ISR(void);
static void WaitForWinch(short);
static bool CurrentWarning();
static void initHW(ulong *);
static void systemStatus(char *stringout);
static int Incoming_Data();
