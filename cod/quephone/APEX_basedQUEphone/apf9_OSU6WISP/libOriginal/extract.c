#ifndef EXTRACT_H
#define EXTRACT_H

char *extract(const char *source,int index,int n);

#endif /* EXTRACT_H */

#include <string.h>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: extract.c,v 1.7 2007/04/24 01:43:29 swift Exp $
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
/** RCS Log of revisions to the C source code.
 *
 * \begin{verbatim}
 * $Log: extract.c,v $
 * Revision 1.7  2007/04/24 01:43:29  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.6  2006/04/21 13:45:40  swift
 * Changed copyright attribute.
 *
 * Revision 1.5  2003/11/20 18:59:42  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.4  2002/10/08 23:32:30  swift
 * Modified the scratch buffer to be static and local.  Combined header file
 * into source file.
 *
 * Revision 1.3  2002/05/07 22:15:14  swift
 * Fixed usage string.
 *
 * Revision 1.2  1998/10/09 16:57:14  swift
 * Correct misspelling of `usage'.
 *
 * Revision 1.1  1996/06/15 15:48:04  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define ExtractrChangeLog "$RCSfile: extract.c,v $  $Revision: 1.7 $   $Date: 2007/04/24 01:43:29 $"

/*========================================================================*/
/*   function to extract a substring from source string                   */
/*========================================================================*/
/**
   This function extracts n characters from the source string starting
   at the index(th) position into a global character buffer "scrbuf".  If
   either index<=0 or n=0 then the function returns the null character
   If (n<0) then the validity check which ensures that the value of
   index is less than the string length is suppressed...this saves time
   since calculation of the string length is not required but opens up
   the possibility that garbage is returned if the user is not careful.
   This function terminates extraction and returns to the calling function
   if the null character is encountered in the  source string.

      Proper usage: character_pointer = extract(source,index,n)
*/
char *extract(const char *source,int index,int n)
{
   #define INFINITE (32767)
   #define MAXSTRLEN (4096)
   static char scrbuf[MAXSTRLEN+1]="";
   int i,len;
   
   /* setting n<0 suppresses time-consuming calculation of string length */
   if (n<0) {len=INFINITE; n *= -1;}
   else len = (index<=1) ? INFINITE : strlen(source);

   if (!source || !(*source) || index>len || index<1 || !n)
   {
      *scrbuf = 0;
      return(scrbuf);
   }

   for (i=0; i<n && i<MAXSTRLEN; i++)
   {
      *(scrbuf+i) = *(source+index+i-1);
      if (!(*(scrbuf+i))) break;
   }
   *(scrbuf+i)=0;

   return(scrbuf);

   #undef INFINITE
}
