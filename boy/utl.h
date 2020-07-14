// utl.h - utility and shared stuff
#ifndef H_UTL
#define H_UTL

// 4K and 32K
#define BUFSZ 4096
#define FILESZ ((size_t)1024 * 32)
#define SETTLE 200          // settle time for electronics power on
#define null NULL

// millisecond delay
#define utlDelay(x) RTCDelayMicroSeconds(1000L*x);

typedef TUPort * Serial;

// sync with utlInit()
typedef enum {
    ant_err, boy_err, cfg_err, s16_err, s39_err,
    iri_err, ngk_err, wsp_err, log_err, dog_err,
    sizeof_err} ErrType;

typedef struct UtlInfo {
  bool console;             // respond to console input
  char *buf;
  char *errName[sizeof_err];
  char *ret;                // semi-global, returned by some char *utlFunc()
  char *str;
  int errCnt[sizeof_err];
  long bone;                // default bone to give dog for utlPet(0)
} UtlInfo;

typedef struct AllData {
  char *buf;                // 4K buffer (BUFSZ)
  char *str;                // 4K buffer (BUFSZ)
  int cycle;                // RiseCallFallData cycles
  int starts;               // number of starts (VEEPROM)
  long watch;               // watchdog countdown timer secs
  time_t startCycle;        // cycle start time
  time_t startProg;         // program start time
} AllData;

// the globals below are used by all modules // malloc'd in utlInit()
extern AllData all;

char *utlDate(void);
char *utlDateTime(void);
char *utlDateTimeSBE(void);
char *utlDateTimeFmt(time_t secs);
char *utlReadExpect(Serial port, char *in, char *expect, int wait);
char *utlNonPrint (char *in);        // format unprintable string
char *utlNonPrintBlock (char *in, int len);
char *utlTime(void);
int utlGetBlock(Serial port, char *buff, int msgSz, int respms);
int utlLookFor(Serial port, char *in, char *lookFor);
int utlLookForSecs(Serial port, char *in, char *lookFor, int secs);
int utlLogOpen(int *log, char *base);
int utlLogClose(int *fd);
int utlMatchAfter(char *out, char *str, char *sub, char *set);
int utlRead(Serial port, char *in);
int utlReadWait(Serial port, char *in, int wait);
void utlCloseErr(char *str);
void utlErr( ErrType err, char *str);
void utlInit(void);
void utlLogPathName(char *path, char *base, int day);
void utlLogTime(void);
void utlNap(int sec);
void utlPet(long pet);
void utlStop(char *out);
void utlTestLoop(void);
void utlX(void);
void utlWrite(Serial port, char *out, char *eol);
void utlWriteBlock(Serial port, char *out, int len);

#endif H_UTL
