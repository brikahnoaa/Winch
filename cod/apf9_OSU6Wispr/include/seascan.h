#ifndef SEASCAN_H
#define SEASCAN_H (0x0008U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: seascan.c,v 1.1.2.1 2008/09/11 19:49:30 dbliudnikas Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/** RCS log of revisions to the C source code.
 *
 * \begin{verbatim}
 * $Log: seascan.c,v $
 * Revision 1.1.2.1  2008/09/11 19:49:30  dbliudnikas
 * Initial version of Seascan TD sensor interface.
 *
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define seascanChangeLog "$RCSfile: seascan.c,v $ $Revision: 1.1.2.1 $ $Date: 2008/09/11 19:49:30 $"

#include <serial.h>
#include <stdio.h>

/* function prototypes */
int    SeascanEnterCmdMode(char *SerNo, float *p);
int    SeascanExitCmdMode(void);
int    SeascanGetP(float *p);
int    SeascanGetPt(float *p, float *t);
int    SeascanSerialNumber(char *serStr);

/* define the return states of the Seascan API */
extern const char SeascanTooFew;          /* Too few samples */
extern const char SeascanChatFail;        /* Failed chat attempt. */
extern const char SeascanNoResponse;      /* No response received from Seascan. */
extern const char SeascanRegExceptn;      /* Response received, regexec() exception */
extern const char SeascanRegexFail;       /* response received, regex no-match */
extern const char SeascanNullArg;         /* Null function argument. */
extern const char SeascanFail;            /* General failure */
extern const char SeascanOk;              /* response received, regex match */
extern const char SeascanPedanticFail;    /* response received, pedantic regex no-match */
extern const char SeascanPedanticExceptn; /* response received, pedantic regex exception */

/* define the maximum length of the Seascan response */
#define MAXLEN 80

#define ETX  0x03 /* ETX character */
#define NUL  0x00 /* NULL character */
#define LF   0x0a /* LINE FEED character */

#define SERNOSIZE 5 /* size of serial number +1 for null terminate */
#define PTSIZE 7 /* size of temp/pressure + 1 for null terminate */

#endif /* SEASCAN_H */
