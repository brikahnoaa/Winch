// s39.h
#ifndef H_S39
#define H_S39

typedef struct S39Info {
  Serial port;                // this is ant module
  bool auton;                 // now doing autonomous sampling
  bool on;                    // now init'd
  char *me;                   // s39
  char initStr[256];          // init time setting for ctd
  char startStr[256];         // start time setting for ctd
  char *takeSamp;             // TSSON or TS
  float depth;
  float temp;
  int log;                    // log fileid
  //int pumpMode;               // 0=no 1=.5sec 2=during
  int sampInter;              // sample Interval for auton
  int timer;                  // Delay seconds expected between polled samples
  time_t sampT;
} S39Info;

static void s39Break(void);
static void s39Flush(void);
static bool s39Pending(void);

bool s39Data(void);
bool s39DataWait(void);
bool s39Prompt(void);
bool s39Read(void);
float s39Depth(void);
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
