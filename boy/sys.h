// sys.h

typedef struct systemData {
  char logfile[12];
  char platform[6];   // rudicsland
  char programName[20]; // added HM
  char projectID[6];    // rudicsland
  int filenum;
  int starts;
  int startsMax;
} systemData;
extern systemData sys;

static void preRun(int delay);
static void sysInit(void);
static void sysStarts(void);
void main(void);
