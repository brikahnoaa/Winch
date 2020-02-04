#ifndef LOGGER_H
#define LOGGER_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: logger.c,v 1.29 2007/04/24 01:43:29 swift Exp $
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
 * $Log: logger.c,v $
 * Revision 1.29  2007/04/24 01:43:29  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.28  2007/01/22 18:07:34  swift
 * Fixed a serious bug that caused infinite stack regression when the logfile
 * grew to maximum size while the debuglevel was greater than three.
 *
 * Revision 1.27  2006/12/26 21:05:40  swift
 * Modified the way that full logfiles are handled and closed.
 *
 * Revision 1.26  2006/12/19 23:42:56  swift
 * Fixed a serious bug that caused infinite stack regression when the logfile
 * grew to maximum size.
 *
 * Revision 1.25  2006/11/07 15:19:34  swift
 * Minor modification to maintain compatibility with linux use.
 *
 * Revision 1.24  2006/10/13 18:56:20  swift
 * Eliminated fflush(logstream) from LogEntry() and LogAdd() because it's too
 * expensive with the new flashio file system.
 *
 * Revision 1.23  2006/10/11 21:02:31  swift
 * Modified LogStream() to accurately reflect the output stream.
 *
 * Revision 1.22  2006/08/17 21:17:56  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.21  2006/04/21 13:45:40  swift
 * Changed copyright attribute.
 *
 * Revision 1.20  2006/01/06 23:15:07  swift
 * Added calls to pet the WatchDog.
 *
 * Revision 1.19  2005/07/13 15:08:58  swift
 * More tweaking to enable logging facility to be used under linux.
 *
 * Revision 1.18  2005/07/05 22:01:34  swift
 * Minor tweaking to enable logging facility to be used under linux.
 *
 * Revision 1.17  2005/06/28 18:58:14  swift
 * Made the MaxLogSize object persistent so that 2-way remote assignments work.
 *
 * Revision 1.16  2005/02/22 21:50:36  swift
 * Undefine 'persistent' when XA not in use.
 *
 * Revision 1.15  2004/12/29 23:06:50  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.14  2004/06/07 21:19:41  swift
 * Modifications to write mission time to flash-based log file.
 *
 * Revision 1.13  2004/03/24 00:13:52  swift
 * Changed the debuglevel to be a persistent variable.
 *
 * Revision 1.12  2003/11/20 18:59:42  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.11  2003/07/19 22:41:22  swift
 * Implement a new definition of the most terse logging mode (debuglevel=0).
 *
 * Revision 1.10  2003/07/03 22:52:13  swift
 * Eliminated the need to include 'ds2404.h' as a portability enhancement.
 *
 * Revision 1.9  2003/06/11 18:35:18  swift
 * Modified LogEntry() to use a static character buffer rather
 * than an automatic variable that is created on the program
 * stack.  The purpose is to reduce stack requirements and the
 * probability of stack overflows.
 *
 * Revision 1.8  2003/06/07 20:32:58  swift
 * Modifications to include interval-timer in the log output.
 *
 * Revision 1.7  2003/05/31 21:19:48  swift
 * Tightened up some argument checking in LogEntry() and LogAdd().
 *
 * Revision 1.6  2002/12/31 15:22:08  swift
 * Added some documentation about the debuglevel.
 *
 * Revision 1.5  2002/11/27 00:43:17  swift
 * Fixed a bug caused when logstream is NULL.
 *
 * Revision 1.4  2002/10/21 13:08:09  swift
 * Fixed a formatting bug in LogSize() that reversed the order of the timestamp
 * and function name in the log file.
 *
 * Revision 1.3  2002/10/16 16:10:04  swift
 * Changed debuglevel for log entry of LogOpen() and LogClose() functions.
 *
 * Revision 1.2  2002/10/08 23:39:25  swift
 * Combined header file into source file.  Modifications to accomodate
 * file sizes larger than the 32K limit of int's.
 *
 * Revision 1.1  2002/05/07 22:09:16  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define LoggerChangeLog "$RCSfile: logger.c,v $  $Revision: 1.29 $   $Date: 2007/04/24 01:43:29 $"

#include <stdio.h>

#ifdef _XA_
   typedef code const char cc;
#else
   typedef const char cc;
   #define persistent
#endif /* _XA_ */

/* external prototypes of functions for the logging module */
long int LogAdd(const char *format,...);
int      LogAttach(FILE *stream);
int      LogClose(void);
long int LogEntry(cc *function_name,cc *format,...);
int      LogOpen(const char *fname,char mode);
int      LogOpenAuto(const char *basepath);
long int LogSize(void);
FILE    *LogStream(void);

/* external declaration for maximum allowed log size */
extern persistent long int MaxLogSize;

/* external declaration of debug_level */
extern persistent unsigned int debugbits;

/* declare the default message stream */
extern FILE *persistent logstream;

#define VERBOSITYMASK (0x07)
#define debuglevel (debugbits&VERBOSITYMASK)

#endif /* LOGGER_H */
