// com.h  common to all
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


#define ANTSW 1

// PINS
#define ADCPWR 19
#define A3LAPWR 21 // IRIDGPS PWR
#define COM4PWR 22 // COM4 Enable
#define SBEPWR 23 // SB#39plus TD power
#define PAMPWR 24     // PAM PWR on/off
#define COM4TX 25
#define COM4RX 26  // Tied to /IRQ5
#define PAMTX 27
#define PAMRX 28
#define PAMZEROBIT 29 // PAM selecton
#define PAMONEBIT 30  // PAM selection
#define SBETX 31
#define SBERX 32  // Tied to IRQ2
#define A3LARX 33  // IRIDGPS tied to /IRQ3
#define A3LATX 35

