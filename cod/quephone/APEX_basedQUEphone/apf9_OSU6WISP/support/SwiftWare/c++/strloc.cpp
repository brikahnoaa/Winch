/****************************************/
/*     member of utils library          */
/****************************************/
#include <defs.oop>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: strloc.cpp,v 1.1 2006/11/18 17:22:31 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: strloc.cpp,v $
 * Revision 1.1  2006/11/18 17:22:31  swift
 * Added new functionality to library.
 *
 * Revision 1.1  1999/01/16 06:10:00  swift
 * Initial revision
 *
 *========================================================================*/
/* function to locate next occurrence of key within file                  */
/*========================================================================*/
/*
   This function searches a text stream for the next occurrence of a key
   string within a source stream.  If the search is successful, the function
   returns a pointer (strfind) to the key within the current stream record.
   The entire current record is stored in the character buffer (buf) and the
   pointer (strfind) is positioned to the position within (buf) that is
   occupied by the key.  The file pointer is also set to the first character
   past the end of the key so that multiple searches of the same key are
   easily possible.  If the search is not successful, the function returns
   NULL and the file pointer remains unchanged.

      INPUT:

         istream &source...text stream to be searched.
         char *key......the key string that is the object of the search.

      OUTPUT:

         char *dest.....optional storage buffer in which the stream record
            containing the key is stored.  It is up to the user to ensure
            that a sufficiently large buffer is passed to hold any given
            record in the text stream.  If a NULL value is passed to this
            function then records are read into the global scratch buffer
            (scrbuf).  Since (scrbuf) is used by a number of string
            manipulation functions then there exists the potential for
            inadvertent corruption of the record.

         char *strfind...this function returns a pointer to the position
            within the current record (dest or scrbuf, whichever is
            relevant) containing the key.  Please note that pointers to
            both the entire current record as well as the key are readily
            available.

   NOTE...this function is a slightly modified version of one by the same
   name ("strloc") that uses standard C FILE structures.  This is an
   overloaded version that uses an istream object instead.

   written by Dana Swift, Nov 1994
 */
char *strloc(istream &source,char *key,char *dest)
{
   char const *strfind;
   char *buf=(dest)?dest:scrbuf;
   long fpos,mark=source.tellg();
   int len;

   do
   {
      /* record position of file pointer */
      fpos = source.tellg();

      /* read next line from source stream */
      if (!source.getline(buf,MAXSTRLEN))
      {
         /* reset file pointer to initial mark since the key wasn't found */
         source.seekg(mark);
         return NULL;
      }
   }
   while (!(strfind=strstr((const char *)buf,key)));

   /* position file pointer to first character past end of key */
   source.seekg(fpos+strlen(buf)-strlen(strfind)+strlen(key));

   /* remove <CR> at the end of line (if one exists) */
   len = strlen(buf); if (buf[len-1]=='\n') buf[len-1]=0;

   return (char *)strfind;
}
