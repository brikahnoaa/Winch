#ifndef CTDIO_H
#define CTDIO_H (0x0008U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: ctdio.c,v 1.26.2.1 2008/09/11 19:40:29 dbliudnikas Exp $
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
 * $Log: ctdio.c,v $
 * Revision 1.26.2.1  2008/09/11 19:40:29  dbliudnikas
 * Update for Webb development system: line length issue.
 *
 * Revision 1.26  2008/07/14 17:08:27  swift
 * Modify state of ModeControl pin.
 *
 * Revision 1.25  2007/04/24 01:43:05  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.24  2006/10/23 16:49:13  swift
 * Modifications to code-sequencing in CtdPtSample() to avoid premature
 * activation of sample.
 *
 * Revision 1.23  2006/10/11 21:07:18  swift
 * Changed handling of the mode-pin signal during CP mode.
 *
 * Revision 1.22  2006/08/17 21:17:48  swift
 * MOdifications to allow better logging control.
 *
 * Revision 1.21  2006/04/21 13:45:38  swift
 * Changed copyright attribute.
 *
 * Revision 1.20  2005/10/25 17:01:01  swift
 * Changes induced by SBE41CP-IDO.
 *
 * Revision 1.19  2005/08/17 21:53:57  swift
 * Minor modification of Tx pin states during STP samples and power samples.
 *
 * Revision 1.18  2005/08/14 23:37:56  swift
 * Changed timing for more robust P-only sampling.
 *
 * Revision 1.17  2005/08/12 19:49:39  swift
 * Fixed more timing issues with P-only sampling.
 *
 * Revision 1.16  2005/08/11 16:32:25  swift
 * Modifications to fix a timing-related bug when requesting a P-only
 * measurement while the ctd is in CP mode.
 *
 * Revision 1.15  2005/06/22 19:23:50  swift
 * Fixed bugs caused by reducing use of TXD0 manipulations.
 *
 * Revision 1.14  2005/06/21 14:35:18  swift
 * Reduced manipulation of TXD0 and TXD1.
 *
 * Revision 1.13  2005/05/13 23:24:57  swift
 * Modifications to increase the baud rate of the 20ma serial loop from
 * 4800baud to 9600baud.
 *
 * Revision 1.12  2005/01/06 00:48:30  swift
 * Alphabetized include directives and eliminated use of trace() functionality.
 *
 * Revision 1.11  2004/12/29 23:04:36  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.10  2004/04/26 21:40:15  swift
 * Moved SetBaud() to apf9.c module.
 *
 * Revision 1.9  2004/04/23 23:42:49  swift
 * Changes to work around deficiencies in the XA serial port implementation.
 *
 * Revision 1.8  2004/04/14 20:48:21  swift
 * Revision E of the APF9 made some hardware changes to earlier versions.  The
 * PSD935 was replaced by a PSD835 and the 20mA serial loop was changed to be a
 * real serial IO port.
 *
 * Revision 1.7  2003/12/20 19:44:34  swift
 * Increased the size of the CTD serial port fifo from 80 bytes to 32760 bytes
 * to allow large blocks of data to be received.  Added the CtdActiveIo()
 * function.
 *
 * Revision 1.6  2003/11/20 18:59:35  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.5  2003/11/12 22:36:53  swift
 * Added CtdPower() function.
 *
 * Revision 1.4  2003/07/19 22:38:26  swift
 * Modifications that reflect the refactorization and remodeling of the Psd835 ports.
 *
 * Revision 1.3  2003/07/06 00:07:56  swift
 * Changed 'SetBaud()' to have static linkage since this compilation unit is
 * the only one that needs to use this function.
 *
 * Revision 1.2  2003/07/05 21:21:29  swift
 * Added a function 'CtdInitiateSample()' to initiate a sample and receive the
 * response from the CTD serial port.
 *
 * Revision 1.1  2003/07/03 22:41:57  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define ctdioChangeLog "$RCSfile: ctdio.c,v $  $Revision: 1.26.2.1 $ $Date: 2008/09/11 19:40:29 $"

#include <serial.h>
#include <seascan.h>

/* function prototypes */
int CtdAssertModePin(void);
int CtdAssertWakePin(void);
int CtdAssertTxPin(void);
int CtdClearModePin(void);
int CtdClearTxPin(void);
int CtdClearWakePin(void);
int CtdActiveIo(time_t timeout);
int CtdEnableIo(void);
int CtdDisableIo(void);
int CtdPower(unsigned char *VoltCnt, unsigned char *AmpCnt, time_t timeout);

/* declare the console serial port */
extern struct SerialPort ctdio;

#endif /* CTDIO_H */
