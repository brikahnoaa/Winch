#ifndef APF9COM_H
#define APF9COM_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: apf9com.c,v 1.12 2008/07/14 17:06:13 swift Exp $
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
 * $Log: apf9com.c,v $
 * Revision 1.12  2008/07/14 17:06:13  swift
 * Increased the size of the IO buffers to facilitate Sbe41cp gateway mode.
 *
 * Revision 1.11  2007/06/08 02:16:56  swift
 * Fixed return values of some functions to report success.
 *
 * Revision 1.10  2007/04/24 01:43:05  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.9  2006/04/21 13:45:38  swift
 * Changed copyright attribute.
 *
 * Revision 1.8  2006/01/06 23:10:07  swift
 * Added rts(), cts(), dtr(), and dsr() primitives to the SerialPort interface.
 *
 * Revision 1.7  2005/06/14 18:57:19  swift
 * Modifications to initialize low-level UART hardware before VALID line is active.
 *
 * Revision 1.6  2005/01/06 00:50:42  swift
 * Fixed bugs in RTS/CTS control.
 *
 * Revision 1.5  2004/12/29 23:02:52  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.4  2004/07/14 22:49:28  swift
 * Modified SerialPort objects to be constant.
 *
 * Revision 1.3  2004/04/14 20:48:21  swift
 * Revision E of the APF9 made some hardware changes to earlier versions.  The
 * PSD935 was replaced by a PSD835 and the 20mA serial loop was changed to be a
 * real serial IO port.
 *
 * Revision 1.2  2003/11/20 18:59:35  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.1  2003/09/19 23:41:43  swift
 * Initial revision
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define apf9comChangeLog "$RCSfile: apf9com.c,v $ $Revision: 1.12 $ $Date: 2008/07/14 17:06:13 $"

#include <serial.h>

/* prototypes for functions with external linkage */
int  Com1Cts(void);
void Com1Disable(void);
void Com1Enable(unsigned int BaudRate);
int  Com1RtsAssert(void);
int  Com1RtsClear(void);
int  Com2Cts(void);
void Com2Disable(void);
void Com2Enable(unsigned int BaudRate);
int  Com2RtsAssert(void);
int  Com2RtsClear(void);
void InitDUart2(void);

/* extern references to expansion serial ports */
extern const struct SerialPort com1,com2;

#endif /* APF9COM_H */
