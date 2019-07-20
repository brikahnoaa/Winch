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
