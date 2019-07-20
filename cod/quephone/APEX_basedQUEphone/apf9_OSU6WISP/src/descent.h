#ifndef DESCENT_H
#define DESCENT_H (0x0010U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: descent.c,v 1.16.2.1 2008/09/11 19:58:56 dbliudnikas Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
 * $Log: descent.c,v $
 * Revision 1.16.2.1  2008/09/11 19:58:56  dbliudnikas
 * Replace SBE41 with Seascan TD: status.
 *
 * Revision 1.16  2008/07/14 16:54:24  swift
 * Implement compensator hyper-retraction mechanism for parking N2
 * floats at mid-water pressures.
 *
 * Revision 1.15  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.14  2006/10/11 21:00:34  swift
 * Integrated the new flashio file system.
 *
 * Revision 1.13  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.12  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.11  2005/10/11 20:45:29  swift
 * Added a flag for 8-bit profile counter overflow (for use with SBD floats).
 *
 * Revision 1.10  2005/10/03 16:11:25  swift
 * Change extension of data from 'prf' to 'msg'.
 *
 * Revision 1.9  2005/06/20 18:52:58  swift
 * Change handling of the surface pressure data so that the actual value is telemetered.
 *
 * Revision 1.8  2005/02/22 21:03:50  swift
 * Added code to define the profile path name.
 *
 * Revision 1.7  2005/01/06 00:54:57  swift
 * Added code to initialize prf_path.
 *
 * Revision 1.6  2004/12/29 23:11:27  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.5  2004/04/26 23:32:53  swift
 * Moved an include directive to get it out of the header section.
 *
 * Revision 1.4  2004/03/26 16:31:40  swift
 * Added criteria for limiting the surface pressure offset to be within a
 * 20 decibar deadband.
 *
 * Revision 1.3  2003/11/20 18:59:47  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.2  2003/11/12 22:41:22  swift
 * Modifications to use GetP() rather than Sbe41GetP().
 *
 * Revision 1.1  2003/09/11 22:05:01  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define DescentChangeLog "$RCSfile: descent.c,v $ $Revision: 1.16.2.1 $ $Date: 2008/09/11 19:58:56 $"
/* define the number of accumulated detections*/
extern persistent long int accum_dtx; //HM accumulated detections per profile

/* function prototypes for statically linked functions */
int Descent(void);
int DescentInit(void);
int DescentTerminate(void);

#endif /* DESCENT_H */
