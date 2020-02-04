#ifndef DS2404_H
#define DS2404_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: ds2404.c,v 1.11 2007/04/24 01:43:05 swift Exp $
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
 * This compilation unit provides an interface to the Dallas Semiconductor
 * DS2404 EconoRAM Time Chip which implements a real time clock and an
 * interval timer.   These functions were written using the data sheet
 * (document number 110501) which was downloaded from the web URL:
 * www.maxim-ic.com
 *
 * \begin{verbatim}
 * $Log: ds2404.c,v $
 * Revision 1.11  2007/04/24 01:43:05  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.10  2006/04/21 13:45:38  swift
 * Changed copyright attribute.
 *
 * Revision 1.9  2005/06/14 18:56:44  swift
 * Implement a watch-dog alarm to prevent power-down exceptions from killing a float.
 *
 * Revision 1.8  2004/12/29 23:04:36  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.7  2003/11/20 18:59:35  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.6  2003/07/19 22:39:32  swift
 * Minor changes related to definitions of various time periods (hour, min,
 * day, year).
 *
 * Revision 1.5  2003/07/03 22:51:10  swift
 * Added 'trace()' to several functions.
 *
 * Revision 1.4  2003/06/10 00:11:13  swift
 * Modified RTC RAM test.
 *
 * Revision 1.3  2003/06/07 20:31:19  swift
 * Modifications to use difftime() to compute time differences.
 *
 * Revision 1.2  2003/06/04 23:24:32  swift
 * Implemented RTC alarm functionality.
 *
 * Revision 1.1  2003/06/03 14:43:37  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define Ds2404RtcChangeLog "$RCSfile: ds2404.c,v $  $Revision: 1.11 $   $Date: 2007/04/24 01:43:05 $"

#include <time.h>

/* declare the masks for the alarm flags of the DS2404's status register */
extern const unsigned char Ds2404RtfMask;
extern const unsigned char Ds2404ItfMask;

/* function prototypes */
int Ds2404Init(void);
int Ds2404RamTest(void);
int Ds2404Reset(void);
unsigned char Ds2404StatusByte(void);
time_t ialarm(void);
int IntervalAlarmCheck(void);
int IntervalAlarmDisable(void);
int IntervalAlarmEnable(void);
int IntervalAlarmGet(time_t *sec,unsigned char *tics);
int IntervalAlarmSet(time_t sec,unsigned char tics);
int IntervalTimerGet(time_t *sec,unsigned char *tics);
int IntervalTimerSet(time_t sec,unsigned char tics);
time_t itimer(void);
int RtcAlarmCheck(void);
int RtcAlarmDisable(void);
int RtcAlarmEnable(void);
int RtcAlarmGet(time_t *sec,unsigned char *tics);
int RtcAlarmSet(time_t sec,unsigned char tics);
int RtcGet(time_t *sec,unsigned char *tics);
int RtcSet(time_t sec,unsigned char tics);
int SetAlarm(time_t sec);
time_t time(time_t *sec);

/* define some constants of nature */
#define Year (31536000L)
#define Day  (86400L)
#define Hour (3600L)
#define Min  (60L)

#endif /* DS2404_H */
