#include <defs.p>
#include <regex.h>
#include <assert.h>

/*========================================================================*/
/*                                                                        */
/*========================================================================*/
int main(int argc,char **argv)
{
   enum CMD {if_,of_,D_,NCMD};
   struct MetaCmd cmd[NCMD]=
   {
      {"if="," Pathname for input stream. [stdin]"},
      {"of="," Pathname for output stream. [stdout]"},
      {"D=","  Exclusion token."}
   };
   regex_t ifndef,endif;
   FILE *source=stdin, *dest=stdout;
   char pattern[MAXSTRLEN];
   int echo=0;
   
   // check for usage query
   check_cmd_line(1,make_usage(cmd,NCMD));

   // link command line arguments to metacommands
   link_meta_cmds(cmd,NCMD,argc,argv);

   if (!cmd[D_].arg) {swifterr("Missing search token.\n");}
       
   // connect to input stream
   if (cmd[if_].arg) {open_file(cmd[if_].arg,"r",source);}
   else source=stdin;

   // connect to output stream
   if (!cmd[of_].arg) dest=stdout;
   else if (cmd[if_].arg && !strcmp(cmd[if_].arg,cmd[of_].arg))
   {
      swifterr("Input and output streams are the same.\n");
   }
   else {open_file(cmd[of_].arg,"w",dest);}

   // define the option pattern
   sprintf(pattern,"^[ \\t]*#ifndef[ \\t]+%s",cmd[D_].arg);

   // compile the option pattern 
   assert(!regcomp(&ifndef,pattern,REG_NOSUB|REG_EXTENDED|REG_NEWLINE));
   
   sprintf(pattern,"^[ \\t]*#endif.*%s",cmd[D_].arg);

   // compile the option pattern 
   assert(!regcomp(&endif,pattern,REG_NOSUB|REG_EXTENDED|REG_NEWLINE));

   while (fgets(scrbuf,MAXSTRLEN,source))
   {
      if (!regexec(&ifndef,scrbuf,strlen(scrbuf),NULL,0)) {echo=1;}
      else if (!regexec(&endif,scrbuf,strlen(scrbuf),NULL,0))
      {
         fprintf(dest,"%s",scrbuf); break;
      }
      
      if (echo) fprintf(dest,"%s",scrbuf);
   }
   
   if (!echo && cmd[of_].arg) {fclose(dest); remove(cmd[of_].arg);}
      
   return 0;
}
