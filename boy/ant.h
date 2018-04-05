// ant.h
#define ANT_H

#define ANT_BAUD 9600

typedef enum { cf2_dev, a3la_dev } DevType;
typedef enum { gps_ant, irid_ant } AntType;

typedef struct AntInfo {
  bool auton;
  bool pending;               // TS query or start of auton before 1st sample
  bool surf;                  // on surface
  char gpsLong[32];           // 123:45.6789W
  char gpsLat[32];            // 45:67.8900N
  float depth;
  float surfD;                // depth of CTD when ant is floating
  float samples[10];          // depth measurement during auto_mod
  float sampleRes;            // accuracy - change greater than this to count
  float temp;
  int delay;
  int sampleCnt;
  DevType dev;
  Serial port;
} AntInfo;

static bool antData(void);
static bool antPrompt(void);
static void antRead(void);
static void antSample(void);

bool antSurf(void);
float antDepth(void);
void antDevice(DevType dev);
float antMoving(void);
float antTemp(void);
void antAuton(bool auton);
void antInit(void);
void antStart(void);
void antStop(void);
