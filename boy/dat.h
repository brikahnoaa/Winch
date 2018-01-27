// dat.h

#define DATABAUD 9600L
// This baud rate works with only the new data_053116 killerwhale file at the
// moment. 060116- AT

typedef struct DataInfo {
  bool on;
  short gain; //-g 0-3
  short num;    // number of DATA Boards installed
  short detInt;   //-D      //Minutes   //DATA DET INTERVAL
  short detMax; // Maximum Number of Detections to return
  short detNum; // Number of detections per one call to initiate #REALTIME call
  short dutycycl; // Duty cycle of recorder during one detection interval
  Serial port;
} DataInfo;
extern DataInfo data;

short dataData(void);
void dataPower(bool);
bool dataExit(void);
void dataGain(short);
float getDataFreeSpace(void);
void dataDFP(void);
void dataTFP(void);
void dataDet(int);
void dataSafeShutdown(void);
void changeData(short);
void getDataSettings(void);
void dataWriteFile(int);
void createDtxFile(void);
bool dataStatus(void);
void gatherDataFreeSpace(void);
void updateDataFRS(void);
void dataGPS(void);
void dataInit(bool);
bool dataExpectedReturn(short, bool);
