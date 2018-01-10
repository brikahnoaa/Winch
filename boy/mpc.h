// mpc.h

#define WTMODE nsStdSmallBusAdj // choose: nsMotoSpecAdj or nsStdSmallBusAdj
#define SYSCLK 16000 // Clock speed: 2000 works 160-32000 kHz Default: 16000
#define COM1BAUD 9600L
// PINS
#define AMODEMPWR 21
#define ANTMODPWR 22  // antMod Power pin (1=ON, 0=OFF)
#define COM1SELECT 23  // set = antMod, clear = sbe16
#define WISPR3 24
#define WISPR4 25
#define DFRPWR 26   // unused
#define WISPR1 29
#define WISPR2 30
#define COM1TX 31
#define COM1RX 32
#define AMODEMRX 33
#define AMODEMTX 35
#define WISPR_PWR_ON 37
#define WISPR_PWR_OFF 42

typedef enum { ctd_dev=0, ant_dev } SerialDevType;

typedef struct MpcData {
  float joules;
  float volts;
  float voltMin;
  Serial com1;
  SerialDevType device;
} MpcData;
extern MpcData mpc;

void mpcDevSwitch(SerialDevType dev);
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
