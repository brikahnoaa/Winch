// gps.h
#define GPS_H

typedef struct GpsInfo {
  char phoneNum[16];
  char date[16];
  char lat[16];             // 45:67.8900N
  char lon[16];             // 123:45.6789W
  char platform[16];        // rudicsland
  char project[16];         // rudicsland
  char projHdr[16];         // ???csProjPlat
  char time[16];
  char logFile[64];
  int redial;               // how many calls to make
  int rudResp;              // wait secs for rudics to respond to a block
  int signal;
  int signalMin;
  int timeout;
  int log;
  int sats;
  Serial port;
} GpsInfo;

static int gpsSats(void);
bool gpsResetTime(void);
int gpsStart(void);
int gpsStats(void);
int iridCRC(char *buf, int cnt);
int iridDial(void);
int iridPrompt(void);
int iridSendTest(int msgLen);
int iridSig(void);
void gpsInit(void);
void gpsStop(void);
void iridHup(void);
