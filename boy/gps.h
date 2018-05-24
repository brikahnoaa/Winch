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
  int signal;
  int timeout;
  int log;
  int sats;
  Serial port;
} GpsInfo;

int gpsStats(void);
void gpsInit(void);
void gpsStart(void);
void gpsStop(void);
int iridCRC(char *buf, int cnt);
int iridSig(void);
int iridPrompt(void);
