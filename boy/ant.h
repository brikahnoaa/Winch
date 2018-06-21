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
  bool on;
  bool surf;                  // on surface
  char logFile[64];
  char samCmd[32];            // TS or TSSon
  float depth;
  float subD;                 // subsurfaceD; ant.surfD + boy.ant2tip
  float surfD;                // surfaceDepth of CTD when ant is floating
  float temp;
  int delay;
  int log;
  int ringSize;               // number of nodes in the (depth,time) ring
  time_t sampT;               // read time() of last sample 
  RingNode *ring;             // nodes in the (depth,time) ring
  AntType antenna;
  DevType dev;
  Serial port;
} AntInfo;

static int ringDir(float v);
static void ringSamp(void);
void ringPrint(void);

static bool antPending(void);
static bool antRead(void);
static void antBreak(void);
static void antMovSam(void);
static void antSample(void);

bool antData(void);
bool antDataWait(void);
bool antPrompt(void);
bool antSurf(void);
float antDepth(void);
float antSurfD(void);
float antTemp(void);
int antAvg(float *avg);
int antVelo(float *velo);
void antDevice(DevType dev);
void antDevPwr(char c, bool on);
void antFlush(void);
void antGetSamples(void);
void antInit(void);
void antReset(void);
void antStart(void);
void antStop(void);
void antSwitch(AntType antenna);
Serial antPort(void);
