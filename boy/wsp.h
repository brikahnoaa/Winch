// wsp.h

#define WSP_H

typedef struct WspInfo {
  bool on;                    // run wispr
  bool logging;
  char logFile[32];
  int card;                   // current wispr card in use
  int cards;                  // number of cards installed
  int cfSize;                 // size of CF card
  int detInt;                 // how often to log detections (10 min)
  int detMax;                 // max detections to be logged (10)
  int duty;                   // percent of cycle to run wispr (50%)
  int freeMin;                // free disk min GB, else go on to new wispr
  int gain;                   // mic sensitivity
  int hour;                   // hour of day to finish cycle and rise
  int log;                    // log fileid
  int minimum;                // need at least this much to start (3min)
  int minute;                 // seconds in a minute, for fast tests (60)
  Serial port;                // mpcPamPort()
} WspInfo;

static int wspQuery(int *det);
int wspDetectDay(int *detections);
int wspDetectHour(int *detections);
int wspDetectMin(int minutes, int *detections);
int wspSpace(float *disk);
int wspStart(void);
int wspStorm(char *buf);
void wspExit(void);
void wspInit(void);
void wspLog(char *str);
void wspStop(void);
