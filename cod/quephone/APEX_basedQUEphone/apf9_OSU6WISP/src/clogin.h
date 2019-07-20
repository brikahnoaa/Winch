#ifndef CLOGIN_H
#define CLOGIN_H (0x0100U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: clogin.c,v 1.7 2007/05/08 18:10:39 swift Exp $
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
 * $Log: clogin.c,v $
 * Revision 1.7  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.6  2006/10/11 20:57:10  swift
 * Added a toggle of the DTR signal just before each connection attempt.
 *
 * Revision 1.5  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.4  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.3  2005/03/30 20:25:22  swift
 * Fixed a typo.
 *
 * Revision 1.2  2005/01/06 00:52:06  swift
 * Fixed problems with DTR control of modem.
 *
 * Revision 1.1  2004/12/29 23:11:26  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define cloginChangeLog "$RCSfile: clogin.c,v $ $Revision: 1.7 $ $Date: 2007/05/08 18:10:39 $"

#include <serial.h>

int CLogin(const struct SerialPort *modem);

#endif /* CLOGIN_H */
