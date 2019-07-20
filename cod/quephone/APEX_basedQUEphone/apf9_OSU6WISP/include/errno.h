#ifndef ERRNO_H
#define ERRNO_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: errno.c,v 1.4 2007/04/24 01:43:05 swift Exp $
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
 * $Log: errno.c,v $
 * Revision 1.4  2007/04/24 01:43:05  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.3  2004/07/14 22:47:22  swift
 * Modifications for the new stdio library.
 *
 * Revision 1.1  2003/07/16 23:45:01  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define errnoChangeLog "$RCSfile: errno.c,v $ $Revision: 1.4 $ $Date: 2007/04/24 01:43:05 $"

#define EOK       (0)   /* normal */
#define EINVAL    (1)
#define ENOENT    (2)  /* nonexistent entry */
#define NOPATH    (3)
#define EMFILE    (4)  /* too many files open */
#define EACCESS   (5)
#define EBADF     (6)
#define EMCBD     (7)
#define ENOMEM    (8)
#define EIMBA     (9)
#define EINVENV   (10)
#define ENOEXEC   (11)
#define EPERM     (12)  /* permission denied */
#define EDATA     (13)
#define EDRIVE    (15)
#define ECURDIR   (16)
#define EXDEV     (17)
#define ENFILE    (18)
#define ERANGE    (19)
#define ENULLARG  (20)  /* null function argument */
#define EFNAME    (21)  /* illegal filename */
#define EMODE     (22)  /* illegal mode */
#define ENIMP     (23)  /* not implemented */
#define EBUSY     (24)  /* file is busy */
#define EFULL     (25)  /* file buffer is full */

extern int errno;

#endif /* ERRNO_H */
