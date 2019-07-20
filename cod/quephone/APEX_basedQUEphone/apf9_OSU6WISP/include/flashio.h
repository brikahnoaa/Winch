#ifndef FLASHIO_H
#define FLASHIO_H (0x1000U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: flashio.c,v 1.2 2007/04/24 01:43:05 swift Exp $
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
 * $Log: flashio.c,v $
 * Revision 1.2  2007/04/24 01:43:05  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.1  2006/10/11 23:35:12  swift
 * Implement new flashio file system.
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define flashioChangeLog "$RCSfile: flashio.c,v $ $Revision: 1.2 $ $Date: 2007/04/24 01:43:05 $"

#include <stdio.h>

#define KB            (1L<<10)
#define MB            (1L<<20)
#define FLASHSIZE     (8*MB)
#define FILESIZE      (64*KB)
#define BLOCKSIZE     (8*KB)
#define PAGESIZE      (512U)
#define MAXFILES      (FLASHSIZE/FILESIZE)
#define BUFSIZE       (FILESIZE-PAGESIZE)
#define NFIOBLK       (NFIO)

#define BUSY   (-6)
#define FEMPTY (-5)
#define BADLEN (-4)
#define WFAIL  (-3)
#define RFAIL  (-2)
#define BADARG (-1)
#define FAIL    (0)
#define FTRUNC  (0)
#define OK      (1)

/* define the structure for file control blocks */
typedef far struct
{
   int    fid;
   size_t fpos;
   size_t len;
   char   fname[FILENAME_MAX+1];
   unsigned char buf[BUFSIZE];
} FioBlk;

/* define file control blocks */
extern persistent FioBlk fioblk_[NFIOBLK];

/* function prototypes */
int    fidnext(void);
int    fioFind(const char *fname);
int    fioFormat(void);
int    fioInit(void);
int    fioGet(int fid,FioBlk *fioblk);
size_t fioLen(int fid);
int    fioName(int fid,char *fname,size_t bufsize);
int    fioPut(const FioBlk *fioblk);
int    fioRename(int fid,const char *fname);
int    fioUnlink(int fid);
int    fioWipe(int fid);

#endif /* FLASHIO_H */
