// s39.h
#ifndef H_S39
#define H_S39

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
  int sampInter;              // sample interval for auton
  int timer;                  // timeout
  time_t sampT;               // read time() of last sample 
  Serial port;
} S39Info;

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
void s39Sample(void);
void s39Test(void);

#endif
