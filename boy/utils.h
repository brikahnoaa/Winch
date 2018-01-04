// utils.h - utility and shared stuff

typedef TUPort *Serial;

// global extern
extern char scratch[];

void delayms(int x);
void serWrite(Serial port, char *out);
int serReadWait(Serial port, char *in, int wait);
char *unprintf (char *out, *in);    // format unprintable string

#define BUFSZ 1024

#define PLATFORM LARA
#define PROG_VERSION 4.0


// #define DEBUGx enables a macro named DBGx()
// see also DBGLVL below

// - blocks of code, as is
#define DEBUG
// 0 print func names
#define DEBUG0
// 1 print more interesting things
#undef DEBUG1
// 2 print details
#define DEBUG2
// debug related to wispr
#define DEBUGWISPR

// levels of debug
#undef DBGLVL
#ifdef DBGLVL
  #if (DBGLVL<=0)
    #define DEBUG0
  #endif
  #if (DBGLVL<=1)
    #define DEBUG1
  #endif
  #if (DBGLVL<=2)
    #define DEBUG2
  #endif
#endif // #ifdef DBGLVL

// DBG*
#ifdef DEBUG
#define DBG(X) X
#else
#define DBG(X)
#endif
// DEBUG0
#ifdef DEBUG0
#define DBG0(X) flogf("\n" X);
#else
#define DBG0(X)
#endif
// DEBUG1
#ifdef DEBUG1
#define DBG1(X) flogf("\n" X);
#else
#define DBG1(X)
#endif
// DEBUG2
#ifdef DEBUG2
#define DBG2(X) flogf("\n" X);
#else
#define DBG2(X)
#endif
