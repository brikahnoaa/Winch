#ifndef TC58V64_H
#define TC58V64_H (0x1000U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: tc58v64.c,v 1.2 2007/04/24 01:43:05 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
 * $Log: tc58v64.c,v $
 * Revision 1.2  2007/04/24 01:43:05  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.1  2006/10/11 23:40:10  swift
 * Low-level API to the TC58V64 flash memory chip.
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define tc58v64ChangeLog "$RCSfile: tc58v64.c,v $ $Revision: 1.2 $ $Date: 2007/04/24 01:43:05 $"

/* function prototypes */
unsigned int Tc58v64BadBlockCrc(void);
int          Tc58v64BadBlockListInit(void);
int          Tc58v64Busy(void);
int          Tc58v64Erase(unsigned int block);
void         Tc58v64GenEccTbl(void);
unsigned int Tc58v64Id(void);
int          Tc58v64Init(void);
int          Tc58v64IsBadBlock(unsigned int block);
int          Tc58v64PageGet(unsigned int block, unsigned char page, far unsigned char *buf);
int          Tc58v64PageGetNoEcc(unsigned int block, unsigned char page, far unsigned char *buf);
int          Tc58v64PagePut(unsigned int block, unsigned char page, far const unsigned char *buf);
void         Tc58v64Reset(void);
int          Tc58v64WriteConstant(unsigned int block,unsigned char page,unsigned char byte);

/* define the maximum number of bad blocks allowed */
#define Tc58v64BadBlockMax (20)

/* define a static global array to hold the bad-block ids */
extern persistent far unsigned int BadBlock[Tc58v64BadBlockMax];
extern persistent far unsigned int BadBlockCrc;

#endif /* TC58V64_H */
