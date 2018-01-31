// sys.h
#define SYS_STARTS 0L
#define SYS_STARTS_MAX 50L

typedef struct systemData {
  char logfile[12];
  char platform[6];   // rudicsland
  char program[12]; // added HM
  char project[6];    // rudicsland
  char version[12];
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
void sysInfo(char *out);
void sysShutdown(void);
void sysSleepUntilWoken(void);
