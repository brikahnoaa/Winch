#ifndef PARK_H
#define PARK_H (0x0020U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: park.c,v 1.20.2.1 2008/09/11 20:06:59 dbliudnikas Exp $
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
 * $Log: park.c,v $
 * Revision 1.20.2.1  2008/09/11 20:06:59  dbliudnikas
 * Replace SBE41 with Seascan TD: status, getobs becomes getpt (no salinity).
 *
 * Revision 1.20  2008/07/14 16:57:34  swift
 * Implemented 32-bit status words.
 *
 * Revision 1.19  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.18  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.17  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.16  2006/03/15 16:00:48  swift
 * Eliminate vestiges of ModemDefibrillator() calls by removing include directives.
 *
 * Revision 1.15  2006/02/28 18:43:22  swift
 * Eliminated the apf9icom.h header.
 *
 * Revision 1.14  2006/02/28 16:34:23  swift
 * Eliminated the modem defibrillation kludge because the fundamental problem
 * with nonresponsive LBT9522a modems was found and fixed.
 *
 * Revision 1.13  2006/01/31 19:44:43  swift
 * Added a missing include directive for apf9icom services.
 *
 * Revision 1.12  2006/01/31 19:25:05  swift
 * Added LBT defibrillation kludge.
 *
 * Revision 1.11  2005/09/02 22:21:31  swift
 * Reduced text to profile file.
 *
 * Revision 1.10  2005/08/17 21:56:22  swift
 * Changed logging verbosity of PT samples to reduce typical log file size.
 *
 * Revision 1.9  2005/08/06 17:20:03  swift
 * Modify the park PT report slightly.
 *
 * Revision 1.8  2005/06/27 15:14:18  swift
 * Added a logentry if the profile file couldn't be opened.
 *
 * Revision 1.7  2005/05/01 14:31:28  swift
 * Changed the Sbe41cpStatus masks for low-power TP sampling.
 *
 * Revision 1.6  2004/12/29 23:11:27  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.5  2004/05/03 18:06:34  swift
 * Added a 1 second pause in ParkTerminate() between CTD samples as required
 * for the CTD to wake up the second time.
 *
 * Revision 1.4  2004/04/26 15:55:18  swift
 * Changed location of an include directive to get it out of the header secion.
 *
 * Revision 1.3  2003/11/20 18:59:47  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.2  2003/11/12 23:02:10  swift
 * Modifications to logging.
 *
 * Revision 1.1  2003/09/11 21:34:13  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define ParkChangeLog "$RCSfile: park.c,v $ $Revision: 1.20.2.1 $ $Date: 2008/09/11 20:06:59 $"

/* function prototypes */
int GetPt(float *p, float *t);
int Park(void);
int ParkInit(void);
int ParkTerminate(void);

#endif /* PARK_H */
