#ifndef DOWNLOAD_H
#define DOWNLOAD_H (0x0100U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: download.c,v 1.7 2007/05/08 18:10:39 swift Exp $
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
 * $Log: download.c,v $
 * Revision 1.7  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.6  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.5  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.4  2005/06/27 15:15:37  swift
 * Added an additional bit of modem control if the download failed.
 *
 * Revision 1.3  2005/06/14 19:02:09  swift
 * Changed the return value of the download manager.
 *
 * Revision 1.2  2005/02/22 21:04:39  swift
 * Remove the temporary mission config file.
 *
 * Revision 1.1  2004/12/29 23:11:27  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define downloadChangeLog "$RCSfile: download.c,v $ $Revision: 1.7 $ $Date: 2007/05/08 18:10:39 $"
 
#include <serial.h>

int DownLoadMissionCfg(const struct SerialPort *modem,int NRetries);

#endif /* DOWNLOAD_H */
