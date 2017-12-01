// mpc.h

#define WTMODE nsStdSmallBusAdj // choose: nsMotoSpecAdj or nsStdSmallBusAdj
#define SYSCLK 16000 // Clock speed: 2000 works 160-32000 kHz Default: 16000

void PreRun(void);
void initMPC(void);
int System_Timer();
char *Time(ulong *);
char *TimeDate(ulong *);
void Make_Directory(char *);
void DOS_Com(char *, long, char *, char *);
float Check_Timers(ushort);
bool Append_Files(int, const char *, bool, long);
long Free_Disk_Space();
void VEEStoreShort(char *, short);
void Sleep();
void CTDSleep();
void GetSettings();
short Check_Vitals();
void print_clock_cycle_count(clock_t, clock_t, char *);
