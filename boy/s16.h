// s16.h
#ifndef H_S16
#define H_S16

typedef struct S16Info {
  Serial port;                // this is pam4
  bool auton;                 // now doing autonomous sampling
  bool on;                    // now init'd
  char *me;                   // s16
  char initStr[256];          // init time setting for ctd
  char startStr[256];         // start time setting for ctd
  char *takeSamp;             // TSSON or TS
  float depth;
  float temp;
  int log;                    // log fileid
  int pumpMode;               // 0=no 1=.5sec 2=during
  int sampInter;              // sample Interval for auton
  int timer;                  // Delay seconds expected between polled samples
  time_t sampT;
} S16Info;

static void s16Break(void);
static void s16Flush(void);
static bool s16Pending(void);

bool s16Data(void);
bool s16DataWait(void);
bool s16Prompt(void);
bool s16Read(void);
float s16Depth(void);
int s16Auton(bool auton);
int s16LogClose(void);
int s16LogOpen(void);
int s16Start(void);
int s16Stop(void);
void s16GetSamples(void);
void s16Init(void);
void s16Sample(void);
void s16Test(void);



#endif
