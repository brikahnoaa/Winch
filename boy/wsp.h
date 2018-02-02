// wsp.h

#define DATABAUD 9600L
// This baud rate works with only the new wspa_053116 killerwhale file at the
// moment. 060116- AT

typedef struct WspInfo {
  bool on;
  short gain; //-g 0-3
  short num;    // number of DATA Boards installed
  short detInt;   //-D      //Minutes   //DATA DET INTERVAL
  short detMax; // Maximum Number of Detections to return
  short detNum; // Number of detections per one call to initiate #REALTIME call
  short dutycycl; // Duty cycle of recorder during one detection interval
  Serial port;
} WspInfo;
extern WspInfo wsp;

void wspInit(int board);
void wspStop(void);
