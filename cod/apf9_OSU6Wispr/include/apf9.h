#ifndef APF9_H
#define APF9_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: apf9.c,v 1.22 2007/04/24 01:43:05 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* Copyright University of Washington.   Written by Dana Swift.
 *
 * This software was developed at the University of Washington using funds
 * generously provided by the US Office of Naval Research, the National
 * Science Foundation, and NOAA.
 *  
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or (at
 * your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
/** RCS log of revisions to the C source code.
 *
 * \begin{verbatim}
 * $Log: apf9.c,v $
 * Revision 1.22  2007/04/24 01:43:05  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.21  2006/08/17 21:17:48  swift
 * MOdifications to allow better logging control.
 *
 * Revision 1.20  2006/04/21 13:45:38  swift
 * Changed copyright attribute.
 *
 * Revision 1.19  2005/06/28 18:58:54  swift
 * Validate MaxLogSize to control maximum size of logfile.
 *
 * Revision 1.18  2005/06/22 19:25:17  swift
 * Rework of the RTC watchdog alarm and reiniatialization of TXD0.
 *
 * Revision 1.17  2005/06/21 14:35:19  swift
 * Reduced manipulation of TXD0 and TXD1.
 *
 * Revision 1.16  2005/06/17 16:30:27  swift
 * Implement a watch-dog alarm to prevent power-down exceptions from killing a float.
 *
 * Revision 1.15  2005/06/14 18:56:44  swift
 * Implement a watch-dog alarm to prevent power-down exceptions from killing a float.
 *
 * Revision 1.14  2005/05/13 23:24:57  swift
 * Modifications to increase the baud rate of the 20ma serial loop from
 * 4800baud to 9600baud.
 *
 * Revision 1.13  2005/02/22 21:55:35  swift
 * Eliminated the SetAlarm() call in the initialization code...causes bugs.
 *
 * Revision 1.12  2005/01/15 00:51:29  swift
 * Added code in Apf9Init() to set the alarm for an hour after each start-up to
 * protect against the possibility that the APF9 may shutdown uncleanly.
 *
 * Revision 1.11  2005/01/06 00:49:40  swift
 * Added interrupt control to SetBaud().
 *
 * Revision 1.10  2004/12/29 23:02:17  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.9  2004/04/26 21:42:55  swift
 * Added function SetBaud() to set the buad rate of the XA serial ports.
 *
 * Revision 1.8  2004/04/15 23:17:31  swift
 * Changed initialization state of TXD1 to eliminate stray characters on start-up.
 *
 * Revision 1.7  2004/04/14 20:48:21  swift
 * Revision E of the APF9 made some hardware changes to earlier versions.  The
 * PSD935 was replaced by a PSD835 and the 20mA serial loop was changed to be a
 * real serial IO port.
 *
 * Revision 1.6  2004/04/14 16:35:48  swift
 * Fixed a syntax error in calling Com2Disable();
 *
 * Revision 1.5  2003/12/07 22:08:11  swift
 * Debugged and fixed the APF9's annoying habit of generating garbage
 * characters during power-down.  It turned out to be an artifact of an RC
 * circuit somewhere in the hardware.
 *
 * Revision 1.4  2003/11/20 18:59:34  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.3  2003/11/12 22:33:14  swift
 * Another round of refactorization and API enhancement.
 *
 * Revision 1.2  2003/07/19 22:54:42  swift
 * Modifications to reflect refactorization and remodeling of the
 * Psd835 ports.
 *
 * Revision 1.1  2003/07/06 00:05:56  swift
 * Initial revision
 * \end{verbatim}
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define apf9ChangeLog "$RCSfile: apf9.c,v $ $Revision: 1.22 $ $Date: 2007/04/24 01:43:05 $"

#include <conio.h>
#include <xa.h>

/* XA-G3 I/O port definitions */
static unsigned char bd1          @ 0x20;
static bit BD1_LSB                @ 0x100;
static bit BD1_MSB                @ 0x107;
static bit RTC_DQ                 @ 0x38E; // p1.6
static bit EE_SDA                 @ 0x38F; // p1.7
static bit ADC_DOUT               @ 0x39c; // p3.4

/* macro to restart the watchdog timer */
#define WatchDog() {EA=0; WFEED1=0xa5; WFEED2=0x5a; EA=1;}

/* lowest address for far RAM */
extern const unsigned long Apf9FarRamLo;

/* highest address of far RAM */
extern const unsigned long Apf9FarRamHi;

/* starting address of the program stack */
extern unsigned int Apf9StackBase;

/* maximum size of the program stack */
extern const int Apf9StackSize;

/* define reserve required to prevent activation of stack-overflow assertions */
extern const int Apf9StackReserve;

/* macro to perform a stack-check computation */
#define StackOk() (Apf9StackSize-Apf9StackLen()>Apf9StackReserve)

extern bit timer0;
extern persistent far long dzero;

/* function prototypes */
int           AirPumpRun(time_t seconds, unsigned char *VoltCnt, unsigned char *AmpCnt);
int           AirValveOpen(void);
int           AirValveClose(void);
int           Apf9Init(void);
void          Apf9PowerOff(time_t AlarmSec);
int           Apf9StackLen(void);
unsigned int  Apf9StackPeek(unsigned int *StackAddr);
int           Apf9Tp1(unsigned int millisec,unsigned int Hz);
time_t        Apf9WakeTime(void);
unsigned char Apf9WakeUpByRtc(void);
int           Ds2404Read(unsigned int addr,unsigned int bytes,void *buf);
int           Ds2404Write(unsigned int addr,unsigned int bytes,const void *buf);
int           HDrive(int dir);
int           MagSwitchReset(void);
int           MagSwitchToggled(void);
int           SeimacXmit(const unsigned char *msg,unsigned int N);
void          SetBaud(unsigned int baud);                 
void          Wait(unsigned int millisec);
void          WaitTicks(unsigned char tics);

#endif /* APF9_H */
