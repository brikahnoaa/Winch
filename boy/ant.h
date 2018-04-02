// ant.h

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
  Serial port;
} AntInfo;

static void antRead(void);

bool antPrompt(void);
bool antData(void);
bool antSurf(void);
float antDepth(void);
float antMoving(void);
float antTemp(void);
void antAuto(bool auton);
void antInit(void);
void antSample(void);
void antStop(void);
