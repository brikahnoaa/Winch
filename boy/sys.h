// sys.h
#define STARTS "0"
#define STARTS_MAX "50"
#define SYS_LOG "C:SYS.LOG"
#define SYS_CFG "C:SYS.CFG"

typedef enum {
  null_alm=0,
  bottomCurrent_alm,
  midwayCurrent_alm,
  ice_alm,
  ngkTimeout_alm,
  sizeof_alm
} AlarmType;

typedef struct systemData {
  char cfg[16];
  char log[16];
  char platform[16];   // rudicsland
  char program[16];
  char project[16];    // rudicsland
  char version[16];
  int alarm[sizeof_alm];
  int filenum;
  long starts;
  long startsMax;
} systemData;
extern systemData sys;

static int checkVitals(void);
static void configFile(void);
static void dirSetup(char *path);
static void logInit(void);
static void preRun(int delay);
static void restartCheck(void);
static void sysInit(void);

int sysOSCmd(char *command, long filenum, char *ext, char *extt);
void main(void);
void sysAlarm(AlarmType alm);
void sysInfo(char *out);
void sysShutdown(void);
void sysSleepUntilWoken(void);
