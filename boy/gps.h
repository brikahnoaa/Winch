// gps.h
#define GPS_H

typedef struct GpsInfo {
  char latitude[32];            // 45:67.8900N
  char longitude[32];           // 123:45.6789W
  char logFile[64];
  char phoneNum[32];
  char utcdate[32];             // 05-08-2018
  char utctime[32];             // 22:49:17.000
  int log;
  Serial port;
} GpsInfo;

int gpsInit(void);
int gpsSats(void);
int gpsStart(void);
int gpsStop(void);
