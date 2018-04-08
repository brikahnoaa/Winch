// ant.h
#define ANT_H

#define ANT_BAUD 9600

typedef enum { null_dev, cf2_dev, a3la_dev } DevType;
typedef enum { null_ant, gps_ant, irid_ant } AntType;

typedef struct AntInfo {
  bool auton;
  bool surf;                  // on surface
  char gpsLong[32];           // 123:45.6789W
  char gpsLat[32];            // 45:67.8900N
  float depth;
  float surfD;                // depth of CTD when ant is floating
  float samples[10];          // depth measurement during auto_mod
  float sampleRes;            // accuracy - change greater than this to count
  float temp;
  int delay;
  int fresh;                  // time()-ant.time < fresh is usable
  int sampleCnt;
  time_t time;                // time() of last sample
  AntType antenna;
  DevType dev;
  Serial port;
} AntInfo;

static bool antData(void);
static bool antPrompt(void);
static bool fresh(void);
static bool pending(void);
static void antBreak(void);
static void antRead(void);
static void antSample(void);
static void antSwitch(AntType antenna);

bool antSurf(void);
float antDepth(void);
float antMoving(void);
float antTemp(void);
void antAuton(bool auton);
void antDevice(DevType dev);
void antInit(void);
void antStart(void);
void antStop(void);
