#ifndef MODEM_H
#define MODEM_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: modem.c,v 1.6 2007/04/24 01:43:29 swift Exp $
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
 * $Log: modem.c,v $
 * Revision 1.6  2007/04/24 01:43:29  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.5  2007/03/26 20:43:21  swift
 * Added prototype for snprintf() and removed include directive for snprintf.h.
 *
 * Revision 1.4  2006/10/11 21:02:59  swift
 * Added a toggle of the DTR signal prior to each connection attempt.
 *
 * Revision 1.3  2005/06/27 17:05:02  swift
 * Replaced references to 'Motorola IRIDIUM' to 'IRIDIUM' to account for
 * changes in manufacturer of the Daytona.
 *
 * Revision 1.2  2005/06/14 19:03:33  swift
 * Changed linkage of modem_initialize from static to external.
 *
 * Revision 1.1  2004/12/29 23:08:42  swift
 * Module to manage communications via modem.
 *
 * Revision 1.5  2002/12/31 16:58:29  swift
 * Eliminated the reference to the unistd.h header file.
 *
 * Revision 1.4  2002/10/21 18:24:04  swift
 * Reduce the number of connect tries from 5 to 3.
 *
 * Revision 1.3  2002/10/11 13:59:16  swift
 * Minor rework of the modem_initialize() function.
 *
 * Revision 1.2  2002/10/08 23:49:23  swift
 * Modifications for const correctness.  Added documentation.  Combined header file
 * into source file.
 *
 * Revision 1.1  2002/05/07 22:16:08  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define ModemChangeLog "$RCSfile: modem.c,v $  $Revision: 1.6 $   $Date: 2007/04/24 01:43:29 $"

#include <serial.h>

/* prototypes for external functions */
int connect(const struct SerialPort *port,const char *dialstring,time_t sec);
int hangup(const struct SerialPort *port);
int modem_initialize(const struct SerialPort *port);

#endif /* MODEM_H */
