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

/* prototypes for static functions */
static unsigned char CrcScrambler(unsigned char byte);
static unsigned char CrcDorson(const unsigned char *msg, unsigned int n);

/*========================================================================*/
/* function to compute the CRC for argos messages of arbitrary length     */
/*========================================================================*/
/**
   This function computes the CRC for argos messages of arbitrary length.
   Don Dorson's CRC algoritm is used. 

      \begin{verbatim}
      input:
         msg ... An array of bytes each of which can take the range
                 0x00 - 0xff.  The first element of the array is the CRC
                 byte which is computed from the remaining n-1 elements.

         n ..... The number of bytes in the message including the CRC byte.
         
      output:
         This function returns the CRC of the last n-1 bytes.
      \end{verbatim}
 */
int Crc(const unsigned char *msg, unsigned int n)
{
   return CrcDorson(msg,n);
}

/*========================================================================*/
/* function to check if a message is corrupted or not                     */
/*========================================================================*/
/*
   This function uses Don Dorson's CRC algorithm to verify correct
   transmission of data to the ARGOS system.

      \begin{verbatim}
      input:
         msg ... An array of bytes each of which can take the range
                 0x00 - 0xff.  The first element of the array is the CRC
                 byte which is computed from the remaining n-1 elements.

         n ..... The number of bytes in the message including the CRC byte.
         
      output:
         This function returns true if the CRC calculation matches the value
         of the first byte.  If the CRC check fails then this function
         returns false.
      \end{verbatim}
         
   written by Dana Swift
*/
int CrcOk(const unsigned char *msg, unsigned int n)
{
   return (CrcDorson(msg,n)==msg[0]) ? 1 : 0;
}

/*========================================================================*/
/* function to implement Don Dorson's CRC algorithm                       */
/*========================================================================*/
/*
   This function implements Don Dorson's CRC algorithm to compute an 8-bit
   CRC for the last 31 bytes of a 32 byte message.

      \begin{verbatim}
      input:
         msg ... An array of n bytes each of which can take the range
                 0x00 - 0xff.  The first element of the array is the CRC
                 byte which is computed from the remaining n-1 elements.

      output:
         This function returns the 8-bit CRC as given by Don Dorson's
         algorithm.
      \end{verbatim}

   written by Dana Swift
*/
static unsigned char CrcDorson(const unsigned char *msg, unsigned int n)
{
   unsigned char i,crc=CrcScrambler(msg[1]);

   for (i=2; i<n; i++)
   {
      crc ^= msg[i];
      crc  = CrcScrambler(crc);
   }
   
   return crc;
}

/*========================================================================*/
/* Random number generator used in Don Dorson's CRC check                 */
/*========================================================================*/
/*
   This function is a translation of the fortran function `randy' used
   in Don Dorson's CRC algorithm.  Don faxed me a copy of his CRC function
   (fortran version) which is given below:

   \begin{verbatim}
      subroutine randy(ichar)
      integer*2 ichar,itst,isum,two
      data two/2/
      isum=0
      if (ichar.eq.0) ichar=255
      itst=ichar
      if (mod(itst,two).eq.1) isum=isum+1
      itst=itst/4
      if (mod(itst,two).eq.1) isum=isum+1
      itst=itst/2
      if (mod(itst,two).eq.1) isum=isum+1
      itst=itst/2
      if (mod(itst,two).eq.1) isum=isum+1
      isum=mod(isum,two)
      ichar=ichar/2 + isum*128
      return
      end
   \end{verbatim}

   I'm not clear on what the principle is behind the algorithm used above or
   what this function is supposed to do in theory.  The name appears to
   suggest that a random number is to be generated based on the argument
   passed to the subroutine.  However, as the table below clearly shows, the
   algorithm performs poorly in this regard.  Each triplet in the table
   below maps a 2 byte input to a 2 byte output.  The first element of the
   triplet is the input expressed as a hexidecimal number, the second &
   third elements are the output expressed in hexidecimal & decimal, resp.
   Hence, the each of the triplets below have the form: (hex-input,hex-output,dec-output).

   \begin{verbatim}
   (00,7f,127)   (2b,15, 21)   (56,2b, 43)   (81,c0,192)   (ac,56, 86)   (d7,eb,235)
   (01,80,128)   (2c,16, 22)   (57,ab,171)   (82,41, 65)   (ad,d6,214)   (d8,6c,108)
   (02,01,  1)   (2d,96,150)   (58,2c, 44)   (83,c1,193)   (ae,57, 87)   (d9,ec,236)
   (03,81,129)   (2e,17, 23)   (59,ac,172)   (84,c2,194)   (af,d7,215)   (da,6d,109)
   (04,82,130)   (2f,97,151)   (5a,2d, 45)   (85,42, 66)   (b0,d8,216)   (db,ed,237)
   (05,02,  2)   (30,98,152)   (5b,ad,173)   (86,c3,195)   (b1,58, 88)   (dc,ee,238)
   (06,83,131)   (31,18, 24)   (5c,ae,174)   (87,43, 67)   (b2,d9,217)   (dd,6e,110)
   (07,03,  3)   (32,99,153)   (5d,2e, 46)   (88,c4,196)   (b3,59, 89)   (de,ef,239)
   (08,84,132)   (33,19, 25)   (5e,af,175)   (89,44, 68)   (b4,5a, 90)   (df,6f,111)
   (09,04,  4)   (34,1a, 26)   (5f,2f, 47)   (8a,c5,197)   (b5,da,218)   (e0,70,112)
   (0a,85,133)   (35,9a,154)   (60,30, 48)   (8b,45, 69)   (b6,5b, 91)   (e1,f0,240)
   (0b,05,  5)   (36,1b, 27)   (61,b0,176)   (8c,46, 70)   (b7,db,219)   (e2,71,113)
   (0c,06,  6)   (37,9b,155)   (62,31, 49)   (8d,c6,198)   (b8,5c, 92)   (e3,f1,241)
   (0d,86,134)   (38,1c, 28)   (63,b1,177)   (8e,47, 71)   (b9,dc,220)   (e4,f2,242)
   (0e,07,  7)   (39,9c,156)   (64,b2,178)   (8f,c7,199)   (ba,5d, 93)   (e5,72,114)
   (0f,87,135)   (3a,1d, 29)   (65,32, 50)   (90,c8,200)   (bb,dd,221)   (e6,f3,243)
   (10,88,136)   (3b,9d,157)   (66,b3,179)   (91,48, 72)   (bc,de,222)   (e7,73,115)
   (11,08,  8)   (3c,9e,158)   (67,33, 51)   (92,c9,201)   (bd,5e, 94)   (e8,f4,244)
   (12,89,137)   (3d,1e, 30)   (68,b4,180)   (93,49, 73)   (be,df,223)   (e9,74,116)
   (13,09,  9)   (3e,9f,159)   (69,34, 52)   (94,4a, 74)   (bf,5f, 95)   (ea,f5,245)
   (14,0a, 10)   (3f,1f, 31)   (6a,b5,181)   (95,ca,202)   (c0,60, 96)   (eb,75,117)
   (15,8a,138)   (40,20, 32)   (6b,35, 53)   (96,4b, 75)   (c1,e0,224)   (ec,76,118)
   (16,0b, 11)   (41,a0,160)   (6c,36, 54)   (97,cb,203)   (c2,61, 97)   (ed,f6,246)
   (17,8b,139)   (42,21, 33)   (6d,b6,182)   (98,4c, 76)   (c3,e1,225)   (ee,77,119)
   (18,0c, 12)   (43,a1,161)   (6e,37, 55)   (99,cc,204)   (c4,e2,226)   (ef,f7,247)
   (19,8c,140)   (44,a2,162)   (6f,b7,183)   (9a,4d, 77)   (c5,62, 98)   (f0,f8,248)
   (1a,0d, 13)   (45,22, 34)   (70,b8,184)   (9b,cd,205)   (c6,e3,227)   (f1,78,120)
   (1b,8d,141)   (46,a3,163)   (71,38, 56)   (9c,ce,206)   (c7,63, 99)   (f2,f9,249)
   (1c,8e,142)   (47,23, 35)   (72,b9,185)   (9d,4e, 78)   (c8,e4,228)   (f3,79,121)
   (1d,0e, 14)   (48,a4,164)   (73,39, 57)   (9e,cf,207)   (c9,64,100)   (f4,7a,122)
   (1e,8f,143)   (49,24, 36)   (74,3a, 58)   (9f,4f, 79)   (ca,e5,229)   (f5,fa,250)
   (1f,0f, 15)   (4a,a5,165)   (75,ba,186)   (a0,50, 80)   (cb,65,101)   (f6,7b,123)
   (20,10, 16)   (4b,25, 37)   (76,3b, 59)   (a1,d0,208)   (cc,66,102)   (f7,fb,251)
   (21,90,144)   (4c,26, 38)   (77,bb,187)   (a2,51, 81)   (cd,e6,230)   (f8,7c,124)
   (22,11, 17)   (4d,a6,166)   (78,3c, 60)   (a3,d1,209)   (ce,67,103)   (f9,fc,252)
   (23,91,145)   (4e,27, 39)   (79,bc,188)   (a4,d2,210)   (cf,e7,231)   (fa,7d,125)
   (24,92,146)   (4f,a7,167)   (7a,3d, 61)   (a5,52, 82)   (d0,e8,232)   (fb,fd,253)
   (25,12, 18)   (50,a8,168)   (7b,bd,189)   (a6,d3,211)   (d1,68,104)   (fc,fe,254)
   (26,93,147)   (51,28, 40)   (7c,be,190)   (a7,53, 83)   (d2,e9,233)   (fd,7e,126)
   (27,13, 19)   (52,a9,169)   (7d,3e, 62)   (a8,d4,212)   (d3,69,105)   (fe,ff,255)
   (28,94,148)   (53,29, 41)   (7e,bf,191)   (a9,54, 84)   (d4,6a,106)   (ff,7f,127)
   (29,14, 20)   (54,2a, 42)   (7f,3f, 63)   (aa,d5,213)   (d5,ea,234)
   (2a,95,149)   (55,aa,170)   (80,40, 64)   (ab,55, 85)   (d6,6b,107)
   \end{verbatim}

   The table above was sorted by output to produce an inverse mapping as
   shown in the table below.  As in the table above each of the triplets
   below have the form: (hex-input,hex-output,dec-output).
   
   \begin{verbatim}
   (02,01,  1)   (58,2c, 44)   (ae,57, 87)   (03,81,129)   (59,ac,172)   (af,d7,215)
   (05,02,  2)   (5a,2d, 45)   (b1,58, 88)   (04,82,130)   (5b,ad,173)   (b0,d8,216)
   (07,03,  3)   (5d,2e, 46)   (b3,59, 89)   (06,83,131)   (5c,ae,174)   (b2,d9,217)
   (09,04,  4)   (5f,2f, 47)   (b4,5a, 90)   (08,84,132)   (5e,af,175)   (b5,da,218)
   (0b,05,  5)   (60,30, 48)   (b6,5b, 91)   (0a,85,133)   (61,b0,176)   (b7,db,219)
   (0c,06,  6)   (62,31, 49)   (b8,5c, 92)   (0d,86,134)   (63,b1,177)   (b9,dc,220)
   (0e,07,  7)   (65,32, 50)   (ba,5d, 93)   (0f,87,135)   (64,b2,178)   (bb,dd,221)
   (11,08,  8)   (67,33, 51)   (bd,5e, 94)   (10,88,136)   (66,b3,179)   (bc,de,222)
   (13,09,  9)   (69,34, 52)   (bf,5f, 95)   (12,89,137)   (68,b4,180)   (be,df,223)
   (14,0a, 10)   (6b,35, 53)   (c0,60, 96)   (15,8a,138)   (6a,b5,181)   (c1,e0,224)
   (16,0b, 11)   (6c,36, 54)   (c2,61, 97)   (17,8b,139)   (6d,b6,182)   (c3,e1,225)
   (18,0c, 12)   (6e,37, 55)   (c5,62, 98)   (19,8c,140)   (6f,b7,183)   (c4,e2,226)
   (1a,0d, 13)   (71,38, 56)   (c7,63, 99)   (1b,8d,141)   (70,b8,184)   (c6,e3,227)
   (1d,0e, 14)   (73,39, 57)   (c9,64,100)   (1c,8e,142)   (72,b9,185)   (c8,e4,228)
   (1f,0f, 15)   (74,3a, 58)   (cb,65,101)   (1e,8f,143)   (75,ba,186)   (ca,e5,229)
   (20,10, 16)   (76,3b, 59)   (cc,66,102)   (21,90,144)   (77,bb,187)   (cd,e6,230)
   (22,11, 17)   (78,3c, 60)   (ce,67,103)   (23,91,145)   (79,bc,188)   (cf,e7,231)
   (25,12, 18)   (7a,3d, 61)   (d1,68,104)   (24,92,146)   (7b,bd,189)   (d0,e8,232)
   (27,13, 19)   (7d,3e, 62)   (d3,69,105)   (26,93,147)   (7c,be,190)   (d2,e9,233)
   (29,14, 20)   (7f,3f, 63)   (d4,6a,106)   (28,94,148)   (7e,bf,191)   (d5,ea,234)
   (2b,15, 21)   (80,40, 64)   (d6,6b,107)   (2a,95,149)   (81,c0,192)   (d7,eb,235)
   (2c,16, 22)   (82,41, 65)   (d8,6c,108)   (2d,96,150)   (83,c1,193)   (d9,ec,236)
   (2e,17, 23)   (85,42, 66)   (da,6d,109)   (2f,97,151)   (84,c2,194)   (db,ed,237)
   (31,18, 24)   (87,43, 67)   (dd,6e,110)   (30,98,152)   (86,c3,195)   (dc,ee,238)
   (33,19, 25)   (89,44, 68)   (df,6f,111)   (32,99,153)   (88,c4,196)   (de,ef,239)
   (34,1a, 26)   (8b,45, 69)   (e0,70,112)   (35,9a,154)   (8a,c5,197)   (e1,f0,240)
   (36,1b, 27)   (8c,46, 70)   (e2,71,113)   (37,9b,155)   (8d,c6,198)   (e3,f1,241)
   (38,1c, 28)   (8e,47, 71)   (e5,72,114)   (39,9c,156)   (8f,c7,199)   (e4,f2,242)
   (3a,1d, 29)   (91,48, 72)   (e7,73,115)   (3b,9d,157)   (90,c8,200)   (e6,f3,243)
   (3d,1e, 30)   (93,49, 73)   (e9,74,116)   (3c,9e,158)   (92,c9,201)   (e8,f4,244)
   (3f,1f, 31)   (94,4a, 74)   (eb,75,117)   (3e,9f,159)   (95,ca,202)   (ea,f5,245)
   (40,20, 32)   (96,4b, 75)   (ec,76,118)   (41,a0,160)   (97,cb,203)   (ed,f6,246)
   (42,21, 33)   (98,4c, 76)   (ee,77,119)   (43,a1,161)   (99,cc,204)   (ef,f7,247)
   (45,22, 34)   (9a,4d, 77)   (f1,78,120)   (44,a2,162)   (9b,cd,205)   (f0,f8,248)
   (47,23, 35)   (9d,4e, 78)   (f3,79,121)   (46,a3,163)   (9c,ce,206)   (f2,f9,249)
   (49,24, 36)   (9f,4f, 79)   (f4,7a,122)   (48,a4,164)   (9e,cf,207)   (f5,fa,250)
   (4b,25, 37)   (a0,50, 80)   (f6,7b,123)   (4a,a5,165)   (a1,d0,208)   (f7,fb,251)
   (4c,26, 38)   (a2,51, 81)   (f8,7c,124)   (4d,a6,166)   (a3,d1,209)   (f9,fc,252)
   (4e,27, 39)   (a5,52, 82)   (fa,7d,125)   (4f,a7,167)   (a4,d2,210)   (fb,fd,253)
   (51,28, 40)   (a7,53, 83)   (fd,7e,126)   (50,a8,168)   (a6,d3,211)   (fc,fe,254)
   (53,29, 41)   (a9,54, 84)   (ff,7f,127)   (52,a9,169)   (a8,d4,212)   (fe,ff,255)
   (54,2a, 42)   (ab,55, 85)   (00,7f,127)   (55,aa,170)   (aa,d5,213)
   (56,2b, 43)   (ac,56, 86)   (01,80,128)   (57,ab,171)   (ad,d6,214)
   \end{verbatim}

   The above table exhibits 2 important characteristics.  The algorithm does
   not exhibit the essentials of a random distribution.  Also both 0x00 and
   0xff are mapped to 0x7f by this algorithm.
   
   written by Dana Swift
*/
static unsigned char CrcScrambler(unsigned char byte)
{
   unsigned char sum=0,tst;

   if (!byte) byte = 0xff;

   tst = byte; if (tst % 2) sum++;
   tst >>= 2; if (tst % 2) sum++;
   tst >>= 1; if (tst % 2) sum++;
   tst >>= 1; if (tst % 2) sum++;

   sum %= 2;

   return (byte>>1) + (sum<<7);
}
