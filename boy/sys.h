// sys.h

typedef enum {
  null_alm=0,
  bottomCurr_alm,
  midwayCurr_alm,
  ice_alm,
  ngkTimeout_alm,
  sizeof_alm
} AlarmType;

typedef struct systemData {
  char cfgFile[16];
  char logFile[16];
  char platform[16];    // rudicsland
  char program[16];
  char project[16];     // rudicsland
  char version[16];
  int alarm[sizeof_alm];
  int log;              // log filehandle
  long starts;
  long startsMax;
} systemData;

static int checkVitals(void);
static void configFile(void);
static void dirSetup(char *path);
static void logInit(void);
static void preRun(int delay);
static void startCheck(void);
static void sysInit(void);

int sysOSCmd(char *command, long filenum, char *ext, char *extt);
void main(void);
void sysAlarm(AlarmType alm);
void sysInfo(char *out);
void sysShutdown(char *out);
void sysSleepUntilWoken(void);
