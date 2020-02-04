#include <defs.p>
#include <config.h>
#include <logger.h>
#include <string.h>

int    MissionParametersWrite(struct MissionParameters *mission);
void   PowerOff(time_t AlarmSec);
int    RecoveryInit(void);
int    fioFormat(void);
int    Tc58v64Init(void);

int    MissionParametersWrite(struct MissionParameters *mission) {return 1;}
void   PowerOff(time_t AlarmSec) {LogEntry("PowerOff()","Alarm set for %ld sec.\n",AlarmSec);}
int    RecoveryInit(void) {LogEntry("RecoveryInit()","Activating recovery mode.\n"); return 1;}
int    fioFormat(void) {return 1;}
int    Tc58v64Init(void) {return 1;}

const unsigned long FwRev = FWREV;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: chkconfig.c,v 1.3 2006/10/11 20:51:24 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: chkconfig.c,v $
 * Revision 1.3  2006/10/11 20:51:24  swift
 * Added stubs to maintain compatibility with the flashio file system.
 *
 * Revision 1.2  2006/08/17 21:18:02  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.1  2005/08/06 17:21:57  swift
 * Support utilities for remote host.
 *
 * Revision 1.4  2005/07/13 20:07:18  swift
 * Include firmware revision in output.
 *
 * Revision 1.3  2002/10/03 17:48:48  swift
 * Modifications to accomodate changes in library structure.
 *
 * Revision 1.2  2002/06/16 03:36:33  swift
 * Revision to convert from C++ to C source.
 *
 * Revision 1.1  2002/06/16 03:33:16  swift
 * Initial revision
 *
 *========================================================================*/
/*                                                                        */
/*========================================================================*/
int main(int argc, char **argv)
{
   enum CMD {if_,cfg_,NCMD};
   struct MetaCmd cmd[NCMD]=
   {
      {"if=","  Pathname for configuration modifications.",0,0},
      {"cfg="," Pathname for the float's present configuration.",0,0}
   };
   struct MissionParameters config = DefaultMission;

   debugbits=2;
   
   /* check for usage query */
   if (argc<2) 
   { 
      printf("usage:%s %s\n",argv[0],make_usage(cmd,NCMD));
      exit(0);
   }

   /* link the metacommands to the command line arguments */
   link_meta_cmds(cmd,NCMD,argc,argv);

   if (!cmd[cfg_].arg)
   {
      LogEntry(argv[0],"Unspecified current configuration; "
               "assuming default configuration.\n");
   }
   else
   { 
      LogEntry(argv[0],"Validating the float's current configuration.\n");
      
      if (configure(&config,cmd[cfg_].arg)<=0)
      {
         /* log an entry that the configuration file is invalid */
         LogEntry(argv[0],"The float's present configuration is invalid.\n"); exit(1);
      }
      else LogEntry(argv[0],"The float's current configuration is accepted.\n");
   }
   
   /* make sure that an input file was specified */
   if (!cmd[if_].arg)
   {
      LogEntry(argv[0],"No input configuration file specified.\n");
   }

   /* check configuration file */
   else
   {
      LogEntry(argv[0],"\n");
      LogEntry(argv[0],"Validating the float's new configuration.\n");
      
      if (configure(&config,cmd[if_].arg)<=0)
      {
         /* log an entry that the configuration file is invalid */
         LogEntry(argv[0],"Configuration file invalid.\n");
      }
      
      /* log an entry that the configuration file is good */
      else {LogEntry(argv[0],"Configuration file OK.\n");}
   }
   
   return 0;
}

