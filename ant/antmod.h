// ant.h

//  July 12, 2017 blk, April 2018 blk

#include <cfxbios.h> // Persistor BIOS and I/O Definitions
#include <cfxpico.h> // Persistor PicoDOS Definitions

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

#include <dirent.h>   // PicoDOS POSIX-like Directory Access Defines
#include <dosdrive.h> // PicoDOS DOS Drive and Directory Definitions
#include <fcntl.h>    // PicoDOS POSIX-like File Access Definitions
#include <stat.h>     // PicoDOS POSIX-like File Status Definitions
#include <termios.h>  // PicoDOS POSIX-like Terminal I/O Definitions
#include <unistd.h>   // PicoDOS POSIX-like UNIX Function Definitions

// #define DEBUG
#ifdef DEBUG
#define DBG(X) X // template:   DBG( cprintf("\n"); )
#else               /*  */
#define DBG(X)      // nothing
#endif              /*  */
// #define DEBUG1
#ifdef DEBUG1
#define DBG1(X) X // template:   DBG0( cprintf("\n"); )
#pragma message("!!! "__FILE__                                                 \
                ": Don't ship with DEBUG1 compile flag set!")
#else               /*  */
#define DBG1(X)      // nothing
#endif              /*  */

// TUPort* OpenIridPt(bool on);
TUPort* OpenSbePt(void);
TUPort* OpenBuoyPt(void);
short power(short c, bool on);
short char2id(short ch);
void init();
void help();
void status();
void antennaSwitch(char c);
void transBlock(long b);
void printchar(char c);
void prerun();
void antBreak();
void buoyStr(char *out);
void buoyCout(char ch);
char buoyCin(void);
int buoyQ(void);
