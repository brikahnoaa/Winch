#ifndef PKT_H
#define PKT_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: pkt.c,v 1.2 2007/04/24 01:43:29 swift Exp $
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
 * $Log: pkt.c,v $
 * Revision 1.2  2007/04/24 01:43:29  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.1  2004/12/29 23:11:27  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.2  2002/10/08 23:52:36  swift
 * Combined header file into source file.
 *
 * Revision 1.1  2002/05/07 22:16:38  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define PktChangeLog "$RCSfile: pkt.c,v $  $Revision: 1.2 $   $Date: 2007/04/24 01:43:29 $"

unsigned char PktMonitor(int status);
unsigned char PktType(void);
void Pkt1k(void);
void Pkt128b(void);

#endif /* PKT_H */
