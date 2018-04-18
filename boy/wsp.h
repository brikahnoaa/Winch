// wsp.h

#define WSP_H

typedef struct WspInfo {
  bool on;
  bool logging;
  char logFile[32];
  int detMax;                 // stop detections at max; 0 means no limit
  int duty;                   // duty cycle = percent of each hour to run
  int gain;                   // param
  int num;                    // number of wspr cards
  int log;                    // log fileid
  time_t time;
  Serial port;                // same as mpc.port, ant.port
} WspInfo;

static int wspRead(char *buf);
static bool wspData(void);

void wspInit(void);
void wspLog(bool on);
void wspStart(void);
void wspStop(void);
