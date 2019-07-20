#ifndef SNPRINTF_H
#define SNPRINTF_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: snprintf.c,v 1.9 2007/04/24 01:43:29 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   SYNOPSIS
         #include <stdio.h>
         #include <stdarg.h>
         
         int snprintf(char *str, size_t size, const char *format, ...);
         int vsnprintf(char *str, size_t size, const char *format, va_list ap);
   
   DESCRIPTION
   
         These functions produce output according to a format just as with
         the printf family of functions.  The functions snprintf and
         vsnprintf write to the character string str.
         
         The function vsnprintf is equivalent to the functions snprintf,
         except that it is called with a va_list instead of a variable
         number of arguments.  This function does not call the va_end
         macro. Consequently, the value of ap is undefined after the
         call. The application should call va_end(ap) itself afterwards.
         
         The two functions write the output under the control of a format
         string that specifies how subsequent arguments (or arguments
         accessed via the variable-length argument facilities of stdarg) are
         converted for output.
   
      Return value
      
         These functions return the number of characters printed (not
         including the trailing `\0' used to end output to strings).
         snprintf and vsnprintf do not write more than size bytes (including
         the trailing '\0'), and, in accordance with the C99 standard,
         return the number of characters (excluding the trailing '\0') which
         would have been written to the final string if enough space had
         been available.)
*/
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
 * $Log: snprintf.c,v $
 * Revision 1.9  2007/04/24 01:43:29  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.8  2006/04/21 13:45:40  swift
 * Changed copyright attribute.
 *
 * Revision 1.7  2005/02/22 21:46:49  swift
 * Moved pragma directive out of header section into module body.
 *
 * Revision 1.6  2004/07/14 22:51:40  swift
 * Modifications to reflect the transition to the new stdio library.
 *
 * Revision 1.5  2003/11/20 18:59:42  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.4  2003/07/03 22:53:39  swift
 * Added some documentation.
 *
 * Revision 1.3  2003/06/23 18:59:47  swift
 * Fixed a bug that caused the buffer to be non-terminated.
 *
 * Revision 1.2  2003/06/20 21:43:05  swift
 * Fixed a bug that incorrectly terminated the string buffer.
 *
 * Revision 1.1  2003/06/20 20:16:25  swift
 * Initial revision
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define snprintfChangeLog "$RCSfile: snprintf.c,v $ $Revision: 1.9 $ $Date: 2007/04/24 01:43:29 $"
#include <stdio.h>
#include <stdarg.h>

/* function prototypes */
int snprintf(char *str, size_t size, const char *format, ...);
int vsnprintf(char *str, size_t size, const char *format, va_list ap);

#endif /* SNPRINTF_H */

#include <limits.h>
#include <assert.h>

/* static pointer to the character buffer */
static char *buf;

/* static counter for the maximum size of the buffer */
static size_t cmax;

/* static counter for the number of characters written */
static int ccnt; 

/* declaration of external function to do the actual work of formatting output */
extern int _doprnt(struct __prbuf *pb, const char *format, va_list ap);

/* define the structure needed by _doprnt() */
struct __prbuf {char *ptr; void (*func)(char);};

#pragma printf_check(snprintf)

/*------------------------------------------------------------------------*/
/* function to print to a string buffer                                   */
/*------------------------------------------------------------------------*/
/**
   This function performs the same services as sprintf() except that it
   writes no more than 'size' bytes (including the terminating NULL) in
   order to protect against writing beyond the end of the character buffer.

      \begin{verbatim}
      input:
         size.....The size of the character buffer 'str' into which the
                  formatted output will be written.  Nothing will be written
                  beyond 'str[size-1]'.
         format...The conversion specifier for formatting output.  See
                  printf() documentation for details.

      output:
         
         str......The character buffer into which the formatted output is
                  written.

         This function returns the number of characters that would have been
         written to the string buffer if the string buffer were sufficiently
         large.  This is in accordance with the C99 standard.
      \end{verbatim}
*/
int snprintf(char *str, size_t size, const char *format, ...)
{
   int n=0;
   va_list arg_pointer;
   va_start(arg_pointer,format);
   n=vsnprintf(str, size, format, arg_pointer);
   va_end(arg_pointer);
   return n;
}

/*------------------------------------------------------------------------*/
/* static function to write a single character to the character buffer    */
/*------------------------------------------------------------------------*/
/*
   This function writes up to a single character into the character buffer
   and it makes sure that no more than 'cmax' characters are written into
   the buffer.

      input:
         c ... The character to write to 'buf'.

      output:
         This function keeps a running total of the number of characters
         actually written to the buffer in the global variable 'ccnt'.
*/
static void snputc(char c)
{
   /* logic dicatates that cmax must be in the range [1,INT_MAX] */
   assert(cmax>=1 && cmax<=INT_MAX);
   
   /* check for invalid buffer or character-counter */
   if (!buf || ccnt<0) ccnt=-1;

   /* validate the character counter */
   else if (ccnt>=0 && ccnt<cmax)
   {
      /* write the character into the string buffer */
      buf[ccnt] = (ccnt==cmax-1) ? 0 : c;

      /* increment the character counter */
      ccnt++; if (ccnt<cmax) buf[ccnt]=0;
   }
}

/*------------------------------------------------------------------------*/
/* function to print to a string buffer                                   */
/*------------------------------------------------------------------------*/
/**
   This function performs the same services as sprintf() except that it
   writes no more than 'size' bytes (including the terminating NULL) in
   order to protect against writing beyond the end of the character buffer.

      \begin{verbatim}
      input:
         size.....The size of the character buffer 'str' into which the
                  formatted output will be written.  Nothing will be written
                  beyond 'str[size-1]'.
         format...The conversion specifier for formatting output.  See
                  printf() documentation for details.
                  
         ap.......The variable argument object.  This function does not call
                  the va_end macro. Consequently, the value of ap is
                  undefined after the call. The application should call
                  va_end(ap) itself afterwards.
                  
      output:
         
         str......The character buffer into which the formatted output is
                  written.

         This function returns the number of characters that would have been
         written to the string buffer if the string buffer were sufficiently
         large.  This is in accordance with the C99 standard.
      \end{verbatim}
*/
int vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
   struct __prbuf pb={NULL,snputc}; 
   
   /* initialize return value */
   int n=-1;

   /* validate the buffer */
   if (str && format && size>0)
   {
      /* initialize the counters and buffer pointer */
      ccnt=0; cmax=(size>INT_MAX)?INT_MAX:size; buf=str;

      /* call _doprnt() to carray out the work of formatting the output */
      n = _doprnt(&pb,format,ap);
   }
   
   return n; 
}
