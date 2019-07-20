#ifndef METACMD_H
#define METACMD_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: metacmd.c,v 1.1 2005/02/22 22:02:55 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/** RCS log of revisions to the C source code.
 *
 * \begin{verbatim}
 * $Log: metacmd.c,v $
 * Revision 1.1  2005/02/22 22:02:55  swift
 * Added function to implement metacommands.
 *
 * Revision 1.1  2002/10/08 23:48:06  swift
 * Initial revision
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define UtilsChangeLog "$RCSfile: metacmd.c,v $  $Revision: 1.1 $   $Date: 2005/02/22 22:02:55 $"

#include <time.h>

/* declare structure for modeling metacommands */
struct MetaCmd
{
   #define MAXCMDLEN (25)
   #define MAXDESCRIPTIONLEN (71)

   char cmd[MAXCMDLEN+1];
   char description[MAXDESCRIPTIONLEN+1];
   int  len;
   char *arg;
   
   #undef MAXCMDLEN
   #undef MAXDESCRIPTIONLEN
};

/* function prototypes */
void link_meta_cmds(struct MetaCmd *cmd,int ncmd,int argc,char *argv[]);
char *make_usage(struct MetaCmd *cmd,int ncmd); 

#endif /* METACMD_H */

#include <ctype.h>
#include <string.h>

int snprintf(char *str, size_t size, const char *format, ...);

/*========================================================================*/
/* procedure to link meta commands to command line argument strings       */
/*========================================================================*/
/*
   This procedure is designed to search an array of strings (argv) (where
   the array contains (argc) strings) in search of meta commands contained
   within the MetaCmd structure (cmd) which contains a number (ncmd) of
   meta command strings.  See the header file "utils.p" for the declaration
   of the MetaCmd structure.  Once a match is found, then a pointer (cmd[n].arg)
   is set to the argument of the meta command.  For example, if the program
   "foo" were invoked as follows:

      foo if=input_file of=output_file n=5 ten=10 Pi=3.1415926

   and 4 meta commands were defined as follows:

      struct MetaCmd cmd[4] = { {"Pi="}, {"if="}, {"of="}, {"name="} };

   then on return this procedure will determine and set the length of each
   meta command:

      cmd[0].len=3, cmd[1].len=3, cmd[2].len=3, cmd[3].len=5

   In addition, the character pointers for each meta command will be set to
   the argument of each meta command as follows:

      cmd[0].arg="3.1415926", cmd[1].arg="input_file", cmd[2].arg="output_file", cmd[3].arg=NULL

   Meta commands which are not present in the argument list will have NULL pointer
   assignments. Moreover, argument list elements which have no associated meta
   commands are simply ignored.

   The comparison is case sensitive so that the case of each character is important.

   written by Dana Swift 3/28/92
*/
void link_meta_cmds(struct MetaCmd *cmd,int ncmd,int argc,char *argv[])
{
   int i,n; 

   for (n=0; n<ncmd; n++)
   {
      /* initialize string length of each meta command and pointer to command argument */
      cmd[n].len=strlen(cmd[n].cmd); cmd[n].arg=NULL;

      for (i=1; i<argc; i++)
      {
         /* check to see if current meta command matches any command line arguments */
         if (!strncmp(argv[i],cmd[n].cmd,cmd[n].len))
         {
            cmd[n].arg = (argv[i][cmd[n].len]) ? argv[i]+cmd[n].len : argv[i];
         }
      }
   }
}


/*========================================================================*/
/* function to create usage string from metacommand structure            */
/*========================================================================*/
/*
  This function uses the meta command structure to construct a usage string
  useable by the macro "check_cmd_line()".  It uses "cmd" and "description"
  fields of the MetaCmd structure to construct the usage string.

  If the function parameter (char *usage) is non-NULL then the usage string
  is stored in the corresponding buffer, if (char *usage) is NULL then the
  usage string is stored in the static buffer (char *scrbuf).

  written by Dana Swift 5/8/92
*/
char *make_usage(struct MetaCmd *cmd,int ncmd)
{
   #define MAXSTRLEN (4096)
   static char buf[MAXSTRLEN+1];
   
   /* initialize usage string */
   int n,len=snprintf(buf,MAXSTRLEN,"\n");

   /* construct buf string from meta command structure elements */
   for (n=0; n<ncmd; n++)
   {
      len+=snprintf(buf+len,MAXSTRLEN-len,"\t");
      len+=snprintf(buf+len,MAXSTRLEN-len,"%s",cmd[n].cmd);
      len+=snprintf(buf+len,MAXSTRLEN-len,"%s",cmd[n].description);
      len+=snprintf(buf+len,MAXSTRLEN-len,"\n");
   }

   return(buf);
}
