#include <defs.p>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: strseek.c,v 1.1.1.1 2003/11/12 23:22:58 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: strseek.c,v $
 * Revision 1.1.1.1  2003/11/12 23:22:58  swift
 * Apf9 firmware for a 260ml Apex with SBE41 CTD.
 *
 * Revision 1.1  1996/06/15 15:55:16  swift
 * Initial revision
 *
 *========================================================================*/
/* function to search a stream for the next occurrence of a key string    */
/*========================================================================*/
/*
   This function is designed to search a FILE stream for the next occurrence
   of a key string.  If successful, the stream is positioned to the beginning
   of the key string.  If the key string is not found, the initial stream
   pointer position remains unaltered.  It is important to realize that this
   function does not reposition the stream to its beginning prior to initiating
   the search...the search begins where ever the stream pointer happens to be
   when this function is called.

   INPUT:

      key.....the string to be found within the stream.
      source..the FILE stream to be searched.

   OUTPUT:

      If the search for the key string is successfull, this function returns
      (1) and the stream pointer is positioned to the beginning of the key
      string within the stream.  If unsuccessfull, this function returns (0)
      and the stream pointer is unaltered.

   written by Dana Swift 6/28/93
*/
int strseek(const char *key,FILE *source)
{
   char c;
   int i=0,len=strlen(key);
   long fpos_init,fpos;
   
   for (fpos=fpos_init=ftell(source),i=0; i<len;)
   {
      /* read next character from source stream */
      if (!fread(&c,sizeof(char),1,source)) {fseek(source,fpos_init,SEEK_SET); return 0;}
      
      /* see if stream character matches next key character, if not then reset key index and stream pointer */
      if (c==key[i]) i++; else {fpos=ftell(source); if (i) fseek(source,fpos,SEEK_SET); i=0;}
   }         

   /* key string has been found so position stream pointer to its beginning */
   fseek(source,fpos,SEEK_SET); return 1;
}
