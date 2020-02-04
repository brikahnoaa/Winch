#ifndef GPS_H
#define GPS_H (0x0100U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: gps.c,v 1.14 2007/05/08 18:10:39 swift Exp $
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
 * $Log: gps.c,v $
 * Revision 1.14  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.13  2006/10/11 20:55:57  swift
 * Changed the timestamp-ing of almanac uploads to account for manually-induced
 * uploads.
 *
 * Revision 1.12  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.11  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.10  2006/01/06 23:23:44  swift
 * Improved time-skew checking.
 *
 * Revision 1.9  2005/08/06 17:19:35  swift
 * Add a logentry for magnet swipes during GPS acquisition.
 *
 * Revision 1.8  2005/07/05 21:59:43  swift
 * Fixed a minor formatting error in a logentry.
 *
 * Revision 1.7  2005/06/27 15:16:23  swift
 * Added a retry loop to make GPS fix acquisition more fault tolerant.
 *
 * Revision 1.6  2005/06/23 20:31:21  swift
 * Elminated an extraneous call to LogNmeaSentences().
 *
 * Revision 1.5  2005/06/20 18:55:23  swift
 * Changed location of call to LogNmeaSentences() so that logging happens even
 * if a fix was not obtained.
 *
 * Revision 1.4  2005/04/04 22:35:01  swift
 * Change to a one-time GPS configuration policy.
 *
 * Revision 1.3  2005/03/31 17:26:10  swift
 * Added facility to log NMEA sentences.
 *
 * Revision 1.2  2005/02/22 21:05:49  swift
 * Added function to compute the time relative to the Unix epoch.
 *
 * Revision 1.1  2005/01/18 18:49:44  swift
 * Module to manage GPS services for iridium APEX floats.
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define GpsChangeLog "$RCSfile: gps.c,v $ $Revision: 1.14 $ $Date: 2007/05/08 18:10:39 $"

#include <stdio.h>
#include <nmea.h>

/* prototypes for functions with external linkage */
int GpsServices(FILE *dest);
time_t UnixTime(const struct NmeaGpsFields *gps);
   
#endif /* GPS_H */
