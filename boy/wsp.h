// wsp.h
#ifndef H_WSP
#define H_WSP

typedef struct WspInfo {
  Serial port;                // mpcPamPort()
  bool on;                    // run wispr
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
  int spectGain;              // mic sensitivity
  int spectRun;               // 0=no 1=before rise 2=hourly
  int wisprGain;              // mic sensitivity
} WspInfo;

// declared in boy.c, passed to wspDetectD
typedef struct WspData {      // pass from boy to wsp
  char spectr[128];           // wsp: output from spectrogram
  float free;                 // wsp: free disk on current card
  int card;                   // wsp: current card
  int detects;                // wsp: detections
} WspData;

static int wspClose(void);
static int wspOpen(void);
static int wspStart(void);
static void wspRemainS(int *remains);

int wspCardSwap(void);
int wspDateTime(void);
int wspDetect(WspData *wspd, int minutes);
int wspDetectH(int *detectH, char *spectr);
int wspDetectM(int *detectM, int minutes);
int wspLog(char *str);
int wspQuery(int *detect);
int wspSpace(float *disk);
int wspStorm(char *buf);
int wspStop(void);
void wspInit(void);

#endif
