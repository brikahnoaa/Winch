// wsp.h
#ifndef H_WSP
#define H_WSP

typedef struct WspInfo {
  Serial port;                // mpcPamPort()
  bool on;                    // wispr running <wispr>
  bool logging;               // maintain log file
  bool wisprTest;             // set -W flaf
  char spectCmd[32];          // spectragram command
  char spectFlag[64];         // -v1 -C12 -n512 -o256 
  char spectGain[8];          // mic sensitivity
  char spectLog[32];          // -l noise
  char wisprCmd[32];          // wispr command
  char wisprFlag[64];         // -v2 -T8 -b15 -M2 -F2
  char wisprGain[8];          // mic sensitivity
  char wisprLog[32];          // -l detect
  float diskFree;             // free space on card
  int card;                   // wispr card in use; starts high, goes down
  int detMax;                 // max detections to be logged (10)
  int diskMin;                // free disk min GB, else go on to new wispr
  int log;                    // log fileid
  int restM;                  // minutes to rest detection
  int runM;                   // minutes to run detection
} WspInfo;

// declared in boy.c, passed to wspDetectD
typedef struct WspData {      // pass from boy to wsp
  bool logging;
  char spectr[128];           // wsp: output from spectrogram
  float free;                 // wsp: free disk on current card
  int card;                   // wsp: current card
  int detects;                // wsp: detections
  int log;
} WspData;

int wspCardSwap(void);
int wspClose(void);
int wspCmd(char *out, char *cmd, int seconds);
int wspDateTime(void);
int wspDetect(WspData *wspd, int minutes);
int wspDetectM(int *detected, float *free, int minutes);
int wspLog(char *str);
int wspOpen(void);
int wspQuery(int *detect);
int wspSpace(float *disk);
int wspSpectr(char *buf);
int wspStart(void);
int wspStop(void);
void wspInit(void);

#endif
