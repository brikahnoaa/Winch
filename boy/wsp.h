// wsp.h

#define WSP_H

typedef struct WspInfo {
  bool on;                    // run wispr
  bool logging;
  char logFile[32];
  int card;                   // current wispr card in use
  int cards;                  // number of cards installed
  int cfSize;                 // size of CF card
  int cycle;                  // (60 min)
  int detInt;                 // how often to log detections (10 min)
  int detMax;                 // max detections to be logged
  int duty;                   // percent of cycle to run wispr
  int freeMin;                // free disk min GB, else go on to new wispr
  int gain;                   // mic sensitivity
  int log;                    // log fileid
  Serial port;                // mpcPamPort()
} WspInfo;

int wspDetect(int *detections);
int wspSpace(float *disk);
int wspStart(int card);
void wspInit(void);
void wspLog(char *str);
void wspStop(void);
int wspStorm(char *buf);
