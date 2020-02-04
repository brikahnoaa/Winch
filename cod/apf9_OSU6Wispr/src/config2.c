#ifndef CONFIG_H
#define CONFIG_H (0x0100U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: config.c,v 1.19.2.1 2008/09/11 20:00:06 dbliudnikas Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/** RCS log of revisions to the C source code.
 *
 * \begin{verbatim}
 * $Log: config.c,v $
 * Revision 1.19.2.1HM.1 2009/11/02 Use User to change the gain and max 
 * detections.  During the profile cycle, if accumuated number of detection 
 * exceeds Max_Detect_Prof, float changes to the PROFILE state so that it can
 * ascent.
 * 
 * Revision 1.19.2.1  2008/09/11 20:00:06  dbliudnikas
 * Replace SBE41 with Seascan TD: PressureCp not applicable (default off).
 *
 * Revision 1.19  2008/07/14 16:52:12  swift
 * Added configurator for compensator hyper-retraction feature.
 *
 * Revision 1.18  2007/10/05 22:32:21  swift
 * Add configurators for the buoyancy nudge and initial buoyancy nudge.
 *
 * Revision 1.17  2007/05/08 18:10:16  swift
 * Added TimeOfDay feature to implement ability to schedule profiles to start
 * at a specified time of day.  Added attribution just below the copyright in
 * the main comment section.
 *
 * Revision 1.16  2007/01/15 20:30:59  swift
 * Modified the default configuration.
 *
 * Revision 1.15  2006/12/19 23:41:47  swift
 * Fixed a bug in the ConfigurationSupervisor that used the 'mission' object
 * rather than the 'cfg' object.
 *
 * Revision 1.14  2006/11/25 00:09:14  swift
 * Correction from FlashFsCreate to FlashCreate.
 *
 * Revision 1.13  2006/11/22 03:57:24  swift
 * Add a sanity check for the CP activation pressure to warn against spot
 * sampling in the main thermocline or above.
 *
 * Revision 1.12  2006/10/11 20:58:15  swift
 * Integrated the new flashio files system.
 *
 * Revision 1.11  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.10  2006/05/19 19:28:05  swift
 * Modifications to allow the bearer service type to be included in the AT dial string.
 *
 * Revision 1.9  2005/06/28 19:23:56  swift
 * Fixed some minor formatting errors in logentries.
 *
 * Revision 1.8  2005/06/27 15:15:00  swift
 * Added firmware revision to logentry.
 *
 * Revision 1.7  2005/06/23 20:37:16  swift
 * Fixed bug in range-check of the maximum air-bladder pressure.
 *
 * Revision 1.6  2005/06/23 20:32:56  swift
 * Added Added a two-way command to adjust the maximum air-bladder pressure.
 *
 * Revision 1.5  2005/06/14 19:01:15  swift
 * Added a log-entry when configuration supervisor rejects a configuration.
 *
 * Revision 1.4  2005/04/04 22:35:33  swift
 * Change maximum telemetry-retry interval to 6 hours.
 *
 * Revision 1.3  2005/03/30 20:24:37  swift
 * Changed MissionNazi() to ConfigSupervisor().
 *
 * Revision 1.2  2005/02/22 21:00:26  swift
 * Modifications to implement recovery mode.
 *
 * Revision 1.1  2004/12/29 23:11:26  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define configureChangeLog "$RCSfile: config.c,v $  $Revision: 1.19.2.1 $   $Date: 2008/09/11 20:00:06 $"

#include <time.h>
//#include <control.h>  //HM

#define CfgBufLen (63)


/*------------------------------------------------------------------------*/
/* structure to contain the parameters that define the float mission      */
/*------------------------------------------------------------------------*/
struct MissionParameters
{
   unsigned int   FloatId;      
   unsigned char  MaxAirBladder;
   unsigned char  OkVacuumCount;            
   unsigned char  PistonBuoyancyNudge;
   unsigned char  PistonDeepProfilePosition;
   unsigned char  PistonFullExtension;      
   unsigned char  PistonFullRetraction;      
   unsigned char  PistonInitialBuoyancyNudge;
   unsigned char  PistonParkHyperRetraction;
   unsigned char  PistonParkPosition;
   unsigned char  PistonStoragePosition;
   unsigned char  PnpCycleLength;
   float          PressurePark;
   float          PressureProfile;
   float          PressureCP;
   time_t         TimeDeepProfileDescent;
   time_t         TimeDown;
   time_t         TimeOutAscent;      
   time_t         TimeParkDescent;
   time_t         TimePrelude;
   time_t         TimeTelemetryRetry;
   time_t         TimeUp;
   time_t         ToD;
   time_t         ConnectTimeOut;    /* Seconds allowed to establish host connection */
   char           alt[CfgBufLen+1];  /* Alternate modem AT dialstring */
   char           at[CfgBufLen+1];   /* Modem AT dialstring */
   char           pwd[16];           /* Password for logging in to host computer */
   char           user[16];          /* Login name on host computer */
   unsigned int   crc;
};

/* external prototypes */
int  configure(struct MissionParameters *config, const char *fname); 
int  inRange(double a,double x,double b);
int  inCRange(double a,double x,double b);
int  LogConfiguration(const struct MissionParameters *config,const char *id);

/* define some constants of nature */
#define Hour   (3600L)
#define Day   (86400L)
#define Min      (60L)
#define HeartBeat (6L)

/* store the mission parameters in persistent non-far ram to facilitate CRC computations */
extern persistent struct MissionParameters mission;

/* declare an initialization object for use when the APFx loses its mind */
extern const struct MissionParameters DefaultMission;

/* declare the mission configuration filename */
extern const char config_path[];

/* declare the log filename */
extern persistent char log_path[32];

/* object to store the encoded firmware revision */
extern const unsigned long FwRev;

/* unit of time that defines the timescale of the mission (ie., 60sec or 3600sec) */
extern const time_t TQuantum;

/* the units of the time-quantization above (ie., minutes or hours) */
#define TUnits "Minutes"

/* estimated vertical ascent rate */
extern const float dPdt;

/* declarations for variables with external linkage */
extern const float MinN2ParkPressure;
  
#endif /* CONFIG_H */

/* define the quantum of time (should match TUnits above) */
const time_t TQuantum = Min;

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "crc16bit.h"
#include "extract.h"
#include <limits.h>
#include "logger.h"
#include "regex.h"
#include "strtoul.h"

#include "quecom.h"   //HM 
#include <cmds.h>     //HM

#ifdef _XA_
   #include "ds2404.h"
#else
   time_t ialarm(void);
#endif /* _XA_ */

/* structure to store the mission configuration */
persistent struct MissionParameters mission;

/* default mission configuration */
/* change default PressureCP to 0 - not appicable to Seascan PT module */
const struct MissionParameters DefaultMission =
{
   0,124,96,10,16,227,9,22,0,66,16,1,1000,2000,0,
   6*Hour,10*Day,9*Hour,6*Hour,6*Hour,15*Min,11*Hour,-1,
   1*Min,"ATDT0012066163256","ATDT0012066859312","e=2.718",
   "iridium"
};

/* define the name of the configuration file */
const char config_path[] = "mission.cfg";

/* define the name of the configuration file */
persistent char log_path[32];

/* define the estimated vertical ascent rate */
const float dPdt=0.08;

/* define the minimum park pressure for N2 floats */
const float MinN2ParkPressure=850;

/* prototypes for functions with static linkage */
static int ConfigSupervisor(struct MissionParameters *cfg);
static int fatal(const char *expr, const char *why);
static int warn(const char *expr, const char *why);

/* prototypes for functions with external linkage */
time_t itimer(void);
int    MissionParametersWrite(struct MissionParameters *mission);
void   PowerOff(time_t AlarmSec);
int    RecoveryInit(void);
int    fioFormat(void);
int    Tc58v64Init(void);

/*------------------------------------------------------------------------*/
/* function to parse a configuration file                                 */
/*------------------------------------------------------------------------*/
/**
   This function is designed a parse a configuration file and install
   configurator arguments into a MissionParameters object.  The ability to
   accomplish 2-way communication and remote control via the Iridium system
   was the major motivator for implementing configurable operation.  Remote
   control of the mission is accomplished by creating a configuration file
   on the float's host computer.  The syntax of configuration files is
   tightly controlled and accomplished through the use of "configurators".

   Strict syntax rules are rigidly enforced as a protective measure against
   accidental and perhaps fatal misconfiguration.  Every line in the
   configuration file must be either a blank line (ie., all white space), a
   comment (first non-whitespace character must be '\#'), or a well-formed
   configurator.  Configurators have a fixed syntax:

      \begin{verbatim}
      ParameterName(argument) [CRC]
      \end{verbatim}
  
   where ParameterName satisfies the regex "[a-zA-Z0-9]{1,31}" (ie., maximum
   of 31 characters), argument satisfies the regex ".*", and the [CRC] field
   is optional but, if present, then must satisfy the regex
   "\[(0x[0-9a-fA-F]{1,4})\]".  That is, the opening and closing brackets
   are literal characters "[]" that bracket a string that represents a 4-16
   bit hexidecimal number.  If the CRC field is present then it represents
   the 16-bit CRC of the configurator: "ParameterName(argument)".  The CRC
   of the configurator is computed and checked against the CRC specified in
   the configurator.  The CRCs must have the same value or else the
   configuration attempt fails.  The CRC is generated by the CCITT
   polynomial.  See comment section of Crc16Bit() for details.

   It is very important to note that any white space in the argument is
   treated as potentially significant.  Every byte (including white space)
   between the parentheses is considered to be a non-negligible part of the
   argument.  In cases where the argument string is converted to a number
   then the presence of extraneous white space won't matter.  However, if
   the argument represents, say, a login name or a passord then extraneous
   space would be fatal.
   
   Only one configurator per line is allowed and the configurator must be
   the left-most text on the line except that it can be preceeded by an
   arbitrary amount of whitespace.  No text, except for an arbitrary amount
   of white space, is allowed to the right of the rightmost closing
   parenthesis.  The maximum length of a line (including white space) is 126
   bytes and the maximum length of the ParameterName is 31 bytes.

   If any syntax error is detected in the configuration file or if the
   argument of a configurator failes a range check then the configuration
   attempt fails.  In this case then the (const) default configuration will
   be used.  The default configuration is hardwired into the source code to
   prevent it from being tampered with or corrupted in any way.

   \begin{verbatim}
   MissionParameters parameters:

      ActivateRecoveryMode ..... Induce the float into recovery mode and
                                 initiate telemetry at regular intervals
                                 given by the TelemetryRetry.  No argument
                                 is required and the argument, if present,
                                 is ignored.

      AirBladderMaxP ........... The cut-off pressure (in A/D counts) for
                                 air-bladder inflation.  The air pump will
                                 be deactivated when the air bladder
                                 pressure exceeds the cut-off.  The valid
                                 range is 1 to 240 counts.  

      AscentTimeOut ............ The initial segment of the uptime that is
                                 designated for profiling and vertical
                                 ascent.  If the surface has not been
                                 detected by the time this timeout expires
                                 then the profile will be aborted and the
                                 telemetry phase will begin.  The valid
                                 range is 1 minute to 10 hours.
                               
      AtDialCmd ................ The modem AT dialstring used to connect to
                                 the primary host computer.  
                               
      AltDialCmd ............... The modem AT dialstring used to connect to
                                 the alternate host computer. 

      BuoyancyNudge ............ The piston extension (counts) applied each
                                 time the ascent rate falls below the
                                 user-specified minimum.  This adds buoyancy
                                 to the float in order to maintain the
                                 ascent rate.

      CompensatorHyperRetraction Floats with N2-compensators require the
                                 piston to be hyper-retracted in order to
                                 descend from the surface to the park
                                 level.  The valid range is 0-254 counts.

      ConnectTimeOut ........... The number of seconds allowed after dialing
                                 for a connection to be established with the
                                 host computer.  The valid range is 30-300
                                 sec.

      *CpActivationP ............ The pressure where the control firmware
                                 transitions from subsampling the water
                                 column (in the deep water) to where the CP
                                 mode of the SBE41cp is activated for a high
                                 resolution profile to the surface.

                                 Important note: The SBE41CP is not designed
                                 for subsampling in the presence of
                                 significant temperature gradients.  The
                                 pump period for spot samples is
                                 insufficient to drive thermal mass errors
                                 down to an acceptable level.

          *NOT APPICABLE TO THE SEASCAN PT MODULE

      DeepProfileDescentTime ... This time determines the maximum amount of
                                 time allowed for the float to descent from
                                 the park pressure to the deep profile
                                 pressure.  The deep profile is initiated
                                 when the DeepProfileDescentTime expires or
                                 else the float reaches the deep profile
                                 pressure, whichever occurs first.  The
                                 valid range is 0-8 hours.

      DeepProfilePistonPos ..... The control firmware retracts the piston to
                                 the DeepProfilePistonPos in order to
                                 descend from the park pressure to the deep
                                 profile pressure.  The DeepProfilePistonPos
                                 should be set so that the float can reach
                                 the deep profile pressure before the deep
                                 profile descent period expires.  The valid
                                 range is 1-254 counts.

      DeepProfilePressure ...... This is the target pressure for deep
                                 profiles.  The valid range is 0-2000 dbar.

      DownTime ................. This determines the length of time that the
                                 float drifts at the park pressure before
                                 initiating a profile.  The valid range is 1
                                 min to 30 days.

      FlashErase ............... This command requires no argument and
                                 causes the FLASH memory chip to be
                                 reformatted.  WARNING: All contents of the
                                 FLASH file system will be destroyed.

      FlashCreate .............. This command requires no argument and
                                 causes the FLASH file system to be
                                 reinitialized.  This command is time
                                 consuming (~30 minutes) and
                                 energy-expensive.  The process involves
                                 writing a test pattern to each 8KB block of
                                 the FLASH ram and then re-reading the
                                 contents to ensure that the test pattern
                                 matches what was written.  If bad blocks
                                 are discovered then they are added to a
                                 bad-block list.  Blocks identified in the
                                 bad block list are not used for storage.
                                 WARNING: All contents of the FLASH file
                                 system will be destroyed.

      FloatId .................. The float identifier.
      
      InitialBuoyancyNudge ......The piston extension (counts) applied in
                                 order to initiate the vertical ascent at
                                 the beginning of the profile.  This same
                                 nudge is also applied at the end of the
                                 profile to make sure the float reaches the
                                 surface.

      MaxLogKb ................. The maximum size of the logfile in
                                 kilobytes.  Once the log grows beyond this
                                 size, logging is inhibited and the logfile
                                 will be automatically deleted at the start
                                 of the next profile.  The valid range is
                                 5-60KB.
                                 
      ParkDescentTime .......... This time determines the maximum amount of
                                 time allowed for the float to descent from
                                 the surface to the park pressure.  The
                                 active ballasting phase is initiated when
                                 the ParkDescentTime expires.  The valid
                                 range is 0-8 hours.

      ParkPistonPos ............ The control firmware retracts the piston to
                                 the ParkPistonPos in order to descend from
                                 the surface to the park pressure.  The
                                 ParkPistonPos should be set so that the
                                 float will become neutrally buoyant at the
                                 park pressure.  The valid range is 1-254
                                 counts.
                                 
      ParkPressure ............. This is the target pressure for the active
                                 ballasting algorithm during the park phase
                                 of the mission cycle.  The valid range is
                                 0-2000 dbar.

      PnPCycleLen .............. A deep profile is initiated when the
                                 profile id is an integral multiple of
                                 PnPCycleLen.  All other profiles will be
                                 collected from the park pressure to the
                                 surface. 
      
      Pwd ...................... The password used to login to the host
                                 computer.

      TelemetryRetry ........... This determines the time period between
                                 attempts to successfully complete telemetry
                                 tasks after each profile.  The valid range
                                 is 1 minute to 6 hours.

      TimeOfDay ................ This allows the user to specify that the
                                 down-time should expire at a specific time
                                 of day (ToD).  The ToD feature allows the
                                 user to schedule profiles to happen at
                                 night (for example).  

                                 The ToD is expressed as the number of
                                 minutes after midnight (GMT).  The valid
                                 range is 0-1439 minutes.  Any value outside
                                 this range will cause the ToD feature to be
                                 disabled.

      UpTime ................... This determines the maximum amount time
                                 allowed to execute the profile and complete
                                 telemetry.  The valid range is 1 minute to
                                 1 day.

      User ..................... The login name on the host computer that
                                 the float uses to upload and download data.
                                 itself.

      Verbosity ................ An integer in the range [0,4] that
                                 determines the logging verbosity with lower
                                 values producing more terse logging.  A
                                 verbosity of 2 yields standard logging.
                         
   \end{verbatim}

   A sample configuration file is below.
   
      \begin{verbatim}
      # Modem AT command to dial the phone
      AtDialCmd(ATDT12066859312) [0x0dc8]
      
      # Time-out period (seconds) for connect-attempt
      # ConnectTimeOut(60) [0xeb41]

      # Password  - It is now a QUEphone name for login (e.g., Q001)
      # Pwd(e=2.718) [0xb369]
      
      # user name (G1D000020T0400) G1 is gain. D0000020 is the number of detection
					per profile.  If the detection exceeds this value, QUEphone stops
					parking and starts profiling. T0400 is the threshold value can be 
					passed on to the DSP.
					
      # User(iridium) [0x835f]
      
      # Debug level for logging
      Verbosity(2) [0x7afc]

      # Maximum number of kilobytes for the log file
      # MaxLogKb(25) [0xDD06]
      \end{verbatim}

*/ 
int configure(struct MissionParameters *config, const char *fname) 
{
   static cc FuncName[] = "configure()";
   int status=-1;
   FILE *source=NULL;
   
   /* validate pointer to configuration object */
   if (!config)
   {
      /* create the message */
      static cc msg[]="Invalid configuration object.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* validate pointer to configuration file */
   else if (!fname)
   {
      /* create the message */
      static cc msg[]="Invalid configuration file.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* open the configuration file */
   else if (!(source=fopen(fname,"r")))
   {
      /* create the message */
      static cc format[]="Attempt to open \"%s\" failed.\n";

      /* log the message */
      LogEntry(FuncName,format,fname);

      /* reinitialize the function's return value */
      status=0;
   }
   
   else
   {
      #define MAXLEN (126)
      #define NSUB (6)
      int i,k,len;
      unsigned int crc1,crc0;
      char *p,buf[MAXLEN+2],configurator[MAXLEN+2],parm[CfgBufLen+1],arg[CfgBufLen+1];
      regex_t regex;
      regmatch_t regs[NSUB+1];
      int errcode;
      int RecoveryMode=0,Erase=0,FsCreate=0;
      struct MissionParameters cfg; 
      
      #define WS  "[ \t\r]*"
      #define VAR "([a-zA-Z0-9]+)"
      #define VAL "\\((.*)\\)"
      #define CRC "(\\[(0x[0-9a-fA-F]{1,4})\\])?"
   
      /* construct the regex pattern string for files with message locks */ 
      const char *pattern = "^" WS "((" VAR WS VAL ")" WS CRC ")" WS "$";
		
      /* define the configuration parameters */
      enum {ActivateRecoveryMode, AirBladderMaxP, AscentTimeOut, AtDialCmd, AltDialCmd,
            BuoyancyNudge, CompensatorHyperRetraction, ConnectTimeOut, CpActivationP,
            DeepProfileDescentTime, DeepProfilePistonPos, DeepProfilePressure, DownTime,
            FlashErase, FlashCreate, FloatId, InitialBuoyancyNudge, MaxLogKb, ParkDescentTime,
            ParkPistonPos, ParkPressure, PnPCycleLen, Pwd, TelemetryRetry, ToD, UpTime, User,
            Verbosity, NVAR};

      /* define the names of the configuration parameters */ 
      const char *parms[NVAR];

      /* reinitialize the return value */
      status=1;
		
      
      /* initialize the configuration */
      cfg=*config;
 
      /* create the metacommand table */
      parms[ActivateRecoveryMode]       = "activaterecoverymode";
      parms[AirBladderMaxP]             = "airbladdermaxp";
      parms[AscentTimeOut]              = "ascenttimeout";
      parms[AtDialCmd]                  = "atdialcmd";
      parms[AltDialCmd]                 = "altdialcmd";
      parms[BuoyancyNudge]              = "buoyancynudge";
      parms[CompensatorHyperRetraction] = "compensatorhyperretraction";
      parms[ConnectTimeOut]             = "connecttimeout";
      parms[CpActivationP]              = "cpactivationp";
      parms[DeepProfileDescentTime]     = "deepprofiledescenttime";
      parms[DeepProfilePistonPos]       = "deepprofilepistonpos";
      parms[DeepProfilePressure]        = "deepprofilepressure";
      parms[DownTime]                   = "downtime";
      parms[FlashErase]                 = "flasherase";
      parms[FlashCreate]                = "flashcreate";
      parms[FloatId]                    = "floatid";
      parms[InitialBuoyancyNudge]       = "initialbuoyancynudge";
      parms[MaxLogKb]                   = "maxlogkb";
      parms[ParkDescentTime]            = "parkdescenttime";
      parms[ParkPistonPos]              = "parkpistonpos";
      parms[ParkPressure]               = "parkpressure";
      parms[PnPCycleLen]                = "pnpcyclelen";
      parms[Pwd]                        = "pwd";
      parms[TelemetryRetry]             = "telemetryretry";
      parms[ToD]                        = "timeofday";
      parms[UpTime]                     = "uptime";
      parms[User]                       = "user";
      parms[Verbosity]                  = "verbosity";
		
      /* compile the option pattern */
      assert(regcomp(&regex,pattern,REG_EXTENDED|REG_NEWLINE)==0);
		
      /* protect against segfaults */
      assert(NSUB==regex.re_nsub);
      
      /* make a log entry that the configuration file will be parsed */
      if (debuglevel>=2 || (debugbits&CONFIG_H))
      {
         /* create the message */
         static cc format[]="Parsing configurators in \"%s\".\n";

         /* log the message */
         LogEntry(FuncName,format,fname);
      }
      
      /* read each line of the configuration file */
      while (fgets(buf,MAXLEN+2,source))
      {
         /* make sure the line length doesn't exceed maximum */
         if ((len=strlen(buf))>MAXLEN)
         {
            /* create the message */
            static cc format[]="Line length exceeds %d bytes: %s\n";

            /* make a log entry */
            LogEntry(FuncName,format,MAXLEN,buf);

            /* indicate failure of the configuration attempt */
            status=0; continue;
         }

         /* check if the current line matches the regex */
         if ((errcode=regexec(&regex,buf,regex.re_nsub+1,regs,0))==REG_NOMATCH)
         {
            /* find the location in buf of the first non-whitespace character */
            int index = strspn(buf," \t\r\n");
            
            /* check if current line is all white space */
            if (index!=len && buf[index]!='#')
            {
               /* copy the configurator to the log file */
               if (debuglevel>=2 || (debugbits&CONFIG_H))
               {
                  /* create the message */
                  static cc format[]="Syntax error: %s";

                  /* make a log entry */
                  LogEntry(FuncName,format,buf);
               }
               
               /* indicate failure of the configuration attempt */
               status=0;
            }
            
            continue;
         }

         /* check for pathological regex conditions */
         else if (errcode)
         {
            /* create the message */
            static cc format[]="Exception in regexec(): %s\n";

            /* map the regex error code to an error string */
            char errbuf[128]; regerror(errcode,&regex,errbuf,128);

            /* print the regex error string */
            LogEntry(FuncName,format,errbuf); 
 
            /* indicate failure of the configuration attempt */
            status=0;
         }

         /* regex match found - process the configurator */
         else
         {
            /* extract the configurator from the line */
            strcpy(configurator,extract(buf,regs[1].rm_so+1,regs[1].rm_eo-regs[1].rm_so));
         
            /* copy the configurator to the log file */
            if (debuglevel>=2 || (debugbits&CONFIG_H))
            {
               /* create the message */
               static cc format[]="%s ";

               /* write the configurator to the logfile */
               LogEntry(FuncName,format,configurator);
            }
            
            /* check length of parameter identifier */
            if ((regs[3].rm_eo-regs[3].rm_so)>CfgBufLen)
            {
              LogAdd("[Configurator name exceeds %d bytes].\n",CfgBufLen);

               /* indicate failure of the configuration attempt */
               status=0; continue;
            } 

            /* extract the parameter from the configurator */
            else strcpy(parm,extract(buf,regs[3].rm_so+1,regs[3].rm_eo-regs[3].rm_so));

            /* check length of parameter argument identifier */
            if ((regs[4].rm_eo-regs[4].rm_so)>CfgBufLen)
            {
               LogAdd("[Configurator argument exceeds %d bytes].\n",CfgBufLen);

               /* indicate failure of the configuration attempt */
               status=0; continue;
            } 

            /* extract the argument from the configurator */
            else strcpy(arg,extract(buf,regs[4].rm_so+1,regs[4].rm_eo-regs[4].rm_so));
            
            /* extract the configurator (less CRC) from the line */
            p = extract(buf,regs[2].rm_so+1,regs[2].rm_eo-regs[2].rm_so);
            
            /* compute the CRC of the configurator */
            crc0 = Crc16Bit((unsigned char *)p,regs[2].rm_eo-regs[2].rm_so);

            /* check for an optional CRC */
            if (regs[5].rm_so!=-1 && regs[5].rm_eo!=-1)
            {
               /* extract the hex CRC from the configurator */
               p = extract(buf,regs[6].rm_so+1,regs[6].rm_eo-regs[6].rm_so);
            
               /* convert the hex CRC to decimal */
               crc1 = strtoul(p,NULL,16);

               /* check if the CRCs match */
               if (crc0!=crc1) 
               {
                  if (debuglevel>=2 || (debugbits&CONFIG_H))
                  {
                     /* make a log entry that the CRC check failed */
                     LogAdd("[CRC check failed: 0x%04X != 0x%04X].\n",crc0,crc1);
                  }
                  
                  /* indicate failure of the configuration attempt */
                  status=0; continue;
               }
            }

            /* add the configurator CRC */
            else if (debuglevel>=2 || (debugbits&CONFIG_H)) LogAdd("[CRC=0x%04X] ",crc0);  
         
            /* convert the variable name to lower case */
            for (i=regs[3].rm_so; i<regs[3].rm_eo; i++) {buf[i] = tolower(buf[i]);}

            /* search the configuration for a matching parameter name */
            for (k=0; k<NVAR; k++)
            {
               /* check if the regex parameter matches a configuration parameter */
               if (!strncmp(parms[k],buf+regs[3].rm_so,strlen(parms[k]))) break;
            }

            switch (k)
            {
               /* activate recovery mode */
               case ActivateRecoveryMode:
               {
                  RecoveryMode=1;

                  /* make a log entry */
                  if (debuglevel>=2 || (debugbits&CONFIG_H)) LogAdd("[ActivateRecoveryMode()].\n");

                  break;
               }
               
               /* configurator for maximum air bladder pressure */
               case AirBladderMaxP:
               {
                  /* insert the timeout period into configuration */
                  cfg.MaxAirBladder=atoi(arg);

                  if (debuglevel>=2 || (debugbits&CONFIG_H))
                  {
                     /* make a log entry */                     
                     LogAdd("[AirBladderMaxP(%d)].\n",cfg.MaxAirBladder);
                  }
                  
                  break;
               }
               
               /* configurator for ascent time-out period */
               case AscentTimeOut: 
               {
                  /* insert the timeout period into configuration */
                  cfg.TimeOutAscent=atol(arg)*Min;

                  if (debuglevel>=2 || (debugbits&CONFIG_H))
                  {
                     /* make a log entry */                     
                     LogAdd("[AscentTimeOut(%ld)].\n",cfg.TimeOutAscent/Min);
                  }
                  
                  break;
               }

               /* Modem AT dialstring configurator */
               case AtDialCmd: 
               {
                  /* copy the dialstring into static storage and insert into configuration */
                  strncpy(cfg.at,arg,sizeof(cfg.at)-1);

                  /* make a log entry */
                  if (debuglevel>=2 || (debugbits&CONFIG_H)) LogAdd("[AtDialCmd(%s)].\n",cfg.at);
               
                  break;
               }

               /* Modem AT dialstring configurator for alternate host */
               case AltDialCmd: 
               {
                  /* copy the dialstring into static storage and insert into configuration */
                  strncpy(cfg.alt,arg,sizeof(cfg.alt)-1);

                  /* make a log entry */
                  if (debuglevel>=2 || (debugbits&CONFIG_H)) LogAdd("[AltDialCmd(%s)].\n",cfg.alt);
               
                  break;
               }

               /* configurator for buoyancy nudge */
               case BuoyancyNudge:
               {
                  /* insert the buoyancy nudge into configuration */
                  cfg.PistonBuoyancyNudge=atoi(arg);

                  if (debuglevel>=2 || (debugbits&CONFIG_H))
                  {
                     /* make a log entry */                     
                     LogAdd("[BuoyancyNudge(%d)].\n",cfg.PistonBuoyancyNudge);
                  }
                  
                  break;
               }
               
               /* configurator for descent hyper-retraction */
               case CompensatorHyperRetraction:
               {
                  /* insert the park pressure into configuration */
                  cfg.PistonParkHyperRetraction=atoi(arg);

                  if (debuglevel>=2 || (debugbits&CONFIG_H))
                  {
                     /* make a log entry */                     
                     LogAdd("[CompensatorHyperRetraction(%d)].\n",
                            cfg.PistonParkHyperRetraction);
                  }
                  
                  break;
               }
               
               /* configurator for connection time-out period */
               case ConnectTimeOut: 
               {
                  /* insert the timeout period into configuration */
                  cfg.ConnectTimeOut=atol(arg);

                  if (debuglevel>=2 || (debugbits&CONFIG_H))
                  {
                     /* make a log entry */                     
                     LogAdd("[ConnectTimeOut(%ld)].\n",cfg.ConnectTimeOut);
                  }
                  
                  break;
               }

               /* configurator for SBE41cp activation pressure */
               /* NOT APPLICABLE TO THE SEASCAN PT MODULE */
               case CpActivationP:
               {
                  cfg.PressureCP = 0;

                  /* make a log entry */
                  if (debuglevel>=2 || (debugbits&CONFIG_H))
                  {
                     LogAdd("[CpActivationP(%g)].\n",cfg.PressureCP);
                  }

                  break;
               }

               /* configurator for profilek descent time */
               case DeepProfileDescentTime:
               {
                  /* insert the downtime into configuration */
                  cfg.TimeDeepProfileDescent=atol(arg)*Min;

                  if (debuglevel>=2 || (debugbits&CONFIG_H))
                  {
                     /* make a log entry */                     
                     LogAdd("[DeepProfileDescentTime(%ld)].\n",
                            cfg.TimeDeepProfileDescent/Min);
                  }
                  
                  break;
               }

               /* configurator for park pressure */
               case DeepProfilePistonPos:
               {
                  /* insert the park pressure into configuration */
                  cfg.PistonDeepProfilePosition=atoi(arg);

                  if (debuglevel>=2 || (debugbits&CONFIG_H))
                  {
                     /* make a log entry */                     
                     LogAdd("[DeepProfilePistonPos(%d)].\n",
                            cfg.PistonDeepProfilePosition);
                  }
                  
                  break;
               }

               /* configurator for profile pressure */
               case DeepProfilePressure:
               {
                  /* insert the profile pressure into configuration */
                  cfg.PressureProfile=atof(arg);

                  /* make a log entry */
                  if (debuglevel>=2 || (debugbits&CONFIG_H))
                  {
                     LogAdd("[ProfilePressure(%g)].\n",cfg.PressureProfile);
                  }

                  break;
               }

               /* configurator for profile cycle downtime */
               case DownTime:
               {
                  /* insert the downtime into configuration */
                  cfg.TimeDown=atol(arg)*Min;

                  if (debuglevel>=2 || (debugbits&CONFIG_H))
                  {
                     /* make a log entry */
                     LogAdd("[DownTime(%ld)].\n",cfg.TimeDown/Min);
                  }
                  
                  break;
               }

               /* configurator to reformat the flash chip */
               case FlashErase:
               {
                  Erase=1;

                  /* make a log entry */
                  if (debuglevel>=2 || (debugbits&CONFIG_H)) LogAdd("[FlashErase()].\n");

                  break;
               }

               /* configurator to reinitialize the flash file system */
               case FlashCreate:
               {
                  FsCreate=1;

                  /* make a log entry */
                  if (debuglevel>=2 || (debugbits&CONFIG_H)) LogAdd("[FlashCreate()].\n");

                  break;
               }
               
               /* configurator for the float id */
               case FloatId:
               {
                  /* insert the downtime into configuration */
                  cfg.FloatId=atoi(arg);

                  if (debuglevel>=2 || (debugbits&CONFIG_H))
                  {
                     /* make a log entry */                     
                     LogAdd("[FloatId(%d)].\n",cfg.FloatId);
                  }
                  
                  break;
               }

               /* configurator for initial buoyancy nudge */
               case InitialBuoyancyNudge:
               {
                  /* insert the initial buoyancy nudge into configuration */
                  cfg.PistonInitialBuoyancyNudge=atoi(arg);

                  if (debuglevel>=2 || (debugbits&CONFIG_H))
                  {
                     /* make a log entry */                     
                     LogAdd("[InitialBuoyancyNudge(%d)].\n",
                            cfg.PistonInitialBuoyancyNudge);
                  }
                  
                  break;
               }
 
               /* configurator for park descent time */
               case ParkDescentTime:
               {
                  /* insert the park descent time into configuration */
                  cfg.TimeParkDescent=atol(arg)*Min;

                  if (debuglevel>=2 || (debugbits&CONFIG_H))
                  {
                     /* make a log entry */                     
                     LogAdd("[ParkDescentTime(%ld)].\n",cfg.TimeParkDescent/Min);
                  }
                  
                  break;
               }

               /* configurator for park piston position */
               case ParkPistonPos:
               {
                  /* insert the park piston position into configuration */
                  cfg.PistonParkPosition=atoi(arg);

                  if (debuglevel>=2 || (debugbits&CONFIG_H))
                  {
                     /* make a log entry */                     
                     LogAdd("[ParkPistonPos(%d)].\n",cfg.PistonParkPosition);
                  }
                  
                  break;
               }

               /* configurator for park pressure */
               case ParkPressure:
               {
                  /* insert the park pressure into configuration */
                  cfg.PressurePark=atof(arg);

                  if (debuglevel>=2 || (debugbits&CONFIG_H))
                  {
                     /* make a log entry */                     
                     LogAdd("[ParkPressure(%g)].\n",cfg.PressurePark);
                  }
                  
                  break;
               }

               /* configurator for PnP cycle length */
               case PnPCycleLen:
               {
                  /* insert the PnP cycle length into configuration */
                  cfg.PnpCycleLength=atoi(arg);

                  if (debuglevel>=2 || (debugbits&CONFIG_H))
                  {
                     /* make a log entry */                     
                     LogAdd("[PnpCycleLength(%d)].\n",cfg.PnpCycleLength);
                  }
                  
                  break;
               }

               /* password configurator */
               case Pwd: 
               {
                  /* copy the password into the configuration */
                  strncpy(cfg.pwd,arg,sizeof(cfg.pwd)-1);

                  /* make a log entry */
                  if (debuglevel>=2 || (debugbits&CONFIG_H)) LogAdd("[Pwd(%s)].\n",cfg.pwd);
               
                  break;
               }

               /* configurator for telemetry retry period */
               case TelemetryRetry:
               {
                  /* insert the telemetryretry into configuration */
                  cfg.TimeTelemetryRetry=atol(arg)*Min;

                  if (debuglevel>=2 || (debugbits&CONFIG_H))
                  {
                     /* make a log entry */                     
                     LogAdd("[TelemetryRetry(%ld)].\n",cfg.TimeTelemetryRetry/Min);
                  }
                  
                  break;
               }

               /* configurator for profile time-of-day specification */
               case ToD:
               {
                  long int tod=atol(arg);
                  
                  /* insert the time-of-day specification into configuration */
                  cfg.ToD=(inRange(0,tod,1440)) ? (tod*Min) : -1;
                  
                  if (!inRange(0,cfg.ToD,Day))
                  {
                     LogAdd("Time-of-day specification disabled.\n");
                  }
                  
                  else if (debuglevel>=2 || (debugbits&CONFIG_H))
                  {
                     /* make a log entry */
                     LogAdd("[TimeOfDay(%ld)].\n",cfg.ToD/Min);
                  }
                     
                  break;
               }

               /* configurator for profile cycle uptime */
               case UpTime:
               {
                  /* insert the uptime into configuration */
                  cfg.TimeUp=atol(arg)*Min;

                  /* make a log entry */
                  if (debuglevel>=2 || (debugbits&CONFIG_H)) LogAdd("[UpTime(%ld)].\n",cfg.TimeUp/Min);

                  break;
               }
               
               /* user configurator */
               case User: 
               {
                  /* copy the username into the configuration */
                  strncpy(cfg.user,arg,sizeof(cfg.user)-1);

                  /* make a log entry */
                  if (debuglevel>=2 || (debugbits&CONFIG_H)) LogAdd("[User(%s)].\n",cfg.user);
               
                  break;
               }

               /* Logging verbosity configurator */
               case Verbosity: 
               {
                  /* convert the string to an integer */
                  long int verbosity=strtoul(arg,NULL,16);

                  /* validate the verbosity specification */
                  if (verbosity>=0 && (verbosity&VERBOSITYMASK)<=4 && verbosity<=UINT_MAX)
                  {
                     if (debuglevel>=2 || (debugbits&CONFIG_H))
                     {
                        /* make a log entry warning of terse debuggin mode */
                        if (!verbosity) LogAdd("[Warning: terse mode enabled - "
                                               "Verbosity(0x%04x)].\n",verbosity);

                        /* make a log entry */
                        else LogAdd("[Verbosity(0x%04x)].\n",verbosity);
                     }
                     
                     /* insert debug level into configuration */
                     debugbits=verbosity;
                  }
                  else {LogAdd("[Ignoring invalid verbosity: 0x%04x].\n",verbosity);}
                     
                  break;
               }

               /* Maximum log size configurator */
               case MaxLogKb:
               {
                  /* convert the string to an integer */
                  int maxlogkb = atoi(arg);

                  /* validate the maximum log size */
                  if (maxlogkb>=5 && maxlogkb<=63) 
                  {
                     /* insert debug level into configuration */
                     MaxLogSize=maxlogkb*1024L;

                     /* make a log entry that maximum log size */
                     if (debuglevel>=2 || (debugbits&CONFIG_H)) LogAdd("[MaxLogKb(%d)].\n",maxlogkb);
                  }
                  else {LogAdd("[Ignoring invalid log size limit: %d Kbytes].\n",maxlogkb);}
                  
                  break;
               }
               
               default:
               {
                  /* make a log entry */
                  if (debuglevel>=2 || (debugbits&CONFIG_H)) LogAdd("[Unrecognized configurator].\n");

                  /* indicate configuration failure */
                  status=0;
               }
            }
         }
      }

      /* close the configuration file */
      fclose(source);
      
      /* clean up the regex pattern buffer */
      regfree(&regex);

      /* validate config file against CRC and syntax errors */
      if (status>0)
      {
         if (debuglevel>=2 || (debugbits&CONFIG_H))
         {
            /* create the message */
            static cc msg[]="Configuration CRCs and syntax OK.\n";

            /* log the message */
            LogEntry(FuncName,msg);
         }

         /* subject the proposed configuration to the mission nazi */
         if ((status=ConfigSupervisor(&cfg))>0)
         {
            /* determine if the configurations are different */
            if (config->crc != cfg.crc)
            {
               /* activate the local/temporary configuration */
               *config=cfg;

               /* write the new mission configuration to EEPROM */
               if (MissionParametersWrite(config)<=0)
               {
                  static cc msg[]="Attempt to write mission "
                     "configuration to EEPROM failed.\n";
                  LogEntry(FuncName,msg);
               }

               /* Log the new mission configuration */
               else LogConfiguration(config,"Mission");
            }
         }
         else
         {
            /* log the failure */
            static cc msg[]="Configuration rejected by configuration supervisor.\n";
            LogEntry(FuncName,msg);
         }

         /* check if the flash should be reinitialized */
         if (FsCreate>0)
         {
            {static cc msg[]="Creating the FLASH file system.\n"; LogEntry(FuncName,msg);}

            /* reinitialize the flash file system */
            Tc58v64Init();

            {static cc msg[]="FLASH file system created\n"; LogEntry(FuncName,msg);}
         }

         /* check if the flash should be reformatted */
         else if (Erase>0)
         {
            /* reformat the flash chip */
            fioFormat();
            
            {static cc msg[]="All files in FLASH file system erased\n"; LogEntry(FuncName,msg);}
         }
         
         /* check if recovery mode is enabled */
         if (RecoveryMode>0) 
         {
            time_t SeqTime=itimer(),alarm=ialarm();
               
            /* create the message */
            static cc format[]="Recovery mode activated. [itimer:%ld, alarm:%ld]\n";

            /* make a logentry */
            RecoveryInit(); LogEntry(FuncName,format,SeqTime,alarm);
               
            /* get the sequence time */ 
            SeqTime=itimer() + HeartBeat; 
             
            /* compute the next regularly scheduled alarm-time */
            alarm = SeqTime - SeqTime%cfg.TimeTelemetryRetry + cfg.TimeTelemetryRetry;
               
            /* set the alarm and go to sleep */
            PowerOff(alarm);
         }
      }
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to analyze the mission parameters for internal consistency    */
/*------------------------------------------------------------------------*/
/**
   This function is the mission Nazi that analyzes the mission configuration
   for intenal consistency, prints warnings when problems are detected, and
   computes a status metric that can be used to determine if the mission
   should be allowed to proceed.  Two classes of violations are currently
   implemented; those that are fatal and those that are merely insane.
   Mission configurations that have fatal violations should not be allowed
   to proceed.  Mission configurations that are computed to be insane should
   proceed only after careful consideration of each warning.
*/
static int ConfigSupervisor(struct MissionParameters *cfg)
{
   static cc FuncName[]="ConfigSupervisor()";
   int status=1;
   
   /* assertion-like macro to trap fatal violations of mission constraints */
   #define Fatal(why,expr) ((expr) ? 0 : fatal(#expr,why))

   /* assertion-like macro to trap violations of mission sanity checks */
   #define Warn(why,expr)  ((expr) ? 0 : warn(#expr,why))

   /* compute the signature of the mission program */
   cfg->crc = Crc16Bit((unsigned char *)(&cfg), sizeof(cfg)-sizeof(cfg->crc));
    
   /* these are absolute range constraints that should be self explanatory */
   if (Fatal(NULL, inCRange(      0, debuglevel,                       5))) status=0;
   if (Fatal(NULL, inCRange(    240, cfg->MaxAirBladder,               1))) status=0;
   if (Fatal(NULL, inCRange(    254, cfg->PistonBuoyancyNudge,         1))) status=0;
   if (Fatal(NULL, inCRange(    254, cfg->PistonDeepProfilePosition,   1))) status=0;
   if (Fatal(NULL, inCRange(    254, cfg->PistonInitialBuoyancyNudge,  1))) status=0;
   if (Fatal(NULL, inCRange(    254, cfg->PistonParkHyperRetraction,   0))) status=0;
   if (Fatal(NULL, inCRange(    254, cfg->PistonParkPosition,          1))) status=0;
   if (Fatal(NULL, inCRange(    254, cfg->PnpCycleLength,              1))) status=0;
   if (Fatal(NULL, inCRange(   2000, cfg->PressurePark,                0))) status=0;
   if (Fatal(NULL, inCRange(   2000, cfg->PressureProfile,             0))) status=0;
   if (Fatal(NULL, inCRange( 8*Hour, cfg->TimeDeepProfileDescent,      0))) status=0;
   if (Fatal(NULL, inRange(  30*Day, cfg->TimeDown,                  Min))) status=0;
   if (Fatal(NULL, inCRange(10*Hour, cfg->TimeOutAscent,             Min))) status=0;
   if (Fatal(NULL, inRange(  8*Hour, cfg->TimeParkDescent,             0))) status=0;
   if (Fatal(NULL, inCRange( 6*Hour, cfg->TimeTelemetryRetry,        Min))) status=0;
   if (Fatal(NULL, inRange( 24*Hour, cfg->TimeUp,                    Min))) status=0;
   if (Fatal(NULL, inCRange(    300, cfg->ConnectTimeOut,             30))) status=0;

   /****** Constraints to protect against potentially fatal pilot errors *********/
   
   /* the down-time has to be greater than the park-descent time + deep-profile descent time */
   if (Fatal(NULL, cfg->TimeDown > cfg->TimeParkDescent+cfg->TimeDeepProfileDescent)) status=0;

   /* the up-time has to be greater than the ascent time-out period */
   if (Fatal(NULL, cfg->TimeUp > cfg->TimeOutAscent)) status=0;

   /* make sure there's enough uptime to do the deep profile and the telemetry */
   if (Fatal(NULL, cfg->TimeUp >= (cfg->PressureProfile/dPdt) + 2*Hour)) status=0;

   /* make sure there's enough uptime to do the park profile and the telemetry */
   if (Fatal(NULL, cfg->TimeUp >= (cfg->PressurePark/dPdt) + 2*Hour)) status=0;

   /* make sure the profile pressure is not less than the park pressure */
   if (Fatal(NULL, cfg->PressureProfile >= cfg->PressurePark)) status=0;

   /* validate the dial command s*/
   if (Fatal(NULL, !strncmp(cfg->at,"AT",2))) status=0;
   if (Fatal(NULL, !strncmp(cfg->alt,"AT",2))) status=0;
   
   /****** Sanity checks *********/

   /* allow time for completion of deep profile */
   Warn(NULL, cfg->TimeOutAscent >= (cfg->PressureProfile/dPdt) + 1*Hour);

   /* allow time for completion of park profile */
   Warn(NULL, cfg->TimeOutAscent >= (cfg->PressurePark/dPdt) + 1*Hour);

   /* allow time for telemetry */
   Warn(NULL, cfg->TimeUp >= cfg->TimeOutAscent + 2*Hour);

   /* allow time for descent to park depth */
   Warn(NULL, cfg->TimeParkDescent >= cfg->PressurePark/dPdt);

   /* allow time for descent to profile depth */
   Warn(NULL, cfg->TimeDeepProfileDescent >= (cfg->PressureProfile-cfg->PressurePark)/dPdt);

   /* allow time for at least one active-ballast adjustment */
   Warn(NULL, cfg->TimeDown > cfg->TimeDeepProfileDescent + cfg->TimeParkDescent + 2*Hour);

   /* check for overly conservative park-descent period */
   Warn(NULL, cfg->TimeParkDescent <= 1.5*cfg->PressurePark/dPdt + 1*Hour);

   /* check for overly conservative deep-profile-descent period */
   Warn(NULL, cfg->TimeDeepProfileDescent <= 1.5*(cfg->PressureProfile-cfg->PressurePark)/dPdt + 1*Hour);

   /* check for sensible piston postions */
   Warn(NULL, cfg->PistonDeepProfilePosition <= cfg->PistonParkPosition);

   /* if N2 compressee then check for sensible park pressure  */
   if (cfg->PistonParkHyperRetraction) Warn(NULL,cfg->PressurePark >= MinN2ParkPressure);

   /* check for sensible maximum air bladder pressure */
   Warn(NULL, inCRange(120,cfg->MaxAirBladder,128));

   if (status>0)
   {
      static cc msg[]="Configuration accepted.\n"; LogEntry(FuncName,msg);

      /* compute the signature of the mission program */
      cfg->crc = Crc16Bit((unsigned char *)(cfg),sizeof(*cfg)-sizeof(cfg->crc));
   }
   else {static cc msg[]="Configuration rejected.\n"; LogEntry(FuncName,msg);}
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to notify the user of a mission sanity check violation        */
/*------------------------------------------------------------------------*/
/*
   This function is called by ConfigSupervisor() to notify the user of a
   mission sanity check violation.
*/
static int warn(const char *expr, const char *why)
{
   static cc FuncName[]="warn()";
   
   if (!expr)
   {
      static cc msg[]="NULL function argument.\n";
      
      LogEntry(FuncName,msg);
   }
   
   else
   {
      static cc FuncName[]="ConfigSupervisor()";
      static cc format[]="Sanity check violated: %s\n";
      LogEntry(FuncName,format,expr);

      if (why && *why) {static cc format[]="  (%s)\n"; LogEntry(FuncName,format,why);}
   }
   
   return 1;
}

/*------------------------------------------------------------------------*/
/* function to notify the user of a fatal mission constraint violation    */
/*------------------------------------------------------------------------*/
/*
   This function is called by ConfigSupervisor() to notify the user of a fatal
   mission constraint violation.
*/
static int fatal(const char *expr, const char *why)
{
   static cc FuncName[]="warn()";
   
   if (!expr) 
   {
      static cc msg[]="NULL function argument.\n";
      
      LogEntry(FuncName,msg);
   }
   else
   {
      static cc FuncName[]="ConfigSupervisor()";
      static cc format[]="Constraint violated: %s\n";

      LogEntry(FuncName,format,expr);

      if (why && *why) {static cc format[]="  (%s)\n"; LogEntry(FuncName,format,why);}
   }
   
   return 1;
}

 
/*------------------------------------------------------------------------*/
/* function to determine if a number is within a closed interval          */
/*------------------------------------------------------------------------*/
/*
  This function determines whether a number (x) is in the closed interval
  of [a,b].  The function returns true if (x) is within the range and false
  otherwise.

  written by Dana Swift, October 1995
*/
int inCRange(double a,double x,double b)
{
   return ( ((x-a)*(x-b)<=0) ? 1 : 0);
}

/*------------------------------------------------------------------------*/
/* function to determine if a number is within a semiclosed range         */
/*------------------------------------------------------------------------*/
/*
  This function determines whether a number (x) is in the semi-closed range
  of [a,b).  The function returns true if (x) is within the range and false
  otherwise.

  written by Dana Swift, March 1993
*/
int inRange(double a,double x,double b)
{
   return ( ((x-a)*(x-b)<=0 && x!=b) ? 1 : 0);
}

/*------------------------------------------------------------------------*/
/* function to write the configuration to the log stream                  */
/*------------------------------------------------------------------------*/
//Added activation command here 11/15/2010 HM
//Changed the User name as a mean to change Gain and max detection number.  
//When the float receives a new command User(G2D000020) for example, 
//it changes Gain=2, and Max_Detect_Prof=20 and sends message to DSP.  HM 11/2/2009

int LogConfiguration(const struct MissionParameters *config,const char *id)
{
   static cc FuncName[] = "LogConfig..()";
   int status=-1;
   char ChGain[2], ChMax_Detect_Prof[6], ChDspPwrDpt[5];
   
   /* validate the MissionParameters pointer */
   if (!config)
   {
      /* create the message */
      static cc msg[]="NULL configuration.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* validate the configuration id */
   else if (!id) 
   {
      /* create the message */
      static cc msg[]="NULL configuration id.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   else
   {
      static cc cfg[]="%s configuration for Apf9i FwRev %06lx:\n"; 
	  static cc indent[]=" ";//HM removed 3 spaces 
      
      /* write the configuration to the log file */
      LogEntry(FuncName,cfg,id,FwRev);
      LogEntry(FuncName,indent); LogAdd("AscentTimeOut(%ld) [min]\n",config->TimeOutAscent/Min);
      LogEntry(FuncName,indent); LogAdd("AtDialCmd(%s) [primary]\n",config->at);
      LogEntry(FuncName,indent); LogAdd("AtDialCmd(%s) [alternate]\n",config->alt);
      LogEntry(FuncName,indent); LogAdd("BuoyancyNudge(%u) [count]\n",config->PistonBuoyancyNudge);
      LogEntry(FuncName,indent); LogAdd("BuoyancyNudgeInitial(%u) [count]\n",config->PistonInitialBuoyancyNudge);
      LogEntry(FuncName,indent); LogAdd("ConnectTimeOut(%ld) [sec]\n",config->ConnectTimeOut);
      LogEntry(FuncName,indent); LogAdd("CpActivationP(%g) [dbar]\n",config->PressureCP);
      LogEntry(FuncName,indent); LogAdd("DeepProfileDescentTime(%ld) [min]\n",config->TimeDeepProfileDescent/Min);
      LogEntry(FuncName,indent); LogAdd("DeepProfilePistonPos(%d) [count]\n",config->PistonDeepProfilePosition);
      LogEntry(FuncName,indent); LogAdd("DeepProfilePressure(%g) [dbar]\n",config->PressureProfile);
      LogEntry(FuncName,indent); LogAdd("CompensatorHyperRetraction(%d) [count]\n",config->PistonParkHyperRetraction);
      LogEntry(FuncName,indent); LogAdd("DownTime(%ld) [min]\n",config->TimeDown/Min);
      LogEntry(FuncName,indent); LogAdd("FloatId(%04u)\n",config->FloatId);
      LogEntry(FuncName,indent); LogAdd("FullExtension(%u) [count]\n",config->PistonFullExtension);
      LogEntry(FuncName,indent); LogAdd("FullRetraction(%u) [count]\n",config->PistonFullRetraction);
      LogEntry(FuncName,indent); LogAdd("MaxAirBladder(%u) [count]\n",config->MaxAirBladder);
      LogEntry(FuncName,indent); LogAdd("MaxLogKb(%ld) [KByte]\n",MaxLogSize/1024);
      LogEntry(FuncName,indent); LogAdd("MissionPrelude(%ld) [min]\n",config->TimePrelude/Min);
      LogEntry(FuncName,indent); LogAdd("OkVacuum(%u) [count]\n",config->OkVacuumCount);
      LogEntry(FuncName,indent); LogAdd("ParkDescentTime(%ld) [min]\n",config->TimeParkDescent/Min);
      LogEntry(FuncName,indent); LogAdd("ParkPistonPos(%d) [count]\n",config->PistonParkPosition);
      LogEntry(FuncName,indent); LogAdd("ParkPressure(%g) [dbar]\n",config->PressurePark);
      LogEntry(FuncName,indent); LogAdd("PnPCycleLen(%d)\n",config->PnpCycleLength);
      //LogEntry(FuncName,indent); LogAdd("Pwd(0x%04x)\n",
                                        //Crc16Bit((const unsigned char *)config->pwd,strlen(config->pwd)));
      LogEntry(FuncName,indent); LogAdd("Pwd(%s)\n",config->pwd);//HM
				
      LogEntry(FuncName,indent); LogAdd("TelemetryRetry(%ld) [min]\n",config->TimeTelemetryRetry/Min);

      LogEntry(FuncName,indent); if (inRange(0,config->ToD,Day)) LogAdd("TimeOfDay(%ld) [min]\n",config->ToD/Min);
                                 else LogAdd("TimeOfDay(DISABLED) [min]\n");
      
      LogEntry(FuncName,indent); LogAdd("UpTime(%ld) [min]\n",config->TimeUp/Min);
      LogEntry(FuncName,indent); LogAdd("User(%s)\n",config->user);
	  
	  //Check if Reactivating command
	  if(config->user[0]=='E' || config->user[0]=='e')
		    /* Re-execute the mission */ 
		{
            /* read the mission state from EEPROM */
            enum State state = StateGet();
            
            /* check if the mission is presently inactive */
            //if (state<=INACTIVE)
			if(RecoveryMode>0)
			{
				/* create the message */
				static cc msg[]="Restarting mission activation sequence.\n";

				/* log the message */
				LogEntry(FuncName,msg);
             
				/* put the float into surface diagnostics mode */
				if (MissionLaunch()>0)
					{
					/* notify that the mission has been activated */
					/* create the message */
					static cc msg[]="Mission reactivated.\n";

					/* log the message */
					LogEntry(FuncName,msg);

					/* set the float to exit the command loop */
					SetAlarm(itimer()+7); status=0;
				}
               else {
					static cc msg[]="Attempt to activate mission failed.\n";
					/* log the message */
					LogEntry(FuncName,msg);
			   
				}
			}	

			/* notify the user that the mission was already active */
			else {
					static cc msg[]="Useless request. The mission is already active.\n";
					/* log the message */
					LogEntry(FuncName,msg);
			   
					}
				break;
			}
		}
	  
		//Scan the new Gain and Detection parameters HM
		else if(config->user[0]=='G' || config->user[0]=='g') ChGain[0]=config->user[1];
		{
		 else ChGain[0]='0';//illegal character set gain=0 //HM
		  if(config->user[2]=='D' || config->user[2]=='d'){
			ChMax_Detect_Prof[0]=config->user[3];
			ChMax_Detect_Prof[1]=config->user[4];
			ChMax_Detect_Prof[2]=config->user[5];
			ChMax_Detect_Prof[3]=config->user[6];
			ChMax_Detect_Prof[4]=config->user[7];
			ChMax_Detect_Prof[5]=config->user[8];		
			} else { //if illegal characater
			ChMax_Detect_Prof[0]='9'; 
			ChMax_Detect_Prof[1]='9';
			ChMax_Detect_Prof[2]='9';
			ChMax_Detect_Prof[3]='9';
			ChMax_Detect_Prof[4]='9';// set Max_detection to write file = 20//HM
			ChMax_Detect_Prof[5]='9';	
			}
		  if(config->user[9]=='P' || config->user[9]=='p'){
			//Set a new DSP power ON/OFF depth 
			ChDspPwrDpt[0]=config->user[10];
			ChDspPwrDpt[1]=config->user[11];
			ChDspPwrDpt[2]=config->user[12];
			ChDspPwrDpt[3]=config->user[13];
			} else { //if illegal characater
			ChDspPwrDpt[0]='0'; 
			ChDspPwrDpt[1]='1';
			ChDspPwrDpt[2]='0';
			ChDspPwrDpt[3]='0';
			}
		  //sscanf(config->user,"G%sD%s",ChGain, ChMax_Detect_Prof);//HM
		  Gain=atoi(ChGain);	//define a new gain
		  Max_Detect_Prof=atol(ChMax_Detect_Prof);//define a new max number of detects per profile
		  DspPwrDpt=atoi(ChDspPwrDpt);
		  
		  LogEntry(FuncName,indent); LogAdd("Gain=%d, Max # of Detect/Prof=%ld, DSP ON/OFF Depth =%d\n", Gain, Max_Detect_Prof, DspPwrDpt);//HM
		  //check if the new gain is reasonable.  Definition of DspPwrDpt is undecided yet. Use bogus number for now.
		  if(Gain<10 && Gain >=0 && DspPwrDpt<2000 && DspPwrDpt>=0) {QuecomSetNCM(Gain, DspPwrDpt);}
		  //End HM
		  
		  LogEntry(FuncName,indent); LogAdd("Verbosity(%d)\n",debuglevel);
		  LogEntry(FuncName,indent); LogAdd("DebugBits(0x%04x)\n",debugbits);
	  }
      /* reinitialize the function's return value */
      status=1;
   }
   
   return status;
}
/*------------------------------------------------------------------------*/
/* function to write the configuration to the log stream                  */
/*------------------------------------------------------------------------*/
//Changed the User name as a mean to change Gain and max detection number.  
//When the float receives a new command User(G2D000020) for example, 
//it changes Gain=2, and Max_Detect_Prof=20 and sends message to DSP.  HM 11/2/2009

//int LogConfiguration(const struct MissionParameters *config,const char *id)
//{
//   static cc FuncName[] = "LogConfig..()";
//   int status=-1;
//   char ChGain[2], ChMax_Detect_Prof[6], ChDspPwrDpt[5];
   
//   /* validate the MissionParameters pointer */
//   if (!config)
//   {
//      /* create the message */
//      static cc msg[]="NULL configuration.\n";

//      /* log the message */
//      LogEntry(FuncName,msg);
//   }

//   /* validate the configuration id */
//   else if (!id) 
//   {
//      /* create the message */
//      static cc msg[]="NULL configuration id.\n";

//      /* log the message */
//      LogEntry(FuncName,msg);
//   }

//   else
//   {
//      static cc cfg[]="%s configuration for Apf9i FwRev %06lx:\n"; 
//	  static cc indent[]=" ";//HM removed 3 spaces 
      
//      /* write the configuration to the log file */
//      LogEntry(FuncName,cfg,id,FwRev);
//      LogEntry(FuncName,indent); LogAdd("AscentTimeOut(%ld) [min]\n",config->TimeOutAscent/Min);
//      LogEntry(FuncName,indent); LogAdd("AtDialCmd(%s) [primary]\n",config->at);
//      LogEntry(FuncName,indent); LogAdd("AtDialCmd(%s) [alternate]\n",config->alt);
//      LogEntry(FuncName,indent); LogAdd("BuoyancyNudge(%u) [count]\n",config->PistonBuoyancyNudge);
//      LogEntry(FuncName,indent); LogAdd("BuoyancyNudgeInitial(%u) [count]\n",config->PistonInitialBuoyancyNudge);
//      LogEntry(FuncName,indent); LogAdd("ConnectTimeOut(%ld) [sec]\n",config->ConnectTimeOut);
//      LogEntry(FuncName,indent); LogAdd("CpActivationP(%g) [dbar]\n",config->PressureCP);
//      LogEntry(FuncName,indent); LogAdd("DeepProfileDescentTime(%ld) [min]\n",config->TimeDeepProfileDescent/Min);
//      LogEntry(FuncName,indent); LogAdd("DeepProfilePistonPos(%d) [count]\n",config->PistonDeepProfilePosition);
//      LogEntry(FuncName,indent); LogAdd("DeepProfilePressure(%g) [dbar]\n",config->PressureProfile);
//      LogEntry(FuncName,indent); LogAdd("CompensatorHyperRetraction(%d) [count]\n",config->PistonParkHyperRetraction);
//      LogEntry(FuncName,indent); LogAdd("DownTime(%ld) [min]\n",config->TimeDown/Min);
//      LogEntry(FuncName,indent); LogAdd("FloatId(%04u)\n",config->FloatId);
//      LogEntry(FuncName,indent); LogAdd("FullExtension(%u) [count]\n",config->PistonFullExtension);
//      LogEntry(FuncName,indent); LogAdd("FullRetraction(%u) [count]\n",config->PistonFullRetraction);
//      LogEntry(FuncName,indent); LogAdd("MaxAirBladder(%u) [count]\n",config->MaxAirBladder);
//      LogEntry(FuncName,indent); LogAdd("MaxLogKb(%ld) [KByte]\n",MaxLogSize/1024);
//      LogEntry(FuncName,indent); LogAdd("MissionPrelude(%ld) [min]\n",config->TimePrelude/Min);
//      LogEntry(FuncName,indent); LogAdd("OkVacuum(%u) [count]\n",config->OkVacuumCount);
//      LogEntry(FuncName,indent); LogAdd("ParkDescentTime(%ld) [min]\n",config->TimeParkDescent/Min);
//      LogEntry(FuncName,indent); LogAdd("ParkPistonPos(%d) [count]\n",config->PistonParkPosition);
//      LogEntry(FuncName,indent); LogAdd("ParkPressure(%g) [dbar]\n",config->PressurePark);
//      LogEntry(FuncName,indent); LogAdd("PnPCycleLen(%d)\n",config->PnpCycleLength);
//      //LogEntry(FuncName,indent); LogAdd("Pwd(0x%04x)\n",
                                        //Crc16Bit((const unsigned char *)config->pwd,strlen(config->pwd)));
//      LogEntry(FuncName,indent); LogAdd("Pwd(%s)\n",config->pwd);//HM
				
//      LogEntry(FuncName,indent); LogAdd("TelemetryRetry(%ld) [min]\n",config->TimeTelemetryRetry/Min);

//      LogEntry(FuncName,indent); if (inRange(0,config->ToD,Day)) LogAdd("TimeOfDay(%ld) [min]\n",config->ToD/Min);
//                                 else LogAdd("TimeOfDay(DISABLED) [min]\n");
      
//      LogEntry(FuncName,indent); LogAdd("UpTime(%ld) [min]\n",config->TimeUp/Min);
//      LogEntry(FuncName,indent); LogAdd("User(%s)\n",config->user);
	  
//	  //Scan the new Gain and Detection parameters HM
//	  if(config->user[0]=='G' || config->user[0]=='g') ChGain[0]=config->user[1];
//	  else ChGain[0]='0';//illegal character set gain=0 //HM
//	  if(config->user[2]=='D' || config->user[2]=='d'){
//		ChMax_Detect_Prof[0]=config->user[3];
//		ChMax_Detect_Prof[1]=config->user[4];
//		ChMax_Detect_Prof[2]=config->user[5];
//		ChMax_Detect_Prof[3]=config->user[6];
//		ChMax_Detect_Prof[4]=config->user[7];
//		ChMax_Detect_Prof[5]=config->user[8];		
//		} else { //if illegal characater
//		ChMax_Detect_Prof[0]='9'; 
//		ChMax_Detect_Prof[1]='9';
//		ChMax_Detect_Prof[2]='9';
//		ChMax_Detect_Prof[3]='9';
//		ChMax_Detect_Prof[4]='9';// set Max_detection to write file = 20//HM
//		ChMax_Detect_Prof[5]='9';	
//		}
//	  if(config->user[9]=='P' || config->user[9]=='p'){
//		//Set a new DSP power ON/OFF depth 
//		ChDspPwrDpt[0]=config->user[10];
//		ChDspPwrDpt[1]=config->user[11];
//		ChDspPwrDpt[2]=config->user[12];
//		ChDspPwrDpt[3]=config->user[13];
//		} else { //if illegal characater
//		ChDspPwrDpt[0]='0'; 
//		ChDspPwrDpt[1]='1';
//		ChDspPwrDpt[2]='0';
//		ChDspPwrDpt[3]='0';
//		}
//	  //sscanf(config->user,"G%sD%s",ChGain, ChMax_Detect_Prof);//HM
//	  Gain=atoi(ChGain);	//define a new gain
//	  Max_Detect_Prof=atol(ChMax_Detect_Prof);//define a new max number of detects per profile
//	  DspPwrDpt=atoi(ChDspPwrDpt);
	  
//	  LogEntry(FuncName,indent); LogAdd("Gain=%d, Max # of Detect/Prof=%ld, DSP ON/OFF Depth =%d\n", Gain, Max_Detect_Prof, DspPwrDpt);//HM
//	  //check if the new gain is reasonable.  Definition of DspPwrDpt is undecided yet. Use bogus number for now.
//	  if(Gain<10 && Gain >=0 && DspPwrDpt<2000 && DspPwrDpt>=0) {QuecomSetNCM(Gain, DspPwrDpt);}
//	  //End HM
	  
//      LogEntry(FuncName,indent); LogAdd("Verbosity(%d)\n",debuglevel);
//      LogEntry(FuncName,indent); LogAdd("DebugBits(0x%04x)\n",debugbits);

//      /* reinitialize the function's return value */
//      status=1;
//   }
   
//   return status;
//}
