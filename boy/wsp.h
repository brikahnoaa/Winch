// wsp.h
#ifndef H_WSP
#define H_WSP

#define WSP_OPEN "<wispr>"
#define WSP_CLOSE "</wispr>"
typedef struct WspInfo {
  Serial port;                // mpcPamPort()
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
  int log;                    // log fileid
  int phaseH;                 // hours per data phase (24)
  int riseH;                  // hour of day to rise (2)
  int riseInt;                // rise interval; if set, ignore riseH (0)
  int spectGain;              // mic sensitivity
  int spectRun;               // 0=no 1=before rise 2=hourly
  int wisprGain;              // mic sensitivity
} WspInfo;

static void wspRemainS(int *remains);

int wspCardSwap(void);
int wspDateTime(void);
int wspDetectD(int *detect);
int wspDetectH(int *detect);
int wspDetectM(int *detect, int minutes);
int wspLog(char *str);
int wspQuery(int *detect);
int wspSpace(float *disk);
int wspStart(void);
int wspStop(void);
int wspStorm(char *buf);
void wspInit(void);
void wspRiseT(time_t *riseT);

#endif
