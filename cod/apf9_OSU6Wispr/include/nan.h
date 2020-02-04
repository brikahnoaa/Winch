#ifndef NAN_H
#define NAN_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: nan.c,v 1.4 2007/04/24 01:43:29 swift Exp $
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
/**
   \begin{verbatim}
   IEEE representation of single precision floating point variables
   ----------------------------------------------------------------
   
   The IEEE single precision floating point standard representation requires
   a 32 bit word, which may be represented as numbered from 0 to 31, left
   to right.  The first bit is the sign bit, S, the next eight bits are the
   exponent bits, 'E', and the final 23 bits are the fraction 'F':<p>
   
     S EEEEEEEE FFFFFFFFFFFFFFFFFFFFFFF
     0 1      8 9                    31
   
   The value V represented by the word may be determined as follows:
   
     1) If E=255 and F is nonzero, then V=NaN  ("Not a number").
     
     2) If E=255 and F is zero and S is 1, then V=-Infinity.
     
     3) If E=255 and F is zero and S is 0, then V=Infinity.
     
     4) If 0 < E < 255 then V=(-1)**S * 2 ** (E-127) * (1.F)
        where "1.F" is intended to represent the binary number created by
        prefixing F with an implicit leading 1 and a binary point.
        
     5) If E=0 and F is nonzero, then V=(-1)**S * 2 ** (-126) * (0.F)
        These are "unnormalized" values.
   
     6) If E=0 and F is zero and S is 1, then V=-0.
   
     7) If E=0 and F is zero and S is 0, then V=0.
   
   In particular,
   
     0 00000000 00000000000000000000000 = 0
     1 00000000 00000000000000000000000 = -0
   
     0 11111111 00000000000000000000000 = Infinity
     1 11111111 00000000000000000000000 = -Infinity
   
     0 11111111 00000100000000000000000 = NaN
     1 11111111 00100010001001010101010 = NaN
   
     0 10000000 00000000000000000000000 = +1 * 2**(128-127) * 1.0 = 2
     0 10000001 10100000000000000000000 = +1 * 2**(129-127) * 1.101 = 6.5
     1 10000001 10100000000000000000000 = -1 * 2**(129-127) * 1.101 = -6.5
   
     0 00000001 00000000000000000000000 = +1 * 2**(1-127) * 1.0 = 2**(-126)
     0 00000000 10000000000000000000000 = +1 * 2**(-126) * 0.1 = 2**(-127) 
     0 00000000 00000000000000000000001 = +1 * 2**(-126) * 0.00000000000000000000001 = 
                                          2**(-149)  (Smallest positive value)
 * \end{verbatim}
 *
 * RCS log of revisions to the C source code.
 *
 * \begin{verbatim}
 * $Log: nan.c,v $
 * Revision 1.4  2007/04/24 01:43:29  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.3  2006/04/21 13:45:40  swift
 * Changed copyright attribute.
 *
 * Revision 1.2  2003/11/20 18:59:42  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.1  2003/07/07 20:50:06  swift
 * Initial revision
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* function prototypes */
float nan(void);
int isnan(float f);
int isinf(float f);
int finite(float f);

#endif /* NAN_H */
