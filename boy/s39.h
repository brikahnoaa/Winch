// s39.h
#ifndef H_S39
#define H_S39

typedef struct S39Info {
  Serial port;
  bool auton;                 // autonomous mode, silent
  bool on;
  bool sampClear;             // clear after getSamples
  bool sampStore;             // store on device with TSSon - except auton
  char *me;
  float depth;
  float temp;
  int delay;                  // Delay seconds expected between polled samples
  int log;                    // log fileid
  int sampInt;                // sample interval for auton
  time_t sampT;               // read time() of last sample 
} S39Info;

static bool s39Pending(void);
static void s39Break(void);

Serial s39Port(void);
bool s39Data(void);
bool s39DataWait(void);
bool s39Prompt(void);
bool s39Read(void);
bool s39Surf(void);
float s39Depth(void);
float s39SurfD(void);
float s39Temp(void);
int s39Auton(bool auton);
int s39Avg(float *avg);
int s39Start(void);
int s39Stop(void);
int s39Velo(float *velo);
int s39LogClose(void);
int s39LogOpen(void);
void s39DevPwr(char c, bool on);
void s39Device(DevType dev);
void s39Flush(void);
void s39GetSamples(void);
void s39Init(void);
void s39Reset(void);
void s39Sample(void);
void s39Switch(AntType antenna);

#endif
