/****************************************/
/*     member of utils library          */
/****************************************/
#include <defs.p>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: stderr.c,v 1.1.1.1 2003/11/12 23:22:58 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: stderr.c,v $
 * Revision 1.1.1.1  2003/11/12 23:22:58  swift
 * Apf9 firmware for a 260ml Apex with SBE41 CTD.
 *
 * Revision 1.3  1997/06/28 16:05:06  swift
 * Version 2.7.2.2 of gcc will not permit non-function-scope
 * initialization with a nonconstant object.  Previously I had
 * initialized `FILE *swiftware_stderr=stderr' in the global space
 * to ensure it's initialization prior to execution of main().  I
 * was forced to add the `get_stderr()' function to return a valid
 * pointer.
 *
 * Revision 1.2  1997/03/07 02:47:14  swift
 * Modifications to permit cross-compilation on various computers.
 *
 * Revision 1.1  1996/06/15 15:53:43  swift
 * Initial revision
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

FILE *swiftware_stderr_ptr = NULL;

/*========================================================================*/
/* function to return a pointer to the swiftware stderr stream            */
/*========================================================================*/
FILE *get_stderr(void)
{
   return (swiftware_stderr_ptr) ? swiftware_stderr_ptr : stderr;
}

/*========================================================================*/
/* procedure to set destination of messages intended for stderr           */
/*========================================================================*/
void cmd_stderr(int *argc,char *argv[])
{
   int i,j;
   char *tmp;

   for (i=1; i<*argc; i++)
   {
      /* copy command line argument to scratch buffer for further processing */
      copy(argv[i],1,MAXSTRLEN,scrbuf);

      /* convert to lower case */
      strlwr(scrbuf);

      /* does command line argument change stderr to stdout? */
      if (!strncmp("stderr=",scrbuf,7) && !strncmp("stdout",&argv[i][7],6))
      {
         swiftware_stderr_ptr=stdout;
         break;
      }

      #ifdef IBM

         /* does command line argument change stderr to stdprn? */
         if (!strncmp("stderr=",scrbuf,7) && !strncmp("stdprn",&argv[i][7],6))
         {
            swiftware_stderr_ptr=stdprn;
            break;
         }

      #endif

      /* does command line argument change stderr to a disk file? */
      if (!strncmp("stderr=",scrbuf,7))
      {
         FILE *fp = NULL;

         /* attempt to open file */
         if ((fp=fopen(&argv[i][7],"w"))) {swiftware_stderr_ptr=fp; break;}
      }
   }

   if (i==(*argc)) return;

   /* rearrange command line arguments so "stderr=..." is at the end */
   tmp=argv[i]; for(j=i; j<(*argc)-1; j++) argv[j] = argv[j+1]; argv[j]=tmp; (*argc)--;
}
