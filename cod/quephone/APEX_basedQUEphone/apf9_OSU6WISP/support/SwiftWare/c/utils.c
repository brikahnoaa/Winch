/****************************************/
/*     member of utils library          */
/****************************************/
#include <defs.p>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: utils.c,v 1.2 2005/02/22 22:01:25 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: utils.c,v $
 * Revision 1.2  2005/02/22 22:01:25  swift
 * Eliminated GNU regex implementation.
 *
 * Revision 1.1.1.1  2003/11/12 23:22:58  swift
 * Apf9 firmware for a 260ml Apex with SBE41 CTD.
 *
 * Revision 1.7  2001/06/11 16:06:05  navo
 * Minor change to swifterr() function to work around a segfault
 * that happens when mixing C++ streams with C streams.
 *
 * Revision 1.6  2001/05/13 10:38:45  navo
 * Modifications for porting from UW to NAVO.
 *
 * Revision 1.5  2001/03/07 20:03:29  swift
 * Modifications to account for changes in arguments to functions.
 *
 * Revision 1.4  1998/10/09 16:57:14  swift
 * Correct misspelling of `usage'.
 *
 * Revision 1.3  1997/06/28 16:07:34  swift
 * Eliminated the non-function-scope initialization:
 * `FILE *swiftware_stderr=stderr'.  Version 2.7.2.2 of
 * gcc will not permit the initialization of an object
 * with a non-constant object.
 *
 * Revision 1.2  1997/03/07 02:47:14  swift
 * Modifications to permit cross-compilation on various computers.
 *
 * Revision 1.1  1996/06/15 15:55:44  swift
 * Initial revision
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* define scratch buffer */
char scrbuf[MAXSTRLEN+1]="";

/*========================================================================*/
/* function to check for null pointer and return message                  */
/*========================================================================*/
void validate(void *ptr,char *ptr_name,char *func_name)
{
   if (!ptr) swifterr("ERROR [%s]: Unable to validate pointer \"%s\".\n",func_name,
ptr_name);
}

/*========================================================================*/
/*  procedure to print program LEADER string to output stream             */
/*========================================================================*/
void doLeader(char *leader,FILE *dest)
{
   /* make sure we get a valid file pointer */
   FILE *dst = (dest) ? dest : swiftware_stderr;

   /* print leader to dest stream */
   if (leader) fprintf(dst,"%s%s\n",((dst==stdout)?"$ ":"\n"),leader);
}

/*========================================================================*/
/* procedure to write formatted error message to stderr and exit          */
/*========================================================================*/
/* 
  This function behaves identically to the "printf" function except for one
  difference.  It writes the message to the "swifterr_stderr" stream rather 
  than the "stdout" stream.

  For information on what the format codes are, see the standard c-library
  reference manual for "printf".

  written by Dana Swift 3/20/95

*/
void message(char *format,...)
{
   va_list arg_pointer;

   /* get the format argument from the argument list */
   va_start(arg_pointer,format);

   /* print the message */
   vfprintf(swiftware_stderr,format,arg_pointer);

   /* clean up call */
   va_end(arg_pointer);

   /* flush the output */
   fflush(swiftware_stderr);
}

/*========================================================================*/
/* procedure to write formatted error message to stderr and exit          */
/*========================================================================*/
/* 
  This function behaves identically to the "printf" function except for two
  differences.  1) It exits the program after printing its message.  2) It 
  writes the message to the "stderr" stream rather than the "stdout" stream.

  For information on what the format codes are, see the standard c-library
  reference manual for "printf".

  written by Dana Swift 6/21/91

*/
void swifterr(char *format,...)
{
   va_list arg_pointer;

   /* get the format argument from the argument list */
   va_start(arg_pointer,format);

   /* print the message */
   vfprintf(swiftware_stderr,format,arg_pointer);

   /* clean up call */
   va_end(arg_pointer);

   /* flush the output */
   fflush(swiftware_stderr);

   exit(0);
}

/*========================================================================*/
/*  function to convert string to lower case                              */
/*========================================================================*/
char *strlwr(char rec[])
{
   int i;

   for (i=0; rec && rec[i]; i++)
   {
      rec[i]=lower_case(rec[i]);
   }

   return(rec);
}

/*========================================================================*/
/*  function to convert string to upper case                              */
/*========================================================================*/
char *strupr(char rec[])
{
   int i;

   for (i=0; rec && rec[i]; i++)
   {
      rec[i]=upper_case(rec[i]);
   }

   return(rec);
}
