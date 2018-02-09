// wsp.h

#define WSP_BAUD 9600L
// This baud rate works with only the new wspa_053116 killerwhale file at the
// moment. 060116- AT

typedef struct WspInfo {
  bool on;
  char logFile[16];
  int detInt;   //-D      //Minutes   //DATA DET INTERVAL
  int detMax;   // Maximum Number of Detections to return
  int detOff;   // sleep
  int gain; //-g 0-3
  int count;    // number of DATA Boards
  int board;    // active board
  int log;
  Serial port;
} WspInfo;

void wspInit(int board);
void wspStop(void);
void wspFlush(void);
