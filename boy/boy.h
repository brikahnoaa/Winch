// boy.h

#define CTDBAUD 9600L
#define ANTBAUD 9600L
#ifdef DEBUGWISPR
#define WISPRNUMBER 0
#else
#define WISPRNUMBER 4
#endif

void phase0();
void phase1();
void phase2();
void phase3();
void phase4();
void reboot();
void Console(char);
bool CheckTime(ulong, short, short);
ulong WriteFile(ulong);
static void IRQ2_ISR(void);
static void IRQ3_ISR(void);
static void IRQ4_ISR(void);
static void IRQ5_ISR(void);
void WaitForWinch(short);
void SleepUntilWoken();
bool CurrentWarning();

void shutdown();
void inithw(ulong *);

char *PrintSystemStatus(void);
int Incoming_Data();
