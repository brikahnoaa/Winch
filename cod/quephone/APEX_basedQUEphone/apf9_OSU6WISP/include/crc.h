#ifndef CRC_H
#define CRC_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: crc.c,v 1.8 2007/04/24 01:43:29 swift Exp $
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
 * $Log: crc.c,v $
 * Revision 1.8  2007/04/24 01:43:29  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.7  2006/04/21 13:45:40  swift
 * Changed copyright attribute.
 *
 * Revision 1.6  2003/11/20 18:59:42  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.5  2003/09/10 17:58:15  swift
 * Added Crc() and comments.
 *
 * Revision 1.4  2002/08/15 17:59:16  swift
 * Modifications to allow for 28-bit argos ids.
 *
 * Revision 1.3  1997/08/24 12:57:17  swift
 * Modified & added some comments.
 *
 * Revision 1.2  1997/08/24 12:53:43  swift
 * Split the CRC function into 2 functions...one that computes the
 * CRC and the other that validates it.
 *
 * Revision 1.1  1997/07/31 23:45:21  swift
 * Initial revision
 * \end{verbatim}
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define crcChangeLog "$RCSfile: crc.c,v $ $Revision: 1.8 $ $Date"

/* function prototypes */
int CrcOk(const unsigned char *msg, unsigned int n);
int Crc(const unsigned char *msg, unsigned int n);

#endif /* CRC_H */
