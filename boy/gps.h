// gps.h
#define GPS_H

typedef struct GpsInfo {
  bool setTime;             // flag, reset time via gps, starts true
  char phoneNum[16];
  char date[16];
  char lat[16];             // 45:67.8900N
  char lng[16];             // 123:45.6789W
  char platform[16];        // rudicsland
  char project[16];         // rudicsland
  char projHdr[16];         // ???csProjPlat
  char time[16];
  char logFile[64];
  int fileSz;               // how many bytes to upload in a file/block
  int fileCnt;              // how many files to upload in a connection
  int hdrPause;             // wait sec for rudics header response (20)
  int hdrTry;               // header retry (3)
  int redial;               // how many calls to make (5)
  int rudBaud;              // effective baud rate for rudics (2400)
  int rudResp;              // wait secs for respond to a block (30)
  int rudUsec;              // microsec delay from rudBaud
  int sendSz;               // how many bytes to send in a burst
  int signal;
  int signalMin;
  int timeout;
  int log;
  int sats;
  Serial port;
} GpsInfo;

static int gpsSats(void);
static int iridCRC(char *buf, int cnt);
static int iridPrompt(void);
bool gpsSetTime(void);
int gpsStart(void);
int gpsStats(void);
int iridDial(void);
int iridLandCmds(char *buff, int *len);
int iridLandResp(char *buff);
int iridProjHdr(void);
int iridSendBlock(char *buff, int msgLen, int blockNum, int blockMany);
int iridSendFile(char *fname);
int iridSig(void);
void gpsInit(void);
void gpsStop(void);
void iridHup(void);
