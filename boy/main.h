// main.h
#ifndef H_MAIN
#define H_MAIN

#define STRSZ 128
#define BUFSZ 4096
#define C_DRV ('C'-'A')
#define null NULL
#define SETTLE 200
// allow up to .05 second between chars, normally chars take .001-.016
#define CHAR_DELAY 50

// PINS
#define MDM_PWR 21
#define ANT_PWR 22  // antenna module Power pin (1=ON, 0=OFF)
#define ANT_SEL 23  // set = antMod cf2, clear = a3la
#define PAM_34 24   // 24 cannot be on with 29
#define PAM_4 25    // 24&25 -> pam4 (sbe16)
#define HPSENS 26   // power for pressure sensor
#define PAM_TX 27   // com2,3: (pam_12)(pam_34) 
#define PAM_RX 28   // selected by 24, 29
#define PAM_12 29   // 24 cannot be on with 29
#define PAM_2 30    // 29&30 -> wsp2
#define ANT_TX 31   // com1: ant mod, uMPC and A3LA
#define ANT_RX 32   // selected by 23
#define MDM_RX 33   // acoustic modem
#define MDM_TX 35   // 
#define WISPR_PWR_ON 37
#define WISPR_PWR_OFF 42
#define MDM_RX_TTL 48
#define MDM_TX_TTL 50

#include <cfxpico.h> // Persistor PicoDOS Definitions
#include <cfxbios.h>
#include <cfxad.h>
#include <dirent.h>   // PicoDOS POSIX-like Directory Access Defines
#include <dosdrive.h> // PicoDOS DOS Drive and Directory Definitions
#include <fcntl.h> // PicoDOS POSIX-like File Access Definitions
#include <stat.h> // PicoDOS POSIX-like File Status Definitions
#include <termios.h> // PicoDOS POSIX-like Terminal I/O Definitions
#include <unistd.h> // PicoDOS POSIX-like UNIX Function Definitions
// std c
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
// ADEx defines DBG(), naughty!
#include <ADExamples.h>
// DBG set by ADExamples.h
#undef DBG

typedef TUPort * Serial;

#include <ant.h>
#include <boy.h>
#include <cfg.h>
#include <ctd.h>
#include <dbg.h>
#include <dos.h>
#include <eng.h>
#include <gps.h>
#include <hps.h>
#include <mpc.h>
#include <ngk.h>
#include <pwr.h>
#include <sys.h>
#include <tmr.h>
#include <utl.h>
#include <wsp.h>

#endif
