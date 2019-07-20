#ifndef PRELUDE_H
#define PRELUDE_H (0x0008U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: prelude.c,v 1.13 2007/05/08 18:10:39 swift Exp $
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
 * $Log: prelude.c,v $
 * Revision 1.13  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.12  2006/10/11 21:01:08  swift
 * Implemented the module-wise logging facility.
 *
 * Revision 1.11  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.10  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.9  2005/10/03 16:11:25  swift
 * Change extension of data from 'prf' to 'msg'.
 *
 * Revision 1.8  2005/08/06 21:42:19  swift
 * Modifications to properly handle recover mode during the mission prelude as
 * well as during the mission.
 *
 * Revision 1.7  2005/02/22 21:06:56  swift
 * Changed the name of the config logger function.
 *
 * Revision 1.6  2005/01/06 00:55:56  swift
 * Changed log_path length in snprintf().
 *
 * Revision 1.5  2004/12/29 23:11:27  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.4  2004/04/26 15:59:55  swift
 * Changed location of an include directive to get it out of the header secion.
 *
 * Revision 1.3  2003/11/20 18:59:47  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.2  2003/11/12 23:03:49  swift
 * Unset TestMsg bit of the status byte.
 *
 * Revision 1.1  2003/09/11 21:35:36  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define PreludeChangeLog "$RCSfile: prelude.c,v $ $Revision: 1.13 $ $Date: 2007/05/08 18:10:39 $"

/* function prototypes */
int Prelude(void);
int PreludeInit(void);
int PreludeTerminate(void);

#endif /* PRELUDE_H */
