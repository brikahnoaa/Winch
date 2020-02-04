#ifndef PROFILE_H
#define PROFILE_H (0x0080U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: profile.c,v 1.26.2.2 2009/01/19 16:43:32 dbliudnikas Exp $
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
 * $Log: profile.c,v $
 * Revision 1.26.2.2  2009/01/19 16:43:32  dbliudnikas
 * Remove 5 minute energy saving wait in profile not applicable to Seascan (leftover from Sbe41CP mods).
 *
 * Revision 1.26.2.1  2008/09/11 20:09:24  dbliudnikas
 * Replace SBE41 with Seascan TD: status, no getobs, getp becomes getpt to ensure new data, no CP mode, no salinity.
 *
 * Revision 1.26  2008/07/14 16:57:34  swift
 * Implemented 32-bit status words.
 *
 * Revision 1.25  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.24  2006/12/06 15:54:48  swift
 * Added a LogEntry() if pressure acquisition failed for any reason.
 *
 * Revision 1.23  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.22  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.21  2005/10/03 16:11:25  swift
 * Change extension of data from 'prf' to 'msg'.
 *
 * Revision 1.20  2005/09/02 22:20:26  swift
 * Added status bit-mask and logentries for shallow-water trap.
 *
 * Revision 1.19  2005/08/14 23:42:09  swift
 * Modified the surface-detection algorithm to confirm a tentative surface
 * detection with a second pressure sample in order to ensure that the profile
 * will not be prematurely terminated.
 *
 * Revision 1.18  2005/08/11 20:15:34  swift
 * Added sanity checks on pressure to detect unreliable pressure data.
 *
 * Revision 1.17  2005/08/06 17:20:46  swift
 * Add profile termination time to output file.
 *
 * Revision 1.16  2005/07/05 21:44:56  swift
 * Minor modifications to format of data file.
 *
 * Revision 1.15  2005/06/20 18:54:35  swift
 * Eliminated bogus sanity check on AscentControl.SurfacePressure variable.
 * Added a sanity check on the valid pressure range reported by the pressure sensor.
 *
 * Revision 1.14  2005/06/17 16:32:25  swift
 * Fixed a typo of Sbe41cpPUnreliable flag.
 *
 * Revision 1.13  2005/06/14 18:59:19  swift
 * Add range check to reliability check of pressure data.
 *
 * Revision 1.12  2005/05/01 14:26:24  swift
 * Reduced the post surface-detection wait period from 15 minutes to 5 minutes.
 *
 * Revision 1.11  2005/02/22 21:07:50  swift
 * Moved code to open the profile file to 'descent.c'.
 *
 * Revision 1.10  2005/01/06 00:57:05  swift
 * Write the profile to the file system.
 *
 * Revision 1.9  2004/12/29 23:11:27  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.8  2004/06/07 21:14:39  swift
 * Fixed a bug that caused an array under-indexing error if the float was at
 * the surface at the end of the drift phase and the SBE41 reported a negative
 * pressure.
 *
 * Revision 1.7  2004/04/26 16:02:18  swift
 * Added an ascent control parameter called SurfacePressure to record the
 * surface reference.  The surface detection algorithm uses this reference.
 * The parameter 'SurfacePressure' in the engineering data records the pressure
 * measurement at the initiation of the descent phase.
 *
 * Revision 1.6  2004/04/14 16:39:31  swift
 * Included an external declaration of the pressure table.
 *
 * Revision 1.5  2004/03/26 16:34:53  swift
 * Changed the criteria for computing the near-surface time-out period of the
 * buoyancy pump to include the surface-pressure offset.
 *
 * Revision 1.4  2004/03/24 00:41:45  swift
 * Eliminated SurfaceDetect() from being executed twice.
 *
 * Revision 1.3  2003/11/20 18:59:47  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.2  2003/10/28 23:29:37  swift
 * Fixed a bug that caused the 'SbePUnreliable' bit to be incorrectly set.
 *
 * Revision 1.1  2003/09/11 21:54:37  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define profileChangeLog "$RCSfile: profile.c,v $ $Revision: 1.26.2.2 $ $Date: 2009/01/19 16:43:32 $"

#include <time.h>

/* definition of structure to contain an observation */
struct Obs {float p, t, s;};

/* function prototypes */
int    GetP(float *p);
time_t Profile(void);
int    ProfileInit(void);
int    ProfileTerminate(void);

/* define the number of elements in the pressure table */
extern const int pTableSize;
extern const float pTable[];

extern persistent far struct Obs obs[];
extern persistent far int pTableIndex;
extern persistent char prf_path[32];

#endif /* PROFILE_H */
