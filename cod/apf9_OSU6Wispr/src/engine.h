#ifndef ENGINE_H
#define ENGINE_H (0x01f0U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: engine.c,v 1.11 2008/07/14 16:54:56 swift Exp $
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
 * $Log: engine.c,v $
 * Revision 1.11  2008/07/14 16:54:56  swift
 * Reduced timeout specification to 2 hours.
 *
 * Revision 1.10  2007/06/08 02:14:52  swift
 * Reduced maximum time-out to 7200 seconds.
 *
 * Revision 1.9  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.8  2006/10/11 21:01:08  swift
 * Implemented the module-wise logging facility.
 *
 * Revision 1.7  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.6  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.5  2004/12/29 23:11:27  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.4  2003/12/09 15:53:59  swift
 * Eliminated the Maxwell counter as a termination criterion.
 *
 * Revision 1.3  2003/11/20 18:59:47  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.2  2003/10/25 19:30:16  swift
 * Changed function arguments from 'unsigned char' to 'int' to avoid variable
 * overflow bugs.
 *
 * Revision 1.1  2003/09/10 18:12:31  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define engineChangeLog "$RCSfile: engine.c,v $  $Revision: 1.11 $   $Date: 2008/07/14 16:54:56 $"

#include <time.h>

/* function prototypes */
int PistonMoveAbs(int newPosition); 
int PistonMoveAbsWTO(int newPosition,unsigned char *Volt,
                     unsigned char *Amp, time_t timeout);
int PistonMoveRel(int counts); 
   
#endif /* ENGINE_H */
