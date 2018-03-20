// com.h  common to all
typedef struct ComInfo {
  bool dbg0;
  bool dbg1;
  bool dbg2;
} ComInfo;

extern ComInfo com;

#include <cfxpico.h> // Persistor PicoDOS Definitions
#include <cfxad.h>
#include <dirent.h>   // PicoDOS POSIX-like Directory Access Defines
#include <dosdrive.h> // PicoDOS DOS Drive and Directory Definitions
#include <fcntl.h> // PicoDOS POSIX-like File Access Definitions
#include <stat.h> // PicoDOS POSIX-like File Status Definitions
#include <termios.h> // PicoDOS POSIX-like Terminal I/O Definitions
#include <unistd.h> // PicoDOS POSIX-like UNIX Function Definitions

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <utl.h> // shared procs, SW

///
// the DBG* global vars are used in macros
// #ifdef DEBUG0
// #define DBG0 if (dbg0) flogf(args);
// DBG0( "is printed if both #define DEBUG0  and  c:> set DBG0=1   (!=999)" )
//
// DBG() blocks of code, as is
// DBG0() print func names
// DBG1() print more interesting things
// DBG2() print details

#define DEBUG
#define DEBUG0
#define DEBUG1
#define DEBUG2

#ifdef DEBUG
#define DBG(...) __VA_ARGS__
#else
#define DBG(...)
#endif

#ifdef DEBUG0
#define DBG0(...) if (com.dbg0) flogf("\n" __VA_ARGS__);
#else
#define DBG0(...)
#endif

#ifdef DEBUG1
#define DBG1(...) if (com.dbg1) flogf(" " __VA_ARGS__);
#else
#define DBG1(...)
#endif

#ifdef DEBUG2
#define DBG2(...) if (com.dbg2) flogf(" " __VA_ARGS__);
#else
#define DBG2(...)
#endif

// PINS
#define MDM_PWR 21
#define ANT_PWR 22  // antenna module Power pin (1=ON, 0=OFF)
#define COM1SELECT 23  // set = antMod, clear = sbe16
#define WISPR3 24
#define WISPR4 25
#define DFRPWR 26   // unused
#define WISPR1 29
#define WISPR2 30
#define COM1_TX 31
#define COM1_RX 32
#define MDM_RX 33
#define MDM_TX 35
#define WISPR_PWR_ON 37
#define WISPR_PWR_OFF 42
#define MDM_RX_TTL 48
#define MDM_TX_TTL 50

void comInit(void);
