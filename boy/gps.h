// gps.h
#define GPS_H

typedef struct GpsInfo {
  char phoneNum[32];
  char gpsLong[32];           // 123:45.6789W
  char gpsLat[32];            // 45:67.8900N
  char logFile[64];
  int log;
  Serial port;
} GpsInfo;

void gpsTst(void);
int gpsInit(void);
