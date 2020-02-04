#ifndef LOGIN_H
#define LOGIN_H
//Project ID is QUEH and the following 4 letters are platform ID.
//Changed the login procedure (line 219-244) and timing to received ACK.
//HM
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: login.c,v 1.2 2007/04/24 01:43:29 swift Exp $
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
 * $Log: login.c,v $
 * Password is now platform ID. Change it by i*PQ001 command, which makes 
 * platform ID to Q001. HM Jan 11, 2010
 * 
 * Login changed heavily to incorporate the NOAA buoy-Rudics protocol HM, LM 10/1/2009
 *
 * Revision 1.2  2007/04/24 01:43:29  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.1  2004/12/29 23:11:27  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define loginChangeLog "$RCSfile: login.c,v $  $Revision: 1.2 $   $Date: 2007/04/24 01:43:29 $"
//#define SYSNAME "QUEHQ002"  //HM

#include <serial.h>

/* function prototypes for external functions */
int login(const struct SerialPort *port, const char *user, const char *pwd);
int logout(const struct SerialPort *port);
int Calc_Crc(unsigned char *buf, int cnt);
void itoa(int a, unsigned char *b);

#endif /* LOGIN_H */
