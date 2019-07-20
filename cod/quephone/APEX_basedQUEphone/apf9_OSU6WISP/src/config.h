#ifndef CONFIG_H
#define CONFIG_H (0x0100U)

//Fixed the problem of DpsPwrDpt becoming a strange number by atoi(). Putting a NULL 
//at the last address of character string helped to terminate the string properly and 
//solved the strange behaviour of atoi(). HM Jan 2015.  
//User function changed. User(G2D999999P0020). If it starts with "G", it changes the 
//gain. 6 digit number after D is max detections per dive, 4 digit number after P is DSP 
//power ON/OFF depth.  Oct, 2010 HM
//Original code reactivated the mission by sending any legitimate command. Changed this
//behaviour, i.e., activates the mission if it received "E" in User command.  This allows
//to change other mission parameters including telemetry interval without reactivating 
//the mission.  Oct, 2010
//Pwd is changed. It is now used to define the QUEphone ID, e.g., Q001. 
//Haru Matsumoto, Oregon State Univ.

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: config.c,v 1.19.2.1 2010/11/17 20:00:06 dbliudnikas Exp $
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
#define configureChangeLog "$RCSfile: config.c,v $  $Revision: 1.19.2.1 $   $Date: 2010/11/18 20:00:06 $"

#include <time.h>
#include <config.h>  //HM
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
#define Min      (60L)
#define Day   (86400L)
#define HeartBeat (6L)

//#define Hour (1800L)
/* store the mission parameters in persistent non-far ram to facilitate CRC computations */
extern persistent struct MissionParameters mission;

/* declare an initialization object for use when the APFx loses its mind */
extern const struct MissionParameters DefaultMission;

/* declare the mission configuration file name */
extern const char config_path[];

/* declare the log filename */
extern persistent char log_path[32];

/* object to store the encoded firmware revision */
extern const unsigned long FwRev;

/* unit of time that defines the time scale of the mission (ie., 60sec or 3600sec) */
extern const time_t TQuantum;

/* the units of the time-quantization above (ie., minutes or hours) */
#define TUnits "Minutes"

/* estimated vertical ascent rate */
extern const float dPdt;

/* declarations for variables with external linkage */
extern const float MinN2ParkPressure;

//extern persistent int ReactivatedByE;//HM
//extern persistent int RecoveryMode;//HM

#endif /* CONFIG_H */
