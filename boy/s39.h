// s39.h
#ifndef H_S39
#define H_S39

// nodes in a ant.ring store previous depth/time values
typedef struct RingNode {
  float depth;
  time_t sampT;
  struct RingNode *next;
} RingNode;

typedef struct S39Info {
  bool auton;                 // autonomous mode, silent
  bool on;
  bool sampClear;             // clear after getSamples
  bool sampStore;             // store on device with TSSon - except auton
  char *me;
  char initStr[256];          // init time setting for ctd
  char startStr[256];         // start time setting for ctd
  float depth;
  float temp;
  int delay;
  int log;
  int ringSize;               // number of nodes in the (depth,time) ring
  int sampInter;              // sample interval for auton
  time_t sampT;               // read time() of last sample 
  RingNode *ring;             // nodes in the (depth,time) ring
  Serial port;
} S39Info;

int s39Avg(float *avg);
int s39Velo(float *velo);
void s39Ring(float depth, time_t sampT);

static bool s39Pending(void);
static void s39Break(void);
static void s39Flush(void);

bool s39Data(void);
bool s39DataWait(void);
bool s39Prompt(void);
bool s39Read(void);
float s39Depth(void);
float s39Temp(void);
int s39Auton(bool auton);
int s39LogClose(void);
int s39LogOpen(void);
int s39Start(void);
int s39Stop(void);
void s39GetSamples(void);
void s39Init(void);
void s39Reset(void);
void s39Sample(void);
void s39Test(void);

#endif
