// mpc.h

#define WTMODE nsStdSmallBusAdj // choose: nsMotoSpecAdj or nsStdSmallBusAdj
#define SYSCLK 16000 // Clock speed: 2000 works 160-32000 kHz Default: 16000
// PINS
#define AMODEMPWR 21
#define ANTMODPWR 22  // antMod Power pin (1=ON, 0=OFF)
#define DEVICECOM 23  // set = antMod, clear = sbe16
#define WISPRTHREE 24
#define WISPRFOUR 25
#define DIFARPWR 26   // unused
#define WISPRONE 29
#define WISPRTWO 30
#define DEVICETX 31
#define DEVICERX 32
#define AMODEMRX 33
#define AMODEMTX 35
#define WISPR_PWR_ON 37
#define WISPR_PWR_OFF 42

typedef struct MpcInfo {
  float joules;
  float volts;
  float voltMin;
} MpcInfo;

void PreRun(void);
void mpcVoltage(float *volts);
void mpcInit(void);
int System_Timer(void);
char *Time(ulong *);
char *TimeDate(ulong *);
void Make_Directory(char *);
void DOS_Com(char *, long, char *, char *);
float Check_Timers(ushort);
bool Append_Files(int, const char *, bool, long);
long Free_Disk_Space(void);
void VEEStoreShort(char *, short);
void Sleep(void);
void CTDSleep(void);
void GetSettings(void);
short Check_Vitals(void);
void print_clock_cycle_count(clock_t, clock_t, char *);
