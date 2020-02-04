#ifndef APF9ICOM_H
#define APF9ICOM_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: apf9icom.c,v 1.15 2007/04/24 01:43:05 swift Exp $
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
 * $Log: apf9icom.c,v $
 * Revision 1.15  2007/04/24 01:43:05  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.14  2006/04/21 13:45:38  swift
 * Changed copyright attribute.
 *
 * Revision 1.13  2006/03/24 23:37:55  swift
 * Eliminated conditional 3s pause in favor of unconditional 3 sec pause after
 * enable.
 *
 * Revision 1.12  2006/03/24 23:05:29  swift
 * Eliminated firmware kludge to fix nonresponsive LBT9522a modems.  This
 * problem was solved correctly with a hardware fix.
 *
 * Revision 1.11  2006/03/15 15:59:57  swift
 * Eliminate persistent trait of DUART_OPR variable.
 *
 * Revision 1.10  2006/02/28 16:35:48  swift
 * Implemented a kludge to fix nonresponsive LBT9522a modems.  After a hardware
 * fix is implemented and tested, this kludge should be eliminated.
 *
 * Revision 1.9  2006/01/16 18:34:31  swift
 * Fixed problems with implementation of na() and na_().
 *
 * Revision 1.8  2006/01/06 23:08:54  swift
 * Added dtr(), and dsr() primitives to the SerialPort interface for the modem port.
 *
 * Revision 1.7  2005/07/07 19:45:04  swift
 * Assert the DTR line when the iridum module is powered up.
 *
 * Revision 1.6  2005/06/14 18:57:19  swift
 * Modifications to initialize low-level UART hardware before VALID line is active.
 *
 * Revision 1.5  2005/02/22 21:56:51  swift
 * Clean-up and fix-up of GPS serial port code.
 *
 * Revision 1.4  2005/01/15 00:52:03  swift
 * Added a time delay of 500ms to GpsEnable() to allow time for the GPS to boot
 * up.
 *
 * Revision 1.3  2005/01/06 00:51:13  swift
 * Fixed bugs in DTR/DSR control.
 *
 * Revision 1.2  2004/12/30 17:09:20  swift
 * Added functions to manipulate pin 7 of the GPS port and pins 2,3 of the RF
 * switch port.
 *
 * Revision 1.1  2004/12/29 23:03:32  swift
 * Module for Iridium and GPS serial ports.
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define apf9icomChangeLog "$RCSfile: apf9icom.c,v $ $Revision: 1.15 $ $Date: 2007/04/24 01:43:05 $"

#include <serial.h>

/* prototypes for functions with external linkage */
void   GpsDisable(void);
void   GpsEnable(unsigned int BaudRate);
size_t GpsFifoLen(void);
size_t GpsFifoSize(void);
int    GpsPin7Assert(void);
int    GpsPin7Clear(void);
void   InitDUart1(void);
int    ModemCd(void);
void   ModemDisable(void);
int    ModemDsr(void);
int    ModemDtrAssert(void);
int    ModemDtrClear(void);
void   ModemEnable(unsigned int BaudRate);
size_t ModemFifoLen(void);
size_t ModemFifoSize(void);
int    RfSwitchPin2Assert(void);
int    RfSwitchPin2Clear(void);
int    RfSwitchPin3Assert(void);
int    RfSwitchPin3Clear(void);

/* extern references to expansion serial ports */
extern const struct SerialPort modem,gps;

#endif /* APF9ICOM_H */
