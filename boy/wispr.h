// wispr.h

#define WISPRBAUD 9600L
// This baud rate works with only the new wispr_053116 killerwhale file at the
// moment. 060116- AT

typedef struct WisprInfo {
  bool off;
  short gain; //-g 0-3
  short num;    // number of WISPR Boards installed
  short detInt;   //-D      //Minutes   //WISPR DET INTERVAL
  short detMax; // Maximum Number of Detections to return
  short detNum; // Number of detections per one call to initiate #REALTIME call
  short dutycycl; // Duty cycle of recorder during one detection interval
  Serial port;
} WisprInfo;
extern WisprInfo wisp;

short wisprData();
void wisprPower(bool);
bool wisprExit(void);
void wisprGain(short);
float getWisprFreeSpace();
void wisprDFP(void);
void wisprTFP();
void wisprDet(int);
void wisprSafeShutdown();
void changeWispr(short);
void getWisprSettings();
void wisprWriteFile(int);
void createDtxFile();
bool wisprStatus();
void gatherWisprFreeSpace();
void updateWisprFRS();
void wisprGPS();
void wisprInit(bool);
bool wisprExpectedReturn(short, bool);
