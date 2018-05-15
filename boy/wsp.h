// wsp.h
/*
#ifndef MPC_H
#include <mpc.h>
#endif
*/

#define WSP_H

typedef struct WspInfo {
  bool on;
  bool logging;
  char logFile[32];
  float freeMin;              // free disk minimum, else go on to new pam
  int detMax;                 // stop detections at max; 0 means no limit
  int duty;                   // duty cycle = percent of each hour to run
  int gain;                   // param
  int num;                    // number of wspr cards
  int log;                    // log fileid
  time_t time;
  int pam;             // last and current pam card in use
  Serial port;                // same as mpc.port, ant.port
} WspInfo;

static int wspRead(char *buf);
static bool wspData(void);

int wspStart(int pam);
void wspInit(void);
void wspLog(bool on);
void wspStop(void);
