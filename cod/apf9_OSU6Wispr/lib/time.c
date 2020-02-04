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

#include <assert.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/* prototypes for local functions */
static int DaysTo(int year, int mon);
size_t Strftime(char *buf, size_t bufsize, const char *format, const struct tm *t);
static size_t StrftimeSpec(char spec, char *buf, size_t bufsize, const struct tm *t);
struct tm *Tm(time_t sec, int isdst, struct tm *t);

/* define the number of seconds between 1900 and the UNIX EPOCH */
static const unsigned long EPOCH = ((70*365UL + 17)*86400UL); 

/* define the timezone offset in seconds */
static time_t TzOffset = 0;

/* define some conversion factors */
static const time_t DaysPerYear   = 365L;
static const time_t SecsPerYear   = 31536000L;
static const time_t SecsPerDay    = 86400L;
static const time_t SecsPerHour   = 3600L;
static const time_t SecsPerMin    = 60L;

/* define the number of days to the beginning of each month of a leap-year */
static const short lmos[]={0,31,60,91,121,152,182,213,244,274,305,335};

/* define the number of days to the beginning of each month of a non leap-year */
static const short  mos[]={0,31,59,90,120,151,181,212,243,273,304,334};

/* macro to determine if a year is a leap year and return the correct table */
#define MONTAB(year) ((((year)&0x03) || (year)==0) ? mos : lmos)

/* definition used to get the weekday right */
#define WDAY 1

/*------------------------------------------------------------------------*/
/* function to convert broken-down time to a string                       */
/*------------------------------------------------------------------------*/
/**
   This function converts the broken-down time into a string with the
   format: "Day Mon dd hh:mm:ss yyyy\n".  The return value points to a
   static buffer that is overwritten by subsequent calls to this and other
   time functions.

      \begin{verbatim}
      input:
      
         t ... This is the broken-down time object that is converted into a
               printable string.  No checks are made to ensure that the
               members of the tm structure are internally consistent.  
               
      output:
      
         This function returns a 26-character printable string that contains
         the date and time as in this example: "Fri May  9 16:43:05 2003\n".  
         Note the linefeed at the end of the string.
      \end{verbatm}
*/
char *asctime(const struct tm *t)
{
   /* create a static buffer in which to write the date/time string */
   static char buf[]="Day Mon dd hh:mm:ss yyyy\n";
          
   /* write the date/time string */
   Strftime(buf,sizeof(buf),"%c\n",t);

   return buf;
}

/*------------------------------------------------------------------------*/
/* simple version of the ANSI clock function                              */
/*------------------------------------------------------------------------*/
/**
   This function returns an approximation of processor time used by the
   program.  The C standard allows for arbitrary values at the start of the
   program; take the difference between the value returned from a call to
   clock() at the start of the program to get the actual elapsed time.
*/
clock_t clock(void)
{
   return ((clock_t)time(NULL));
}

/*------------------------------------------------------------------------*/
/* function to convert calendar time to a string                          */
/*------------------------------------------------------------------------*/
/**
   This function converts the calendar time into a string with the format:
   "Day Mon dd hh:mm:ss yyyy\n".  The return value points to a static buffer
   that is overwritten by subsequent calls to this and other time functions.

      \begin{verbatim}
      input:
      
         sec ... This is the calendar time that is converted into a
                 printable string.
               
      output:
      
         This function returns a 26-character printable string that contains
         the date and time as in this example: "Fri May  9 16:43:05 2003\n".  
         Note the linefeed at the end of the string.
      \end{verbatm}
*/
char *ctime(const time_t *sec)
{
   /* convert calendar time to broken-down time and then call asctime() */
   return asctime(localtime(sec));
}

/*------------------------------------------------------------------------*/
/* function to compute the number of leap-days + year-days between dates  */
/*------------------------------------------------------------------------*/
/**
   This function computes the number of leap-days plus year-days between
   January 1, 1900 and a specified month & year.  This is an internal
   (static) function that is called by several other functions but is
   unlikely to be of direct use to users.

      \begin{verbatim}
      input:
      
         mon,year....These parameters represent the month and year of one
                     endpoint of the timespan of interest.  The other
                     endpoint is fixed at Jan 1, 1900.

      output:
      
         This function returns the number of leap-days plus the number of
         year-days between the endpoints of the timespan of interest.  The
         number of year-days is equal to the number of days between Jan 1
         and the first day of the month of 'year'.
         
      \end{verbatim}
*/
static int DaysTo(int year, int mon)
{
   int days=0;

   /* validate the month */
   assert(mon>=0 && mon<12);
   
   if (mon>=0 && mon<12) 
   {
      if (year>0) days = (year-1)/4;
      else if (year <= -4) days = 1 + (4-year)/4;
      
      days += MONTAB(year)[mon];
   }
      
   return days;
}

/*------------------------------------------------------------------------*/
/* function to compute the difference between two times                   */
/*------------------------------------------------------------------------*/
/**
   This function is the only safe way to compute the difference between two
   times Tf and Ti.  The result is measured in seconds and is positive if Tf
   is greater than Ti.  This function implements the behavior of the ANSI C
   standard library function by the same name. 

      \begin{verbatim}
      input:
         Tf ... The final time (seconds).
         Ti ... The initial time (seconds).
         
      output:
         This function returns the time difference between times Tf and Ti.
         The result is measured in seconds and is positive if Tf is greater
         than Ti.
      \end{verbatm}
*/
double difftime(time_t Tf, time_t Ti)
{
   double dT;
   
   /* offset each time to reference it to Jan 1, 1900 */
   Tf-=EPOCH; Ti-=EPOCH;
   
   /* compute the difference in times with special attention to signedness */
   dT = (Ti<=Tf) ? (double)(Tf-Ti) : -(double)(Ti-Tf);

   return dT;
}

/*------------------------------------------------------------------------*/
/* function to convert calendar time to broken-down UTC time              */
/*------------------------------------------------------------------------*/
/**
   This function converts the calendar time (seconds) to broken-down time
   representation, expressed in Coordinated Universal Time (UTC).

      \begin{verbatim}
      input:
      
         seconds...When this pointer is dereferenced, the value represents
                   the number of seconds elapsed since 00:00:00 on Jan 1,
                   1970, Coordinated Universal Time (UTC).
         
      output:

         This function returns the broken-down time in a communal static
         data object.
         
      \end{verbatim}
*/
struct tm *gmtime(const time_t *seconds)
{
   /* define the calendar time to use if the function argument is NULL */
   const time_t Jan1970=0;

   /* make sure the pointer is valid */
   if (!seconds) seconds=&Jan1970;

   /* compute the broken-down time */
   return Tm(*seconds,0,NULL);
}

/*------------------------------------------------------------------------*/
/* function to convert calendar time to broken-down local time            */
/*------------------------------------------------------------------------*/
/**
   This function converts the calendar time (seconds) to broken-down local
   time representation, expressed in Coordinated Universal Time (UTC).

      \begin{verbatim}
      input:
      
         seconds...When this pointer is dereferenced, the value represents
                   the number of seconds elapsed since 00:00:00 on Jan 1,
                   1970, local time.
         
      output:

         This function returns the broken-down time in a communal static
         data object.
         
      \end{verbatim}
*/
struct tm *localtime(const time_t *seconds)
{
   /* define the calendar time to use if the function argument is NULL */
   const time_t Jan1970=0;

   /* make sure the pointer is valid */
   if (!seconds) {seconds=&Jan1970;}
           
   /* compute the broken-down time */
   return Tm((*seconds)+TzOffset,0,NULL);
}

/*------------------------------------------------------------------------*/
/* function to compute the calendar time from the broken-down time        */
/*------------------------------------------------------------------------*/
/**
   This function converts a broken-down time structure, expressed as local
   time, to calendar time representation.  The function ignores the
   specified contents of the structure members tm_wday and tm_yday and
   recomputes them from the other information in the broken-down time
   structure.  If structure members are outside their legal interval, they
   will be normalized (eg., 40 October is changed into 9 November).

      \begin{verbatim}
      input:

         t .... This object contains the broken-down time.  Its elements are
                allowed to be non-normalized as a convenience mechanism for
                adding or subtracting arbitrary intervals of time.  For
                example, it is acceptable to add 60 days to a date simply by
                doing this: t->tm_mday+=60 and then calling this function in
                order to renormalize the broken-down time object.
         
      output:

         t .... This object contains the normalized broken-down time.

         This function returns the calendar time expressed as the number of
         seconds since 00:00:00 GMT on Jan 1, 1970.
      \end{verbatm}
*/
time_t mktime(struct tm *t)
{
   time_t secs;

   /* initialize a flag to record overflow of time_t object (secs) */
   int overflow=0;

   /* determine the extremes of type time_t based on its signedness */
   const time_t MAX = ((time_t)(-1)<0) ? LONG_MAX : ULONG_MAX;
   const time_t MIN = ((time_t)(-1)<0) ? LONG_MIN : 0;

   /* initialize the long representation of the broken-down time */
   long int sec=t->tm_sec,  min=t->tm_min, hour=t->tm_hour;
   long int day=t->tm_mday, mon=t->tm_mon, year=t->tm_year;

   /* initialize the time reference */
   unsigned long dt=EPOCH;
   
   /* make sure that time_t and unsigned long int each has 4 bytes */
   assert(sizeof(time_t)==4 && sizeof(unsigned long int)==4);

   /* normalize the month of the year to be in the range [0,11] */
   if (mon<0) {dt=(-mon+11)/12; year-=dt; mon+=dt*12;} else {year+=mon/12; mon%=12;}

   /* compute the number of leapdays and year-days to beginning of month */
   day += (DaysTo(year,mon)-1);

   /* compute the broken-down time relative to EPOCH */
   year -= dt/SecsPerYear; dt%=SecsPerYear;
   day  -= dt/SecsPerDay;  dt%=SecsPerDay;
   hour -= dt/SecsPerHour; dt%=SecsPerHour;
   min  -= dt/SecsPerMin;  dt%=SecsPerMin;
   sec  -= dt;

   /* normalize the days, hours, minutes, and seconds of the broken-down time */
   if (sec<0)  {dt=(-sec+59)/60;   min-=dt;   sec+=dt*60;}  else {min+=sec/60;   sec%=60;}
   if (min<0)  {dt=(-min+59)/60;   hour-=dt;  min+=dt*60;}  else {hour+=min/60;  min%=60;}
   if (hour<0) {dt=(-hour+23)/24;  day-=dt;   hour+=dt*24;} else {day+=hour/24;  hour%=24;}
   if (day<0)  {dt=(-day+364)/365; year-=dt;  day+=dt*365;} else {year+=day/365; day%=365;}

   /* overflow detection depends strongly on these assertions */
   assert(sec>=0  && sec<60);
   assert(min>=0  && min<60);
   assert(hour>=0 && hour<24);
   assert(day>=0  && day<365);
   assert(mon>=0  && mon<12);

   /* add in the months, days, hours, minutes, and seconds (this can't overflow) */
   secs = day*SecsPerDay + hour*SecsPerHour + min*SecsPerMin + sec;

   /* normalization implies that 'secs' must be non-negative and less than one year */
   assert(secs>=0 && secs<SecsPerYear); 
   
   /* computation and overflow detection for case where 'year' is positive */
   if (year>0)
   {
      /* overflow detection criteria when 'year' is positive */
      if (year>(ULONG_MAX/(unsigned long)SecsPerYear) ||
          (dt=(unsigned long)year*SecsPerYear)>MAX-secs) overflow=1;

      /* add in the years */
      else secs+=dt;
   }
   
   /* computation and overflow detection for case where 'year' is negative */
   else if (year<0)
   {
      /* overflow detection criteria when 'year' is negative */
      if ((-year)>(ULONG_MAX/(unsigned long)SecsPerYear) ||
          (dt=(unsigned long)(-year)*SecsPerYear)>secs-MIN) overflow=1;

      /* add in the years */
      else secs-=dt;
   }

   /* account for timezone and DST */
   sec = -TzOffset - ((t->tm_isdst) ? SecsPerHour : 0);

   /* computation and overflow detection for case where 'sec' is positive */
   if (sec>0)
   {
      /* overflow detection criteria when 'sec' is positive */
      if (secs>0 && sec>(MAX-secs)) overflow=1; 
   }
   
   /* computation and overflow detection for case where 'sec' is negative */
   else if (sec<0)
   {
      /* overflow detection criteria when 'sec' is negative */
      if ((-sec)>(secs-MIN)) overflow=1;
   }
   
   /* determine if the broken-down time was representable */
   if (!overflow) 
   {
      /* compute the canonical broken-down time */
      Tm(secs,t->tm_isdst,t);

      /* add in the timezone and DST */
      secs += sec;
   }

   /* indicate that the broken-down time is not representable as a time_t object */
   else secs=(time_t)(-1);
   
   return secs;
}

/*------------------------------------------------------------------------*/
/* function to create a printable representation of date and time         */
/*------------------------------------------------------------------------*/
/**
   This function formats the broken-down time tm according to the format
   specification format and places the result in the character array
   'buf'.  

       \begin{verbatim}
       input:
       
          format....This is the formatting string that contains conversion
                    specifiers that determine what printable representation
                    is produced.

          bufsize...This is the maximum size of 'buf' including the NULL
                    terminator.  This function protects against buffer
                    overflow.  If a conversion would cause a buffer overflow
                    then the conversion will be truncated as necessary in
                    order to prevent buffer overflow.

          t.........This is the broken-down time structure that contains the
                    date/time information.  This function does nothing to
                    ensure that the members of this structure are internally
                    consistent.
          
       output:

          buf.......This buffer contains the printable representation of the
                    date/time according to the conversion specifiers in the
                    format string.

          This function returns the number of bytes written into the buffer.
       \end{verbatim}
*/
size_t strftime(char *buf, size_t bufsize, const char *format, const struct tm *t)
{
   return Strftime(buf,bufsize,format,t);
}

/*------------------------------------------------------------------------*/
/* function to create a printable representation of date and time         */
/*------------------------------------------------------------------------*/
/**
    This function formats the broken-down time tm according to the format
    specification format and places the result in the character array
    'buf'.

       \begin{verbatim}
       input:
       
          format....This is the formatting string that contains conversion
                    specifiers that determine what printable representation
                    is produced.

          bufsize...This is the maximum size of 'buf' including the NULL
                    terminator.  This function protects against buffer
                    overflow.  If a conversion would cause a buffer overflow
                    then the conversion will be truncated as necessary in
                    order to prevent buffer overflow.

          t.........This is the broken-down time structure that contains the
                    date/time information.  This function does nothing to
                    ensure that the members of this structure are internally
                    consistent.
          
       output:

          buf.......This buffer contains the printable representation of the
                    date/time according to the conversion specifiers in the
                    format string.

          This function returns the number of bytes written into the buffer.
       \end{verbatim}
*/
size_t Strftime(char *buf, size_t bufsize, const char *format, const struct tm *t)
{
   size_t nchar=0;

   /* initialize the buffer */
   if (buf) *buf=0;
   
   /* validate the function parameters */
   if (buf && bufsize>1 && format && t)
   {
      const char *s; char *b;

      for (b=buf, s=format; ;s++)
      {
         /* check for end-of-format or end-of-buf */
         if (!(*s) || nchar>=(bufsize-1)) {break;}

         /* check for conversion specifier: '%' */
         if ((*s) == '%')
         {
            /* generate the conversion using the specifier */
            size_t n = StrftimeSpec((*(++s)),b,bufsize-nchar,t);

            /* increment the pointer and byte counter */
            nchar+=n; b+=n;
         }
      
         /* copy literal text to the buffer */
         else {(*b)=(*s); b++; nchar++;} 
      }

      /* terminate the string in the buffer */
      (*b)=0;
   }
   
   return nchar;
}

/*------------------------------------------------------------------------*/
/* function to impliment conversion specifiers for Strftime()             */
/*------------------------------------------------------------------------*/
/**
   This function implements the task of transforming conversion specifiers
   for the Strftime() function into printable strings.  Ordinary characters
   placed in the format string are copied to 'buf' without conversion.
   Conversion specifiers are introduced by a `%' character, and are replaced
   in s as follows:

       \begin{verbatim}
       %a The abbreviated weekday name according to the current locale.
          
       %A The full weekday name according to the current locale.
          
       %b The abbreviated month name according to the current locale.
          
       %B The full month name according to the current locale.
          
       %c The  preferred date and time representation for the current locale.
          
       %d The day of the month as a decimal number (range 01 to 31).

       %D Equivalent to %m/%d/%y. (Americans should note that in other
          countries %d/%m/%y is rather common. This means that in
          international context this format is ambiguous and should not be
          used.)

       %e Like  %d,  the  day of the month as a decimal number, but a leading
          zero is replaced by a space. 
          
       %H The  hour  as  a  decimal number using a 24-hour clock (range 00 to
          23).
          
       %I The hour as a decimal number using a 12-hour  clock  (range  01  to
          12).
          
       %j The day of the year as a decimal number (range 001 to 366).
          
       %k The hour (24-hour clock) as a decimal number (range 0 to 23);
          single digits are preceded by a blank. (See also %H.)
          
       %l The hour (12-hour clock) as a decimal number (range 1 to 12);  single
          digits are preceded by a blank. (See also %I.)
          
       %m The month as a decimal number (range 01 to 12).
          
       %M The minute as a decimal number (range 00 to 59).
          
       %p Either  `AM' or `PM' according to the given time value, or the cor
          responding strings for the current locale.  Noon is treated as `pm'
          and midnight as `am'.
          
       %P Like  %p  but  in lowercase: `am' or `pm'.
          
       %S The second as a decimal number (range 00 to 61).
          
       %T The time in 24-hour notation (%H:%M:%S). 
          
       %w The  day  of  the  week as a decimal, range 0 to 6, Sunday being 0.
          See also %u.
          
       %x The preferred date representation for the  current  locale  without
          the time.
          
       %X The  preferred  time  representation for the current locale without
          the date.
          
       %y The year as a decimal number without a century (range 00 to 99).
          
       %Y The year as a decimal number including the century.
          
       %Z For compatibility only.  Time zones aren't implemented.

       %% A literal `%' character.
       \end{verbatim}

       \begin{verbatim}
       input:
       
          spec......This is the 1-byte converter.  For example, if the
                    conversion specifier is "%a" then the 1-byte convertor
                    is 'a'.

          bufsize...This is the maximum size of 'buf' including the NULL
                    terminator.  This function protects against buffer
                    overflow.  If a conversion would cause a buffer overflow
                    then the conversion will be truncated as necessary in
                    order to prevent buffer overflow.

          t.........This is the broken-down time structure that contains the
                    date/time information.  This function does nothing to
                    ensure that the members of this structure are internally
                    consistent.
          
       output:

          buf.......This buffer contains the printable representation of the
                    date/time according to the conversion specifier.

          This function returns the number of bytes written into the buffer.
       \end{verbatim}
*/
static size_t StrftimeSpec(char spec,char *buf,size_t bufsize, const struct tm *t)
{
   #define MAXBUF 25
   char scr[MAXBUF];
   int nchar=0;

   /* define abbreviated weekday names */
   static const char *a[]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

   /* define unabbreviated weekday names */
   static const char *A[]={"Sunday","Monday","Tuesday","Wednesday",
                     "Thursday","Friday","Saturday"};

   /* define abbreviated month name */
   static const char *b[]={"Jan","Feb","Mar","Apr","May","Jun",
                           "Jul","Aug","Sep","Oct","Nov","Dec"};

   /* define unabbreviated month name */
   static const char *B[]={"January","February","March","April","May","June","July",
                           "August","September","October","November","December"};

   /* initialize the buffer */
   if (buf) (*buf)=0;

   /* validate the function parameters */
   if (buf && bufsize>1 && t)
   {
      switch (spec)
      {
         /* convert the specifier for abbreviated weekday name */
         case 'a': {nchar=sprintf(scr,"%s",a[t->tm_wday%7]); break;}

         /* convert the specifier for abbreviated weekday name */
         case 'A': {nchar=sprintf(scr,"%s",A[t->tm_wday%7]); break;}
            
         /* convert the specifier for abbreviated month name */
         case 'b': {nchar=sprintf(scr,"%s",b[t->tm_mon%12]); break;}

         /* convert the specifier for abbreviated month name */
         case 'B': {nchar=sprintf(scr,"%s",B[t->tm_mon%12]); break;}

         /* convert the specifier for date and time */
         case 'c':
         {
            nchar=sprintf(scr,"%s %s %2d %02d:%02d:%02d %4d",a[t->tm_wday%7],
                          b[t->tm_mon%12],t->tm_mday%32,t->tm_hour%24,
                          t->tm_min%60,t->tm_sec%60,1900+(t->tm_year%8100));
            break;
         }

         /* convert the specifier for the day of the month */
         case 'd': {nchar=sprintf(scr,"%02d",t->tm_mday%32); break;}

         /* convert the specifier for date in mm/dd/yy format */
         case 'D':
         {
            nchar=sprintf(scr,"%02d/%02d/%02d",1+(t->tm_mon%12),
                          t->tm_mday%32,t->tm_year%100);
            break;
         }

         /* convert the specifier for the day of the month */
         case 'e': {nchar=sprintf(scr,"%2d",t->tm_mday%32); break;}

         /* convert the specifier for the hour of the day (24-hour clock) */
         case 'H': {nchar=sprintf(scr,"%02d",t->tm_hour%24); break;}

         /* convert the specifier for the hour of the day (12-hour clock) */
         case 'I': {nchar=sprintf(scr,"%02d",((t->tm_hour%12)?(t->tm_hour%12):12)); break;}

         /* convert the specifier for the day of the year */
         case 'j': {nchar=sprintf(scr,"%03d",(t->tm_yday%367)+1); break;}

         /* convert the specifier for the hour of the day (24-hour clock) */
         case 'k': {nchar=sprintf(scr,"%2d",t->tm_hour%24); break;}

         /* convert the specifier for the hour of the day (12-hour clock) */
         case 'l': {nchar=sprintf(scr,"%2d",((t->tm_hour%12)?(t->tm_hour%12):12)); break;}

         /* convert the specifier for the month of the year */
         case 'm': {nchar=sprintf(scr,"%02d",(t->tm_mon%12)+1); break;}

         /* convert the specifier for the minute */
         case 'M': {nchar=sprintf(scr,"%02d",t->tm_min%60); break;}

         /* convert the specifier for AM or PM */
         case 'p': {nchar=sprintf(scr,"%s",((t->tm_hour%24)/12)?"PM":"AM"); break;}

         /* convert the specifier for AM or PM */
         case 'P': {nchar=sprintf(scr,"%s",((t->tm_hour%24)/12)?"pm":"am"); break;}
        
         /* convert the specifier for the second */
         case 'S': {nchar=sprintf(scr,"%02d",t->tm_sec%60); break;}
        
         /* convert the specifier for the time in hh:mm:ss format */
         case 'T':
         {
            nchar=sprintf(scr,"%02d:%02d:%02d",t->tm_hour%24,
                          t->tm_min%60,t->tm_sec%60);
            
            break;
         }
       
         /* convert the specifier for the week-number of the year */
         case 'w': {nchar=sprintf(scr,"%d",t->tm_wday%7); break;}

         /* convert the specifier for date */
         case 'x':
         {
            nchar=sprintf(scr,"%02d/%02d/%02d",(t->tm_mon%12)+1,
                          t->tm_mday%32,t->tm_year%100);
            break;
         }
         
         /* convert the specifier for time */
         case 'X':
         {
            nchar=sprintf(scr,"%02d:%02d:%02d",t->tm_hour%24,t->tm_min%60,t->tm_sec%60);
            break;
         }
       
         /* convert the specifier for the 2-digit year */
         case 'y': {nchar=sprintf(scr,"%02d",t->tm_year%100); break;}
       
         /* convert the specifier for the 4-digit year */
         case 'Y': {nchar=sprintf(scr,"%04d",1900+(t->tm_year%8100)); break;}

         /* convert the specifier for the time zone */
         case 'Z': {nchar=0; break;}

         /* convert the specifier for the percent sign */
         case '%': {nchar=sprintf(scr,"%%"); break;}
            
         /* check for a premature end of the format specification */
         case 0:  {nchar=sprintf(scr,"<<%%>>"); break;}

         /* convert an undefined specifier */
         default:
         {
            if (isspace(spec)) nchar=sprintf(scr,"<<%%>> ");
            else if (isgraph(spec)) nchar=sprintf(scr,"<<%%%c>>",spec);
            else nchar=sprintf(scr,"<<%%0x%02x>>",spec);
            break;
         }
      }

      /* assert that the maximum number of bytes written to the buffer is less than MAXBUF */
      assert(nchar<MAXBUF);
      
      /* ensure the number of characters copied doesn't exceed the buffer capacity */
      if (nchar>=bufsize) nchar=bufsize-1;

      /* copy the conversion from to the buffer */
      if (nchar>0) strncpy(buf,scr,nchar); else nchar=0;
   }
   
   return nchar;
   #undef MAXBUF
}

/*------------------------------------------------------------------------*/
/* function to convert calendar time to broken-down time                  */
/*------------------------------------------------------------------------*/
/**
   This function converts the calendar time (seconds) to broken-down time
   representation.

      \begin{verbatim}
      input:
      
         seconds...When this pointer is dereferenced, the value represents
                   the number of seconds elapsed since 00:00:00 on Jan 1,
                   1970, Coordinated Universal Time (UTC).

         isdst.....This parameter determines if daylight savings is in
                   effect.  A non-zero value indicates that DST is in effect
                   whereas a zero indicates that DST is not in effect.
         
      output:

         t.........If non-NULL, this structure will contain the broken-down
                    time corresponding to the 'seconds' argument.

         This function returns a pointer the object containing the
         broken-down time.  If the function parameter 't' is non-NULL then
         the this function returns a pointer to t.  Otherwise, broken-down
         time is stored in a communal static data object and the pointer to
         this static data object is returned.
         
      \end{verbatim}
*/
struct tm *Tm(time_t seconds, int isdst, struct tm *t)
{
   int year,mon;
   long int i,days=0;
   unsigned long sec;
   const short *pm;
   static struct tm ts;

   /* make sure that time_t has 4 bytes */
   assert(sizeof(time_t)==4);
   
   /* check if the conversion should be stored locally or passed back to the caller */
   if (!t) t = &ts;

   /* reference the time to the Jan 1, 1900 */
   if (seconds == (time_t)(-1)) {memset((void *)t,1,sizeof(struct tm)); return t;}
   else if (seconds<=0) {days=0; sec = EPOCH + seconds;}
   else {days = seconds/SecsPerDay; sec = EPOCH + seconds%SecsPerDay;}

   /* set the DST field */
   t->tm_isdst = (isdst>0) ? 1 : 0;

   /* correct for DST if it's in effect */
   if (t->tm_isdst) sec+=SecsPerHour;
   
   /* compute the number of days */
   days+=sec/SecsPerDay; 

   /* determine the weekday */
   t->tm_wday = (days+WDAY)%7;

   /* compute the year */
   for (year=days/365; days<(i=DaysTo(year,0)+DaysPerYear*year);) {--year;}

   /* subtract the number of days up to Jan 1 of the year determined above */
   days-=i;
      
   /* initialize the year and the day in the tm structure */
   t->tm_year=year; t->tm_yday=days; 

   /* compute the month */
   for (pm=MONTAB(year), mon=12; days < pm[--mon];) {} 

   /* initialize the month and the day in the tm structure */
   t->tm_mon=mon; t->tm_mday = days - pm[mon] + 1;

   /* compute the seconds left in the day */
   sec %= SecsPerDay;

   /* compute the number of hours left in the day */
   t->tm_hour = sec/SecsPerHour;

   /* compute the number of seconds left in the hour */
   sec %= SecsPerHour;

   /* compute the number of minutes left in the hour */
   t->tm_min = sec/SecsPerMin;

   /* compute the number of seconds left in the minute */
   t->tm_sec = sec % 60;

   return t;
}
