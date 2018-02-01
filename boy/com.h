// com.h  common to all
#include <cfxpico.h> // Persistor PicoDOS Definitions

#include <assert.h>
#include <cfxad.h>
#include <ctype.h>
#include <dirent.h>   // PicoDOS POSIX-like Directory Access Defines
#include <dosdrive.h> // PicoDOS DOS Drive and Directory Definitions
#include <errno.h>
#include <fcntl.h> // PicoDOS POSIX-like File Access Definitions
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stat.h> // PicoDOS POSIX-like File Status Definitions
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h> // PicoDOS POSIX-like Terminal I/O Definitions
#include <time.h>
#include <unistd.h> // PicoDOS POSIX-like UNIX Function Definitions

#include <utl.h> // shared procs, SW

#define DBG_LVL 1

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
#define DBG1(...) flogf(" " __VA_ARGS__);
#else
#define DBG1(...)
#endif

#ifdef DEBUG2
#define DBG2(...) flogf(" " __VA_ARGS__);
#else
#define DBG2(...)
#endif
