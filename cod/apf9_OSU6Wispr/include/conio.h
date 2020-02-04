#ifndef CONIO_H
#define CONIO_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: conio.c,v 1.21.2.1 2008/09/11 19:40:29 dbliudnikas Exp $
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
 * $Log: conio.c,v $
 * Revision 1.21.2.1  2008/09/11 19:40:29  dbliudnikas
 * Update for Webb development system: line length issue.
 *
 * Revision 1.21  2008/07/14 17:07:00  swift
 * Eliminate an un-needed wait to speed-up console IO.
 *
 * Revision 1.20  2007/04/24 01:43:05  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.19  2006/04/21 13:45:38  swift
 * Changed copyright attribute.
 *
 * Revision 1.18  2006/01/06 23:09:22  swift
 * Added a check to see if 20ma serial loop is active before printing a byte.
 * Avoiding the write attempt will save energy.
 *
 * Revision 1.17  2005/06/21 14:35:18  swift
 * Reduced manipulation of TXD0 and TXD1.
 *
 * Revision 1.16  2005/05/13 23:24:57  swift
 * Modifications to increase the baud rate of the 20ma serial loop from
 * 4800baud to 9600baud.
 *
 * Revision 1.15  2005/05/09 17:41:37  swift
 * Incorporated speed-ups to 20ma serial loop as suggested by John Dunlap.
 *
 * Revision 1.14  2005/01/06 00:48:30  swift
 * Alphabetized include directives and eliminated use of trace() functionality.
 *
 * Revision 1.13  2004/12/29 23:04:36  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.12  2004/07/14 22:49:02  swift
 * Added a getch() function to fulfill programming contract with the new stdio
 * library.
 *
 * Revision 1.11  2004/06/07 21:18:30  swift
 * Changed linkage specification of getb20ma() and putb20ma() from static to
 * extern for use by Iridium firmware.  Modified the return value of putb() to
 * satisfy the programming contract with Iridium firmware.
 *
 * Revision 1.10  2004/04/26 21:42:22  swift
 * Turned off recieve-interrupts of the 20mA serial interface during
 * character-transmits so that echos are ignored.
 *
 * Revision 1.9  2004/04/23 23:40:55  swift
 * Eliminated bit-banging code in putb20ma() and replaced it with a write to
 * the serial port.
 *
 * Revision 1.8  2004/04/14 20:48:21  swift
 * Revision E of the APF9 made some hardware changes to earlier versions.  The
 * PSD935 was replaced by a PSD835 and the 20mA serial loop was changed to be a
 * real serial IO port.
 *
 * Revision 1.7  2003/12/09 15:47:29  swift
 * Reversed changes referred to in revision 1.6 because interrupts were
 * disabled on the CTD serial port for long enough to lose bytes.
 *
 * Revision 1.6  2003/12/07 22:10:14  swift
 * Tracked down and fixed the bug that caused garbage characters to be written
 * to the log file during CTD operations.  It was fixed by disabling the Rx
 * interrupt of the CTD serial port during writes to the 20ma console port.
 *
 * Revision 1.5  2003/11/20 18:59:35  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.4  2003/11/12 22:36:22  swift
 * Added ConioActive(), ConioEnable(), and ConioDisable() funtions.
 *
 * Revision 1.3  2003/07/19 22:37:15  swift
 * Changes to better handle CR/LF issues.
 *
 * Revision 1.2  2003/07/03 22:41:18  swift
 * Major revisions.  This revision is not yet stable.
 *
 * Revision 1.1  2003/06/25 17:03:09  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define conioChangeLog "$RCSfile: conio.c,v $ $Revision: 1.21.2.1 $ $Date: 2008/09/11 19:40:29 $"

#include <serial.h>

/* function prototypes */
int ConioActive(void);
int ConioDisable(void);
int ConioEnable(void);
int getb(void); 
int getb20ma(unsigned char *byte); 
int kbdhit(void);
int putb(unsigned char byte);
int putb20ma(unsigned char byte);
void putch(char byte);
int kbdhit_CRLF(void);      /* kbdhit WITHOUT CR/LF translation */
void putch_CRLF(char byte); /* putch WITHOUT CR/LF translation */

/* declare variable to hold the number of breaks received */
extern volatile unsigned int ecnt1;

/* declare the console serial port */
extern struct SerialPort conio;

#endif /* CONIO_H */
