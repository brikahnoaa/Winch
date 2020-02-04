#ifndef GODEEP_H
#define GODEEP_H (0x0040U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: godeep.c,v 1.11 2007/05/08 18:10:39 swift Exp $
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
 * $Log: godeep.c,v $
 * Revision 1.11  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.10  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.9  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.8  2006/03/15 16:00:48  swift
 * Eliminate vestiges of ModemDefibrillator() calls by removing include directives.
 *
 * Revision 1.7  2006/02/28 18:43:22  swift
 * Eliminated the apf9icom.h header.
 *
 * Revision 1.6  2006/02/28 16:34:23  swift
 * Eliminated the modem defibrillation kludge because the fundamental problem
 * with nonresponsive LBT9522a modems was found and fixed.
 *
 * Revision 1.5  2006/01/31 19:26:19  swift
 * Added LBT defibrillation kludge.
 *
 * Revision 1.4  2004/12/29 23:11:27  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.3  2003/11/20 18:59:47  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.2  2003/09/13 14:33:41  swift
 * Added GoDeepTerminate() to clean up the termination of the deep-descent phase.
 *
 * Revision 1.1  2003/09/11 21:32:25  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define GoDeepChangeLog "$RCSfile: godeep.c,v $ $Revision: 1.11 $ $Date: 2007/05/08 18:10:39 $"

/* enumerate piston-adjustment options when the deep-descent phase is terminated */
enum PistonAdjustment {RETRACT=-1,EXTEND=1};

/* function prototypes */
int GoDeep(void);
int GoDeepInit(void);
int GoDeepTerminate(enum PistonAdjustment pa);

#endif /* GODEEP_H */
