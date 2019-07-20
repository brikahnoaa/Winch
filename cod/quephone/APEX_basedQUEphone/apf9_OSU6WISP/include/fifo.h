#ifndef FIFO_H
#define FIFO_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: fifo.c,v 1.10 2007/04/24 01:43:29 swift Exp $
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
 * $Log: fifo.c,v $
 * Revision 1.10  2007/04/24 01:43:29  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.9  2006/04/21 13:45:40  swift
 * Changed copyright attribute.
 *
 * Revision 1.8  2005/10/25 17:01:48  swift
 * Fixed a bug in push() that used 'int' rather than 'long int'.
 *
 * Revision 1.7  2005/02/22 21:47:43  swift
 * Made some integers volatile.
 *
 * Revision 1.6  2004/12/29 23:06:50  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.5  2003/11/25 00:33:29  swift
 * Changed type of 'head' and 'tail' to be long int to avoid integer overflows.
 *
 * Revision 1.4  2003/11/20 18:59:42  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.3  2003/11/12 22:23:20  swift
 * Modified Fifo structure to put the buffer in the far ram.
 *
 * Revision 1.2  2003/07/03 22:44:03  swift
 * Major revisions.  This version is not yet stable.
 *
 * Revision 1.1  2003/06/21 23:20:03  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define fifoChangeLog "$RCSfile: fifo.c,v $  $Revision: 1.10 $ $Date: 2007/04/24 01:43:29 $"

#include <stdlib.h>

/* define a FIFO structure */
struct Fifo
{
      /* define a constant pointer to the fifo buffer */
      unsigned far char* const buf;

      /* object to record the size of the fifo buffer */
      const size_t size;

      /* object to record the number of bytes in the fifo queue */
      volatile size_t length;

      /* object to record the number of bytes lost to buffer overflow */
      volatile size_t BufOverflowCount;
      
      /* define the head and tail of the fifo */
      volatile long int head, tail;
};
 
/* function prototypes */
int del(struct Fifo *fifo);
int flush(struct Fifo *fifo);
int push(struct Fifo *fifo, unsigned char byte);
int pop(struct Fifo *fifo, unsigned char *byte);

#endif /* FIFO_H */
