#include <stdio.h>
#include <math.h>
#include <string.h>

#define LEADER "Portable Archive Controller [SwiftWare]\n" \
               "$Revision: 1.1.1.1 $ $Date: 2003/11/12 23:22:58 $"

/* #define SWIFTWARE ALWAYS delete this line - it's for my use only.         */
                  /* See installation instructions below.                    */

/* #define UNIX   */ /* Uncomment this line if your operating system is UNIX  */
/* #define MSDos  */ /* Uncomment this line if your operating system is MSDos */
#define ANSI  /* Comment this line out if your compiler is not ANSI compliant */

#ifdef SWIFTWARE
   #include "defs.p"

   #ifdef IBM
      #define TOKEN ('\\')
      #include <dir.h>
      #include <dos.h>
   #else
      #define TOKEN ('/')
   #endif
#elif defined(MSDos)
   #define TOKEN ('\\')
   #include <stdlib.h>
#else
#  define TOKEN ('/')  /* <=== non-UNIX and non-MSDos users must    */
#  include <stdlib.h>  /*      change this TOKEN...see installation */
#endif                 /*      section below for details.           */

/* macro to open a disk file for i/o */
#ifndef open_file
#  define open_file(FNAME,MODE,FP)  if ((FP=fopen(FNAME,MODE))==0)               \
   {                                                                             \
      fprintf(stderr,"\nUnable to open \"%s\" using mode \"%s\".\n",FNAME,MODE); \
      exit(0);                                                                   \
   }
#endif

/* macro to check for sufficient number of command line parameters */
#ifndef check_cmd_line
#  define check_cmd_line(arg,cmd_string)                             \
   {                                                                    \
      if (argc<arg+1)                                                   \
      {                                                                 \
         fprintf(stderr,"\n\nusage:%s %s\n\n\n",argv[0],cmd_string);   \
         exit(0);                                                       \
      }                                                                 \
   }
#endif

#ifdef __TURBOC__
   #include <dir.h>
   #include <dos.h>
#endif

#define DEFAULT_TOKEN ('|')

/* macro to replace one character token with another */
#define convert_token(fname,old_token,new_token)   \
{                                                  \
   int k;                                          \
   for (k=0; k<strlen(fname); k++)                 \
   {                                               \
      if (fname[k]==old_token) fname[k]=new_token; \
   }                                               \
}

/* macro to replace a line feed with null terminator */
#define remove_line_feed(rec)                             \
{                                                         \
   if (rec[strlen(rec)-1]=='\n') rec[strlen(rec)-1]=0;    \
}

#ifdef ANSI
   void remove_files(int argc,char *argv[]);
   void add_files(int argc,char *argv[]);
   void add_current_file(char fname[],FILE *arc);
   void directory(int argc,char *argv[]);
   void extract_files(int argc,char *argv[]);
   int strmatch(char dna[],char rna[]);
#else
   int strmatch();
   void add_files();
   void add_current_file();
   void directory();
   void extract_files();
   void remove_files();
   int strmatch();
#endif

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: pac.c,v 1.1.1.1 2003/11/12 23:22:58 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: pac.c,v $
 * Revision 1.1.1.1  2003/11/12 23:22:58  swift
 * Apf9 firmware for a 260ml Apex with SBE41 CTD.
 *
 * Revision 2.6  2001/06/03 07:22:52  navo
 * Blind the utility to the existence of SwiftWare.
 *
 * Revision 2.5  1999/01/16 23:04:07  swift
 * Changed return value of `main()' from void to int.
 *
 * Revision 2.4  1998/10/09 17:09:15  swift
 * Correct misspelling of `usage'.
 *
 * Revision 2.3  1996/08/24 21:32:52  swift
 * Fixed minor problem of mismatched comment delimiters.
 *
 * Revision 2.2  1996/08/24 20:43:19  swift
 * Minor portability enhancements.
 *
 * Revision 2.1  1996/01/20 19:00:03  swift
 * This is the first version subject to RCS control.  Previous versions
 * are extinct.
 *
 *========================================================================*/
/* program to archive mulitple files into one for ease of transport       */
/*========================================================================*/
/*
  See the comments below regarding the following topics:

         1. Notes to UNIX users.
         2. Common usage.
         3. Wild card usage.
         4. Notes to non-UNIX users.
         5. Non-UNIX and non-MSDos users must change TOKEN.
         6. Warning...be cool!
         7. Installation instructions - extremely simple.
         8. Special (optional) installation note to MSDos users.
         9. Distibution comments & Warranty disclaimer.

  1. Notes to UNIX users:

  This program stores multiple ascii files sequentially in one continuous
  ascii file and enables extraction of individual files from the archive.
  It's purpose is to simplify transport of large numbers of files across
  the network.  Do NOT use this utility with non-ascii files.  This program
  does not check to see if it will overwrite an existing file so BEWARE!

  When using the add (a) option this program appends files to the end of the
  archive file.  This means that multiple files with the same name can exist
  in the archive but only the last one can be (easily) extracted.

  When using the extract (x) option this program will attempt to extract a
  given file using exactly the same pathname as was used to add the file to
  the archive so keep this in mind when you add files to an archive.  More-
  over, the program does not attempt to create subdirectories when a file
  is to be extracted so it is the user's responsibility to ensure that any
  subdirectories are created before the extraction is attempted.  When the
  program encounters a file it can't open, the program aborts immediately
  and reports the pathname of the offending file...this applies when either
  adding or extracting files to/from the archive.

  When using the remove (r) option this program makes a new file into which
  it writes all the archived files which are NOT to be removed.  In this way
  the original file is not altered thus allowing disaster recovery.  If
  your operating system is UNIX or MSDos the original file is actually renamed
  with the extensions "~" or "bak" (respectively) added to the original name.
  The new file will have the same name as the original file.  If your
  operating system is neither MSDos nor UNIX then the original file name
  becomes the backup file and the new file will have the last character of the
  original name replaced by a tilde "~".

  2. Common usage:

     To get directory of files on archive : pac d archive_file [file(s) [*?]]
     To add file(s) to an archive         : pac a archive_file file(s) [*?]
     To extract file(s) from an archive   : pac x archive_file file(s) [*?]
     To extract all files from an archive : pac x archive_file .
     To remove file(s) from an archive    : pac r archive_file file(s) [*?]

  To get a list of valid archive operations simply invoke this program with
  no command line arguments.

  3. Wild card usage:

  Wildcards (*?) are allowed as part of the file name but their behavior
  differs slightly when using the extract (x), directory (d), and remove (r)
  options in comparison to the add (a) option.  The reason for this is that
  the unix operating system performs the pattern matching and provides a list
  of matching names to the program when the add (a) option is used but the
  program performs the pattern matching when using the remaining options.
  Incidentally, this means that if you want to use wildcards in combination
  with the extract (x), directory (d), or remove (r) options, you must place
  the whole search string in double quotes in order to avoid having unix
  intercept and change the search string.  The small differences between the
  way unix impliments wild card pattern matching and the way it's done in
  this program can occasionally lead to unexpected results...but this does
  not seem to happen often.  The difference results from the second constraint
  described in the comment section of the function "strmatch" (included as
  last function in this file)...I will not fully describe it here since it
  apparently rarely causes problems.  It is sufficient to say that any
  unexpected results occur only when there are repeated "tokens" (see
  strmatch for definition of a token) within the file name against which the
  search pattern is to be compared.  Special provision has been made in the
  case where all archive files are to be extracted - in this case use a
  period "." in place of the file list...you could also use the wild card (*)
  but the period (.) operator was built in before the program was revised to
  allow wild card pattern matching...the period operator (.) also operates
  slightly faster.

  4. Notes to non-UNIX users:

  The notes above should generally apply but may differ according to your
  particular operating system...especially in the area of overwriting
  files and wildcards.  The above (unix) comments regarding wild cards
  apply to non-unix installations when using the extract (x), remove (r),
  or directory (d) options but the ability to use wild cards with the
  add (a) option is available only if your operating system intercepts the
  command line arguments, performs the pattern matching, and substitutes
  the pattern matching file names before passing the command line arguments
  to this program.  Moreover, you may or may not be required to enclose wild
  card containing strings in quotes when using the x, r, or d options
  depending on whether or not your operating system intercepts and substitutes
  for wild cards.  Incidentally, when using the extract (x), directory (d),
  and remove (r) options, there has been no provision made for treating the
  wild card characters (*?) as anything other than wild cards so I recommend
  that you avoid the use of these characters as part of the pathname or
  filename if your operating system allows these to be used.

  5. Non-UNIX and non-MSDos users must change TOKEN:

  The macro entitled "TOKEN" should be set to the character which delimits
  elements of a path name.  Some operating systems (such as vms) use
  different delimiters for devices than is used for directories.  This may
  or may not cause problems when extracting files from the archive but I
  recommend you move to the target device before using the add (a) operation
  so that you aren't required to explicitly include the device name in the
  pathname.  If you follow my advice then any files will be extracted into
  the current device.

  6. Warning...be cool!:

  I don't pretend that these special notes are exhaustive or even correct
  so please don't flame me if you find a discrepency.  As a general habit
  I suggest you extract files from the archive files into a temporary
  directory to avoid unintentional overwriting of files.

  7. Installation instructions:

  This program was written with portability in mind and therefore uses only
  those functions which are common to both the standard ANSI C libraries as
  well as the older unix C libraries.  This source code file is self-contained
  except for the functions which reside in the libraries mentioned above.  I
  have seen operating systems with a pre-existing program called "pac" that
  had something to do with printer control so if after installing this program
  you get wierd errors or behavior, you should check see if you are really
  running this program as opposed to another program with the same name.  This
  program has been installed on machines with various different operating
  systems (including MSDos, UNIX, VMS, MacIntosh) according to the
  instructions below and has performed properly on all of them.  To create a
  working program follow the three instructions below:

      a. Delete the very first line (#define SWIFTWARE) of this file.  This
         ensures any non-portable code will not be compiled.

      b. Do ONE of the following:
           - If your operating system is UNIX then uncomment line 4 above.
           - If your operating system is MSDos then uncomment line 5 above.
           - If your operating system is neither UNIX nor MSDos then change
               the TOKEN in line 39 to the appropriate path element delimiter
               as described in article 5 above.

      c. Compile this source file and link with the standard libraries.  If
         the standard unix C compiler is used the following command will create
         a working program called pac: "cc pac.c -o pac".  If you have the
         (ANSI) gnu C compiler installed then the appropriate command is given
         by: "gcc pac.c -o pac".  For other operating systems, you might want
         to consult your C compiler manual for the simplest command which
         compiles the source file and links it with the standard libraries.

  8. Special (optional) installation note to MSDos users:

  If you compile this program with Borland's TurboC compiler (version 1.0
  or later) you can also use wild cards when adding files to an archive.
  MSDos does not intercept command line arguments to perform the pattern
  matching so special (optional) code has been added to this file to enable
  this feature.  This portion of the code is non-portable so please don't
  confuse the next person's installation attempts - give them an original
  distribution copy of this source file.  To enable this portion of the code
  to be compiled, uncomment line 6 of this file.  Since MSDos does not
  intercept command line arguments to perform the pattern matching you do not
  need (or want) to place the search pattern string in quotes.

  9. Distibution comments & Warranty disclaimer:

  Other than these few notes you're on your own...this is a free program and
  you may modify it or distribute it freely (but without cost!!) as you desire.

  BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY
  FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN
  OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES
  PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED
  OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS
  TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE
  PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,
  REPAIR OR CORRECTION.

  IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING
  WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR
  REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES,
  INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING
  OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED
  TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY
  YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER
  PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGES.

  written by Dana Swift (Aug 1990)
     e-mail: swift@ocean.washington.edu

*/
int main(argc,argv)
   int argc;
   char *argv[];
{
   /* make sure first argument is only one character long */
   if (argc>1) if (strlen(argv[1])>1) argc=0;

   /* print LEADER to stdout */
   printf("\n%s\n",LEADER);

   /* make sure command line has at least 2 arguments - print usage if not */
   check_cmd_line(2,"[adrx] archive_name file(s) [*?] [.]\n\ta : add file(s) to archive.\n\td : directory of archive.\n\tr : remove file from archive.\n\tx : extract file from archive.\n");

   /* get operation */
   switch (argv[1][0])
   {
      case 'a': {add_files(argc,argv); break;}
      case 'd': {directory(argc,argv); break;}
      case 'x': {extract_files(argc,argv); break;}
      case 'r': {remove_files(argc,argv); break;}
      default :
      {
         fprintf(stderr,"Error: unemplimented operation:\"%c\".\n",argv[1][0]);
         exit(0);
      }
   }
   
   return 0;
}

/*========================================================================*/
/* procedure to remove files from an archive                              */
/*========================================================================*/
void remove_files(argc,argv)
   int argc;
   char *argv[];
{
   FILE *arc,*dest;
   char fname[85],rec[1024],bu_fname[85],arcname[256];
   int i;

#ifdef UNIX

      /* copy name of archive file to fname */
      strcpy(fname,argv[2]);

      /* create backup file name */
      strcpy(bu_fname,fname); strcat(bu_fname,"~");

      /* delete backup file */
      unlink(bu_fname);

      /* rename source file to backup */
      if (rename(fname,bu_fname))
      {
         printf("rename not successful for file: %s\n",fname);
         exit(0);
      }

#else
#ifdef __TURBOC__

      char drive[MAXDRIVE],dir[MAXDIR],name[MAXFILE],ext[MAXEXT];

      /* copy name of archive file to fname */
      strcpy(fname,argv[2]);

      /* decompose source file name to get fname */
      fnsplit(fname,drive,dir,name,ext);

      /* rename extension to ".bak" */
      strcpy(ext,".bak");

      /* create backup file name */
      fnmerge(bu_fname,drive,dir,name,ext);

      /* delete backup file */
      unlink(bu_fname);

      /* rename source file to backup */
      if (rename(fname,bu_fname))
      {
         printf("rename not successful for file: %s\n",fname);
         exit(0);
      }

#else

      /* copy name of archive file to fname */
      strcpy(fname,argv[2]);

      /* create backup file name */
      strcpy(bu_fname,fname); fname[strlen(fname)-1]='~';

#endif
#endif

   if (argc<4)
   {
      fprintf(stderr,"No files on command line.\n");
      exit(0);
   }

   /* open archive file */
   printf("Opening archive file: %s\n",fname);
   open_file(bu_fname,"r",arc);
   open_file(fname,"w",dest);

   while (!feof(arc))
   {
      if (!fgets(rec,1023,arc)) break;

      if (!strncmp(rec,"$ arc_header:",13))
      {
         strcpy(fname,rec+14); remove_line_feed(fname);

         convert_token(fname,DEFAULT_TOKEN,TOKEN);

         for (i=3; i<argc; i++)
         {
            if (strmatch(fname,argv[i]))
            {
               printf("\tdeleting: %s\n",fname);

               rec[0]=0; sprintf(arcname,"$ arc_EOF: %s",fname);
               convert_token(arcname,TOKEN,DEFAULT_TOKEN);


               while (!feof(arc) && strncmp(arcname,rec,strlen(arcname)))
               {
                  if (!fgets(rec,1023,arc)) 
                  {
                     fprintf(stderr,"Premature end of archive file...\n");
                     exit(0);
                  }
               }
               rec[0]=0;
               break;
            }
         }
      }

      if (!feof(arc) && rec[0])
      {
         if (rec[strlen(rec)-1]!='\n') strcat(rec,"\n");
         fprintf(dest,"%s",rec);
      }

   }
}

/*========================================================================*/
/* procedure to extract files from archive                                */
/*========================================================================*/
void extract_files(argc,argv)
   int argc;
   char *argv[];
{
   FILE *arc,*dest;
   char fname[85],rec[1024],arcname[256];
   int i;

   if (argc<4)
   {
      fprintf(stderr,"No files on command line.\n");
      exit(0);
   }

   /* open archive file */
   printf("Opening archive file: %s\n",argv[2]);
   open_file(argv[2],"r",arc);

   while (!feof(arc))
   {
      if (!fgets(rec,1023,arc)) break;

      if (!strncmp(rec,"$ arc_header:",13))
      {
         strcpy(fname,rec+14); remove_line_feed(fname);

         convert_token(fname,DEFAULT_TOKEN,TOKEN);

         for (i=3; i<argc; i++)
         {
            if (!strcmp(".",argv[i]) || strmatch(fname,argv[i]))
            {
               printf("\textracting: %s\n",fname);

               open_file(fname,"w",dest);
               rec[0]=0; sprintf(arcname,"$ arc_EOF: %s",fname);
               convert_token(arcname,TOKEN,DEFAULT_TOKEN);

               while (!feof(arc) && strncmp(arcname,rec,strlen(arcname)))
               {
                  if (!fgets(rec,1023,arc)) {fprintf(stderr,"Premature end of archive file...\n"); exit(0);}

                  if (strncmp(arcname,rec,strlen(arcname)))
                  {
                     fprintf(dest,"%s",rec);
                  }
               }
               fclose(dest);
               break;
            }
         }
      }
   }
}

/*========================================================================*/
/* procedure to get directory of archive                                  */
/*========================================================================*/
void directory(argc,argv)
   int argc;
   char *argv[];
{
   FILE *arc;
   char rec[1024];
   int i,j,n=0;

   /* open archive file */
   printf("Opening archive file: %s\n",argv[2]);
   open_file(argv[2],"r",arc);

   printf("Directory of archive file: %s\n",argv[2]);

   while (!feof(arc))
   {
      if (!fgets(rec,1023,arc)) break;

      if (!strncmp(rec,"$ arc_EOF:",9)) n--;

      if (rec[0] && !strncmp(rec,"$ arc_header:",13))
      {
         convert_token(rec,DEFAULT_TOKEN,TOKEN);
         remove_line_feed(rec); n++;

         if (argc<=3)
         {
            for (j=0; j<n; j++) printf("\t");
            printf("%s\n",rec+14);
         }
         else
         {
            for (i=3; i<argc; i++)
            {
               if (strmatch(rec+14,argv[i]))
               {
                  for (j=0; j<n; j++) printf("\t");
                  printf("%s\n",rec+14);
                  break;
               }
            }
         }
      }
   }
   printf("\n");
}

/*========================================================================*/
/* procedure to add file to archive                                       */
/*========================================================================*/
void add_files(argc,argv)
   int argc;
   char *argv[];
{
#ifdef SWIFTWARE
   char **list;
#endif

   FILE *arc;
   int i;

   if (argc<4)
   {
      fprintf(stderr,"No files on command line.\n");
      exit(0);
   }

   /* open archive file */
   printf("Opening archive file: %s\n",argv[2]);
   open_file(argv[2],"a",arc);

#ifdef SWIFTWARE

   /* construct an alphabetized list of matching file names */
   list = unique_file_list(argc-2,argv+2,NULL);

   for (i=0; list[i]; i++)
   {
#ifdef MSDos
      /* convert file name to lower case */
      strlwr(list[i]);
#endif

      /* add current file to archive file */
      add_current_file(list[i],arc);
   }

#else

   for (i=3; i<argc; i++)
   {
#ifdef __TURBOC__

      int status;
      char drive[MAXDRIVE],dir[MAXDIR],fname[MAXFILE],ext[MAXEXT],
         pathname[1024];
      struct ffblk fileinfo;

      /* decompose pathname */
      fnsplit(argv[i],drive,dir,fname,ext); ext[0]=0;

      /* find first file which matches search pattern */
      status=findfirst(argv[i],&fileinfo,0);

      while (status != -1)
      {
         /* construct pathname */
         fnmerge(pathname,drive,dir,fileinfo.ff_name,ext);

         /* convert file name to lower case */
         strlwr(pathname);

         /* add current file to archive file */
         add_current_file(pathname,arc);

         /* find next matching file */
         status = findnext(&fileinfo);
      }

#else

      add_current_file(argv[i],arc);

#endif
   }

#endif

   fclose(arc);
}

/*-----------------------------------------------------------------------*/
/* procedure to add current file to archive                              */
/*-----------------------------------------------------------------------*/
void add_current_file(fname,arc)
   char fname[];
   FILE *arc;
{
   FILE *source;
   char rec[1024];

   /* open next file for reading */
   if ((source=fopen(fname,"r"))==0)
   {
       fprintf(stderr,"\nUnable to open \"%s\" - ignoring...\n",fname);
       return;
   }

   printf("\tadding: %s\n",fname);

   convert_token(fname,TOKEN,DEFAULT_TOKEN);

   fprintf(arc,"$ arc_header: %s\n",fname);

   while (!feof(source))
   {
      if (!fgets(rec,1023,source)) break;

      if (rec[strlen(rec)-1]!='\n') strcat(rec,"\n");
      fprintf(arc,"%s",rec);
   }

   fprintf(arc,"$ arc_EOF: %s\n",fname);
   fclose(source);
}

#ifndef SWIFTWARE

/*========================================================================*/
/*  function to compare two strings allowing wild cards                   */
/*========================================================================*/
/*
  This function impliments a simple pattern matching algorithm allowing
  a given string (dna) to be compared to another string (rna) containing
  optional wildcards (*?).  Only two wild card characters are currently
  implimented.  The asterisk (*) is a wild card which substitutes for any
  number (including zero) of characters from the dna string.  The question
  mark (?) is a wild card which can substitute for zero or one character
  from the dna string.  The rna string constists of tokens seperated by
  sequences of wild cards. For example, the rna string "a*bc*hij?lm???pq*"
  would match the dna string "abcdefghijklmnopqrstuvwxyz"; note however
  that the same rna string will match many more dna strings due to the
  wildcards.  There are five tokens in the above string..."a", "bc", "hij",
  "lm", and "pq".  A given token is defined to be the largest substring
  possible without containing wildcards.  Naturally, the order of the
  tokens in the rna string must be the same as the same tokens in the dna
  string for a match to result.

  Important note:  Two additional constraints are placed on dna substrings
  for which wild cards may substitute.  First, no contiguous sequence of
  wildcards may substitute for more than 65535 contiguous characters from
  the dna string.  Second, the dna substring may not contain the token
  following the given wildcard in the rna string.  For example, the rna
  string "ab*ef" would match the dna string "abcdef" but would not match
  the dna string "abcdefghef" even though you might imagine that the
  asterisk (*) could substitute for the string "cdefgh".  The reason that
  this is not a possible match is because the token which follows the wild
  card "*" (in the rna string) is the string "ef", but since the proposed
  substitution string "cdefgh" contains the token "ef" then this additional
  constraint would be violated.  A similar constraint is placed on sequences
  of the wild card "?" such as "????".

  No provision has been made for treating the wild card characters (in the rna
  string) as anything other than wild cards.  This means, for example, that
  there is no way to represent the asterisk (*) or question mark (?) in
  literal fashion.  In other words, these wild cards can not be made to be
  part of a search token within the rna string.  These two characters can
  safely exist in the dna string but you must cause them to become part of a
  wild card substitution.  For example, if you want the dna string "me?" to
  be matched, you could use the search (rna) string "me?" but you have no way
  of preventing this from also matching the dna strings "men" or "met", etc.

  Meaningless trivia:  The names dna and rna are metaphorical in the sense
  that the pattern matching in this function is (ever so) slightly
  reminiscient to the pattern matching magic of rna transcriptase in buiding
  new dna molecules within biological cells.

  This function returns true (1) if the dna string matches the pattern of
  the rna string and returns false (0) if not.  Neither the dna string nor
  the rna string is altered by this function.

  written by Dana Swift (Nov 1990)

*/
int strmatch(dna,rna)
   char dna[];
   char rna[];
{
#define INFINITY (65535)
#define TRUE     (1)
#define FALSE    (0)

   int i,token_len,match=0;
   unsigned nwild;
   char *dptr,*rptr;
   char wild[25],*token,*delimiter;

   /* initialize wild card selections */
   strcpy(wild,"*?");

   /* make work space and validate it */
   token = (char *)calloc(strlen(rna)+1,sizeof(char));
   if (!token) {printf("\n\nUnable to allocate %d bytes of workspace in function \"strmatch\"...aborting program.\n\n",strlen(rna)+1); exit(0);}

   /* initialize pointers to dna & rna strings */
   dptr = dna; rptr=rna;

   while (rptr)
   {
      /* find position of next non-wild character in rna string */
      for (nwild=i=0; i<strlen(rptr) && (delimiter = strchr(wild,rptr[i]))!=0; i++)
      {
         if (nwild!=INFINITY) nwild = ((*delimiter)=='*') ? INFINITY : nwild+1;
      }

      /* copy remainder of rna string to token string and convert token */
      strcpy(token,rptr+i); token=strtok(token,wild);

      /* position rna pointer to next wild card */
      rptr = strpbrk(rptr+i,wild);

      /* crash out of loop if last character was wild card */
      if (!token) break;

      /* search dna for the token starting within next "nwild" characters */
      for (token_len=strlen(token),match=FALSE,i=0; i<=nwild && i<strlen(dptr); i++)
      {
         if (!strncmp(dptr+i,token,token_len))
         {
            dptr += i+token_len;
            match=TRUE; nwild=0;
            break;
         }
      }

      /* if match not found then free work space & return */
      if (!match) {free(token); return(0);}
   }

   /* free work space */
   free(token);

   /* check to see if remainder of string matches */
   if (strlen(dptr)<=nwild || nwild == INFINITY) return(1);
   else return(0);

#undef INFINITY
#undef TRUE
#undef FALSE
}

#endif
