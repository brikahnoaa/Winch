// s16.h
#ifndef H_S16
#define H_S16

typedef struct S16Info {
  bool auton;
  bool clearSamp;             // clear stored samples
  bool sampStore;             // store on device with TSSon - except auton
  bool on;
  bool sbe39;                 // init and use sbe39
  char logFile[32];
  char samCmd[16];
  float cond;
  float depth;
  float temp;
  int delay;                  // Delay seconds expected between polled samples
  int log;                    // log fileid
  int sampleInt;              // sample Interval for auton
  time_t time;
  Serial port;                // same as mpc.port, ant.port
} S16Info;

static void s16Break(void);
static void s16Flush(void);

bool s16Data(void);
bool s16DataWait(void);
bool s16Pending(void);
bool s16Prompt(void);
bool s16Read(void);
float s16Depth(void);
int s16Auton(bool auton);
int s16Start(void);
int s16Stop(void);
void s16GetSamples(void);
void s16Init(void);
void s16Sample(void);
void s16Test(void);

#endif
