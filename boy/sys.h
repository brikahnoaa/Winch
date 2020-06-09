// sys.h
#ifndef H_SYS
#define H_SYS

typedef enum {
  null_alm=0,
  bottomCurr_alm,
  midwayCurr_alm,
  ice_alm,
  ngkTimeout_alm,
  cableStuck_alm,
  sizeof_alm
} AlarmType;

typedef struct SysInfo {
  char logFile[32];
  char program[32];
  char version[32];
  int alarm[sizeof_alm];
  int day;                        // day long cycle
  int log;                        // log filehandle
  long diskFree;
  long diskSize;
} SysInfo;

static int checkVitals(void);
static int startCheck(void);
static long sysDiskFree(void);
static void sysDogPit(void);
static void dirSetup(char *path);
static void logInit(char *file);
static void preRun(int delay);

int sysInit(void);
int sysOSCmd(char *command, long filenum, char *ext, char *extt);
void sysAlarm(AlarmType alm);
void sysInfo(char *out);
void sysStop(char *out);
void sysFlush(void);

#endif
