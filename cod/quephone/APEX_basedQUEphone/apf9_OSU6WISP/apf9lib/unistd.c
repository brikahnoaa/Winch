#ifndef UNISTD_H
#define UNISTD_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: unistd.c,v 1.5 2007/04/24 01:43:05 swift Exp $
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
 * $Log: unistd.c,v $
 * Revision 1.5  2007/04/24 01:43:05  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.4  2006/04/21 13:45:38  swift
 * Changed copyright attribute.
 *
 * Revision 1.3  2003/11/20 18:59:35  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.2  2003/07/03 22:49:41  swift
 * Added the function 'usleep()' to sleep in units of microseconds.
 *
 * Revision 1.1  2003/06/07 20:42:15  swift
 * Initial revision
 * \end{verbatim}
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define unistdChangeLog "$RCSfile: unistd.c,v $  $Revision: 1.5 $   $Date: 2007/04/24 01:43:05 $"

/* function prototypes */
unsigned int sleep(unsigned int sec);
void usleep(unsigned long int usec);

#endif /* UNISTD_H */

#include <apf9.h>

/*------------------------------------------------------------------------*/
/* function to pause program execution for a specified number of seconds  */
/*------------------------------------------------------------------------*/
/**
   This function pauses execution of the program for a specified number of
   seconds.  This function is compliant with the POSIX standard.

      \begin{verbatim}
      input:
         sec ... The number of seconds to pause.

      output:
         The POSIX standard allows requires that the sleep() function return the
         number of seconds left in the sleep period.  For UNIX systems this
         can be nonzero but for the APF9 this function will always return zero.
      \end{verbatim}
*/
unsigned int sleep(unsigned int sec)
{
   /* wait for the specified number of seconds */
   while (sec>0) {Wait(1000); --sec;}
   
   return sec;
}

/*------------------------------------------------------------------------*/
/* pause program execution for a specified number of microseconds         */
/*------------------------------------------------------------------------*/
/**
   This function pauses execution of the program for a specified number of
   microseconds.  This function is compliant with the POSIX standard.

      \begin{verbatim}
      input:
         usec ... The number of useconds to pause.
      \end{verbatim}
*/
void usleep(unsigned long int usec)
{
   unsigned long sec = usec/1000000; usec%=1000000;
   
   /* wait for the specified number of seconds */
   while (sec>0) {Wait(1000); --sec;}

   /* wait for the fractional parts of seconds */
   if (usec>0) Wait(usec/1000);
}