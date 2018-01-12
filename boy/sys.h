// sys.h

typedef struct systemData {
  char logfile[12];
  char platformID[6];   // rudicsland
  char programName[20]; // added HM
  char projectID[6];    // rudicsland
  int starts;
  int startsMax;
} systemData;
extern systemData sys;

void preRun(int delay);
void sysStarts(void);
