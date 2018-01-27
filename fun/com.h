#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
typedef enum {false=0, true=1} bool;

#define PLATFORM LARA
#define PROG_VERSION 4.0

#define BUFSZ 1024
#define CRLF "\r\n"

#define flogf(...) printf(__VA_ARGS__)

// DBG() blocks of code, as is
// DBG0() print func names
// DBG1() print more interesting things
// DBG2() print details

// levels of debug
#ifdef DBG_LVL
  #if (DBG_LVL<=0)
    #define DEBUG0
  #endif
  #if (DBG_LVL<=1)
    #define DEBUG1
  #endif
  #if (DBG_LVL<=2)
    #define DEBUG2
  #endif
#endif // #ifdef DBG_LVL


#ifdef DEBUG
#define DBG(...) __VA_ARGS__
#else
#define DBG(...)
#endif

#ifdef DEBUG0
#define DBG0(...) flogf(" " __VA_ARGS__);
#else
#define DBG0(...)
#endif

#ifdef DEBUG1
#define DBG1(...) flogf("\n" __VA_ARGS__);
#else
#define DBG1(...)
#endif

#ifdef DEBUG2
#define DBG2(...) flogf("\n" __VA_ARGS__);
#else
#define DBG2(...)
#endif


