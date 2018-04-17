// wsp.h

#define WSP_H

typedef struct WspInfo {
  bool on;
  bool logging;
  char logFile[32];
  int log;                    // log fileid
  time_t time;
  Serial port;                // same as mpc.port, ant.port
} WspInfo;

static bool wspRead(void);

bool wspData(void);
void wspInit(void);
void wspLog(bool on);
void wspStop(void);
