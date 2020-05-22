// main.h
#ifndef H_MAIN
#define H_MAIN

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

#include <utl.h>

#include <ant.h>
#include <boy.h>
#include <cfg.h>
#include <s16.h>
#include <s39.h>
#include <dbg.h>
#include <dos.h>
#include <eng.h>
#include <iri.h>
#include <mpc.h>
#include <ngk.h>
#include <pwr.h>
#include <sys.h>
#include <tmr.h>
#include <wsp.h>

#endif
