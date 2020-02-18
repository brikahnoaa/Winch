// ant.h
#ifndef H_ANT
#define H_ANT

typedef enum { null_dev, cf2_dev, a3la_dev } DevType;
typedef enum { null_ant, gps_ant, irid_ant } AntType;

typedef struct AntInfo {
  bool auton;                 // autonomous mode, silent
  bool on;
  bool sampClear;             // clear after getSamples
  bool sampStore;             // store on device with TSSon - except auton
  bool surf;                  // on surface
  char *me;
  char initStr[256];          // init time setting for ctd
  char startStr[256];         // start time setting for ctd
  float depth;
  float subD;                 // subsurfaceD; ant.surfD + boy.ant2tip
  float surfD;                // surfaceDepth of buoy when ant is floating
  float temp;
  int delay;
  int log;
  int sampInter;              // sample interval for auton
  time_t sampT;               // read time() of last sample 
  AntType antenna;
  DevType dev;
  Serial port;
} AntInfo;

static bool antPending(void);
static void antBreak(void);

AntType antAntenna(void);
Serial antPort(void);
bool antData(void);
bool antDataWait(void);
bool antPrompt(void);
bool antRead(void);
bool antSurf(void);
float antDepth(void);
float antSurfD(void);
float antTemp(void);
int antAuton(bool auton);
int antAvg(float *avg);
int antStart(void);
int antStop(void);
int antLogClose(void);
int antLogOpen(void);
void antDevPwr(char c, bool on);
void antDevice(DevType dev);
void antFlush(void);
void antGetSamples(void);
void antInit(void);
void antSample(void);
void antSwitch(AntType antenna);

#endif
