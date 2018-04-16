// ant.h
#define ANT_H

#define ANT_BAUD 9600

typedef enum { null_dev, cf2_dev, a3la_dev } DevType;
typedef enum { null_ant, gps_ant, irid_ant } AntType;

typedef struct AntInfo {
  bool auton;                 // autonomous mode
  bool autoSample;            // antSample after antRead
  bool logging;               // use TS or TSSon
  bool on;                    // powered up
  bool surf;                  // on surface
  char gpsLong[32];           // 123:45.6789W
  char gpsLat[32];            // 45:67.8900N
  char logFile[64];
  char samCmd[32];            // TS or TSSon
  float depth;
  float surfD;                // depth of CTD when ant is floating
  float samQue[32];          // depth measurement during auto_mod
  float samRes;            // accuracy - change greater than this to count
  float temp;
  int delay;
  int fresh;                  // time()-ant.time < fresh is usable
  int log;
  int samCnt;                 // sam in samQue
  int samLen;                 // sizeof samQue
  time_t lastT;                // time() of last sample
  AntType antenna;
  DevType dev;
  Serial port;
} AntInfo;

static bool antFresh(void);
static bool antPending(void);
static bool antPrompt(void);
static bool antRead(void);
static bool antSampleRead(void);
static int antData(void);
static void antBreak(void);
static void antMovSam(void);
static void antSample(void);
static void antSwitch(AntType antenna);

bool antSurf(void);
float antDepth(void);
float antMoving(void);
float antTemp(void);
void antAuton(bool auton);
void antAutoSample(bool autos);
void antDevice(DevType dev);
void antFlush(void);
void antGetSamples(void);
void antInit(void);
void antOn(bool on);
void antStop(void);
Serial antPort(void);
