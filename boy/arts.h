void PhaseOne();
void PhaseTwo();
void PhaseThree();
void PhaseFour();
void PhaseFive();
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
void InitializeLARA(ulong *);

char *PrintSystemStatus(void);
int Incoming_Data();
void LARA_Recovery();

