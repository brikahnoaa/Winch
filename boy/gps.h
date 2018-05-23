// gps.h
#define GPS_H

typedef struct GpsInfo {
  char phoneNum[32];
  char date[32];
  char lat[32];             // 45:67.8900N
  char long[32];            // 123:45.6789W
  char platform[32];        // rudicsland
  char project[32];         // rudicsland
  char time[32];
  char logFile[64];
  int log;
  int sats;
  Serial port;
} GpsInfo;

static bool gpsPending(void);
static bool gpsPrompt(void);
static bool gpsRead(void);
static int gpsData(void);

void gpsInit(void);
void gpsStart(void);
void gpsStop(void);
