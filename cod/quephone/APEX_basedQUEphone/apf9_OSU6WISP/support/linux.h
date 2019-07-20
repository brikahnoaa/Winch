#ifndef LINUX_H
#define LINUX_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: linux.c,v 1.2 2003/07/19 22:40:08 swift Exp swift $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: linux.c,v $
 * Revision 1.2  2003/07/19 22:40:08  swift
 * Add definitions of conio.
 *
 * Revision 1.1  2003/07/03 22:45:25  swift
 * Initial revision
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <time.h>
#include <serial.h>

/* function prototypes */
time_t itimer(void);
int kbdflush(void);
int kbdhit(void);
int SetBaud(int);
int Com1Enable(int);
int Com1RtsAssert(void);
int Com1RtsClear(void);
int Com1Cts(void);
int Com1Disable(void);
int ConioEnable(void);
int CtdAssertModePin(void);
int CtdClearModePin(void);
int CtdAssertWakePin(void);
int CtdClearWakePin(void);
int CtdActiveIo(time_t timeout);
int CtdEnableIo(void);
int CtdDisableIo(void);
int CtdInitiateSample(char *buf, int size, time_t timeout);
int CtdAssertTxPin(void);
int CtdClearTxPin(void);
int CtdPSample(char *buf, int size);
int CtdPtSample(char *buf, int size);
int CtdPtsSample(char *buf, int size, time_t timeout);
int Cts(int com);
void ftrace(const char *function_name, const char *msg);
void _fassert(int line, const char *file, const char *expr);
void ModemDisable(void);
void ModemEnable(unsigned int BaudRate);
void Wait(unsigned int millisec);

/* external declarations for serial ports */
extern struct SerialPort ctdio; 
extern unsigned short int CtdPort;

/* external declarations for generic serial ports */
extern const struct SerialPort com[4]; 

extern time_t RefTime;

#endif /* LINUX_H */
