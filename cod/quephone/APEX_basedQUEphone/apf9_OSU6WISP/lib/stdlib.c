/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: stdlib.c,v 1.5 2007/04/24 01:43:29 swift Exp $
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
 * $Log: stdlib.c,v $
 * Revision 1.5  2007/04/24 01:43:29  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.4  2006/04/21 13:45:40  swift
 * Changed copyright attribute.
 *
 * Revision 1.3  2005/02/22 21:46:05  swift
 * Modifcations to abort() to prevent APF9 from being put to sleep permanently.
 *
 * Revision 1.2  2003/11/20 18:59:42  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.1  2003/06/07 20:41:25  swift
 * Initial revision
 *
 * \end{verbatim}
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define stdlibChangeLog "$RCSfile: stdlib.c,v $ $Revision: 1.5 $ $Date: 2007/04/24 01:43:29 $"

#include <stdlib.h>

#ifdef _XA_
   #include <apf9.h>
   #include <stdio.h>
#endif /* _XA_ */

/*------------------------------------------------------------------------*/
/* function to abort exectution                                           */
/*------------------------------------------------------------------------*/
void abort(void)
{
   #ifdef _XA_
      /* close all of the open files */
      fcloseall();

      /* shutdown the APF9 */
      Apf9PowerOff(0);
   #endif
      
   exit(1);
}

/*------------------------------------------------------------------------*/
/* function to return the absolute value of a long integer                */
/*------------------------------------------------------------------------*/
long int labs(long int a)
{
   return (a<0) ? -a : a;
}
