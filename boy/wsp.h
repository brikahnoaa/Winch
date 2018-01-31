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

short wspData(void);
void wspPower(bool);
bool wspExit(void);
void wspGain(short);
float wspFreeSpace(void);
void wspDFP(void);
void wspTFP(void);
void wspDet(int);
void wspSafeShutdown(void);
void wspChangeCard(short);
void wspSettings(void);
void wspWriteFile(int);
void createDtxFile(void);
bool wspStatus(void);
void gatherWspFreeSpace(void);
void upwspeWspFRS(void);
void wspGPS(void);
void wspInit(int board);
bool wspExpectedReturn(short, bool);
