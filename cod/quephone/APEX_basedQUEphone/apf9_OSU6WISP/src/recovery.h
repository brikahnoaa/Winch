#ifndef RECOVERY_H
#define RECOVERY_H (0x0008U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: recovery.c,v 1.11 2007/05/08 18:10:39 swift Exp $
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
 * $Log: recovery.c,v $
 * Revision 1.11  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.10  2007/03/26 20:46:06  swift
 * Fixed a legacy bug in Recovery() that removed all files in storage after
 * each telemetry cycle.  With RAMIO storage this was necessary due to space
 * limitations.  However, with flash storage, this is definitely undesirable.
 *
 * Revision 1.9  2006/10/11 21:01:08  swift
 * Implemented the module-wise logging facility.
 *
 * Revision 1.8  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.7  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.6  2005/08/08 15:01:59  swift
 * Fixed a bug in the determinating whether to resume the mission prelude or
 * a regular profile cycle.
 *
 * Revision 1.5  2005/08/06 21:42:19  swift
 * Modifications to properly handle recover mode during the mission prelude as
 * well as during the mission.
 *
 * Revision 1.4  2005/06/27 15:13:41  swift
 * Fixed some erroneous comments.
 *
 * Revision 1.3  2005/02/22 21:08:56  swift
 * Added code to acquire a GPS fix.
 *
 * Revision 1.2  2005/01/06 00:58:42  swift
 * Initialized prf_path[].
 *
 * Revision 1.1  2004/12/29 23:11:27  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define recoveryChangeLog "$RCSfile: recovery.c,v $ $Revision: 1.11 $ $Date: 2007/05/08 18:10:39 $"

/* function prototypes */
int Recovery(void);
int RecoveryInit(void);
int RecoveryTerminate(void);

#endif /* RECOVERY_H */
