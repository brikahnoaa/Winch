// wsp.h
#ifndef H_WSP
#define H_WSP

#define WSP_OPEN "<wispr>"
#define WSP_CLOSE "</wispr>"
typedef struct WspInfo {
  bool on;                    // run wispr
  char logFile[32];           // wispr.log
  char spectCmd[32];          // spectragram command
  char spectFlag[64];         // -T10 -C12 -v1 -n512 -o256 -g0 -t20
  char spectLog[32];          // -l noise
  char wisprCmd[32];          // wispr command
  char wisprFlag[64];         // -v2 -T8 -b15 -M2 -F2
  char wisprLog[32];          // -l detect
  int card;                   // wispr card in use; starts high, goes down
  int detInt;                 // how often to log detections (10 min)
  int detMax;                 // max detections to be logged (10)
  int diskFree;               // free disk min GB, else go on to new wispr
  int dutyM;                  // percent of cycle to run wispr (50%)
  int gain;                   // mic sensitivity
  int log;                    // log fileid
  int riseH;                  // hour of day to rise
  int spectRun;               // 0=no 1=before rise 2=hourly

  Serial port;                // mpcPamPort()
} WspInfo;

static void wspRemains(int *remains);
static void wspRiseT(time_t *riseT);

int wspCardSwap(void);
int wspDetectD(int *detect);
int wspDetectH(int *detect);
int wspDetectM(int *detect, int minutes);
int wspQuery(int *detect);
int wspSetup(char *gps, int gain);
int wspSpace(float *disk);
int wspStart(void);
int wspStop(void);
int wspStorm(char *buf);
void wspDateTime(void);
void wspInit(void);
void wspLog(char *str);

#endif
