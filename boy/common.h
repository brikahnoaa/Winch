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

#include <define.h> // common defines
#include <global.h> // common externals
#include <utils.h> // shared procs, SW
#include <mpc.h> // shared procs, HW ports and lines

#define PLATFORM LARA
#define PROG_VERSION 4.0

// the DEBUG's each turn on a macro named DBGx()
// - blocks of code
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
#ifdef DEBUG
#define DBG(X) X
#else
#define DBG(X)
#endif
// DEBUG0
#ifdef DEBUG0
#define DBG0(X) X
#else
#define DBG0(X)
#endif
// DEBUG1
#ifdef DEBUG1
#define DBG1(X) X
#else
#define DBG1(X)
#endif
// DEBUG2
#ifdef DEBUG2
#define DBG2(X) X
#else
#define DBG2(X)
#endif
