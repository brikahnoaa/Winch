// utl.h - utility and shared stuff
#ifndef H_UTL
#define H_UTL

#define null NULL

// 4K and 32K
#define BUFSZ 4096
#define FILESZ ((size_t)1024 * 32)

// millisecond delay
#define utlDelay(x) RTCDelayMicroSeconds((long)x*1000L);
// settle time for electronics power on
#define SETTLE 200

typedef TUPort * Serial;

// sync with utlInit()
typedef enum {
    ant_err, boy_err, cfg_err, s16_err, 
    iri_err, ngk_err, wsp_err, 
    phase_err, log_err, 
    sizeof_err} ErrType;

typedef struct UtlInfo {
  bool ignoreCon;             // ignore console input
  char *buf;
  char *errName[sizeof_err];
  char *ret;                // semi-global, returned by some char *utlFunc()
  char *str;
  int errCnt[sizeof_err];
} UtlInfo;

typedef struct AllData {
  char *buf;                // 4K buffer (BUFSZ)
  char *str;                // 4K buffer (BUFSZ)
  int cycle;                // RiseCallFallData cycles
  int starts;               // number of starts (VEEPROM)
  time_t startCycle;        // cycle start time
  time_t startProg;         // program start time
} AllData;

// the globals below are used by all modules // malloc'd in utlInit()
extern AllData all;

char *utlDate(void);
char *utlDateTime(void);
char *utlDateTimeS16(void);
char *utlDateTimeFmt(time_t secs);
char *utlExpect(Serial port, char *buf, char *expect, int wait);
char *utlNonPrint (char *in);        // format unprintable string
char *utlNonPrintBlock (char *in, int len);
char *utlTime(void);
int utlLogOpen(int *log, char *base);
int utlLogClose(int *fd);
int utlMatchAfter(char *out, char *str, char *sub, char *set);
int utlTrim(char *str);
int utlRead(Serial port, char *in);
int utlReadWait(Serial port, char *in, int wait);
void utlCloseErr(char *str);
void utlErr( ErrType err, char *str);
void utlInit(void);
void utlLogPathName(char *path, char *base, int day);
void utlLogTime(void);
void utlNap(int sec);
void utlPet(void);
void utlSleep(void);
void utlStop(char *out);
void utlTestLoop(void);
void utlX(void);
void utlWrite(Serial port, char *out, char *eol);
void utlWriteBlock(Serial port, char *out, int len);

// void utlDelay(int milli);
#endif
