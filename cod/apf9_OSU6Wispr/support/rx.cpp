using namespace std;

#include <defs.p>
#include <stdio.h>
#include <regex.h>
#include <string>
#include <limits.h>
#include <libgen.h>
#include <assert.h>

extern "C"
{
   #include "Rx.h"
   #include "logger.h"
   #include "xmodem.h"
}

#include <GetOpt.oop>
#include "StdioPort.oop"

/* program descriptor to greet user */
#define LEADER  "Xmodem Receiver Utility [SwiftWare]"
#define VERSION "$Revision: 1.4 $  $Date: 2008/07/14 17:00:47 $"

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: rx.cpp,v 1.4 2008/07/14 17:00:47 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/** RCS log of revisions to the C source code.
 *
 * $Log: rx.cpp,v $
 * Revision 1.4  2008/07/14 17:00:47  swift
 * Added missing include directive for regex.h.
 *
 * Revision 1.3  2006/08/17 21:18:02  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.2  2005/09/06 01:56:14  swift
 * Added <EOT> marker to files.
 *
 * Revision 1.1  2005/08/06 17:21:57  swift
 * Support utilities for remote host.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// function prototypes
void usage(const char *prgname, const char *cmdargs);
int  configure(char *prgname,string &logpath,bool &autolog);

/*========================================================================*/
/* program to receive a file using the variants of the xmodem protocol    */
/*========================================================================*/
/**
  This program is the receiver for 4 variants of the xmodem protocol.  The 
  variants are:

     \begin{verbatim}
     1. Xmodem with 128 byte packets and 8-bit checksum for corruption detection.
     2. Xmodem with 128 byte packets and 16-bit CRC for corruption detection.
     3. Xmodem with 1024 byte packets and 8-bit checksum for corruption detection.
     4. Xmodem with 1024 byte packets and 16-bit CRC for corruption detection.
     \end{verbatim}

  Configuration of the utility can be done with command line arguments or
  with a configuration file in the user's home directory.  The configuration
  file is located at $(HOME)/.$(basename(argv[0]))rc where $(HOME) is read
  from the environment variables and where $(basename(argv[0])) is the
  basename (ie., pathname less the directory portion) of this program.  For
  example, if user foo's home directory is '/home/foo' and foo runs this
  program as '/usr/local/bin/rx' then the pathname for the configuration
  file is '/home/foo/.rxrc'.  An example of this configuration file is:

     \begin{verbatom}
      # specify the name of the log file
      LogPath=/app/swift/iridium/xmodem/rxlog
      
      # enable (AutoLog!=0) or disable (AutoLog==0) the auto-log feature
      AutoLog=1
      
      # specify ascii mode (BinaryMode==0) or binary mode (BinaryMode!=0) 
      BinaryMode=0
      
      # specify CRC mode (16bit or 8bit)
      CrcMode=16bit
      
      # set the default debug level (range: 0-4)
      Verbosity=2
     \end{verbatom}

  For configuration by command line arguments, invoke this program with the
  '-h' option.

  written by Dana Swift
*/
int main(int argc, char **argv, char **envp)
{
   int option=0;
   FILE *dest=NULL;
   
   // activate auto-log feature
   bool autolog=false;
   
   // pathname to the log file
   string logpath;

   // define the command line options string
   const char *cmdargs="abcd:hl:p:qsv";

   // define the command line option processor
   GetOpt getopt(argc,argv,cmdargs);

   // check for usage query
   if (argc<2) usage(argv[0],cmdargs);

   // set binary transfer as the default
   BinMode=1;

   // set 16-bit checksum the default corruption detection method
   CrcMode='C';

   // initialize the debuglevel
   debugbits=2; MaxLogSize=LONG_MAX-4096;

   // read the user's configuration file
   configure(argv[0],logpath,autolog);
   
   while ((option = getopt()) != EOF)
   {
      switch (option)
      {
         // set a switch for ascii mode
         case 'a': {BinMode=0; break;}

         // set a switch for binary mode
         case 'b': {BinMode=1; break;}

         // use 16-bit CRC corruption detection
         case 'c': {CrcMode='C'; break;}

         // set the debug level
         case 'd': {debugbits=atoi(getopt.optarg); break;}
            
         // print the usage query
         case 'h': {usage(argv[0],cmdargs); break;}

         // configure a static log file
         case 'l': {logpath=getopt.optarg; autolog=false; break;}

         // configure an auto-log file
         case 'p': {logpath=getopt.optarg; autolog=true; break;}

         // set quiet mode for log entries
         case 'q': {debugbits=0; break;}

         // use 8-bit CRC corruption detection
         case 's': {CrcMode=NAK; break;}

         // increase the verbosity of log entries
         case 'v': {if (debuglevel<5) debugbits++; break;}
 
         // set a trap for unrecognized options
         default: {LogEntry(argv[0],"Unrecognized option: -%c\n",option);}
     }
   }

   // check if a debug log was requested
   if (logpath.length())
   {
      // check of autolog feature is enabled
      if (autolog) LogOpenAuto(logpath.c_str());

      // open a statically-named log
      else LogOpen(logpath.c_str(),'w'); 
   }
   
   if (debuglevel>=1)
   {
      // print LEADER to log file 
      LogEntry(argv[0],"%s\n",LEADER);

      // print VERSION to the log stream
      LogEntry(argv[0],"%s\n",VERSION);

      // print the command line to the log stream
      LogEntry(argv[0],"Cmd Line:");
 
      // write command line to log stream
      for (int len=11,i=0; i<argc; i++)
      {
         // compute the length of the current command line
         len += strlen(argv[i])+1;

         // wrap the command line around to the next line
         if (len>65) {len=11+strlen(argv[0]); LogEntry(argv[0],"\n%*s",len," ");}

         // write the i(th) command line argument to the log stream
         LogAdd(" %s",argv[i]); 
      }

      // terminate the command line in the log stream
      LogAdd("\n");
   }
   
   // check if the output file name was included
   if (!argv[getopt.optind])
   {
      // make a log entry that the output file was missing
      LogEntry(argv[0],"Missing output file name.\n");

      // create a StdioPort to emulate a SerialPort
      StdioPort stdio; abort_transfer(&stdio.port);
   }

   // check if the output file can be opened
   else if (!(dest=fopen(argv[getopt.optind],"w")))
   {
      // make a log entry that the output file was missing
      LogEntry(argv[0],"Unable to open output file [%s].\n",
               argv[getopt.optind]);

      // create a StdioPort to emulate a SerialPort
      StdioPort stdio; abort_transfer(&stdio.port);
   }

   else
   {
      // create a StdioPort to emulate a SerialPort
      StdioPort stdio; sleep(4);
      
      // begin transfer
      if (Rx(&stdio.port,dest)>0) {fprintf(dest,"\n<EOT>\n");}
   }

   // close the log file
   if (logpath.length()) LogClose();
   
   return 0;
}
 
/*------------------------------------------------------------------------*/
/* function to read a configuration file                                  */
/*------------------------------------------------------------------------*/
int configure(char *prgname, string &logpath, bool &autolog)
{
   int status = -1;
   const char *home=NULL;
   char buf[1024];
   FILE *source;
   
   // validate the function parameter that points to the environment
   if (!prgname) LogEntry("configure()","NULL pointer to program name.\n");

   // get the home directory from the environment
   else if (!(home=getenv("HOME")))
   {
      LogEntry("configure()","Environment doesn't contain HOME.\n"); status=0;
   }
   
   else
   {
      // reinitialize the return value of this function
      status=0;
      
      // create the name of the configuration file
      sprintf(buf,"%s/.%src",home,basename(prgname));

      // open the configuration file
      if ((source=fopen(buf,"r")))
      {
         /* define the number of subexpressions in the regex pattern */
         #define NSUB 2

         /* define objects needed for regex matching */
         regex_t regex; regmatch_t regs[NSUB+1];
      
         #define WS  "[ \t]*"
         #define VAR "([a-zA-Z0-9]+)"
         #define VAL "([^ \t\r\n#]+)"

         /* construct the regex pattern string for files with message locks */ 
         const char *pattern = "^" WS VAR WS "=" WS VAL;
         
         /* compile the regex pattern */
         assert(!regcomp(&regex,pattern,REG_EXTENDED|REG_NEWLINE));

         /* protect against segfaults */
         assert(NSUB==regex.re_nsub);

         while (fgets(buf,1023,source))
         {
            // check if the current line matches the regex
            if (regexec(&regex,buf,regex.re_nsub+1,regs,0)) continue;

            // extract the variable name
            string var; var = extract(buf,regs[1].rm_so+1,regs[1].rm_eo-regs[1].rm_so);

            // extract the value
            string val; val = extract(buf,regs[2].rm_so+1,regs[2].rm_eo-regs[2].rm_so);

            // make a log entry
            if (debuglevel>=3) LogEntry("configure()","%s = %s ",var.c_str(),val.c_str());

            // check for autolog configurator
            if (!strcmp("AutoLog",var.c_str()))
            {
               // initialize the value of the autolog switch
               autolog =  (atoi(val.c_str())) ? true : false;

               // make a log entry
               if (debuglevel>=3) LogAdd("[Autolog %s]\n",(autolog)?"enabled":"disabled");
            }
            
            // check for transfer-mode (binary or ascii) configurator
            else if (!strcmp("BinaryMode",var.c_str()))
            {
               // initialize the value of the transfer mode
               BinMode =  (atoi(val.c_str())) ? 1 : 0;

               // make a log entry
               if (debuglevel>=3) LogAdd("[%s mode enabled]\n",(BinMode)?"Binary":"Ascii");
            }
            
            // check for corruption-detection (8bit or 16bit) configurator
            else if (!strcmp("CrcMode",var.c_str()))
            {
               // check for 8bit checksum corruption-detection mode
               if (!strcmp("8bit",val.c_str()))
               {
                  // initialize corruption-detection to 8-bit checksum
                  CrcMode=NAK; if (debuglevel>=3) LogAdd("[checksum enabled].\n");
               }

               // check for 16-bit CRC corruption-detection mode
               else if (!strcmp("16bit",val.c_str()))
               {
                  // initialize corruption-detection to 16-bit CRC
                  CrcMode='C'; if (debuglevel>=3) LogAdd("[16-bit CRC enabled].\n");
               }

               // warn of invalid corruption-detection mode
               else if (debuglevel>=3) LogAdd("[Invalid CRC mode (%s)] ",val.c_str());
            }

            // check for logpath configurator
            else if (!strcmp("LogPath",var.c_str()))
            {
               // set the logpath
               logpath=val; if (debuglevel>=3) LogAdd("[logpath(%s)].\n",logpath.c_str());
            }

            // check for debuglevel configurator
            else if (!strcmp("Verbosity",var.c_str()))
            {
               // get the debuglevel
               debugbits = (atoi(val.c_str()));

               // make a log entry
               if (debuglevel>=3) LogAdd("[debuglevel(%d)].\n",debuglevel);
            }

            // warn of an unimplemented configurator
            else if (debuglevel>=3) LogAdd("[not implemented].\n");
         }

         // reinitialize function's return value
         status=1;
         
         /* clean up the regex pattern buffer and registers */
         regfree(&regex);
      }

      // warn the user that the config file couldn't be opened
      else LogEntry("configure()","Attempt to open configuration file (%s) failed.\n",buf);
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to print the usage-query for this program                    */
/*------------------------------------------------------------------------*/
void usage(const char *prgname, const char *cmdargs)
{
   fprintf(stderr,"# " LEADER "\n# " VERSION "\n"
           "usage: %s [%s] file\n"
           "  -a  Ascii transfer mode.  Convert files to Unix conventions by\n"
           "         stripping carriage returns and all characters beginning\n"
           "         with the first Control-Z (EOF).\n"
           "  -b  Binary transfer mode. [default]\n"
           "  -c  Use 16-bit CRC for corruption detection. [default]\n"
           "  -d  Set the debug-level (range: 0-4). [2]\n"
           "  -h  Print this usage query.\n"
           "  -l  Pathname for log file.\n"
           "  -p  Base pathname used for automatically generated name of log file.\n"
           "  -q  Quiet mode - suppresses verbosity of log entries.\n"
           "  -s  Use an 8-bit checksum for corruption detection.\n"
           "  -v  Increase verbosity of log entries.  More v's generate more output.\n"
           "\n",prgname,cmdargs);

   exit(0);
}