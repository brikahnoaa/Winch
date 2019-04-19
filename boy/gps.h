// gps.h
#ifndef H_GPS
#define H_GPS

typedef struct GpsStats {
  char lat[32];             // 45:67.8900N
  char lng[32];             // 123:45.6789W
  char date[32];
  char time[32];
} GpsStats;

typedef struct GpsInfo {
  bool setTime;             // flag, reset time via gps, starts true
  char phoneNum[16];
  char platform[16];        // rudicsland
  char project[16];         // rudicsland
  char projHdr[16];         // ???csProjPlat
  char logFile[64];
  char *buf;                // buffer for file transfer
  int fileBlock;            // how many bytes to upload in a file block (512)
  int fileMax;              // how many bytes max in a file (8000)
  int fileCnt;              // how many files to upload in a connection
  int hdrPause;             // wait sec for rudics header response (20)
  int hdrTry;               // header retry (3)
  int hupMs;                // ms +++ ms for HUP (2000)
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

bool gpsSetTime(GpsStats *stats);
int gpsDateTime(GpsStats *stats);
int gpsLatLng(GpsStats *stats);
int gpsStart(void);
int iridDial(void);
int iridLandCmds(char *buff, int *len);
int iridLandResp(char *buff);
int iridProcessCmds(char *buff);
int iridProjHdr(void);
int iridSendBlock(char *buff, int msgLen, int blockNum, int blockMany);
int iridSendFile(char *fname);
int iridSig(void);
static int gpsSats(void);
static int iridCRC(char *buf, int cnt);
static int iridPrompt(void);
void gpsInit(void);
void gpsStop(void);
void iridHup(void);

#endif
