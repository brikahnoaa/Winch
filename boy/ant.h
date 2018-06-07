// ant.h
#define ANT_H

#define ANT_BAUD 9600

typedef enum { null_dev, cf2_dev, a3la_dev } DevType;
typedef enum { null_ant, gps_ant, irid_ant } AntType;

// nodes in a ant.ring store previous depth/time values
typedef struct RingNode {
  float depth;
  time_t sampT;
  struct RingNode *next;
} RingNode;

typedef struct AntInfo {
  bool auton;                 // autonomous mode
  bool autoSample;            // antSample after antRead
  bool logging;               // use TS or TSSon
  bool surf;                  // on surface
  char logFile[64];
  char samCmd[32];            // TS or TSSon
  float depth;
  float subD;                 // subsurfaceD; ant.surfD + boy.ant2tip
  float surfD;                // surfaceDepth of CTD when ant is floating
  float temp;
  int delay;
  int fresh;                  // time()-ant.time < fresh is usable
  int log;
  int ringFresh;              // ant.fresh * ant.ringSize
  int ringSize;               // number of nodes in the (depth,time) ring
  time_t sampT;               // time() of last sample
  RingNode *ring;             // nodes in the (depth,time) ring
  AntType antenna;
  DevType dev;
  Serial port;
} AntInfo;

static bool antFresh(void);
static bool antPending(void);
static bool antPrompt(void);
static bool antRead(void);
static bool antDataWait(void);
static int antData(void);
static void antBreak(void);
static void antMovSam(void);
static void antSample(void);

int ringDir(float v);
void ringPrint(void);
void ringSamp(void);

bool antSurf(void);
bool antVelo(float *velo);
float antDepth(void);
float antSurfD(void);
float antTemp(void);
void antAuton(bool auton);
void antAutoSample(bool autos);
void antDevice(DevType dev);
void antDevPwr(char c, bool on);
void antFlush(void);
void antGetSamples(void);
void antInit(void);
void antRingReset(void);
void antStart(void);
void antStop(void);
void antSwitch(AntType antenna);
Serial antPort(void);
