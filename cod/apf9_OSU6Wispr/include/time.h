#ifndef TIME_H
#define TIME_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: time.c,v 1.15 2007/04/24 01:43:29 swift Exp $
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
/**
   This translation unit is a drop-in replacement for the time-related
   services offered in the ANSI/ISO Standard C Library.  It is compliant
   with the ANSI/ISO standard except that multibyte characters are not
   supported and the "C" locale is the only locale implemented.  It has been
   tested against the output of the GNU Standard C Library over the full
   range of inputs for time.

   Many details about how these functions ought to be implemented were
   gleaned from the book, "The Standard C Library" by P.J. Plauger published
   by Prentice Hall (ISBN# 0-13-131509-9).

   written by Dana Swift
 *
 * RCS log of revisions to the C source code.
 *
 * \begin{verbatim}
 * $Log: time.c,v $
 * Revision 1.15  2007/04/24 01:43:29  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.14  2006/04/21 13:45:40  swift
 * Changed copyright attribute.
 *
 * Revision 1.13  2004/02/05 23:45:20  swift
 * Changed declarations of unsigned long constants from 'LU' to 'UL'.
 *
 * Revision 1.12  2003/11/20 18:59:42  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.11  2003/07/19 22:44:51  swift
 * Removed some debugging code that had been inadvertently left in asctime().
 *
 * Revision 1.10  2003/06/17 19:31:53  swift
 * Minor modifications to quiet compiler warnings.
 *
 * Revision 1.9  2003/06/17 17:19:58  swift
 * Added a normalization assertion to mktime().
 *
 * Revision 1.8  2003/06/17 17:03:55  swift
 * Complete rewrite of mktime() to detect integer overflows without resorting
 * to floating point arithmetic.  This implementation is much simpler and more
 * robust than Plauger's.
 *
 * Revision 1.7  2003/06/17 00:37:50  swift
 * Changed implementation of mktime() to avoid the use of floating point math
 * to detect overflow if intrinsically integer quantities.
 *
 * Revision 1.6  2003/06/13 22:49:42  swift
 * Fixes to mktime() to allow for time_t to be either signed or unsigned.
 *
 * Revision 1.5  2003/06/11 18:41:28  swift
 * Modifications required when type time_t was switch from
 * unsigned to signed long integers.
 *
 * Revision 1.4  2003/06/07 20:49:40  swift
 * Modifications to avoid typedef redefinition errors.
 *
 * Revision 1.3  2003/05/30 15:34:29  swift
 * Added documentation.
 *
 * Revision 1.2  2003/05/10 17:53:36  swift
 * Added typedef for size_t
 *
 * Revision 1.1  2003/05/10 16:21:08  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define TimeChangeLog "$RCSfile: time.c,v $  $Revision: 1.15 $   $Date: 2007/04/24 01:43:29 $"

#define CLOCKS_PER_SEC 1
#define NULL ((void *)0)

/* typedefs for ANSI types */
typedef unsigned int clock_t;
typedef long int time_t;

#ifndef _STDDEF
#define _STDDEF
   typedef unsigned size_t;
#endif  /* _STDDEF */

/*========================================================================*/
/* definition of structure 'tm' to represent broken-down time             */
/*========================================================================*/
/**
   This structure defines the broken-down representation of time that is
   used by the ANSI C standard library.  The members of the tm structure
   are listed below.

     \begin{verbatim}
     tm_sec.....The number of seconds after the minute, normally in the
                range 0 to 59, but can be up to 61 to allow for leap seconds.
     
     tm_min.....The number of minutes after the hour, in the range 0 to 59.
     
     tm_hour....The number of hours past midnight, in the range 0 to 23.
     
     tm_mday....The day of the month, in the range 1 to 31.
     
     tm_mon.....The number of months since January, in the range 0 to 11.
     
     tm_year....The number of years since 1900.
     
     tm_wday....The number of days since Sunday, in the range 0 to 6.
     
     tm_yday....The number of days since January 1, in the range 0 to 365.
     
     tm_isdst...A flag that indicates whether daylight saving time is in
                effect at the time described.  The value is positive if
                daylight saving time is in effect and zero if it is not.
     \end{verbatim}
*/
struct tm
{
      int tm_sec;   /* seconds after the minute [0-59] */
      int tm_min;   /* minutes after the hour [0-59] */
      int tm_hour;  /* hours past midnight [0-23] */
      int tm_mday;  /* day of the month [1-31] */
      int tm_mon;   /* month [0-11] */
      int tm_year;  /* years since 1900 */
      int tm_wday;  /* days since Sunday [0-6] */
      int tm_yday;  /* days since Jan 1 [0-365] */
      int tm_isdst; /* daylight saving time [0=non-DST, 1=DST] */
};

/* function prototypes */
char *asctime(const struct tm *t);
clock_t clock(void);
char *ctime(const time_t *sec);
double difftime(time_t Tf, time_t Ti);
struct tm *gmtime(const time_t *sec);
struct tm *localtime(const time_t *seconds);
time_t mktime(struct tm *t);
size_t strftime(char *buf, size_t bufsize, const char *format, const struct tm *t);
time_t time(time_t *sec);

#endif /* TIME_H */
