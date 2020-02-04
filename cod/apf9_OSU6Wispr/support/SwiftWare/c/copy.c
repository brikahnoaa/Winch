/****************************************/
/*     member of utils library          */
/****************************************/
#include <defs.p>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: copy.c,v 1.1.1.1 2003/11/12 23:22:58 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: copy.c,v $
 * Revision 1.1.1.1  2003/11/12 23:22:58  swift
 * Apf9 firmware for a 260ml Apex with SBE41 CTD.
 *
 * Revision 1.2  1998/10/09 16:57:14  swift
 * Correct misspelling of `usage'.
 *
 * Revision 1.1  1996/06/15 15:47:07  swift
 * Initial revision
 *
 *========================================================================*/
/*   function to copy a substring from source string to dest string       */
/*========================================================================*/
/*
   This function copies n characters from the source string starting
   at the index(th) position into the destination string "dest".  If
   either index<=0 or n=0 then the function returns the null character
   If (n<0) then the validity check which ensures that the value of
   index is less than the string length is suppressed...this saves time
   since calculation of the string length is not required but opens up
   the possibility that garbage is returned if the user is not careful.
   This function terminates copying and returns to the calling function
   if the null character is encountered in the  source string.

      Proper usage: character_pointer = copy(source,index,n,dest).
*/
char *copy(char *source,int index,int n,char *dest)
{
   #define INFINITE (32767)
   int i,len;


   /* setting n<0 suppresses time-consuming calculation of string length */
   if (n<0) {len=INFINITE; n *= -1;}
   else len = (index<=1) ? INFINITE : strlen(source);

   if (!source || !dest || !(*source) || index>len || index<1 || !n)
   {
      if (dest) dest[0] = 0;
      return(NULL);
   }

   for (i=0; i<n && i<MAXSTRLEN; i++)
   {
      *(dest+i) = *(source+index+i-1);
      if (!(*(dest+i))) break;
   }
   *(dest+i)=0;

   return(dest);

   #undef INFINITE
}

