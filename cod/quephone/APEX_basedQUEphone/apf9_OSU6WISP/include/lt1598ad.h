#ifndef LT1598AD_H
#define LT1598AD_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: lt1598ad.c,v 1.6 2007/04/24 01:43:05 swift Exp $
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
 * This translation unit is an API to the APF9's LTC1598 AD convertor and
 * associated circuitry.  This API is based on the APF9 schematic (SeaBird
 * Document 32618, Rev A, Sheets 1,2,4,5) and on the data sheet for the
 * Linear Technology LTC1598 12-bit AD convertor.
 *
 * \begin{verbatim}
 * $Log: lt1598ad.c,v $
 * Revision 1.6  2007/04/24 01:43:05  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.5  2006/04/21 13:45:38  swift
 * Changed copyright attribute.
 *
 * Revision 1.4  2004/12/29 23:04:36  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.3  2004/04/14 20:48:21  swift
 * Revision E of the APF9 made some hardware changes to earlier versions.  The
 * PSD935 was replaced by a PSD835 and the 20mA serial loop was changed to be a
 * real serial IO port.
 *
 * Revision 1.2  2003/11/20 18:59:35  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.1  2003/11/12 22:30:48  swift
 * Initial revision
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define apf9adChangeLog "$RCSfile: lt1598ad.c,v $ $Revision: 1.6 $ $Date: 2007/04/24 01:43:05 $"

/* function prototypes */
float         Amps(unsigned char AmpsAd8);
float         BatAmps(void);
unsigned char BatAmpsAd8(void);
float         BatVolts(void);
unsigned char BatVoltsAd8(void);
float         Barometer(void);
unsigned char BarometerAd8(void);
float         Com1Amps(void);
unsigned char Com1AmpsAd8(void);
float         Com2Amps(void);
unsigned char Com2AmpsAd8(void);
float         inHg(unsigned char inHgAd8);
unsigned char PistonPosition(void);
float         Volts(unsigned char VoltsAd8);

#endif /* LT1598AD_H */
