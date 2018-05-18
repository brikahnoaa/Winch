// wsp.h

#define WSP_H

typedef struct WspInfo {
  bool on;                    // run wispr
  bool logging;
  char logFile[32];
  float freeMin;              // free disk minimum, else go on to new wispr
  int card;                   // current wispr card in use
  int cards;                  // number of cards installed
  int cycle;                  // (60 min)
  int detMax;                 // max detections to be logged
  int duty;                   // percent of cycle to run wispr
  int gain;                   // mic sensitivity
  int log;                    // log fileid
  int num;                    // number of wspr cards
  int query;                  // how often to log detections (10 min)
  time_t time;
  Serial port;                // port for all pam science
} WspInfo;

int wspDetect(int *detections);
int wspSpace(float *disk);
int wspStart(int card);
void wspInit(void);
void wspLog(char *str);
void wspStop(void);
int wspStorm(char *buf);
