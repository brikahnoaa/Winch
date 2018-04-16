// gps.h
#define GPS_H

// from ant.h
//typedef enum { null_dev, cf2_dev, a3la_dev } DevType;
//typedef enum { null_ant, gps_ant, irid_ant } AntType;

typedef struct GpsInfo {
  bool on;
  char gpsLong[32];           // 123:45.6789W
  char gpsLat[32];            // 45:67.8900N
  char logFile[64];
  int delay;
  int log;
  Serial port;
} GpsInfo;

static bool gpsPending(void);
static bool gpsPrompt(void);
static bool gpsRead(void);
static int gpsData(void);

void gpsFlush(void);
void gpsInit(void);
void gpsOn(bool on);
void gpsStop(void);
