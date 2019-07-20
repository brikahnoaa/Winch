#ifndef LBT9522_H
#define LBT9522_H (0x0100U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: lbt9522.c,v 1.23.2.1 2008/09/11 20:03:35 dbliudnikas Exp $
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
 * $Log: lbt9522.c,v $
 * Revision 1.23.2.1  2008/09/11 20:03:35  dbliudnikas
 * Update for Apf9i test setup with U.S.Robitic modem, so does not fail on mission start.
 *
 * Added Wait in IrModemRegister line 913. HM Jan 11, 2010.
 * Revision 1.23  2008/07/14 16:56:20  swift
 * Fixed a bug in the function to query the LBT for signal strength.
 *
 * Revision 1.22  2007/06/08 02:14:30  swift
 * Added a logentry to IrModemRegister() that warns when the modem is not a
 * 9522A.  Added delays to resolve a race condition.
 *
 * Revision 1.21  2007/05/09 19:12:28  swift
 * Added a logentry to IrModemRegister() that warns when the modem is not a 9522A.
 *
 * Revision 1.20  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.19  2007/03/26 20:47:22  swift
 * Implemented meachanism for storing and retrieving ICCID/MSISDN numbers on
 * SIM cards.
 *
 * Revision 1.18  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.17  2006/05/15 18:54:49  swift
 * Changed the AT command to set the fixed baud rate at 19200.  The old AT spec
 * was incomplete and did not correctly describe the AT+IPR command.
 *
 * Revision 1.16  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.15  2006/03/24 23:39:02  swift
 * Eliminated IrModemDefibrillate() because the LBT9522a nonresponse problem
 * was found and fixed.
 *
 * Revision 1.14  2006/02/28 16:34:23  swift
 * Eliminated the modem defibrillation kludge because the fundamental problem
 * with nonresponsive LBT9522a modems was found and fixed.
 *
 * Revision 1.13  2006/02/22 21:53:29  swift
 * Added functions to query the LBT for firmware revision, model, and IMEI number.
 *
 * Revision 1.12  2006/02/03 00:22:52  swift
 * Fixed a buffer over-run bug in use of pgets().
 *
 * Revision 1.11  2006/01/31 19:25:52  swift
 * Added LBT9522 debibrillation kludge to prevent nonresponsive modems.
 *
 * Revision 1.10  2005/07/12 18:18:29  swift
 * Minor rework of power-cycling of LBT.
 *
 * Revision 1.9  2005/07/07 19:46:19  swift
 * Changed handling of the DTR signal to work around problems with the LBT9522.
 *
 * Revision 1.8  2005/07/07 15:08:13  swift
 * Fixed a bug caused by 1-second time descretization.
 *
 * Revision 1.7  2005/06/27 17:05:07  swift
 * Replaced references to 'Motorola IRIDIUM' to 'IRIDIUM' to account for
 * changes in manufacturer of the Daytona.
 *
 * Revision 1.6  2005/06/21 14:11:44  swift
 * Reduced power-cycling wait periods to 1 second.
 *
 * Revision 1.5  2005/06/14 19:02:44  swift
 * Added features to configure the LBT.
 *
 * Revision 1.4  2005/05/01 14:30:43  swift
 * Minor adjustments to LBT power-up bug work-around.
 *
 * Revision 1.3  2005/04/30 17:33:28  swift
 * Improvements to the work-around for the power-up bug of Sebring and Daytona LBTs.
 *
 * Revision 1.2  2005/04/29 19:39:14  swift
 * Implemented power-cycling work-around for LBT9522 no-response bug.
 *
 * Revision 1.1  2005/02/22 21:43:07  swift
 * Eliminated use of Motorola 9500 phone in favor of LBT9522.
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define lbt9522ChangeLog "$RCSfile: lbt9522.c,v $  $Revision: 1.23.2.1 $   $Date: 2008/09/11 20:03:35 $"

#include <serial.h>

int IrModemAttention(const struct SerialPort *port);
int IrModemConfigure(const struct SerialPort *port);
int IrModemFwRev(const struct SerialPort *port, char *FwRev, size_t size);
int IrModemIccid(const struct SerialPort *port,char *iccid,char *msisdn,size_t size);
int IrModemIccidSet(const struct SerialPort *port,const char *iccid,const char *msisdn);
int IrModemImei(const struct SerialPort *port, char *imei, size_t size);
int IrModemModel(const struct SerialPort *port, char *model, size_t size);
int IrModemRegister(const struct SerialPort *port);
int IrSignalStrength(const struct SerialPort *port);
int IrSkySearch(const struct SerialPort *port);

#endif /* LBT9522_H */
