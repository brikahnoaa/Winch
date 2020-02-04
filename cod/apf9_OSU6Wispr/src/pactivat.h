#ifndef PACTIVAT_H
#define PACTIVAT_H (0x0008U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: pactivat.c,v 1.7.2.1 2008/09/11 19:53:25 dbliudnikas Exp $
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
 * $Log: pactivat.c,v $
 * Revision 1.7.2.1  2008/09/11 19:53:25  dbliudnikas
 * Replace SBE41 with Seascan TD: comments only
 *
 * Revision 1.7  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.6  2006/10/11 21:01:08  swift
 * Implemented the module-wise logging facility.
 *
 * Revision 1.5  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.4  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.3  2005/10/11 22:48:50  swift
 * Minor bug fixes stemming from porting from argos to iridium.
 *
 * Revision 1.2  2005/10/11 20:37:44  swift
 * Implemented pressure-activation of iridium floats.
 *
 * Revision 1.1  2004/12/29 23:11:27  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.1.2.1  2004/10/30 16:36:28  swift
 * Feature to implement pressure-activation feature.
 *
 * Revision 1.4  2004/10/30 15:16:09  swift
 * Added a safety feature that checks the activation criteria twice
 * (5 seconds apart) before initiating the mission prelude.
 *
 * Revision 1.3  2004/10/30 15:06:38  swift
 * Eliminate the time-out feature of the pressure-activation mode.
 *
 * Revision 1.2  2004/10/30 14:58:49  swift
 * Added a time reference to improve the 8-second timing between test messages.
 *
 * Revision 1.1  2004/10/29 23:39:27  swift
 * Pressure-activation feature that initiates mission if the float sinks deeper
 * than the activation threshold pressure.
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define pactivateChangeLog "$RCSfile: pactivat.c,v $ $Revision: 1.7.2.1 $ $Date: 2008/09/11 19:53:25 $"

#include <time.h>

/* function prototypes */
int    PActivate(void);
int    PActivateInit(void);

#endif /* PACTIVAT_H */
