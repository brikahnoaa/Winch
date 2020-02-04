#ifndef CONFIGN2_H
#define CONFIGN2_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: config.c,v 1.9 2005/06/28 19:23:56 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/** RCS log of revisions to the C source code.
 *
 * \begin{verbatim}
 * $Log: config.c,v $
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
#define configureChangeLog "$RCSfile: config.c,v $  $Revision: 1.9 $   $Date: 2005/06/28 19:23:56 $"

#include <time.h>

#define CfgBufLen (31)

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
   time_t         ConnectTimeOut;    /* Seconds allowed to establish host connection */
   char           alt[CfgBufLen+1];  /* Alternate modem AT dialstring */
   char           at[CfgBufLen+1];   /* Modem AT dialstring */
   char           pwd[CfgBufLen+1];  /* Password for logging in to host computer */
   char           user[CfgBufLen+1]; /* Login name on host computer */
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

#endif /* CONFIGN2_H */

/* define the quantum of time (should match TUnits above) */
const time_t TQuantum = Min;

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <crc16bit.h>
#include <extract.h>
#include <logger.h>
#include <regex.h>
#include <strtoul.h>

#ifdef _XA_
   #include <ds2404.h>
#else
   time_t ialarm(void);
#endif /* _XA_ */

/* structure to store the mission configuration */
persistent struct MissionParameters mission;

/* default mission configuration */
const struct MissionParameters DefaultMission =
{
   0,124,96,10,16,227,9,22,66,16,4,1000,2000,1000,
   6*Hour,10*Day,9*Hour,6*Hour,6*Hour,15*Min,11*Hour,
   1*Min,"ATDT0012066163256","ATDT0012066859312","e=2.718",
   "iridium"
};

/* define the name of the configuration file */
const char config_path[] = "mission.cfg";

/* define the name of the configuration file */
persistent char log_path[32];

/* define the estimated vertical ascent rate */
const float dPdt=0.08;

/* prototypes for functions with static linkage */
static int ConfigSupervisor(struct MissionParameters *cfg);
static int fatal(const char *expr, const char *why);
static int warn(const char *expr, const char *why);

/* prototypes for functions with external linkage */
time_t itimer(void);
int    MissionParametersWrite(struct MissionParameters *mission);
void   PowerOff(time_t AlarmSec);
int    RecoveryInit(void);

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
                                 given by the TelemetryRetry. 

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

      ConnectTimeOut ........... The number of seconds allowed after dialing
                                 for a connection to be established with the
                                 host computer.  The valid range is 30-300
                                 sec.

      CpActivationP ............ The pressure where the control firmware
                                 transitions from subsampling the water
                                 column (in the deep water) to where the CP
                                 mode of the SBE41cp is activated for a high
                                 resolution profile to the surface.
                                 
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
                                 min to 14 days.

      FloatId .................. The float identifier.
      
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
                                 is 5 minutes to 6 hours.

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

      # Password
      # Pwd(e=2.718) [0xb369]
      
      # user name
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
      int RecoveryMode=0;
      struct MissionParameters cfg; 
      
      #define WS  "[ \t\r]*"
      #define VAR "([a-zA-Z0-9]+)"
      #define VAL "\\((.*)\\)"
      #define CRC "(\\[(0x[0-9a-fA-F]{1,4})\\])?"
   
      /* construct the regex pattern string for files with message locks */ 
      const char *pattern = "^" WS "((" VAR WS VAL ")" WS CRC ")" WS "$";
      
      /* define the configuration parameters */
      enum {ActivateRecoveryMode, AirBladderMaxP, AscentTimeOut, AtDialCmd, AltDialCmd, ConnectTimeOut,
            CpActivationP, DeepProfileDescentTime, DeepProfilePistonPos, DeepProfilePressure,
            DownTime, FloatId, MaxLogKb, ParkDescentTime, ParkPistonPos, ParkPressure,
            PnPCycleLen, Pwd, TelemetryRetry, UpTime, User, Verbosity, NVAR};

      /* define the names of the configuration parameters */ 
      const char *parms[NVAR];

      /* reinitialize the return value */
      status=1;
      
      /* initialize the configuration */
      cfg=*config;
 
      /* create the metacommand table */
      parms[ActivateRecoveryMode]   = "activaterecoverymode";
      parms[AirBladderMaxP]         = "airbladdermaxp";
      parms[AscentTimeOut]          = "ascenttimeout";
      parms[AtDialCmd]              = "atdialcmd";
      parms[AltDialCmd]             = "altdialcmd";
      parms[ConnectTimeOut]         = "connecttimeout";
      parms[CpActivationP]          = "cpactivationp";
      parms[DeepProfileDescentTime] = "deepprofiledescenttime";
      parms[DeepProfilePistonPos]   = "deepprofilepistonpos";
      parms[DeepProfilePressure]    = "deepprofilepressure";
      parms[DownTime]               = "downtime";
      parms[FloatId]                = "floatid";
      parms[MaxLogKb]               = "maxlogkb";
      parms[ParkDescentTime]        = "parkdescenttime";
      parms[ParkPistonPos]          = "parkpistonpos";
      parms[ParkPressure]           = "parkpressure";
      parms[PnPCycleLen]            = "pnpcyclelen";
      parms[Pwd]                    = "pwd";
      parms[TelemetryRetry]         = "telemetryretry";
      parms[UpTime]                 = "uptime";
      parms[User]                   = "user";
      parms[Verbosity]              = "verbosity";

      /* compile the option pattern */
      assert(!regcomp(&regex,pattern,REG_EXTENDED|REG_NEWLINE));

      /* protect against segfaults */
      assert(NSUB==regex.re_nsub);
      
      /* make a log entry that the configuration file will be parsed */
      if (debuglevel>=2)
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
               if (debuglevel>=2)
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
            if (debuglevel>=2)
            {
               /* create the message */
               static cc format[]="   %s ";

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
                  /* make a log entry that the CRC check failed */
                  if (debuglevel>=2) LogAdd("[CRC check failed: 0x%04X != 0x%04X].\n",crc0,crc1);

                  /* indicate failure of the configuration attempt */
                  status=0; continue;
               }
            }

            /* add the configurator CRC */
            else if (debuglevel>=2) LogAdd("[CRC=0x%04X] ",crc0);  
         
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
                  if (debuglevel>=2) LogAdd("[ActivateRecoveryMode()].\n");

                  break;
               }
               
               /* configurator for maximum air bladder pressure */
               case AirBladderMaxP:
               {
                  /* insert the timeout period into configuration */
                  cfg.MaxAirBladder=atoi(arg);

                  /* make a log entry */
                  if (debuglevel>=2) LogAdd("[AirBladderMaxP(%d)].\n",cfg.MaxAirBladder);
               
                  break;
               }
               
               /* configurator for ascent time-out period */
               case AscentTimeOut: 
               {
                  /* insert the timeout period into configuration */
                  cfg.TimeOutAscent=atol(arg)*Min;

                  /* make a log entry */
                  if (debuglevel>=2) LogAdd("[AscentTimeOut(%ld)].\n",cfg.TimeOutAscent/Min);
               
                  break;
               }

               /* Modem AT dialstring configurator */
               case AtDialCmd: 
               {
                  /* copy the dialstring into static storage and insert into configuration */
                  strncpy(cfg.at,arg,CfgBufLen);

                  /* make a log entry */
                  if (debuglevel>=2) LogAdd("[AtDialCmd(%s)].\n",cfg.at);
               
                  break;
               }

               /* Modem AT dialstring configurator for alternate host */
               case AltDialCmd: 
               {
                  /* copy the dialstring into static storage and insert into configuration */
                  strncpy(cfg.alt,arg,CfgBufLen);

                  /* make a log entry */
                  if (debuglevel>=2) LogAdd("[AltDialCmd(%s)].\n",cfg.alt);
               
                  break;
               }

               /* configurator for connection time-out period */
               case ConnectTimeOut: 
               {
                  /* insert the timeout period into configuration */
                  cfg.ConnectTimeOut=atol(arg);

                  /* make a log entry */
                  if (debuglevel>=2) LogAdd("[ConnectTimeOut(%ld)].\n",cfg.ConnectTimeOut);
               
                  break;
               }

               /* configurator for SBE41cp acrivation pressure */
               case CpActivationP:
               {
                  float p = atof(arg);

                  /* condition the activation pressure */
                  if (p<0) p=0; else if (p>2500) p=2500;
                  
                  /* insert the profile pressure into configuration */
                  cfg.PressureCP=p;

                  /* make a log entry */
                  if (debuglevel>=2) LogAdd("[CpActivationP(%g)].\n",
                                            cfg.PressureCP);

                  break;
               }

               /* configurator for profilek descent time */
               case DeepProfileDescentTime:
               {
                  /* insert the downtime into configuration */
                  cfg.TimeDeepProfileDescent=atol(arg)*Min;

                  /* make a log entry */
                  if (debuglevel>=2) LogAdd("[DeepProfileDescentTime(%ld)].\n",
                                            cfg.TimeDeepProfileDescent/Min);

                  break;
               }

               /* configurator for park pressure */
               case DeepProfilePistonPos:
               {
                  /* insert the park pressure into configuration */
                  cfg.PistonDeepProfilePosition=atoi(arg);

                  /* make a log entry */
                  if (debuglevel>=2) LogAdd("[DeepProfilePistonPos(%d)].\n",
                                            cfg.PistonDeepProfilePosition);

                  break;
               }

               /* configurator for profile pressure */
               case DeepProfilePressure:
               {
                  /* insert the profile pressure into configuration */
                  cfg.PressureProfile=atof(arg);

                  /* make a log entry */
                  if (debuglevel>=2) LogAdd("[ProfilePressure(%g)].\n",
                                            cfg.PressureProfile);

                  break;
               }

               /* configurator for profile cycle downtime */
               case DownTime:
               {
                  /* insert the downtime into configuration */
                  cfg.TimeDown=atol(arg)*Min;

                  /* make a log entry */
                  if (debuglevel>=2) LogAdd("[DownTime(%ld)].\n",
                                            cfg.TimeDown/Min);

                  break;
               }

               /* configurator for the float id */
               case FloatId:
               {
                  /* insert the downtime into configuration */
                  cfg.FloatId=atoi(arg);

                  /* make a log entry */
                  if (debuglevel>=2) LogAdd("[FloatId(%d)].\n",
                                            cfg.FloatId);

                  break;
               }

               /* configurator for park descent time */
               case ParkDescentTime:
               {
                  /* insert the park descent time into configuration */
                  cfg.TimeParkDescent=atol(arg)*Min;

                  /* make a log entry */
                  if (debuglevel>=2) LogAdd("[ParkDescentTime(%ld)].\n",
                                            cfg.TimeParkDescent/Min);

                  break;
               }

               /* configurator for park piston position */
               case ParkPistonPos:
               {
                  /* insert the park piston position into configuration */
                  cfg.PistonParkPosition=atoi(arg);

                  /* make a log entry */
                  if (debuglevel>=2) LogAdd("[ParkPistonPos(%d)].\n",
                                            cfg.PistonParkPosition);

                  break;
               }

               /* configurator for park pressure */
               case ParkPressure:
               {
                  /* insert the park pressure into configuration */
                  cfg.PressurePark=atof(arg);

                  /* make a log entry */
                  if (debuglevel>=2) LogAdd("[ParkPressure(%g)].\n",
                                            cfg.PressurePark);

                  break;
               }

               /* configurator for PnP cycle length */
               case PnPCycleLen:
               {
                  /* insert the PnP cycle length into configuration */
                  cfg.PnpCycleLength=atoi(arg);

                  /* make a log entry */
                  if (debuglevel>=2) LogAdd("[PnpCycleLength(%d)].\n",
                                            cfg.PnpCycleLength);

                  break;
               }

               /* password configurator */
               case Pwd: 
               {
                  /* copy the password into the configuration */
                  strncpy(cfg.pwd,arg,CfgBufLen);

                  /* make a log entry */
                  if (debuglevel>=2) LogAdd("[Pwd(%s)].\n",cfg.pwd);
               
                  break;
               }

               /* configurator for telemetry retry period */
               case TelemetryRetry:
               {
                  /* insert the telemetryretry into configuration */
                  cfg.TimeTelemetryRetry=atol(arg)*Min;

                  /* make a log entry */
                  if (debuglevel>=2) LogAdd("[TelemetryRetry(%ld)].\n",
                                            cfg.TimeTelemetryRetry/Min);

                  break;
               }

               /* configurator for profile cycle uptime */
               case UpTime:
               {
                  /* insert the uptime into configuration */
                  cfg.TimeUp=atol(arg)*Min;

                  /* make a log entry */
                  if (debuglevel>=2) LogAdd("[UpTime(%ld)].\n",cfg.TimeUp/Min);

                  break;
               }
               
               /* user configurator */
               case User: 
               {
                  /* copy the username into the configuration */
                  strncpy(cfg.user,arg,CfgBufLen);

                  /* make a log entry */
                  if (debuglevel>=2) LogAdd("[User(%s)].\n",cfg.user);
               
                  break;
               }

               /* Logging verbosity configurator */
               case Verbosity: 
               {
                  /* convert the string to an integer */
                  int verbosity=atoi(arg);

                  /* validate the verbosity specification */
                  if (verbosity>=0)
                  {
                     if (debuglevel>=2)
                     {
                        /* make a log entry warning of terse debuggin mode */
                        if (!verbosity) LogAdd("[Warning: terse mode enabled - "
                                               "Verbosity(%d)].\n",verbosity);

                        /* make a log entry */
                        else LogAdd("[Verbosity(%d)].\n",verbosity);
                     }
                     
                     /* insert debug level into configuration */
                     debugbits=verbosity;
                  }
                  else {LogAdd("[Ignoring invalid verbosity: %d].\n",verbosity);}
                     
                  break;
               }

               /* Maximum log size configurator */
               case MaxLogKb:
               {
                  /* convert the string to an integer */
                  int maxlogkb = atoi(arg);

                  /* validate the maximum log size */
                  if (maxlogkb>=5 && maxlogkb<=60) 
                  {
                     /* insert debug level into configuration */
                     MaxLogSize=maxlogkb*1024L;

                     /* make a log entry that maximum log size */
                     if (debuglevel>=2) LogAdd("[MaxLogKb(%d)].\n",maxlogkb);
                  }
                  else {LogAdd("[Ignoring invalid log size limit: %d Kbytes].\n",maxlogkb);}
                  
                  break;
               }
               
               default:
               {
                  /* make a log entry */
                  if (debuglevel>=2) LogAdd("[Unrecognized configurator].\n");

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
         if (debuglevel>=2)
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
         else
         {
            /* log the failure */
            static cc msg[]="Configuration rejected by configuration supervisor.\n";
            LogEntry(FuncName,msg);
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
   if (Fatal(NULL, inCRange(      0, debuglevel,                       4))) status=0;
   if (Fatal(NULL, inCRange(    240, cfg->MaxAirBladder,               1))) status=0;
   if (Fatal(NULL, inCRange(    254, cfg->PistonDeepProfilePosition,   1))) status=0;
   if (Fatal(NULL, inCRange(    254, cfg->PistonParkPosition,          1))) status=0;
   if (Fatal(NULL, inCRange(    254, cfg->PnpCycleLength,              1))) status=0;
   if (Fatal(NULL, inCRange(   2000, cfg->PressurePark,                0))) status=0;
   if (Fatal(NULL, inCRange(   2000, cfg->PressureProfile,             0))) status=0;
   if (Fatal(NULL, inCRange( 8*Hour, cfg->TimeDeepProfileDescent,      0))) status=0;
   if (Fatal(NULL, inRange(  30*Day, cfg->TimeDown,                  Min))) status=0;
   if (Fatal(NULL, inCRange(10*Hour, cfg->TimeOutAscent,             Min))) status=0;
   if (Fatal(NULL, inRange(  8*Hour, cfg->TimeParkDescent,             0))) status=0;
   if (Fatal(NULL, inCRange( 6*Hour, cfg->TimeTelemetryRetry,      5*Min))) status=0;
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

   /* validate the dial command */
   if (Fatal(NULL, !strncmp(cfg->at,"ATDT",4))) status=0;

   /* check for parking of N2 floats are unstable depths */ 
   if (Fatal("N2 floats are unstable when parked shallower than 1500 dbars.",
             cfg->PressurePark >= 1500)) status=0;
   
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
int LogConfiguration(const struct MissionParameters *config,const char *id)
{
   static cc FuncName[] = "LogConfiguration()";
   int status=-1;

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
      static cc cfg[]="%s configuration for Apf9i FwRev %06lx:\n"; static cc indent[]="   ";
      
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
      LogEntry(FuncName,indent); LogAdd("Pwd(0x%04x)\n",
                                        Crc16Bit((const unsigned char *)config->pwd,strlen(config->pwd)));
      LogEntry(FuncName,indent); LogAdd("TelemetryRetry(%ld) [min]\n",config->TimeTelemetryRetry/Min);
      LogEntry(FuncName,indent); LogAdd("UpTime(%ld) [min]\n",config->TimeUp/Min);
      LogEntry(FuncName,indent); LogAdd("User(%s)\n",config->user);
      LogEntry(FuncName,indent); LogAdd("Verbosity(%d)\n",debuglevel);

      /* reinitialize the function's return value */
      status=1;
   }
   
   return status;
}
