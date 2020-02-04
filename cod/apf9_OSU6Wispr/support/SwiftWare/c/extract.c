/****************************************/
/*     member of utils library          */
/****************************************/
#include <defs.p>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: extract.c,v 1.1.1.1 2003/11/12 23:22:58 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: extract.c,v $
 * Revision 1.1.1.1  2003/11/12 23:22:58  swift
 * Apf9 firmware for a 260ml Apex with SBE41 CTD.
 *
 * Revision 1.2  1998/10/09 16:57:14  swift
 * Correct misspelling of `usage'.
 *
 * Revision 1.1  1996/06/15 15:48:04  swift
 * Initial revision
 *
 *========================================================================*/
/*   function to extract a substring from source string                   */
/*========================================================================*/
/*
   This function extracts n characters from the source string starting
   at the index(th) position into a global character buffer "scrbuf".  If
   either index<=0 or n=0 then the function returns the null character
   If (n<0) then the validity check which ensures that the value of
   index is less than the string length is suppressed...this saves time
   since calculation of the string length is not required but opens up
   the possibility that garbage is returned if the user is not careful.
   This function terminates extraction and returns to the calling function
   if the null character is encountered in the  source string.

      Proper usage: character_pointer = extract(source,index,n,dest).
*/
char *extract(const char *source,int index,int n)
{
   #define INFINITE (32767)
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
