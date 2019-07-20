#ifndef TELEMETR_H
#define TELEMETR_H (0x0100U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: telemetr.c,v 1.25.2.1 2008/09/11 19:58:15 dbliudnikas Exp $
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
 * $Log: telemetr.c,v $
 * Revision 1.25.2.1  2008/09/11 19:58:15  dbliudnikas
 * Replace SBE41 with Seascan TD: Remove salinity display.
 *
 * Revision 1.25  2008/07/14 16:58:01  swift
 * Implemented 32-bit status words.
 *
 * Revision 1.24  2007/10/05 22:31:00  swift
 * Impose pneumatic inflation limits.
 *
 * Revision 1.23  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.22  2007/01/15 19:19:01  swift
 * Changed handling of air-valve solenoid signals to reduce bladder inflation
 * time.
 *
 * Revision 1.21  2006/10/11 20:53:55  swift
 * Modifications for compatibility with the new flashio file system.
 *
 * Revision 1.20  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.19  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.18  2005/09/23 14:45:22  swift
 * Minor changes to formatting of the float-id and profile-id.
 *
 * Revision 1.17  2005/09/02 22:20:26  swift
 * Added status bit-mask and logentries for shallow-water trap.
 *
 * Revision 1.16  2005/07/05 21:44:12  swift
 * Refactored air-bladder inflation algorithm into its own function.
 *
 * Revision 1.15  2005/06/28 19:23:56  swift
 * Fixed some minor formatting errors in logentries.
 *
 * Revision 1.14  2005/06/27 15:13:16  swift
 * Minor modifications to the air-bladder inflation algorithm.  Added
 * logentries of number of connections and connection attempts.  Added firmware
 * revisions to the logfile and profile data file.
 *
 * Revision 1.13  2005/06/23 20:30:53  swift
 * Implemented new air-bladder inflation algorithm to be more tolerant of wave fields.
 *
 * Revision 1.12  2005/06/22 19:28:20  swift
 * Rework the air-bladder inflation algorithm to introduce some hysterisis.
 *
 * Revision 1.11  2005/06/17 16:31:50  swift
 * Rework handling of writes by GpsServices() and WriteVitals() to the profile file.
 *
 * Revision 1.10  2005/06/14 18:58:28  swift
 * Change name of RegisterIrModem() to IrModemRegister().
 *
 * Revision 1.9  2005/02/22 21:42:12  swift
 * Eliminated GPS stub and incorporated actual GPS functionality.  Used
 * enhanced LBT9522 functionality for surface detection after the profile phase
 * terminates.
 *
 * Revision 1.8  2005/01/06 00:58:12  swift
 * Refactored some functionality to other source code modules.
 *
 * Revision 1.7  2004/12/29 23:11:27  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.6  2004/04/15 23:20:19  swift
 * Eliminated the piston full-extension count from argos messages with 28-bit ids.
 *
 * Revision 1.5  2004/04/14 20:42:33  swift
 * Added provisions to properly handle either 2-bit or 28-bit argos ids.
 *
 * Revision 1.4  2004/04/14 16:38:43  swift
 * Eliminated argos telemetry of the piston full-extension count if 28-bit
 * argos id is used.  This keeps the test message set to a single message.
 *
 * Revision 1.3  2003/11/20 18:59:47  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.2  2003/11/12 23:05:11  swift
 * Added descent marks to ARGOS message.
 *
 * Revision 1.1  2003/09/11 21:57:42  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define telemetryChangeLog "$RCSfile: telemetr.c,v $ $Revision: 1.25.2.1 $ $Date: 2008/09/11 19:58:15 $"

#include <stdio.h>
#include <serial.h>

/* prototypes for functions with external linkage */
int AirSystem(void);
int Telemetry(void);
int TelemetryInit(void);
int TelemetryTerminate(void);

#endif /* TELEMETR_H */
