#ifndef STRTIME_H
#define STRTIME_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: strtime.c,v 1.6 2007/04/24 01:43:29 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Copyright University of Washington.   Written by Dana Swift.
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
 *
 * RCS Log:
 *
 * $Log: strtime.c,v $
 * Revision 1.6  2007/04/24 01:43:29  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.5  2006/04/21 13:45:40  swift
 * Changed copyright attribute.
 *
 * Revision 1.4  2004/12/29 23:06:50  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.3  2003/11/20 18:59:42  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.2  2003/06/07 20:47:36  swift
 * Changed the function name from ParseTime() to strtime() to
 * satisfy pedantic requirements for file names in #include<>
 * statements.
 *
 * Revision 1.1  2003/05/16 20:26:06  swift
 * Initial revision
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define strtimeChangeLog "$RCSfile: strtime.c,v $  $Revision: 1.6 $   $Date: 2007/04/24 01:43:29 $"

#include <time.h>

/* function prototype */
int strtime(const char *timestr, time_t *sec);

#endif /* STRTIME_H */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <extract.h>
#include <regex.h>
#include <logger.h> 

/*------------------------------------------------------------------------*/
/* function to convert a time string to calendar time                     */
/*------------------------------------------------------------------------*/
/**
   This function parses a time string of the form 'mm/dd/yyyy:hh:mm:ss' and
   this time string into calendar time.

      \begin{verbatim}
      input:
         timestr ... The string containing the date and time in the form:
                     mm/dd/yyyy:HH:mm:ss representing the month, day, year,
                     hour, minute, and second respectively.

      output:
         sec ....... This is the calendar time represented as the number of
                     seconds since 00:00:00 on Jan 1, 1970.

         This function returns a positive value on success, zero if the
         parse attempt failed, and a negative value if either of the
         function arguments is invalid.
      \end{verbatim}
*/
int strtime(const char *timestr, time_t *sec)
{
   /* define the logging signature */
   static cc FuncName[] = "strtime()";
   
   int status=-1;

   /* validate the time string argument */
   if (!timestr)
   {
      /* create the message */
      static cc msg[]="Invalid time string received as function argument.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }
   
   /* validate the broken-down time argument */
   else if (!sec) 
   {
      /* create the message */
      static cc msg[]="Invalid time_t object received as function argument.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   else
   {
      #define NSUB (6)
      regex_t regex;
      regmatch_t regs[NSUB+1];
      int errcode;
      struct tm t;
         
      /* construct the regex pattern for the date/time string */
      const char *pattern = "^[ \t]*([0-9]{1,2})/([0-9]{1,2})/([0-9]{4}):"
         "([0-9]{1,2}):([0-9]{1,2}):([0-9]{1,2})[ \t]*$";
            
      /* re-initialize function's return value */
      status=1;

      /* compile the option pattern */
      assert(!regcomp(&regex,pattern,REG_EXTENDED|REG_NEWLINE));

      /* protect against segfaults */
      assert(NSUB==regex.re_nsub);

      /* check if the current line matches the regex */
      if ((errcode=regexec(&regex,timestr,regex.re_nsub+1,regs,0))==REG_NOMATCH)
      {
         /* create the message */
         static cc format[]="The time string \"%s\" does not "
            "match the regex: mm/dd/yyyy:hh:mm:ss\n";

         /* enter the error into the logfile */
         LogEntry(FuncName,format,timestr);

         /* indicate failure of the parse attempt */
         status=0;
      }
      
      /* check for pathological regex conditions */
      else if (errcode)
      {
         /* create the message */
         static cc format[]="Exception in regexec(): %s\n";
            
         /* map the regex error code to an error string */
         char errbuf[128]; regerror(errcode,&regex,errbuf,128);

         /* print the regex error string */
         LogEntry(FuncName,format,errbuf); 
         
         /* indicate failure of the parse attempt */
         status=0;
      }

      /* extract the date and time from the time string */
      else
      {
         /* initialize the structure with zeros */
         memset(&t,0,sizeof(struct tm));
         
         /* extract the broken-down time from the time string */
         t.tm_mon  = atoi(extract(timestr,regs[1].rm_so+1,regs[1].rm_eo-regs[1].rm_so))-1;
         t.tm_mday = atoi(extract(timestr,regs[2].rm_so+1,regs[2].rm_eo-regs[2].rm_so));
         t.tm_year = atoi(extract(timestr,regs[3].rm_so+1,regs[3].rm_eo-regs[3].rm_so))-1900;
         t.tm_hour = atoi(extract(timestr,regs[4].rm_so+1,regs[4].rm_eo-regs[4].rm_so));
         t.tm_min  = atoi(extract(timestr,regs[5].rm_so+1,regs[5].rm_eo-regs[5].rm_so));
         t.tm_sec  = atoi(extract(timestr,regs[6].rm_so+1,regs[6].rm_eo-regs[6].rm_so));

         /* convert the broken-down time to calendar time */
         *sec = mktime(&t);
         
         /* write the parsed date/time to the logfile */
         if (debuglevel>=2)
         {
            /* create the message */
            static cc format[]="The time string represents the date %s";

            /* log the message */
            LogEntry(FuncName,format,ctime(sec));
         }
      }
   }

   return status;
}
