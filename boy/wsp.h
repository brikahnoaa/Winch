// wsp.h
#ifndef H_WSP
#define H_WSP

typedef struct WspInfo {
  bool on;                    // run wispr
  bool logging;
  bool storm;                 // run storm detect
  char logFile[32];
  int cardUse;                // current wispr card in use
  int cardsNum;               // number of cards installed
  int cfSize;                 // size of CF card
  int detInt;                 // how often to log detections (10 min)
  int detMax;                 // max detections to be logged (10)
  int duty;                   // percent of cycle to run wispr (50%)
  int freeMin;                // free disk min GB, else go on to new wispr
  int gain;                   // mic sensitivity
  int hour;                   // hour of day to finish cycle and rise (7)
  int log;                    // log fileid
  int minimum;                // min minutes needed to start wispr
  time_t riseT;               // time to rise
  Serial port;                // mpcPamPort()
} WspInfo;

int wspQuery(int *det);
int wspDetectDay(int *detections);
int wspDetectHour(int *detections);
int wspDetectMin(int minutes, int *detections);
int wspSetup(char *gps, int gain);
int wspSpace(float *disk);
int wspStart(int card);
int wspStorm(char *buf);
time_t wspRiseT(int riseHour);
void wspInit(void);
void wspLog(char *str);
void wspStop(void);

#endif
