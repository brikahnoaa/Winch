#ifndef GARMIN_H
#define GARMIN_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: garmin.c,v 1.12 2007/04/24 01:43:29 swift Exp $
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
 * $Log: garmin.c,v $
 * Revision 1.12  2007/04/24 01:43:29  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.11  2006/12/06 15:53:37  swift
 * Modified PGRMC configuration string to account for changes in GPS15 firmware.
 *
 * Revision 1.10  2006/11/22 03:58:57  swift
 * Changed configuration of Garmin15 to use automatic mode.
 *
 * Revision 1.9  2006/02/03 00:24:16  swift
 * Rework the interactive configuration commands to improve robustness and
 * error detection.
 *
 * Revision 1.8  2006/01/31 19:28:00  swift
 * Implemented a work-around for a bug in the Garmin GPS15L-W firmware that
 * caused configuration problems.
 *
 * Revision 1.7  2006/01/06 23:15:53  swift
 * Fixed an incorrect warning message.
 *
 * Revision 1.6  2005/04/04 22:34:04  swift
 * Modifications to allow one-time configuration at beginning of mission.
 *
 * Revision 1.5  2005/02/22 21:49:16  swift
 * Added functions to enable/disable various NMEA sentences.  Added function to
 * update the almanac.
 *
 * Revision 1.4  2005/01/15 00:47:41  swift
 * Modified API for the Garmin GPS25 for use with the Garmin 15L.
 *
 * Revision 1.3  2002/12/31 16:58:29  swift
 * Eliminated the reference to the unistd.h header file.
 *
 * Revision 1.2  2002/10/08 23:33:24  swift
 * Fixed a formatting glitch in the comment section.
 *
 * Revision 1.1  2002/08/22 19:04:22  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define GarminChangeLog "$RCSfile: garmin.c,v $  $Revision: 1.12 $   $Date: 2007/04/24 01:43:29 $"

#include <serial.h>

/* function prototypes for global functions */
int    ConfigGarmin15(const struct SerialPort *port);
int    DisableNmeaSentence(const struct SerialPort *port,const char *id);
int    EnableNmeaSentence(const struct SerialPort *port,const char *id);
int    LogNmeaSentences(const struct SerialPort *port);
time_t UpLoadGpsAlmanac(const struct SerialPort *port);

#endif /* GARMIN_H */
