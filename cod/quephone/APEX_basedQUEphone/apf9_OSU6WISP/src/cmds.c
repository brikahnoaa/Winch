#ifndef CMDS_H
#define CMDS_H
//QUEphone version
//Changed i*u (User) command to take a gain (0-3), max detections per dive (1-999999)
//and DSP power on/off depth (0-9999). Pwd is now QUEphone ID, e.g., Q001. HM. OSU
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: cmds.c,v 1.46.2.1 2008/09/11 19:55:27 dbliudnikas Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Copyright University of Washington.   Written by Dana Swift.
 *
 * This software was developed at the University of Washington using funds
 * generously provided by the US Office of Naval Research, the National
 * Science Foundation, and NOAA.
 *  
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or (at
 * your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */
/** RCS log of revisions to the C source code.
 *
 * \begin{verbatim}
 * $Log: cmds.c,v $
 * Revision 1.46.2.1  2008/09/11 19:55:27  dbliudnikas
 * Replace SBE41 with Seascan TD: Seascan agent, gateway, remove not applicable PressureCP
 *
 * User name i*u command is now to control gain, number of detection per profile to make 
 * float to ascent and the detection threshold to DSP board. 
 * A one-digit number followed by G sets the gain. For example i*uG2D001000P0080 set 
 * gain=2, max number of detection to make float to come up(Max_Detects=1000), and 
 * DSP power ON/OFF depth (DspPwrDpt=100 default). HM Feb 11, 2010.

 * Revision 1.46  2008/07/14 16:51:24  swift
 * Implement hyper-retraction mechanism for N2 compensator.  Add
 * command to query LBT for signal strength.  Add Sbe41cp gateway mode.
 *
 * Revision 1.45  2007/10/05 22:30:21  swift
 * Change limits of the telemetry retry interval and down time.
 *
 * Revision 1.44  2007/06/08 02:15:26  swift
 * Added a few comments.
 *
 * Revision 1.43  2007/05/09 19:13:42  swift
 * Added more commands related to the TimeOfDay feature.
 *
 * Revision 1.42  2007/05/08 18:10:16  swift
 * Added TimeOfDay feature to implement ability to schedule profiles to start
 * at a specified time of day.  Added attribution just below the copyright in
 * the main comment section.
 *
 * Revision 1.41  2007/05/07 21:06:15  swift
 * Added commands to set the maximum engineering log size and execute SelfTest().
 *
 * Revision 1.40  2007/03/26 20:52:06  swift
 * Implemented a command to retrieve the ICCID/MSISDN numbers from the SIM card.
 *
 * Revision 1.39  2007/01/24 17:15:51  swift
 * Fixed a minor misuse of fnameok().
 *
 * Revision 1.38  2007/01/22 18:09:02  swift
 * Fixed a minor typo in a format spec.
 *
 * Revision 1.37  2007/01/15 20:51:10  swift
 * Added a function to fill the flash with test files.
 *
 * Revision 1.36  2006/12/20 00:03:54  swift
 * Fixed a syntax error in the command that manually creates a log file.
 *
 * Revision 1.35  2006/12/19 23:42:22  swift
 * Add commands to manually create and close the logfile.
 *
 * Revision 1.34  2006/11/25 00:05:35  swift
 * Fixed a bug in the directory listing of flashio filesystem.
 *
 * Revision 1.33  2006/11/22 03:58:00  swift
 * Add a sanity check for the CP activation pressure to warn against spot
 * sampling in the main thermocline or above.
 *
 * Revision 1.32  2006/11/18 17:43:16  swift
 * Removed orphaned menu selection.
 *
 * Revision 1.31  2006/10/13 18:56:42  swift
 * Added a comment.
 *
 * Revision 1.30  2006/10/11 20:58:15  swift
 * Integrated the new flashio files system.
 *
 * Revision 1.29  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.28  2006/05/19 19:28:05  swift
 * Modifications to allow the bearer service type to be included in the AT dial string.
 *
 * Revision 1.27  2006/05/15 18:52:44  swift
 * Expanded valid range of park descent and profile descent periods from
 * 6 hours to 8 hours.
 *
 * Revision 1.26  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.25  2006/02/22 21:54:28  swift
 * Added biographical queries to the modem agent.
 *
 * Revision 1.24  2006/01/06 23:24:58  swift
 * Fixed an inconsistency in the mission nazi that wouldn't allow for longer
 * than 6-hour descents.
 *
 * Revision 1.23  2005/10/12 20:11:46  swift
 * Modifications to account for changes to SBE41CP API.
 *
 * Revision 1.22  2005/10/11 20:37:44  swift
 * Implemented pressure-activation of iridium floats.
 *
 * Revision 1.21  2005/09/02 22:21:06  swift
 * Added ApfId to the command line leader.
 *
 * Revision 1.20  2005/08/06 17:18:54  swift
 * Allow for telemetry-retry intervals in the range 5 minutes to 6 hours.
 *
 * Revision 1.19  2005/07/12 18:17:50  swift
 * Changed units of time in menus from hours to minutes.
 *
 * Revision 1.18  2005/06/23 20:35:33  swift
 * Added a sanity check for the maximum air-bladder check.
 *
 * Revision 1.17  2005/06/14 19:00:24  swift
 * Added an LBT agent to manually control and exercise the modem.
 *
 * Revision 1.16  2005/05/01 14:27:38  swift
 * Added menu options for displaying SBE41CP firmware and for initiating a
 * low-power TP sample.
 *
 * Revision 1.15  2005/04/04 22:35:33  swift
 * Change maximum telemetry-retry interval to 6 hours.
 *
 * Revision 1.14  2005/03/30 20:25:00  swift
 * Changed maximum telemetry-retry period.
 *
 * Revision 1.13  2005/02/22 20:59:27  swift
 * Added constraints to the mission nazi.  Added commands to control the
 * Garmin GPS15L.
 *
 * Revision 1.12  2005/01/06 00:52:52  swift
 * Added testing functionality in ManufacturerDiagnosticsAgent() for
 * board-level testing.
 *
 * Revision 1.11  2004/12/30 17:10:08  swift
 * Added tests of hardware control lines on the GPS port and RF switch port.
 *
 * Revision 1.10  2004/12/29 23:11:26  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.9  2004/04/23 23:45:42  swift
 * Added a function call to open the air valve when the mission is killed.
 *
 * Revision 1.8  2004/04/14 16:40:15  swift
 * Eliminated most of the sanity checks and added feature to print the
 * pressure table.
 *
 * Revision 1.7  2004/03/24 00:15:01  swift
 * Added provision for debuglevel to be a persistent variable.
 *
 * Revision 1.6  2004/02/05 23:50:40  swift
 * Added include directive for the Xicor eeprom API.
 *
 * Revision 1.5  2003/12/20 19:49:02  swift
 * Added a command to display the SBE41 calibration coefficients.
 *
 * Revision 1.4  2003/11/20 18:59:47  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.3  2003/11/12 22:40:17  swift
 * Increase time-out periods in response to user input.  Implemented special
 * case where the PnP cycle length of 254 disables the PnP feature.
 *
 * Revision 1.2  2003/09/11 17:10:18  swift
 * Modifications to the mission analyzer.  Added many features to the user
 * interface to allow float control and exercise.
 *
 * Revision 1.1  2003/08/22 14:54:17  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define cmdsChangeLog "$RCSfile: cmds.c,v $ $Revision: 1.46.2.1 $ $Date: 2008/09/11 19:55:27 $"

/* function prototypes for exported functions */
void CmdMode(void);
int  CmdModeRequest(void);

#endif /* CMDS_H */

#include <apf9.h>
#include <apf9com.h>
#include <apf9icom.h>
#include <assert.h>
#include <config.h>
#include <conio.h>
#include <control.h>
#include <crc16bit.h>
#include <cstring.h>
#include <ctdio.h>
#include <ctype.h>
#include <ds2404.h>
#include <eeprom.h>
#include <engine.h>
#include <extract.h> 
#include <garmin.h>
#include <lbt9522.h>
#include <limits.h>
#include <logger.h>
#include <lt1598ad.h>
#include <math.h>
#include <nmea.h>
#include <pactivat.h>
#include <psd835.h>
#include <recovery.h>
#include <regex.h>
#include <seascan.h>
#include <serial.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <telemetr.h>
#include <tmparse.h>
#include <x24c16.h>
#include <strtoul.h>
#include <tc58v64.h>
#include <flashio.h>
#include "quecom.h" //HM
#include <gps.h>    //HM

const char AT[]="AT";
const char trm[]="\r\n";
const char cr[]="\r";
const char ns[]="";

/* force all anonymous strings to be stored in ROM */
#pragma strings code

/* define some ascii characters special meaning */
#define LF   0x0a /* line feed */
#define CR   0x0d /* carriage return */
#define BKSP 0x08 /* backspace */
#define DEL  0x7f /* DEL */
#define ESC  0x1b /* escape character */

/* define the termination string for writes to the console serial port */
static cc ctrm[] = "\r\n";

/* define the vertical ascent and descent rates (dbar/sec) */
float AscentRate = 0.08, DescentRate = 0.10; 

/* define a global switch to turn on/off the display of the password */
static int ShowPwd=0;

/* prototypes for functions with static linkage */
static int               AnalyzeMission(void);
static void              BuoyancyControlAgent(void);
static void              cputs(cc *msg,cc *trm);
static void              Diagnostics(void);
static int               fatal(cc *expr,cc *why);
static void              FlashAgent(void);
static void              FloatVitalsAgent(void);
static void              GarminGps15Agent(void);
static void              HostAgent(void);
static void              ManufacturerDiagnosticsAgent(void);
static void              MissionProgrammingAgent(void);
static void              MissionTimingAgent(void);
static void              ModemAgent(void);
static unsigned long int RamTest(void);
static void              SeascanAgent(void);
static void              SeascanGateway(void);
static void              ShowParameters(void);
static int               warn(cc *expr,cc *why);

/* external variable declarations */
extern persistent time_t AlmanacTimeStamp;

/*------------------------------------------------------------------------*/
/* function to analyze the mission parameters for internal consistency    */
/*------------------------------------------------------------------------*/
/**
   This function analyzes the mission configuration for intenal consistency,
   prints warnings when problems are detected, and computes a status metric
   that can be used to determine if the mission should be allowed to
   proceed.  Two classes of violations are currently implemented; those that
   are fatal and those that are merely insane.  Mission configurations that
   have fatal violations should not be allowed to proceed.  Mission
   configurations that are computed to be insane should proceed only after
   careful consideration of each warning.

   This function returns zero if the mission configuration passed all
   checks; otherwise a positive value is returned.  Fatal mission
   configurations are indicated if the return value is true after
   bit-masking with 0x01.  Insane mission configurations are indicated if
   the return value is true after bit-masking with 0x02.
*/
static int AnalyzeMission(void)
{
   int status=0;

   /* assertion-like macro to trap fatal violations of mission constraints */
   #define Fatal(why,expr) ((expr) ? 0 : fatal(#expr,why))

   /* assertion-like macro to trap violations of mission sanity checks */
   #define Warn(why,expr)  ((expr) ? 0 : warn(#expr,why))

   /* compute the signature of the mission program */
   mission.crc = Crc16Bit((unsigned char *)(&mission), sizeof(mission)-sizeof(mission.crc));
   
   /* these are absolute range constraints that should be self explanatory */
   if (Fatal(NULL, inCRange(      0, debuglevel,                          5))) status|=0x01;
   if (Fatal(NULL, inCRange(    240, mission.MaxAirBladder,               1))) status|=0x01;
   if (Fatal(NULL, inCRange(    254, mission.OkVacuumCount,               1))) status|=0x01;
   if (Fatal(NULL, inCRange(    254, mission.PistonBuoyancyNudge,         1))) status|=0x01;
   if (Fatal(NULL, inCRange(    254, mission.PistonDeepProfilePosition,   1))) status|=0x01;
   if (Fatal(NULL, inCRange(    254, mission.PistonFullExtension,         1))) status|=0x01;
   if (Fatal(NULL, inCRange(    254, mission.PistonFullRetraction,        1))) status|=0x01;
   if (Fatal(NULL, inCRange(    254, mission.PistonInitialBuoyancyNudge,  1))) status|=0x01;
   if (Fatal(NULL, inCRange(    254, mission.PistonParkHyperRetraction,   0))) status|=0x01;
   if (Fatal(NULL, inCRange(    254, mission.PistonParkPosition,          1))) status|=0x01;
   if (Fatal(NULL, inCRange(    254, mission.PistonStoragePosition,       1))) status|=0x01;
   if (Fatal(NULL, inCRange(    254, mission.PnpCycleLength,              1))) status|=0x01;
   if (Fatal(NULL, inCRange(   2000, mission.PressurePark,                0))) status|=0x01;
   if (Fatal(NULL, inCRange(   2000, mission.PressureProfile,             0))) status|=0x01;
   if (Fatal(NULL, inCRange( 8*Hour, mission.TimeDeepProfileDescent,      0))) status|=0x01;
   if (Fatal(NULL, inRange(  30*Day, mission.TimeDown,                    0))) status|=0x01;
   if (Fatal(NULL, inCRange(10*Hour, mission.TimeOutAscent,        TQuantum))) status|=0x01;
   if (Fatal(NULL, inRange(  8*Hour, mission.TimeParkDescent,             0))) status|=0x01;
   if (Fatal(NULL, inRange(  6*Hour, mission.TimePrelude,                 0))) status|=0x01;
   if (Fatal(NULL, inCRange( 6*Hour, mission.TimeTelemetryRetry,        Min))) status|=0x01;
   if (Fatal(NULL, inRange( 24*Hour, mission.TimeUp,                      0))) status|=0x01;
   if (Fatal(NULL, inCRange(    300, mission.ConnectTimeOut,             30))) status|=0x01;

   /****** Constraints to protect against potentially fatal pilot errors *********/

   /* make sure there's enough uptime to do the deep profile and the telemetry */
   if (Fatal("The up-time must allow for a deep profile plus 2 hours for telemetry.",
             mission.TimeUp >= (mission.PressureProfile/dPdt) + 2*Hour)) status|=0x01;

   /* make sure there's enough uptime to do the park profile and the telemetry */
   if (Fatal("The up-time must allow for a park profile plus 2 hours for telemetry.",
             mission.TimeUp >= (mission.PressurePark/dPdt) + 2*Hour)) status|=0x01;

   /* the up-time has to be greater than the ascent time-out period */
   if (Fatal("The up-time has to be greater than the ascent time-out period.",
             mission.TimeUp > mission.TimeOutAscent)) status|=0x01;
   
   /* the down-time has to be greater than the park-descent time + deep-profile descent time */
   if (Fatal("The down-time has to be greater than the park-descent time plus deep-profile descent time.",
             mission.TimeDown > mission.TimeParkDescent+mission.TimeDeepProfileDescent)) status|=0x01;

   /* make sure the profile pressure is not less than the park pressure */
   if (Fatal("The profile pressure must be greater than (or equal to) the park pressure.",
             mission.PressureProfile >= mission.PressurePark)) status|=0x01;

   /* validate the dial commands */
   if (Fatal("The primary dial command must begin with AT.",!strncmp(mission.at, AT,2))) status|=0x01;
   if (Fatal("The alternate dial command must begin with AT.",!strncmp(mission.alt,AT,2))) status|=0x01;
      
   /****** Sanity checks *********/

   /* recommend CP mode in upper layers */
   /* Removed - not applicable to Seascan PT module */

   /* allow time for completion of deep profile */
   if (Warn("Ascent time insufficient for a deep profile.",
            mission.TimeOutAscent >= (mission.PressureProfile/dPdt) + 1*Hour)) status|=0x02;

   /* allow time for completion of park profile */
   if (Warn("Ascent time insufficient for a park profile.",
            mission.TimeOutAscent >= (mission.PressurePark/dPdt) + 1*Hour)) status|=0x02;

   /* allow time for telemetry */
   if (Warn("Up time insufficient to guarantee at least 2 hours for telemetry.",
            mission.TimeUp >= mission.TimeOutAscent + 2*Hour)) status|=0x02;

   /* allow time for descent to park depth */
   if (Warn("Park descent period incompatible with park pressure.",
            mission.TimeParkDescent >= mission.PressurePark/dPdt)) status|=0x02;

   /* check for overly conservative park-descent period */
   if (Warn("Park descent period excessive.",
            mission.TimeParkDescent <= 1.5*mission.PressurePark/dPdt + 1*Hour)) status|=0x02;

   /* allow time for descent to profile depth */
   if (Warn("Deep-profile descent period incompatible with profile pressure.",
            mission.TimeDeepProfileDescent >= (mission.PressureProfile-mission.PressurePark)/dPdt)) status|=0x02;

   /* allow time for at least one active-ballast adjustment */
   if (Warn("Down time insufficient for active ballasting algorithm.",
            mission.TimeDown > mission.TimeDeepProfileDescent + mission.TimeParkDescent + 2*Hour)) status|=0x02;

   /* check for overly conservative deep-profile-descent period */
   if (Warn("Deep-profile descent period excessive.",
            mission.TimeDeepProfileDescent <= 1.5*(mission.PressureProfile-mission.PressurePark)/dPdt + 1*Hour)) status|=0x02;

   /* check for sensible piston postions */
   if (Warn("Profile piston position incompatible with park piston position.",
            mission.PistonDeepProfilePosition <= mission.PistonParkPosition)) status|=0x02;

   /* check for N2 compressee */
   if (mission.PistonParkHyperRetraction)
   {
      /* check for sensible piston postions */
      if (Warn("Compensator instability: N2 float should be parked deeper.",
               mission.PressurePark >= MinN2ParkPressure)) status|=0x02;
   }

   /* check for sensible maximum air bladder pressure */
   if (Warn("Maximum air-bladder pressure seems insane.",
            inCRange(120,mission.MaxAirBladder,128))) status|=0x02;

   /* make sure the float id is valid */
   if (Warn("The float serial number should be greater than zero.",
             mission.FloatId>0)) status|=0x02;

   /* print a warm & fuzzy if all is well */
   if (!status) cputs("All constraints and sanity checks passed.",ctrm);
      
   return status;

   #undef Fatal
}

/*------------------------------------------------------------------------*/
/* buoyancy control agent                                                 */
/*------------------------------------------------------------------------*/
/**
   This function implements a user interface for configuring buoyancy
   related mission parameters. 
*/
static void BuoyancyControlAgent(void)
{
   unsigned char key;
   long val;
   char buf[80];

   /* define error message for out-of-range entries */
   static cc RangeError[] = "ERROR: Not in valid range.";
   
   /* define the current time and the time-out period */
   const time_t To=time(NULL), TimeOut=60;

   /* pet the watch dog */
   WatchDog();

   /* flush the console's Rx buffer */
   conio.ioflush();

   /* wait for the next keyboard-hit */
   while ((key=kbdhit())<=0)
   {
      /* arrange for the apf9 to be put in hybernate mode */
      if (!ConioActive() || difftime(time(NULL),To)>TimeOut)
      {
         cputs(" (timeout)",ctrm);
         key=CR; break;
      }
   }

   /* acknowledge key entry by spacing over */
   if (key!=CR) conio.putb(' ');
      
   switch (tolower(key))
   {
      /* get the compressee hyper-retraction for park descent */
      case 'h':
      {
         /* get user input from the console */
         pgets(&conio,buf,sizeof(buf)-1,60,cr);
         
         if (*buf && (val=atoi(buf))>=0 && val<255)
         {
            mission.PistonParkHyperRetraction = val;
            csnprintf(buf,sizeof(buf),"Compensator hyper-retraction: %d counts.",
                     mission.PistonParkHyperRetraction);
            pputs(&conio,buf,2,trm);
         }
         else cputs(RangeError,ctrm);
         
         break;
      }

      /* get the initial buoyancy nudge of the piston */
      case 'i':
      {
         /* get user input from the console */
         pgets(&conio,buf,sizeof(buf)-1,60,cr);

         if (*buf && (val=atoi(buf))>=5 && val<255)
         {
            mission.PistonInitialBuoyancyNudge = val;
            csnprintf(buf,sizeof(buf),"Buoyancy nudge for ascent initiation: "
                     "%d piston counts.",mission.PistonInitialBuoyancyNudge);
            pputs(&conio,buf,2,trm);
         }
         else cputs(RangeError,ctrm);
         
         break;
      }
      
      /* get the piston position for deep profiles */
      case 'j':
      {
         /* get user input from the console */
         pgets(&conio,buf,sizeof(buf)-1,60,cr);

         if (*buf && (val=atoi(buf))>0 && val<255)
         {
            mission.PistonDeepProfilePosition = val;
            csnprintf(buf,sizeof(buf),"Piston position for deep profiles: "
                     "%d counts.",mission.PistonDeepProfilePosition);
            pputs(&conio,buf,2,trm);
         }
         else cputs(RangeError,ctrm);
         
         break;
      }

      /* get the buoyancy nudge of the piston during the profile */
      case 'n':
      {
         /* get user input from the console */
         pgets(&conio,buf,sizeof(buf)-1,60,cr);

         if (*buf && (val=atoi(buf))>=5 && val<255)
         {
            mission.PistonBuoyancyNudge = val;
            csnprintf(buf,sizeof(buf),"Buoyancy nudge for ascent maintenance: "
                     "%d piston counts.",mission.PistonBuoyancyNudge);
            pputs(&conio,buf,2,trm);
         }
         else cputs(RangeError,ctrm);
         
         break;
      }
         
      /* get the piston park position */
      case 'p':
      {
         /* get user input from the console */
         pgets(&conio,buf,sizeof(buf)-1,60,cr);
         
         if (*buf && (val=atoi(buf))>0 && val<255)
         {
            mission.PistonParkPosition = val;
            csnprintf(buf,sizeof(buf),"Piston park position: %d counts.",
                     mission.PistonParkPosition);
            pputs(&conio,buf,2,trm);
         }
         else cputs(RangeError,ctrm);
         
         break;
      }

      /* print the menu */
      case '?': cputs("",ctrm);
            
         /* the default response is to print the menu */
      default:  
      {
         cputs("Menu of buoyancy control parameters.",                               ctrm);
         cputs("?  Print this menu.",                                                ctrm);
         cputs("Bh Compensator hyper-retraction for park descent. [0-254] (counts)", ctrm);
         cputs("Bi Ascent initiation buoyancy nudge. [25-254] (piston counts)",      ctrm);
         cputs("Bj Deep-profile piston position. [1-254] (counts)",                  ctrm);
         cputs("Bn Ascent maintenance buoyancy nudge. [5-254] (piston counts)",      ctrm);
         cputs("Bp Park piston position [1-254] (counts)",                           ctrm);
      } 
   } 
}

/*------------------------------------------------------------------------*/
/* function to implement command mode                                     */
/*------------------------------------------------------------------------*/
/**
   This function is the main control loop that implements the command mode
   for mission programming and float control.
*/
void CmdMode(void)
{
   int status=1;
   time_t alarm;
   char buf[128];

   /* show the firmware revision */
   csnprintf(buf,80,"Entering Command Mode [ApfId %04u, FwRev %06lx]",mission.FloatId,FwRev);
   pputs(&conio,buf,2,trm); 
   
   do
   {
      long val;
      unsigned char key;
         
      /* define error message for out-of-range entries */
      static cc RangeError[] = "ERROR: Not in valid range.";

      /* define the current time and the time-out period */
      const time_t To=time(NULL), TimeOut=300;

      /* pet the watch dog */
      WatchDog();

      /* flush the console's Rx buffer */
      ConioEnable(); conio.ioflush();
      
      /* write a prompt to stdout */
      cputs("> ",""); 

      /* wait for the next keyboard-hit */
      while ((key=kbdhit())<=0)
      {
         /* arrange for the APF9 to be put in hybernate mode */
         if (!ConioActive() || difftime(time(NULL),To)>TimeOut)
         {
            cputs(" (timeout)",ctrm);
            key='q'; break;
         }
      }

      /* acknowledge key entry by spacing over */
      if (key!=CR) conio.putb(' '); 
      
      switch (tolower(key))
      {
         /* initiate pressure-activation of mission */ 
         case 'a': 
         {
            /* read the mission state from EEPROM */
            enum State state = StateGet();
            
            /* check if the mission is presently inactive */
            if (state<=INACTIVE)
            {
               cputs("Initiating pressure-activation of mission.",ctrm);
               
               /* put the float into surface diagnostics mode */
               if (PActivateInit()>0)
               {
                  /* notify that the mission has been activated */
                  cputs("Pressure-activation of mission has been initiated.",ctrm);

                  /* set the float to exit the command loop */
                  SetAlarm(itimer()+7); status=0;
               }
               else cputs("Attempt to initiate pressure-activation failed.",ctrm);
            }

            /* notify the user that the mission was already active */
            else cputs("The mission is already active.",ctrm);
               
            break;
         }
         
         /* display the battery voltage and vacuum */ 
         case 'c': 
         {
            /* create the calibration report */
            csnprintf(buf,sizeof(buf),"Battery [%3dcnt, %4.1fV]  "
                     "Current [%3dcnt, %4.1fmA]  "
                     "Barometer [%3dcnt, %5.1f\"Hg]",
                     BatVoltsAd8(),BatVolts(),BatAmpsAd8(),
                     BatAmps()*1000,BarometerAd8(),Barometer());

            /* write the calibration report to the console */
            pputs(&conio,buf,2,trm);
            
            break;
         }

         /* set the logging verbosity */
         case 'd':
         {
            long int val;
            
            /* get user input from the console */
            pgets(&conio,buf,sizeof(buf)-1,60,cr);

            /* process the user-input */
            if (*buf && (val=strtoul(buf,NULL,16))>=0L && (val&VERBOSITYMASK)<=5L && val<=UINT_MAX)
            {
               /* assign the new verbosity level */
               debugbits=(unsigned int)val;

               /* create a confirmation for the advanced user */
               if (debugbits&(~VERBOSITYMASK))
               {
                  /* create a confirmation for the user */
                  csnprintf(buf,sizeof(buf),"Logging verbosity,bits: %u,0x%04x",
                            debuglevel,debugbits);
               }

               /* create a confirmation for the user */
               else {csnprintf(buf,sizeof(buf),"Logging verbosity: %u",debuglevel);}

               /* write the calibration report to the console */
               pputs(&conio,buf,2,trm);
            }

            /* notify user of input error */
            else cputs(RangeError,ctrm);

            break;
         }
         
         /* execute the mission */ 
         case 'e': 
         {
            /* read the mission state from EEPROM */
            enum State state = StateGet();
            
            /* check if the mission is presently inactive */
            if (state<=INACTIVE)
            {
               cputs("Executing mission activation sequence.",ctrm);
               
               /* put the float into surface diagnostics mode */
               if (MissionLaunch()>0)
               {
                  /* notify that the mission has been activated */
                  cputs("Mission activated.",ctrm);

                  /* set the float to exit the command loop */
                  SetAlarm(itimer()+7); status=0;
               }
               else cputs("Attempt to activate mission failed.",ctrm);
            }

            /* notify the user that the mission was already active */
            else cputs("Useless mission activation request; "
                       "the mission is already active.",ctrm);
               
            break;
         }

         /* exercise the Garmin GPS15 */
         case 'g': {GarminGps15Agent(); break;}

         /* exercise the Modem */
         case 'h': {ModemAgent(); break;}
 
         /* branch to diagnostics loop */
         case 'i': {Diagnostics(); break;}

         /* branch to the FLASH file system agent */
         case 'j': {FlashAgent(); break;}
         
         /* execute the mission */ 
         case 'k': 
         {
            /* read the mission state from EEPROM */
            enum State state = StateGet();

            /* check if the mission is presently inactive */
            if (state!=INACTIVE)
            {
               /* warn the user that terminating the mission makes the float non-deployable */
               cputs("",ctrm);
               cputs("WARNING: Deactivating the mission renders the float nondeployable",ctrm);
               cputs("         until the mission is reactivated.  Kill the mission anyway?",ctrm);
               cputs("",ctrm);
               cputs("         Type 'Y' to kill the mission or any other key to abort. ","");
               conio.ioflush();

               /* get confirmation that the deactivation request is intentional */
               if (pgetbuf(&conio,buf,1,5)>0 && tolower(*buf)=='y')
               {
                  /* put the float mission into inactive mode and open the air valve */
                  cputs("",ctrm); MissionKill(); AirValveOpen();

                  /* warn that the float is not deployable until the mission is reactivated */
                  cputs("WARNING: Mission deactivated; float is not "
                        "deployable until the mission is reactivated.",ctrm);
               }

               /* abort the deactivation request */
               else {cputs("",ctrm); cputs("Mission deactivation aborted; the "
                                           "mission in progress will continue.",ctrm);}
            }

            /* notify that the mission was already inactive */
            else cputs("Useless mission deactivation request; "
                       "the mission is already inactive.",ctrm);

            break;
         }
         
         /* list the mission parameters */ 
         case 'l': 
         {
            if (StateGet()>INACTIVE) {cputs("Mission active - service unavailable.",ctrm);}
            
            else {cputs("",ctrm); ShowParameters();}
            
            break;
         }

         /* specify mission programming */
         case 'm':
         {
            if (StateGet()>INACTIVE) {cputs("Mission active - service unavailable.",ctrm);}

            else MissionProgrammingAgent();

            break;
         }

         /* display the float serial number */
         case 'n':
         {
            csnprintf(buf,sizeof(buf),"Float serial number: %u",mission.FloatId);
            pputs(&conio,buf,2,trm);
            break;
         }
        
         /* print the pressure table */
         case 'p':
         {
            int i,n;

            cputs("Pressure table:",ctrm);

            /* loop through each element of the pressure table */
            for (n=0,i=0; i<pTableSize; i++,n++)
            {
               /* terminate the line */
               if (n>=10) {cputs("",ctrm); n=0;}

               /* write the pressure to a buffer */
               csnprintf(buf,sizeof(buf)-1," %4.0f",pTable[i]);

               /* write the buffer to the console */
               pputs(&conio,buf,2,ns);
            }

            /* terminate the table */
            cputs("",ctrm);
            
            break;
         }
     
         /* key to exit command mode */
         case 'q': 
         { 
            /* get the current state of the mission */
            enum State state = StateGet();

            /* resume p-activation monitoring */
            if (state==PACTIVATE) cputs("Resuming pressure-activation monitor.",ctrm);

            /* resume p-activation monitoring */
            else if (state==RECOVERY) cputs("Resuming recovery mode.",ctrm);
            
            /* float is in an active state - resume the mission */
            else if (state>INACTIVE && state<EOS) cputs("Resuming mission already in progress.",ctrm);

            /* float is in an inactive state - hibernate */
            else
            {
               cputs("Mission not active - hibernating.",ctrm);

               /* make sure the state variable is well defined */
               if (state!=INACTIVE) StateSet(INACTIVE);
            }
            
            status=0; break; 
         } 

         /* recovery mode */
         case 'r':
         {
            cputs("Activating recovery mode.",ctrm);

            /* activate recovery mode */
            fformat(); RecoveryInit();

            /* set the alarm for 8 seconds from now and exit command mode */
            SetAlarm(itimer()+8); status=0; break;
         }

         /* exercise the Seascan */
         case 's': {SeascanAgent(); break;}

         /* get/set the time */
         case 't':
         {
            /* get user input from the console */
            if (pgets(&conio,buf,sizeof(buf)-1,60,cr)>=0)
            {
               /* process the user-input */
               if (*buf && ParseTime(buf,&val)>0)
               {
                  /* set the Apf9's DS2404 RTC */
                  if (RtcSet(val,0)<=0) cputs("Attempt to set RTC failed.",ctrm);
               }
            
               /* get the current time */
               val=time(NULL);

               /* construct the response to the user */
               csnprintf(buf,sizeof(buf),"Real time clock: %s",ctime(&val));

               /* write the response to the console */
               pputs(&conio,buf,2,ns);
            }

            break;
         }
        
         /* open a logfile */
         case 'u':
         {
            cputs("Enter log file name: ","");

            /* get user input from the console */
            if (pgets(&conio,buf,sizeof(buf)-1,60,cr)>=0)
            {
               /* verify that the filename */
               if (fnameok(buf)>0) {LogOpen(buf,'a');}
               else cputs("Invalid file name.",ctrm);
            }
            
            break;
         }
 
         /* close the logfile */
         case 'v': {LogClose(); cputs("Log file closed.",ctrm); break;}
   
         /* print the menu */
         case '?': cputs("",ctrm);
            
         /* the default response is to print the menu */
         default:  
         {
            cputs("Menu selections are not case sensitive.",             ctrm);
            cputs("?  Print this help menu.",                            ctrm);
            cputs("A  Initiate pressure-activation of mission.",         ctrm);
            cputs("C  Calibrate: battery volts, current, & vacuum.",     ctrm);
            cputs("D  Set logging verbosity. [0-5]",                     ctrm);
            cputs("E  Execute (activate) mission.",                      ctrm);
            cputs("G  GPS module agent.",                                ctrm);
            cputs("G? GPS module menu.",                                 ctrm);
            cputs("H  LBT module agent.",                                ctrm);
            cputs("H? LBT module menu.",                                 ctrm);
            cputs("I  Diagnostics agent.",                               ctrm);
            cputs("I? Diagnostics menu.",                                ctrm);
            cputs("J  FLASH file system agent.",                         ctrm);
            cputs("J? FLASH file system menu.",                          ctrm);
            cputs("K  Kill (deactivate) mission.",                       ctrm);
            cputs("L  List mission parameters.",                         ctrm);
            cputs("M  Mission programming agent.",                       ctrm);
            cputs("M? Mission programming menu.",                        ctrm);
            cputs("N  Display float serial number.",                     ctrm);
            cputs("P  Display the pressure table.",                      ctrm);
            cputs("Q  Exit command mode.",                               ctrm);
            cputs("R  Activate recovery mode.",                          ctrm);
            cputs("S  Sensor module agent.",                             ctrm);
            cputs("S? Sensor module menu.",                              ctrm);
            cputs("T  Get/Set RTC time. (format 'mm/dd/yyyy:hh:mm:ss')", ctrm);
            cputs("U  Attach the logstream to a specified file.",        ctrm);
            cputs("V  Close the log file.",                              ctrm);
         } 
      } 
   } 
   while (status>0); 

   /* make sure the alarm-time is at least 7 seconds in the future */
   alarm=((ialarm()-itimer())<7) ? (itimer()+7) : ialarm();
   
   /* power down until the previously set alarm expires */
   PowerOff(alarm);
}
 
/*------------------------------------------------------------------------*/
/* function to check for user request to enter command mode               */
/*------------------------------------------------------------------------*/
/**
   This function checks for a user-request to enter command mode.  Three
   test criteria are applied.  The first is that the magnetic reset switch
   has not been toggled.  The second is that the 20mA current loop is active
   which indicates that a computer is attached to the controller.  The third
   is that the APF9 was awakened asynchronously (ie., not by the RTC alarm).
   If all three criteria are satisfied then this function returns a positive
   number to indicate that command mode should be executed.  If any of the
   criteria are violated then this function returns zero.
*/
int CmdModeRequest(void)
{
   return (!MagSwitchToggled() && ConioActive()>0 && !Apf9WakeUpByRtc()) ? 1 : 0;
}

/*------------------------------------------------------------------------*/
/* function to write a string to the console port                         */
/*------------------------------------------------------------------------*/
static void cputs(cc *msg,cc *trm)
{
   char ch;

   if (msg)
   {
      while (ch=*msg++) {putch(ch);}

      if (trm) while (ch=*trm++) {putch(ch);}
   }
}

/*------------------------------------------------------------------------*/
/* diagnostics interface                                                  */
/*------------------------------------------------------------------------*/
/**
   This function implements a user interface for executing diagnostic
   functions of APEX floats.
*/
static void Diagnostics(void)
{
   unsigned char key;
   long val;
   char buf[80];
   
   /* define error message for out-of-range entries */
   static cc RangeError[] = "ERROR: Not in valid range.";
   
   /* define the current time and the time-out period */
   const time_t To=time(NULL), TimeOut=60;

   /* pet the watch dog */
   WatchDog();

   /* flush the console's Rx buffer */
   conio.ioflush();

   /* wait for the next keyboard-hit */
   while ((key=kbdhit())<=0)
   {
      /* arrange for the APF9 to be put in hybernate mode */
      if (!ConioActive() || difftime(time(NULL),To)>TimeOut)
      {
         cputs(" (timeout)",ctrm);
         key=CR; break;
      }
   }

   /* acknowledge key entry by spacing over */
   if (key!=CR) conio.putb(' ');
      
   switch (tolower(key))
   {
      /* run the air pump for a few seconds */
      case '1': /* deprecated */
      case 'a':
      {
         unsigned char V,A;
         
         /* notify the user that the air pump will run for 6 seconds */
         cputs("Running air pump for 6 seconds.",ctrm);

         /* run the air pump for 6 seconds */
         AirPumpRun(6,&V,&A);

         /* construct response string */
         csnprintf(buf,sizeof(buf),"Battery: [%3dcnt, %4.1fV]  "
                  "Current: [%3dcnt, %5.0fmA]",V,Volts(V),A,Amps(A)*1000);

         /* write the voltage and current to the console */
         pputs(&conio,buf,2,trm);
         
         break;
      }
      
      /* move to the ballast piston position */
      case 'b': {PistonMoveAbs(mission.PistonStoragePosition); break;}

      /* close the air valve */
      case '9': /* deprecated */
      case 'c':
      {
         AirValveClose();
         cputs("Air valve closed.",ctrm);
         break;
      }

      /* display the piston position */
      case '7': /* deprecated */
      case 'd':
      {
         csnprintf(buf,sizeof(buf),"Piston position: %d counts",PistonPosition());
         pputs(&conio,buf,2,trm);
         break;
      }
         
      /* extend the piston by 4 counts */
      case '6': /* deprecated */
      case 'e':
      {
         cputs("Extending piston 4 counts.",ctrm);
         PistonMoveRel(4);
         break;
      }

      /* goto a user specified piston position */
      case 'g':
      {
         /* get user input from the console */
         pgets(&conio,buf,sizeof(buf)-1,60,cr);

         /* process the user-input and move the piston */
         if (*buf && (val=atoi(buf))>=1 && val<=254)
         {
            csnprintf(buf,sizeof(buf),"Seeking piston position: %d",val);
            pputs(&conio,buf,2,trm);
            PistonMoveAbs(val);
         }

         /* notify user of input error */
         else cputs(RangeError,ctrm);

         break;
      }
 
      /* adjust the maximum log file size */
      case 'l':
      {
         /* get user input from the console */
         pgets(&conio,buf,sizeof(buf)-1,60,cr);

         /* process the user-input and move the piston */
         if (*buf && (val=atoi(buf))>=5 && val<=63)
         {
            MaxLogSize=val*KB;
            csnprintf(buf,sizeof(buf),"Maximum engineering log size is %ld kilobytes.",MaxLogSize/KB);
            pputs(&conio,buf,2,trm);
         }

         /* notify user of input error */
         else cputs(RangeError,ctrm);

         break;
      }

      /* open the air valve */
      case '8': /* deprecated */
      case 'o':
      {
         AirValveOpen();
         cputs("Air valve opened.",ctrm);
         break;
      }
         
      /* retract the piston by 4 counts */
      case '5': /* deprecated */
      case 'r':
      {
         cputs("Retracting piston 4 counts.",ctrm);
         PistonMoveRel(-4);
         break;
      }
      
      /* execute the SelfTest() */
      case 's':
      {
         cputs("Executing SelfTest().",ctrm);
         if (SelfTest()>0) cputs("SelfTest() passed.",ctrm);
         else cputs("SelfTest() failed.",ctrm);
         break;
      }
 
      /* exercise ToD computations */
      case 't':
      {
         /* get user input from the console */
         if (pgets(&conio,buf,sizeof(buf)-1,60,cr)>=0)
         {
            /* process the user-input */
            if (*buf && ParseTime(buf,&val)>0)
            {
               /* construct the response to the user */
               csnprintf(buf,sizeof(buf),"Start of profile cycle: %s",ctime(&val));

               /* write the response to the console */
               pputs(&conio,buf,2,ns);

               /* compute the down-time for ToD expiration */
               val += ToD(val,mission.TimeDown,mission.ToD);
               
               /* construct the response to the user */
               csnprintf(buf,sizeof(buf),"User-specified: TimeDown(%ldMin) ToD(%ldMin)\n",
                         mission.TimeDown/Min,mission.ToD/Min);

               /* write the response to the console */
               pputs(&conio,buf,2,ns);
               
               /* construct the response to the user */
               csnprintf(buf,sizeof(buf),"End of down-time: %s",ctime(&val));

               /* write the response to the console */
               pputs(&conio,buf,2,ns);
            }
            else cputs("Enter start of profile cycle: mm/dd/yyyy:hh:mm:ss",ctrm);
         }
 
         break;
      }

      /* enter protected diagnostics mode */
      case '*': {ManufacturerDiagnosticsAgent(); break;}
         
      /* print the menu */
      case '?': cputs("",ctrm); 
            
      /* the default response is to print the menu */
      default:  
      {
         cputs("Menu of diagnostics.",                            ctrm);
         cputs("?  Print this menu.",                             ctrm);
         cputs("a  Run air pump for 6 seconds.",                  ctrm);
         cputs("b  Move piston to the piston storage position.",  ctrm);
         cputs("c  Close air valve.",                             ctrm);
         cputs("d  Display piston position",                      ctrm);
         cputs("e  Extend the piston 4 counts.",                  ctrm);
         cputs("g  Goto a specified position. [1-254] (counts)",  ctrm);
         cputs("l  Set maximum engineering log size. [5-63] (KB)",ctrm);
         cputs("o  Open air valve.",                              ctrm);
         cputs("r  Retract the piston 4 counts.",                 ctrm);
         cputs("s  Execute the SelfTest().",                      ctrm);         
         cputs("t  Calculate ToD down-time expiration.",         ctrm);
         cputs("1  Run air pump for 6 seconds (deprecated).",     ctrm);
         cputs("5  Retract the piston 4 counts (deprecated).",    ctrm);
         cputs("6  Extend the piston 4 counts (deprecated).",     ctrm);
         cputs("7  Display piston position (deprecated).",        ctrm);
         cputs("8  Open air valve (deprecated).",                 ctrm);
         cputs("9  Close air valve (deprecated).",                ctrm);
      } 
   } 
}

/*------------------------------------------------------------------------*/
/* function to notify the user of a fatal mission constraint violation    */
/*------------------------------------------------------------------------*/
/*
   This function is called by AnalyzeMission() to notify the user of a fatal
   mission constraint violation.
*/
static int fatal(cc *expr, cc *why)
{
   if (!expr) cputs("fatal(): NULL function argument.",ctrm);
   else
   {
      cputs("Constraint violated: ","");
      cputs(expr,ctrm);

      if (why && *why)
      {
         cputs("   ","(");
         cputs(why,")");
         cputs(ctrm,ctrm);
      }
   }
   
   return 1;
}

/*------------------------------------------------------------------------*/
/* agent to manage the FLASH file system                                  */
/*------------------------------------------------------------------------*/
static void FlashAgent(void)
{
   unsigned char key;
   char buf[80];

   /* define the current time and the time-out period */
   const time_t To=time(NULL), TimeOut=60;

   /* pet the watch dog */
   WatchDog();

   /* flush the console's Rx buffer */
   conio.ioflush();

   /* initialize the bad-block list */
   Tc58v64BadBlockListInit();

   /* wait for the next keyboard-hit */
   while ((key=kbdhit())<=0)
   {
      /* arrange for the APF9 to be put in hybernate mode */
      if (!ConioActive() || difftime(time(NULL),To)>TimeOut)
      {
         cputs(" (timeout)",ctrm);
         key=CR; break;
      }
   }

   /* acknowledge key entry by spacing over */
   if (key!=CR) conio.putb(' ');
      
   switch (tolower(key))
   {
      /* print the list of bad blocks */
      case 'b':
      {
         /* check if the first element is the end-of-list sentinel value */
         if (BadBlock[0]!=0xffff)
         {
            int i;
            
            cputs("Bad blocks:","");

            for (i=0; i<Tc58v64BadBlockMax && BadBlock[i]!=0xffff; i++)
            {
               csnprintf(buf,sizeof(buf)," %04u",BadBlock[i]);
               pputs(&conio,buf,2,ns);
            }
            cputs("",ctrm);
         }
         else cputs("There are no bad blocks in the FLASH file system.",ctrm);

         break;
      }
      
      /* initialize the flash file system */
      case 'c':
      {
          cputs("WARNING: All of the data in FLASH "
                "will be destroyed.  Proceed? [y/n] ","");
          
         /* get confirmation that the request is intentional */
         if (pgetbuf(&conio,buf,1,5)>0 && tolower(*buf)=='y')
         {
            /* initialize the flash file system */
            cputs("",ctrm); Tc58v64Init(); fioFormat();

            /* set the mission state to inactive */
            StateSet(UNDEFINED);
         }
         else {cputs("",ctrm); cputs("Creation of FLASH file system safely aborted.",ctrm);}
         
         break; 
      }

      /* get id of flash chip */
      case 'd':
      {
         csnprintf(buf,sizeof(buf),"TC58V64 flash chip identifier: 0x%04x.",Tc58v64Id());
         pputs(&conio,buf,2,trm);
         break;
      }

      /* erase the FLASH files system */
      case 'e':
      {
          cputs("WARNING: All of the data in FLASH "
                "will be destroyed.  Proceed? [y/n] ","");
          
         /* get confirmation that the request is intentional */
         if (pgetbuf(&conio,buf,1,5)>0 && tolower(*buf)=='y')
         {
            /* initialize the flash file system */
            cputs("",ctrm); fioFormat();

            /* set the mission state to inactive */
            StateSet(UNDEFINED);
         }
         else {cputs("",ctrm); cputs("Erasing of FLASH file system safely aborted.",ctrm);}
         
         break;
      }

      /* directory listing of the flash file system */
      case 'l':
      {
         int i,n;

         cputs("Directory listing of FLASH file system:",ctrm);
         for (n=0,i=0; i<MAXFILES; i++)
         {
            /* define a buffer to hold the file name */
            char fname[64];
            
            if (fioName(i,fname,sizeof(fname))>0)
            {
               size_t len = fioLen(i); n++;
               csnprintf(buf,sizeof(buf),"   %s (%u bytes)",fname,len);
               pputs(&conio,buf,2,trm);
            }
         }
         csnprintf(buf,sizeof(buf),"Total files: %d",n);
         pputs(&conio,buf,2,trm);
         
         break;
      }
      
      /* create a report of FLASH file system errors */
      case 'r':
      {
         /* detect if the FLASH file system has been initialized */
         if (BadBlockCrc==Tc58v64BadBlockCrc())
         {
            /* create the FLASH error report */
            csnprintf(buf,sizeof(buf),"Errors in FLASH file system: correctable:%u, "
                      "uncorrectable:%u.",Tc58v64ErrorsCorrectable(),
                      Tc58v64ErrorsNoncorrectable());
            pputs(&conio,buf,2,trm);
         }
         else cputs("FLASH file system has not been created yet.",ctrm);
         
         break;
      }

      /* re-zero the error counters */
      case 'z':
      {
         cputs("Resetting FLASH file system error counts to zero.",ctrm);

         /* reset the error counters */
         Tc58v64ErrorsInit();

         break;
      }
      
      
      /* print the menu */
      case '?': cputs("",ctrm);
            
      /* the default response is to print the menu */
      default:  
      {
         cputs("Menu of FLASH file system functions.",               ctrm);
         cputs("?  Print this menu.",                                ctrm);
         cputs("Jb Print bad-block list.",                           ctrm);
         cputs("Jc Create FLASH file system (destructive).",         ctrm);
         cputs("Jd Print the FLASH chip identifier.",                ctrm);
         cputs("Je Erase the FLASH file system (destructive).",      ctrm);
         cputs("Jl Print directory listing of FLASH file system.",   ctrm);
         cputs("Jr Report FLASH errors since file system creation.", ctrm);
         cputs("Jz Reset FLASH error counters to zero.",             ctrm);
      } 
   } 
}

/*------------------------------------------------------------------------*/
/* float vitals agent                                                     */
/*------------------------------------------------------------------------*/
/**
   This function implements a user interface for configuring mission
   parameters related to float personality.
*/
static void FloatVitalsAgent(void)
{
   unsigned char key;
   long val;
   char buf[80];

   /* define error message for out-of-range entries */
   static cc RangeError[] = "ERROR: Not in valid range.";
   
   /* define the current time and the time-out period */
   const time_t To=time(NULL), TimeOut=60;

   /* pet the watch dog */
   WatchDog();

   /* flush the console's Rx buffer */
   conio.ioflush();

   /* wait for the next keyboard-hit */
   while ((key=kbdhit())<=0)
   {
      /* arrange for the APF9 to be put in hybernate mode */
      if (!ConioActive() || difftime(time(NULL),To)>TimeOut)
      {
         cputs(" (timeout)",ctrm);
         key=CR; break;
      }
   }

   /* acknowledge key entry by spacing over */
   if (key!=CR) conio.putb(' ');
      
   switch (tolower(key))
   {
      /* get the maximum air-bladder pressure */
      case 'b':
      {
         /* get user input from the console */
         pgets(&conio,buf,sizeof(buf)-1,60,cr);

         if (*buf && (val=atoi(buf))>0 && val<255)
         {
            /* compute barometric pressure inside air bladder (see SWJ14:79,p122-123) */
            float p = 22.36*3.3*val/255 - 29.92;
            
            /* assign the user-input to the mission parameters */
            mission.MaxAirBladder = val;
            
            /* create feedback string for user */
            csnprintf(buf,sizeof(buf),"Full-bladder detection criteria: "
                     "[%3d counts, %1.1f \"Hg]",mission.MaxAirBladder,p);
            
            /* write the feedback string to the console */
            pputs(&conio,buf,2,trm);
         }
         else cputs(RangeError,ctrm);

         break;
      }
         
      /* get the piston full-extension position */
      case 'f':
      {
         /* get user input from the console */
         pgets(&conio,buf,sizeof(buf)-1,60,cr);
         
         if (*buf && (val=atoi(buf))>0 && val<255)
         {
            mission.PistonFullExtension = val;
            csnprintf(buf,sizeof(buf),"Piston full-extension position: %d counts.",
                     mission.PistonFullExtension);
            pputs(&conio,buf,2,trm);
         }
         else cputs(RangeError,ctrm);
         
         break;
      }
         
      /* get the piston storage position */
      case 's':
      {
         /* get user input from the console */
         pgets(&conio,buf,sizeof(buf)-1,60,cr);
         
         if (*buf && (val=atoi(buf))>0 && val<255)
         {
            mission.PistonStoragePosition = val;
            csnprintf(buf,sizeof(buf),"Piston storage position: %d counts.",
                     mission.PistonStoragePosition);
            pputs(&conio,buf,2,trm);
         }
         else cputs(RangeError,ctrm);
         
         break;
      }
      
      /* get the OK vacuum count */
      case 'v':
      {
         /* get user input from the console */
         pgets(&conio,buf,sizeof(buf)-1,60,cr);

         if (*buf && (val=atoi(buf))>0 && val<255)
         {
            /* compute barometric pressure inside float (see SWJ14:79,p122-123) */
            float p = 22.36*3.3*val/255 - 29.92;

            /* assign the user-input to the mission parameters */
            mission.OkVacuumCount = val;

            /* create feedback string for user */
            csnprintf(buf,sizeof(buf),"Mission aborted if internal "
                      "pressure at reset exceeds %d counts (%1.1f \"Hg).", 
                     mission.OkVacuumCount,p);

            /* write the feedback string to the console */
            pputs(&conio,buf,2,trm); 
         }
         else cputs(RangeError,ctrm);
         
         break;
      }

      /* print the menu */
      case '?': cputs("",ctrm);
            
         /* the default response is to print the menu */
      default:  
      {
         cputs("Menu of float parameters.",                         ctrm);
         cputs("?  Print this menu.",                               ctrm);
         cputs("Fb Maximum air-bladder pressure. [1-254] (counts)", ctrm);
         cputs("Ff Piston full extension. [1-254] (counts)",        ctrm);
         cputs("Fs Storage piston position. [1-254] (counts)",      ctrm);
         cputs("Fv OK vacuum threshold. [1-254] (counts)",          ctrm);
      } 
   }
}

/*------------------------------------------------------------------------*/
/* agent to exercise the Garmin GPS15 engine                              */
/*------------------------------------------------------------------------*/
static void GarminGps15Agent(void)
{
   unsigned char key;
   char buf[80];

   /* define error message for  out-of-range entries */
   static cc GpsError[] = "ERROR: Attempt to query GPS failed.";

   /* define the current time and the time-out period */
   const time_t To=time(NULL), TimeOut=60;

   /* pet the watch dog */
   WatchDog();

   /* flush the console's Rx buffer */
   conio.ioflush();

   /* wait for the next keyboard-hit */
   while ((key=kbdhit())<=0)
   {
      /* arrange for the APF9 to be put in hybernate mode */
      if (!ConioActive() || difftime(time(NULL),To)>TimeOut)
      {
         cputs(" (timeout)",ctrm);
         key=CR; break;
      }
   }

   /* acknowledge key entry by spacing over */
   if (key!=CR) conio.putb(' ');
      
   switch (tolower(key))
   {
      /* upload the almanac into the GPS */
      case 'a':
      { 
         cputs("Uploading the almanac into the Garmin GPS15L-W.",ctrm);
         GpsEnable(4800); UpLoadGpsAlmanac(&gps); GpsDisable();
         AlmanacTimeStamp=time(NULL);
         cputs("Upload finished.",ctrm);
         break;
      }

      /* configure the Garmin GPS15L-W */
      case 'c':
      {
         /* save current debuglevel and reset for maximum verbosity */
         unsigned int verbosity=debugbits; debugbits=5;
         cputs("Configuring the Garmin GPS15L-W.",ctrm);
         GpsEnable(4800);
         if (ConfigGarmin15(&gps)>0) cputs("Configuration successful.",ctrm);
         else cputs(GpsError,ctrm);
         GpsDisable();
         debugbits=verbosity;
         break;
      }
      
      /* get a fix from the Garmin GPS15L-W */
      case 'f':
      {
         time_t dT,timeout=360; struct NmeaGpsFields gpsfix;
         
         cputs("Acquiring a fix from the Garmin GPS15L-W.",ctrm);

         /* power-up gps engine and flush IO ports */
         GpsEnable(4800); if (gps.ioflush) gps.ioflush();
         
         /* get a GPS fix */
         if ((dT=GetGpsFix(&gps,&gpsfix,timeout))>0)
         {
            /* write the GPS fix data to console */
            csnprintf(buf,sizeof(buf),"# GPS fix obtained in %ld seconds.",dT);
            pputs(&conio,buf,2,trm);
            #pragma strings
            snprintf(buf,sizeof(buf),"#    %8s %7s %2s/%2s/%4s %6s %4s",
                     "lon","lat","mm","dd","yyyy","hhmmss","nsat");
            #pragma strings code
            pputs(&conio,buf,2,trm);
            csnprintf(buf,sizeof(buf),"Fix: %8.3f %7.3f %02d/%02d/%04d %06ld %4d",
                      gpsfix.lon,gpsfix.lat,gpsfix.mon,gpsfix.day,gpsfix.year,
                      gpsfix.hhmmss,gpsfix.nsat);
            pputs(&conio,buf,2,trm);

         }
         else cputs(GpsError,ctrm);

         /* power-down the GPS */
         GpsDisable();
         
         break;
      }
       
      /* log output from the Garmin GPS15L-W */
      case 'l':
      {
         cputs("Logging NMEA sentences from the Garmin GPS15L-W.",ctrm);
         GpsEnable(4800);
         if (LogNmeaSentences(&gps)<0) cputs(GpsError,ctrm);
         GpsDisable();
         break;
      }

      case 't':
      {
         time_t T,timeout=360;
         cputs("Synchronizing the Apf9 clock with GPS time.",ctrm);

         /* power-up the GPS engine */
         GpsEnable(4800);

         /* get the time from the GPS */
         if ((T=GetGpsTime(&gps,timeout))>0)
         {
            /* set the Apf9's DS2404 RTC */
            if (RtcSet(T,0)>0)
            { 
               /* get the current time */
               T=time(NULL);

               /* construct the response to the user */
               csnprintf(buf,sizeof(buf),"Real time clock: %s",ctime(&T));
               
               /* write the response to the console */
               pputs(&conio,buf,2,ns);
            }
            else cputs("Attempt to set RTC failed.",ctrm);
         }
         else cputs(GpsError,ctrm);
         
         /* power-down the GPS engine */
         GpsDisable();
         
         break;
      }
       
      /* print the menu */
      case '?': cputs("",ctrm);
            
      /* the default response is to print the menu */
      default:  
      {
         cputs("Menu of Garmin GPS15L-W functions.",                 ctrm);
         cputs("?  Print this menu.",                                ctrm);
         cputs("Ga Upload almanac to GPS15L-W.",                     ctrm);
         cputs("Gc Configure the GPS15L-W.",                         ctrm);
         cputs("Gf Get GPS15L-W fix.",                               ctrm);
         cputs("Gl Log NMEA sentences from GPS15L-W.",               ctrm);
         cputs("Gt Synchronize the Apf9 clock with GPS.",            ctrm);
      } 
   } 
}

/*------------------------------------------------------------------------*/
/* agent to configure telemetry to the remote host                        */
/*------------------------------------------------------------------------*/
/**
   This function allows configuration of the telemetry to a remote host.
*/
static void HostAgent(void)
{
   unsigned char key;
   long val;
   char buf[80];

   /* define error message for out-of-range entries */
   static cc RangeError[] = "ERROR: Not in valid range.";

   /* define the current time and the time-out period */
   const time_t To=time(NULL), TimeOut=60;

   /* pet the watch dog */
   WatchDog();

   /* flush the console's Rx buffer */
   conio.ioflush();

   /* wait for the next keyboard-hit */
   while ((key=kbdhit())<=0)
   {
      /* arrange for the APF9 to be put in hybernate mode */
      if (!ConioActive() || difftime(time(NULL),To)>TimeOut)
      {
         cputs(" (timeout)",ctrm);
         key=CR; break;
      }
   }

   /* acknowledge key entry by spacing over */
   if (key!=CR) conio.putb(' ');
      
   switch (tolower(key))
   {
      /* get the dial command for the alternate host */
      case 'a':
      {
         /* get user input from the console */
         pgets(&conio,buf,sizeof(buf)-1,60,cr);
         
         if (*buf)
         {
            csnprintf(mission.alt,sizeof(mission.alt)-1,"%s",buf);
            csnprintf(buf,sizeof(buf)-1,"Dial command to alternater host: %s",mission.alt);
            pputs(&conio,buf,2,trm);
         }
         else cputs(RangeError,ctrm);
         
         break;
      }

      /* get the dial command for the primary host */
      case 'p':
      {
         /* get user input from the console */
         pgets(&conio,buf,sizeof(buf)-1,60,cr);

         if (*buf)
         {
            csnprintf(mission.at,sizeof(mission.at)-1,"%s",buf);
            csnprintf(buf,sizeof(buf)-1,"Dial command to primary host: %s",mission.at);
            pputs(&conio,buf,2,trm);
         }
         else cputs(RangeError,ctrm);

         break;
      }

      /* get the telemetry-retry period */
      case 'r':
      {
         /* get user input from the console */
         pgets(&conio,buf,sizeof(buf)-1,60,cr);
         
         if (*buf && (val=atoi(buf)*Min)>=Min && val<=6*Hour)
         {
            mission.TimeTelemetryRetry = val;
            csnprintf(buf,sizeof(buf),"The telemetry retry interval is %ld minutes.",
                     mission.TimeTelemetryRetry/Min);
            pputs(&conio,buf,2,trm);
         }
         else cputs(RangeError,ctrm);
         
         break;
      }

   
      /* get the time-out period for host connect attempts */
      case 't':
      {
         /* get user input from the console */
         pgets(&conio,buf,sizeof(buf)-1,60,cr);
         
         if (*buf && (val=atoi(buf))>=30 && val<=300)
         {
            mission.ConnectTimeOut = val;
            csnprintf(buf,sizeof(buf),"The time-out period for connecting to remote "
                     "host is %ld seconds.",mission.ConnectTimeOut);
            pputs(&conio,buf,2,trm);
         }
         else cputs(RangeError,ctrm);
         
         break;
      }
      
      /* print the menu */
      case '?': cputs("",ctrm);
            
      /* the default response is to print the menu */
      default:  
      {
         cputs("Menu for telemetry configuration to remote host.", ctrm);
         cputs("?  Print this menu.",                              ctrm);
         cputs("Ha  Dial command for alternate host.",             ctrm);
         cputs("Hp  Dial command for primary host.",               ctrm);
         cputs("Hr  Telemetry retry interval. [1-360 minutes].",   ctrm);
         cputs("Ht  Host-connect time-out. [30-300 seconds].",     ctrm);            
      } 
   } 
}

/*------------------------------------------------------------------------*/
/* interface for protected diagnostics mode                               */
/*------------------------------------------------------------------------*/
/**
   This function implements a float-manufacturer user interface for
   executing diagnostic functions of APEX floats that are unlikely or
   dangerous to be used by ordinary users.
*/
static void ManufacturerDiagnosticsAgent(void)
{
   char buf[80];
   unsigned char key;
   long val;
   unsigned int crc;
   char ChGain[2], ChMax_Detect_Prof[8], ChDspPwrDpt[6];//HM for DSP control
   
   /* define error message for out-of-range entries */
   static cc RangeError[] = "ERROR: Not in valid range.";
      
   /* define the current time and the time-out period */
   const time_t To=time(NULL), TimeOut=60;

   /* pet the watch dog */
   WatchDog();

   /* flush the console's Rx buffer */
   conio.ioflush();

   /* wait for the next keyboard-hit */
   while ((key=kbdhit())<=0)
   {
      /* arrange for the APF9 to be put in hybernate mode */
      if (!ConioActive() || difftime(time(NULL),To)>TimeOut)
      {
         cputs(" (timeout)",ctrm);
         key=CR; break;
      }
   }

   /* acknowledge key entry by spacing over */
   if (key!=CR) conio.putb(' ');
      
   switch (tolower(key))
   {
      /* fill the flash with useless files */
      case 'a':
      {
         int i; FILE *dest; char fname[32],w[2]={'w',0};

         cputs("Filling flash with test files...",ctrm);
         for (i=0; i<MAXFILES; i++)
         {
            csnprintf(fname,sizeof(fname)-1,"file.%03d",i);
            if ((dest=fopen(fname,w)))
            {
               csnprintf(buf,sizeof(buf)-1,"Created test file: %s",fname);
               fprintf(dest,fname); fclose(dest);
            }
            else {csnprintf(buf,sizeof(buf)-1,"Failed attempt to open: "
                            "%s [errno=%d].",fname,errno);}
            
            pputs(&conio,buf,2,trm);
         }
         
         break;
      }
      
      /* perform the RTC ram test */
      case 'c':
      {
         cputs("WARNING: RTC time/date information "
               "will be lost.  Proceed? [y/n] ","");
         
         /* get confirmation that the deactivation request is intentional */
         if (pgetbuf(&conio,buf,1,5)>0 && tolower(*buf)=='y')
         {
            int status=0; cputs("",ctrm); 
         
            /* execute the RTC ram test */
            cputs("Executing RTC ram test: ",""); status=Ds2404RamTest();
            if (status) {cputs("Reinitializing the RTC.",ctrm); status=Ds2404Init();}
            if (status) {cputs("Resetting the RTC.",ctrm); status= Ds2404Reset();}
            if (status) {cputs("RTC ram test passed.",ctrm);}
            else cputs("RTC ram test failed.",ctrm);
         }
         else cputs("\nRTC ram test safely aborted.",ctrm);

         break;
      }
      
      /* load the mission configuration with defaults */
      case 'd':
      {
         cputs("Mission configuration will be loaded "
               "with defaults.  Proceed? [y/n] ","");
         
         /* get confirmation that the deactivation request is intentional */
         if (pgetbuf(&conio,buf,1,5)>0 && tolower(*buf)=='y')
         {
            /* load the mission configuration with defaults */
            cputs("",ctrm); mission=DefaultMission;
            
            /* compute the signature of the mission program */
            mission.crc = Crc16Bit((unsigned char *)(&mission),
                                   sizeof(mission)-sizeof(mission.crc));
   
            /* write the new mission configuration to EEPROM */
            if (MissionParametersWrite(&mission)>0)
            {
               cputs("Default mission parameters written to EEPROM.",ctrm);
            }
            else cputs("WARNING: Attempt to write mission "
                     "parameters to EEPROM failed.",ctrm);
         }
         else {cputs("",ctrm); cputs("Pending default mission "
                    "configuration aborted.",ctrm);}

         break;
      }
      
      /* execute the eeprom test */
      case 'e':
      {
         cputs("WARNING: All of the data in EEPROM "
               "will be destroyed.  Proceed? [y/n] ","");
         
         /* get confirmation that the deactivation request is intentional */
         if (pgetbuf(&conio,buf,1,5)>0 && tolower(*buf)=='y')
         {
            /* execute the EEPROM test */
            cputs("",ctrm); EETest();

            /* write the mission parameters back to EEPROM */
            MissionParametersWrite(&mission);

            /* set the mission state to inactive */
            StateSet(UNDEFINED);
         }
         else {cputs("",ctrm); cputs("EEPROM test safely aborted.",ctrm);}
         
         break; 
      }
         
      /* set the float serial number */
      case 'f':
      {
         /* get user input from the console */
         pgets(&conio,buf,sizeof(buf)-1,60,cr);

         if (*buf && (val=atoi(buf))>=0 && val<=9999)
         {
            mission.FloatId = val;
            csnprintf(buf,sizeof(buf),"Float id: %04u",mission.FloatId);
            pputs(&conio,buf,2,trm);
         }
         else cputs(RangeError,ctrm);
         
         break;
      }
      
      /* list the mission parameters */ 
      case 'l': 
      {
         cputs("",ctrm);
         ShowPwd=1; ShowParameters();
         break;
      }
      
      /* specify mission programming */
      case 'm':
      {
         cputs("",ctrm);
         MissionProgrammingAgent();
         break;
      }
      
      /* specify password */
      case 'p':
      {
         /* get user input from the console */
         pgets(&conio,buf,sizeof(buf)-1,60,cr);

         if (*buf)
         {
            csnprintf(mission.pwd,sizeof(mission.pwd)-1,"%s",buf);
            csnprintf(buf,sizeof(buf)-1,"Password: %s",mission.pwd);
            pputs(&conio,buf,2,trm); 
            
         }
         else cputs(RangeError,ctrm);
            
         break;
      }
      
      /* perform the APF9 ram test */
      case 'r':
      {
         cputs("WARNING: All of the data in APF9 far RAM "
               "will be destroyed.  Proceed? [y/n] ","");

         /* get confirmation that the deactivation request is intentional */
         if (pgetbuf(&conio,buf,1,5)>0 && tolower(*buf)=='y')
         {
            unsigned long int nError;
            
            cputs("\nExecuting APF9 far RAM test.  RAM must "
                  "be reinitialized before deployment.",ctrm);

            /* execute the far ram test */
            if ((nError=RamTest())==0) cputs("APF9 far RAM test passed.",ctrm);
            else
            {
               csnprintf(buf,sizeof(buf),"APF9 far RAM test failed with %lu errors.",nError);
               pputs(&conio,buf,2,trm);
            }
         }
         else cputs("\nAPF9 far RAM test safely aborted.",ctrm);

         break;
      }

      /* print some important time diagnostics */
      case 's':
      {
         cputs("",ctrm);
         SequencePointsDisplay();
         break;
      }

      case 't':
      {
         /* get user input from the console */
         pgets(&conio,buf,sizeof(buf)-1,60,cr);

         /* reset the mission timer */ 
         if (*buf && (val=atol(buf))>0)
         {
            /* reset the mission timer and mission alarm */
            IntervalTimerSet(val-10,0); SetAlarm(val);

            /* show the new timing info */
            SequencePointsDisplay();
         }
         else cputs("Missing value for mission timer - ignoring.",ctrm);
            
         break;
      }

      /* load the mission configuration with defaults */
      case 'u':
      {
         /* get user input from the console */
		 //Changed substantially to take the pre-amp gain, max num of detections, and DSP power on/off depth HM
         pgets(&conio,buf,sizeof(buf)-1,60,cr);

         if (*buf)
         {
            csnprintf(mission.user,sizeof(mission.user)-1,"%s",buf);
            csnprintf(buf,sizeof(buf)-1,"User name: %s",mission.user);
            pputs(&conio,buf,2,trm); 
			
			//Scan the new Gain and Detection parameters HM
			if(mission.user[0]=='G' || mission.user[0]=='g' || 
			   mission.user[0]=='E' || mission.user[0]=='e') ChGain[0]=mission.user[1];
			else {
				ChGain[0]='0';//illegal character set gain=0 //HM
				cputs("Illegal character - set gain=0\n",ctrm);
			}
			if(mission.user[2]=='D' || mission.user[2]=='d'){
				ChMax_Detect_Prof[0]=mission.user[3];
				ChMax_Detect_Prof[1]=mission.user[4];
				ChMax_Detect_Prof[2]=mission.user[5];
				ChMax_Detect_Prof[3]=mission.user[6];
				ChMax_Detect_Prof[4]=mission.user[7];
				ChMax_Detect_Prof[5]=mission.user[8];		
			} else { //if illegal character
				cputs("Illegal character - Set Max_Detect_Prof to default",ctrm);
				ChMax_Detect_Prof[0]='9'; 
				ChMax_Detect_Prof[1]='9';
				ChMax_Detect_Prof[2]='9';
				ChMax_Detect_Prof[3]='9';
				ChMax_Detect_Prof[4]='9';
				ChMax_Detect_Prof[5]='9';	
			}

			if(mission.user[9]=='P' || mission.user[9]=='p'){
				ChDspPwrDpt[0]=mission.user[10];
				ChDspPwrDpt[1]=mission.user[11];
				ChDspPwrDpt[2]=mission.user[12];
				ChDspPwrDpt[3]=mission.user[13];
			} else { //if illegal character
				cputs("Illegal character - Set DspPwrDpt to default",ctrm);
				ChDspPwrDpt[0]='0'; 
				ChDspPwrDpt[1]='1';
				ChDspPwrDpt[2]='0';
				ChDspPwrDpt[3]='0';
			}

			ChGain[1]=0;		//Set the last char NULL
			Gain=atoi(ChGain);	//define a new gain
			ChMax_Detect_Prof[6]=0;
			Max_Detect_Prof=atol(ChMax_Detect_Prof);//define a new max number of detects per dive
			ChDspPwrDpt[4]=0;
			DspPwrDpt=atoi(ChDspPwrDpt);//DSP power on/off depth
			
			if(Gain >=0 && Gain <4 && Max_Detect_Prof >=0 && DspPwrDpt <10000 && DspPwrDpt >=0){
				csnprintf(buf,sizeof(buf)-1,"Gain=%d, Max # of Detect/Prof=%ld, DSP Pwr ON/OFF Depth=%d",Gain, Max_Detect_Prof, DspPwrDpt);
				pputs(&conio,buf,2,trm);
			} else cputs("Illegal number - do it again", ctrm);	
			
		 }//change end HM
         else cputs(RangeError,ctrm);
            
         break;
      }

      /* set user name */
      case 'v':
      {
         /* create a default user name based on the float id */
         csnprintf(mission.user,sizeof(mission.user)-1,"f%04u",mission.FloatId);
         csnprintf(buf,sizeof(buf)-1,"%sUser name: %s",trm,mission.user);
         pputs(&conio,buf,2,trm);  

         /* create a default password */
         csnprintf(mission.pwd,sizeof(mission.pwd)-1,"e=2.718");
         csnprintf(buf,sizeof(buf)-1,"Password: %s",mission.pwd);
         pputs(&conio,buf,2,trm); 

         break;
      }
      
      /* One Hertz air-solenoid test */
      case '1':
      {
         cputs("Executing 1/2Hz air-solenoid test.  Hit any key to stop.",ctrm);

         /* generate a 1Hz on/off signal */
         do
         {
            /* apply a 500 millisecond current pulse to the air-valve solenoid */
            Psd835PortGSet(AIR_OPEN);  Wait(500); Psd835PortGClear(AIR_OPEN); Wait(500);

            /* apply a 500 millisecond current pulse to the air-valve solenoid */
            Psd835PortGSet(AIR_CLOSE);  Wait(500); Psd835PortGClear(AIR_CLOSE); Wait(500);
         }
         while (kbdhit()<=0); 

         /* clear both inputs */ 
         Psd835PortGClear(AIR_OPEN|AIR_CLOSE); 
         
         break;
      }

      /* One Hertz buoyancy pump test */
      case '2':
      {
         cputs("Executing 1/2Hz buoyancy pump test.  Hit any key to stop.",ctrm);

         /* generate a 1Hz on/off signal */
         do
         {
            HDrive(0); Wait(500); HDrive( 1); Wait(500);
            HDrive(0); Wait(500); HDrive(-1); Wait(500);
         }
         while (kbdhit()<=0); 

         /* put the H-drive in a nonconductive state */
         HDrive(0);
            
         break;
      }

      /* sweep the AD channels */
      case '3':
      {
         cputs("Sweep of AD channels:",ctrm);
         Com1Disable(); Com2Disable();
         csnprintf(buf,sizeof(buf),"Piston position: %d",PistonPosition());     pputs(&conio,buf,2,trm);
         csnprintf(buf,sizeof(buf),"Barometer:       %3.1f\"Hg",Barometer());   pputs(&conio,buf,2,trm);
         csnprintf(buf,sizeof(buf),"Battery voltage: %3.1fV",BatVolts());       pputs(&conio,buf,2,trm);
         csnprintf(buf,sizeof(buf),"Battery current: %1.0fmA",BatAmps()*1000);  pputs(&conio,buf,2,trm);
         Com1Enable(9600); 
         csnprintf(buf,sizeof(buf),"Com1 current:    %1.0fmA",Com1Amps()*1000); pputs(&conio,buf,2,trm);
         Com1Disable(); Com2Enable(9600);
         csnprintf(buf,sizeof(buf),"Com2 current:    %1.0fmA",Com2Amps()*1000); pputs(&conio,buf,2,trm);
         Com1Disable(); Com2Disable();

         break;
      }

      /* timer test */
      case '4':
      {
         cputs("400Hz timer test on the APF9's "
               "TP1 pin. (5 seconds duration)",ctrm);

         /* twiddle the TP1 pin at 400Hz for 5000 milliseconds */
         Apf9Tp1(5000,400); break;
      }

      /* Com1 serial port test */
      case '5':
      {
         int n,status;

         cputs("Executing tests of Com1 serial port.",ctrm);
         
         /* enable Com1 */
         Com1Enable(9600);

         /* execute the hardware control/status line test */
         for (status=1,n=0; n<10; n++)
         {
            Com1RtsAssert(); Wait(5); if (Com1Cts()<=0) status=0;
            Com1RtsClear();  Wait(5); if (Com1Cts()!=0) status=0;
         }

         /* pass/fail report */
         cputs("Com1 hardware control/status line test ","");
         cputs((status)?"passed.":"failed.",ctrm);

         /* transmit a known test pattern */
         for (n='a'; n<='z'; n++) {com1.putb(n); Wait(2);}

         /* check for a match of the test pattern */
         for (status=1, n='a'; n<='z'; n++)
         {
            unsigned char byte=0;
            if (com1.getb(&byte)<=0 || byte!=n) status=0;
         }
         
         /* pass/fail report */
         cputs("Com1 Tx/Rx test ","");
         cputs((status)?"passed.":"failed.",ctrm);
           
         /* disable com ports 1,2 */
         Com1Disable();

         break;
      }

      /* Com2 serial port test */
      case '6':
      {
         int n,status;

         cputs("Executing tests of Com2 serial port.",ctrm);
         
         /* enable Com2 */
         Com2Enable(9600);

         /* execute the hardware control/status line test */
         for (status=1,n=0; n<10; n++)
         {
            Com2RtsAssert(); Wait(5); if (Com2Cts()<=0) status=0;
            Com2RtsClear();  Wait(5); if (Com2Cts()!=0) status=0;
         }

         /* pass/fail report */
         cputs("Com2 hardware control/status line test ","");
         cputs((status)?"passed.":"failed.",ctrm);

         /* transmit a known test pattern */
         for (n='a'; n<='z'; n++) {com2.putb(n); Wait(2);}

         /* check for a match of the test pattern */
         for (status=1, n='a'; n<='z'; n++)
         {
            unsigned char byte=0;
            if (com2.getb(&byte)<=0 || byte!=n) status=0;
         }
         
         /* pass/fail report */
         cputs("Com2 Tx/Rx test ","");
         cputs((status)?"passed.":"failed.",ctrm);
           
         /* disable com ports 1,2 */
         Com2Disable();

         break;
      }

      /* Iridium serial port test */
      case '7':
      {
         int n,status;

         cputs("Executing tests of the Iridium serial port.",ctrm);
         
         /* enable Iridium port */
         ModemEnable(19200);

         /* execute the hardware control/status line test */
         for (status=1,n=0; n<10; n++)
         {
            ModemDtrAssert(); Wait(5); if (ModemCd()<=0 || ModemDsr()<=0) status=0;
            ModemDtrClear();  Wait(5); if (ModemCd()!=0 || ModemDsr()!=0) status=0;
         }

         /* pass/fail report */
         cputs("Iridium serial port control/status line tests ","");
         cputs((status)?"passed.":"failed.",ctrm);

         /* transmit a known test pattern */
         for (n='a'; n<='z'; n++) {modem.putb(n); Wait(2);}

         /* check for a match of the test pattern */
         for (status=1, n='a'; n<='z'; n++)
         {
            unsigned char byte=0;
            if (modem.getb(&byte)<=0 || byte!=n) status=0;
         }
         
         /* pass/fail report */
         cputs("Iridium serial port Tx/Rx test ","");
         cputs((status)?"passed.":"failed.",ctrm);
           
         /* disable Iridium port */
         ModemDisable();

         break;
      }

      /* GPS serial port test */
      case '8':
      {
         int n,status;

         cputs("Executing tests of the GPS serial port.",ctrm);

         /* enable the GPS port */
         GpsEnable(4800);
 
         /* transmit a known test pattern */
         for (n='a'; n<='z'; n++) {gps.putb(n); Wait(2);}

         /* check for a match of the test pattern */
         for (status=1, n='a'; n<='z'; n++)
         {
            unsigned char byte=0;
            if (gps.getb(&byte)<=0 || byte!=n) status=0;
         }
         
         /* pass/fail report */
         cputs("GPS serial port Tx/Rx test ","");
         cputs((status)?"passed.":"failed.",ctrm);

         cputs("Executing 1Hz test of GPS port control Pin7.  "
               "Hit any key to stop.",ctrm);

         /* generate a 1Hz on/off signal */
         do {GpsPin7Assert(); Wait(500); GpsPin7Clear(); Wait(500);}
         while (kbdhit()<=0); 
         
         /* disable Iridium port */
         GpsDisable();

         break;
      }
      
      /* RF switch port test */
      case '9':
      {
         cputs("Executing 1Hz test of RF switch port (Pins 2 & 3).  "
               "Hit any key to stop.",ctrm);

         /* generate a 1Hz on/off signal */
         do
         {
            RfSwitchPin2Assert(); RfSwitchPin3Assert(); Wait(500);
            RfSwitchPin2Clear();  RfSwitchPin3Clear();  Wait(500);            
         }
         while (kbdhit()<=0); 

         break;
      }
	  //Adde to control DSP HM
	  case 'o':
	  {
		cputs("DSP power ON.", ctrm);
		DspOn();
		break;
	  }
	  //Added to control DSP HM
	  case 'k':
	  {
		cputs("DSP power OFF.", ctrm);
		DspOff();
		break;
	  }	
	  //Added to terminate the DSP program gracefully HM
	  case 'x':
	  {
		cputs("Exit DSP program gracefully.", ctrm);
		QuecomDspExit();
		break;
      }
      /* print the menu */
      case '?': cputs("",ctrm); 
            
      /* the default response is to print the menu */
      default:  
      {
         cputs("Menu of manufacturer's diagnostics.",               ctrm);
         cputs("?  Print this menu.",                               ctrm);
         cputs("a  Fill flash with test files.",                    ctrm);
         cputs("c  Perform RTC ram test.",                          ctrm);
         cputs("d  Load mission configuration with defaults.",      ctrm);
         cputs("e  EEPROM test (destructive to stored data).",      ctrm);
         cputs("f  Set float id. [0-9999]",                         ctrm);
         cputs("l  List mission parameters.",                       ctrm);
         cputs("m  Mission programming agent.",                     ctrm);
         cputs("p  Specify password for remote host.",              ctrm);
         cputs("r  Perform APF9 far RAM test.",                     ctrm);
         cputs("s  Display sequence points and timing info.",       ctrm);
         cputs("t  Set the mission timer/alarm.",                   ctrm);
         cputs("u  Specify custom user name.",                      ctrm);
         cputs("v  Set default user name and password.",            ctrm);
         cputs("1  1/2Hz air-solenoid test. (disconnect solenoid)", ctrm);
         cputs("2  1/2Hz buoyancy-pump test. (disconnect pump)",    ctrm);
         cputs("3  Sweep of AD channels.",                          ctrm);
         cputs("4  400Hz timer test on TP1. (5s duration)",         ctrm);
         cputs("5  Com1 serial port tests.",                        ctrm);
         cputs("6  Com2 serial port tests.",                        ctrm);
         cputs("7  Iridium serial port tests.",                     ctrm);
         cputs("8  GPS serial port tests.",                         ctrm);
         cputs("9  RF switch port tests.",                          ctrm);
		 cputs("o  DSP ON.",										ctrm);
		 cputs("k  DSP OFF.",										ctrm);
		 cputs("x  DSP program terminated.",						ctrm);
      } 
   } 
               
   /* compute the signature of the mission program */
   crc = Crc16Bit((unsigned char *)(&mission),sizeof(mission)-sizeof(mission.crc));

   /* check if the mission programming has changed */
   if (mission.crc!=crc)
   {
      /* assign the new mission crc */
      mission.crc=crc;
   
      /* write the new mission configuration to EEPROM */
      if (MissionParametersWrite(&mission)<=0)
      {
         cputs("WARNING: Attempt to write mission "
               "parameters to EEPROM failed.",ctrm);
      }
   }
}

/*------------------------------------------------------------------------*/
/* mission programming agent                                              */
/*------------------------------------------------------------------------*/
/**
   This function implements a user interface for configuring mission
   parameters.
*/
static void MissionProgrammingAgent(void) 
{
   int status=1;
   
   /* define error message for out-of-range entries */
   static cc RangeError[] = "ERROR: Not in valid range.";

   cputs("Entering Mission Programming Agent",ctrm);
   
   do
   {
      char buf[80];
      unsigned char key;
      long val;
      
      /* define the current time and the time-out period */
      const time_t To=time(NULL), TimeOut=300;

      /* pet the watch dog */
      WatchDog();

      /* flush the console's Rx buffer */
      conio.ioflush();
      
      /* write a prompt to stdout */
      cputs("> ",""); 

      /* wait for the next keyboard-hit */
      while ((key=kbdhit())<=0)
      {
         /* arrange for the APF9 to be put in hybernate mode */
         if (!ConioActive() || difftime(time(NULL),To)>TimeOut)
         {
            cputs(" (timeout)",ctrm);
            key=ESC; break;
         }
      }

      /* acknowledge key entry by spacing over */
      if (key!=CR) conio.putb(' ');
      
      switch (tolower(key))
      {
         /* get the pressure to enable continuous profiling */
         case 'c':
         {
            /* get user input from the console */
            pgets(&conio,buf,sizeof(buf)-1,60,cr);

            if (*buf)
            {
               if ((val=atoi(buf))<20)
               {
                  csnprintf(buf,sizeof(buf)-1,"Continuous profiling disabled.");
                  mission.PressureCP=0;
               }
               else 
               {
                  /* set the activation pressure for continuous profiling */
                  mission.PressureCP = (val<5000) ? val : 5000;
 
                  csnprintf(buf,sizeof(buf)-1,"Continuous profile activation "
                            "pressure: %0.0f decibars.",mission.PressureCP);
               }
               
               pputs(&conio,buf,2,trm);
            }
            else cputs(RangeError,ctrm);

            break;
         }

         /* get piston parameters for buoyancy control */
         case 'b': {BuoyancyControlAgent(); break;}
           
         /* branch to the loop for float parameters */
         case 'f': {FloatVitalsAgent(); break;}

         /* branch to host configuration agent */
         case 'h': {HostAgent(); break;}
 
         /* get the deep profile pressure */
         case 'j':
         {
            /* get user input from the console */
            pgets(&conio,buf,sizeof(buf)-1,60,cr);

            if (*buf && (val=atoi(buf))>0 && val<=2000)
            {
               mission.PressureProfile = val;
               csnprintf(buf,sizeof(buf),"Deep-profile pressure: "
                        "%0.0f decibars.",mission.PressureProfile);
               pputs(&conio,buf,2,trm);
            }
            else cputs(RangeError,ctrm);

            break;
         }

         /* get the park pressure */
         case 'k':
         {
            /* get user input from the console */
            pgets(&conio,buf,sizeof(buf)-1,60,cr);

            if (*buf && (val=atoi(buf))>0 && val<=2000)
            {
               mission.PressurePark = val;
               csnprintf(buf,sizeof(buf),"Park pressure: %0.0f decibars.",
                        mission.PressurePark);
               pputs(&conio,buf,2,trm);
            }
            else cputs(RangeError,ctrm);

            break;
         }

         /* list the mission parameters */ 
         case 'l': 
         {
            cputs("",ctrm);
            ShowParameters();
            break;
         }

         /* ignore the m-key - the user is already in the mission agent */
         case 'm': {cputs("",ctrm); break;}
         
         /* get the PnP cycle length */
         case 'n':
         {
            /* get user input from the console */
            pgets(&conio,buf,sizeof(buf)-1,60,cr);

            if (*buf && (val=atoi(buf))>0 && val<255)
            {
               mission.PnpCycleLength = val;
 
               if (val<254)
               {
                  csnprintf(buf,sizeof(buf),"The park-and-profile cycle "
                            "length is %d.",mission.PnpCycleLength);
                  pputs(&conio,buf,2,trm);
               }
               else cputs("Park-and-profile feature disabled.",ctrm);
            }
            else cputs(RangeError,ctrm);

            break;
         }
         
         /* exit the loop and perform sanity checks on mission programming */
         case 'q':
         {
            cputs("",ctrm);

            /* analyze the mission and check for fatal errors by masking with 0x01 */
            status=(AnalyzeMission() & 0x01);
            
            break;
         }
         
         /* get parameters to control mission timing */
         case 't': {MissionTimingAgent(); break;}

         /* perform sanity checks on the current mission programming */
         case 'z':
         {
            cputs("",ctrm);
            AnalyzeMission();
            break;
         }

         /* exit without analyzing the mission */
         case ESC: {status=0; break;}

         /* print the menu */
         case '?': cputs("",ctrm);
            
            /* the default response is to print the menu */
         default:  
         {
            cputs("Menu selections are not case sensitive.",                              ctrm);
            cputs("?  Print this menu.",                                                  ctrm);
            cputs("B  Buoyancy control agent.",                                           ctrm);
            cputs("Bh    Compensator hyper-retraction for park descent. [0-254] (counts)",ctrm);
            cputs("Bi    Ascent initiation buoyancy nudge. [25-254] (counts)",            ctrm);
            cputs("Bj    Deep-profile piston position. [1-254] (counts)",                 ctrm);
            cputs("Bn    Ascent maintenance buoyancy nudge. [5-254] (counts)",            ctrm);
            cputs("Bp    Park piston position [1-254] (counts)",                          ctrm);
            cputs("C  Continuous profile activation pressure (decibars).",                ctrm);
            cputs("F  Float vitals agent.",                                               ctrm);
            cputs("Fb    Maximum air-bladder pressure. [1-254] (counts)",                 ctrm);
            cputs("Ff    Piston full extension. [1-254] (counts)",                        ctrm);
            cputs("Fs    Storage piston position. [1-254] (counts)",                      ctrm);
            cputs("Fv    OK vacuum threshold. [1-254] (counts)",                          ctrm);
            cputs("H  Host configuration agent.",                                         ctrm);
            cputs("Ha    Dial command for alternate host.",                               ctrm);
            cputs("Hp    Dial command for primary host.",                                 ctrm);
            cputs("Hr    Telemetry retry interval. [1-360 minutes].",                     ctrm);
            cputs("Ht    Host-connect time-out. [30-300 seconds].",                       ctrm);
            cputs("J  Deep-profile pressure. (0-2000] (decibars)",                        ctrm);
            cputs("K  Park pressure. (0-2000] (decibars)",                                ctrm);
            cputs("L  List mission parameters.",                                          ctrm);
            cputs("N  Park and profile cycle length. [1-254]",                            ctrm);
            cputs("Q  Quit the mission programming agent.",                               ctrm);
            cputs("T  Mission timing agent.",                                             ctrm);
            cputs("Ta    Ascent time-out period. [120-600] (" TUnits ")",                 ctrm);
            cputs("Tc    Time-of-day for expiration of down-time [0-1439] (" TUnits ").", ctrm);
            cputs("Td    Down time (0-336 hours] (" TUnits ").",                          ctrm);
            cputs("Tj    Deep-profile descent time. [0-480] (" TUnits ").",               ctrm);
            cputs("Tk    Park descent time. (0-480] (" TUnits ").",                       ctrm);
            cputs("Tp    Mission prelude. (0-360] (" TUnits ").",                         ctrm);
            cputs("Tu    Up time (0-1440] (" TUnits ").",                                 ctrm);
            cputs("Z  Analyze the current mission programming.",                          ctrm);
         } 
      } 
   }
   while (status>0);

   /* compute the signature of the mission program */
   mission.crc = Crc16Bit((unsigned char *)(&mission), sizeof(mission)-sizeof(mission.crc));

   /* write the new mission configuration to EEPROM */
   if (MissionParametersWrite(&mission)<=0)
   {
      cputs("WARNING: Attempt to write mission parameters to EEPROM failed.",ctrm);
   }
   
   cputs("Quiting Mission Programming Agent.",ctrm);
}

/*------------------------------------------------------------------------*/
/* mission timing agent                                                   */
/*------------------------------------------------------------------------*/
/**
   This function implements a user interface for configuring mission
   parameters related to timing.
*/
static void MissionTimingAgent(void)
{
   unsigned char key;
   long val;
   char buf[80];

   /* define error message for out-of-range entries */
   static cc RangeError[] = "ERROR: Not in valid range.";
   
   /* define the current time and the time-out period */
   const time_t To=time(NULL), TimeOut=60;

   /* pet the watch dog */
   WatchDog();

   /* flush the console's Rx buffer */
   conio.ioflush();

   /* wait for the next keyboard-hit */
   while ((key=kbdhit())<=0)
   {
      /* arrange for the APF9 to be put in hybernate mode */
      if (!ConioActive() || difftime(time(NULL),To)>TimeOut)
      {
         cputs(" (timeout)",ctrm);
         key=CR; break;
      }
   }

   /* acknowledge key entry by spacing over */
   if (key!=CR) conio.putb(' ');
      
   switch (tolower(key))
   {
      /* get the maximum ascent time */
      case 'a':
      {
         /* get user input from the console */
         pgets(&conio,buf,sizeof(buf)-1,60,cr);

         if (*buf && (val=atoi(buf)*TQuantum)>=2 && val<=10*Hour)
         {
            mission.TimeOutAscent = val;
            csnprintf(buf,sizeof(buf),"The ascent-phase of the profile cycle will "
                     "time-out after %ld " TUnits ".",mission.TimeOutAscent/TQuantum);
            pputs(&conio,buf,2,trm);
         }
         else cputs(RangeError,ctrm);
         
         break;
      }

      /* get the time-of-day for expiration of down-time */
      case 'c':
      {
         /* get user input from the console */
         pgets(&conio,buf,sizeof(buf)-1,60,cr);

         if (*buf && (val=atoi(buf)*TQuantum)>=0 && val<1440*Min)
         {
            mission.ToD = val;
            csnprintf(buf,sizeof(buf),"The down-time will expire at %ld " TUnits
                     " after midnight.",mission.ToD/TQuantum);
            pputs(&conio,buf,2,trm);
         }
         else {mission.ToD=-1; cputs("The time-of-day feature has been disabled.",ctrm);}
         
         break;
      }

      /* get the down time */
      case 'd':
      {
         /* get user input from the console */
         pgets(&conio,buf,sizeof(buf)-1,60,cr);

         if (*buf && (val=atoi(buf)*TQuantum)>0 && val<=30*Day)
         {
            mission.TimeDown = val;
            csnprintf(buf,sizeof(buf),"The drift-phase of the profile cycle will "
                     "consume %ld " TUnits ".",mission.TimeDown/TQuantum);
            pputs(&conio,buf,2,trm);
         }
         else cputs(RangeError,ctrm);
         
         break;
      }

      /* get the estimated descent time from park pressure for a deep profile */
      case 'j':
      {
         /* get user input from the console */
         pgets(&conio,buf,sizeof(buf)-1,60,cr);

         if (*buf && (val=atoi(buf)*TQuantum)>=0 && val<=8*Hour)
         {
            mission.TimeDeepProfileDescent = val;
            csnprintf(buf,sizeof(buf),"Profile will begin no later than "
                     "%ld " TUnits " after descent from park depth.",
                     mission.TimeDeepProfileDescent/TQuantum);
            pputs(&conio,buf,2,trm);
         }
         else cputs(RangeError,ctrm);
         
         break;
      }

      /* get the estimated descent time from surface to park pressure */
      case 'k':
      {
         /* get user input from the console */
         pgets(&conio,buf,sizeof(buf)-1,60,cr);

         if (*buf && (val=atoi(buf)*TQuantum)>0 && val<=8*Hour)
         {
            mission.TimeParkDescent = val;
            csnprintf(buf,sizeof(buf),"Active ballasting will begin "
                      "%ld " TUnits " after descent from surface.",
                     mission.TimeParkDescent/TQuantum);
            pputs(&conio,buf,2,trm);
         }
         else cputs(RangeError,ctrm);
         
         break;
      }

      /* get the length of the mission prelude */
      case 'p':
      {
         /* get user input from the console */
         pgets(&conio,buf,sizeof(buf)-1,60,cr);

         if (*buf && (val=atoi(buf)*TQuantum)>0 && val<=6*Hour)
         {
            mission.TimePrelude = val;
            csnprintf(buf,sizeof(buf),"Mission prelude: Transmit on "
                     "the surface for %ld " TUnits " prior to first descent."
                     ,mission.TimePrelude/TQuantum);
            pputs(&conio,buf,2,trm);
         }
         else cputs(RangeError,ctrm);
         
         break;
      }
      
      /* get the up-time */
      case 'u':
      {
         /* get user input from the console */
         pgets(&conio,buf,sizeof(buf)-1,60,cr);
         
         if (*buf && (val=atoi(buf)*TQuantum)>0 && val<=24*Hour)
         {
            mission.TimeUp = val;
            csnprintf(buf,sizeof(buf),"The profile & telemetry phases of the profile "
                     "cycle will consume %ld " TUnits ".",mission.TimeUp/TQuantum);
            pputs(&conio,buf,2,trm);
         }
         else cputs(RangeError,ctrm);
         
         break;
      }

      /* print the menu */
      case '?': cputs("",ctrm); 
            
         /* the default response is to print the menu */
      default:  
      {
         cputs("Menu of mission timing parameters.",                                ctrm);
         cputs("?  Print this menu.",                                               ctrm);
         cputs("Ta Ascent time-out period. [120-600] (" TUnits ")",                 ctrm);
         cputs("Tc Time-of-day for expiration of down-time [0-1439] (" TUnits ").", ctrm);
         cputs("Td Down time (0-30 days] (" TUnits ").",                            ctrm);
         cputs("Tj Deep-profile descent time. [0-480] (" TUnits ").",               ctrm);
         cputs("Tk Park descent time. (0-480] (" TUnits ").",                       ctrm);
         cputs("Tp Mission prelude. (0-360] (" TUnits ").",                         ctrm);
         cputs("Tu Up time (0-1440] (" TUnits ").",                                 ctrm);
      } 
   } 
}
 
/*------------------------------------------------------------------------*/
/* agent to exercise modem functions                                      */
/*------------------------------------------------------------------------*/
static void ModemAgent(void)
{
   unsigned char key;
   char buf[80];
   
   /* define the current time and the time-out period */
   const time_t To=time(NULL), TimeOut=60;

   /* pet the watch dog */
   WatchDog();

   /* flush the console's Rx buffer */
   conio.ioflush();
 
   /* wait for the next keyboard-hit */
   while ((key=kbdhit())<=0)
   {
      /* arrange for the APF9 to be put in hybernate mode */
      if (!ConioActive() || difftime(time(NULL),To)>TimeOut)
      {
         cputs(" (timeout)",ctrm);
         key=CR; break;
      }
   }

   /* acknowledge key entry by spacing over */
   if (key!=CR) conio.putb(' ');
   
   switch (tolower(key))
   {
      /* query the LBT for its signal strength */
      case 'b':
      {
         float dbars=0;
         cputs("Acquiring","");
         ModemEnable(19200); 
         if ((dbars=IrSignalStrength(&modem))>=0)
         {
            csnprintf(buf,sizeof(buf)," LBT signal strength: %0.1f bars.",dbars/10);
            pputs(&conio,buf,2,trm);
         }
         else cputs(" :Failed.",ctrm);
         ModemDisable();
         break;
      }
      
      /* configure the modem */
      case 'c':
      {
         /* save current debuglevel and reset for maximum verbosity */
         int verbosity=debugbits; debugbits=5;
         cputs("Configuring the Modem.",ctrm);
         ModemEnable(19200); IrModemConfigure(&modem); ModemDisable();
         debugbits=verbosity;
         break;
      }
      
      /* query modem's firmware revision */
      case 'f':
      {
         char fwrev[32];
         
         ModemEnable(19200); 
         if (IrModemFwRev(&modem,fwrev,sizeof(fwrev))>0)
         {
            csnprintf(buf,sizeof(buf),"Modem FwRev: %s",fwrev);
            pputs(&conio,buf,2,trm);
         }
         else {cputs("Attempt to query the modem for its firmware revision failed.",ctrm);}
         
         ModemDisable();
         break;
      }
      
      /* query modem's IMEI number */
      case 'i':
      {
         char imei[16];
         
         ModemEnable(19200); 
         if (IrModemImei(&modem,imei,sizeof(imei))>0)
         {
            csnprintf(buf,sizeof(buf),"Modem IMEI: %s",imei);
            pputs(&conio,buf,2,trm);
         }
         else {cputs("Attempt to query the modem for its IMEI number failed.",ctrm);}
         
         ModemDisable();
         break;
      }
        
      /* query modem's model */
      case 'm':
      {
         char model[16];
         
         ModemEnable(19200); 
         if (IrModemModel(&modem,model,sizeof(model))>0)
         {
            csnprintf(buf,sizeof(buf),"Modem model: %s",model);
            pputs(&conio,buf,2,trm);
         }
         else {cputs("Attempt to query the modem for its model failed.",ctrm);}
         
         ModemDisable();
         break;
      }

      /* attempt to register the LBT with the Iridium system */
      case 'r':
      {
         int debug=debugbits; debugbits=5;
         cputs("Attempting to register LBT with the Iridium system.",ctrm);
         if (IrSkySearch(&modem)>0) {cputs("Iridium registration successful.",ctrm);}
         else  {cputs("Iridium registration failed.",ctrm);}
         debugbits=debug;
         break;
      }

      /* query for the SIM card's ICCID/MSISDN numbers */
      case 's':
      {
         char iccid[32],msisdn[32];

         cputs("Querying SIM card for ICCID/MSISDN (requires 60-90 seconds).",ctrm);
         
         ModemEnable(19200);

         if (IrModemIccid(&modem,iccid,msisdn,sizeof(iccid))>0)
         {
            csnprintf(buf,sizeof(buf),"SIM card ICCID: %s  MSISDN: %s",iccid,msisdn);
            pputs(&conio,buf,2,trm);
         }
         else {cputs("Attempt to query the SIM card for its ICCID/MSISDN failed.",ctrm);}
         
         ModemDisable();
         break;
      }
      
      /* print the menu */
      case '?': cputs("",ctrm);
            
      /* the default response is to print the menu */
      default:  
      {
         cputs("Menu of modem functions.",                           ctrm);
         cputs("?  Print this menu.",                                ctrm);
         cputs("Hb LBT signal strength (bars).",                     ctrm);
         cputs("Hc Configure the modem.",                            ctrm);
         cputs("Hf Query modem's firmware revision.",                ctrm);
         cputs("Hi Query modem's IMEI number.",                      ctrm);
         cputs("Hm Query modem's model.",                            ctrm);
         cputs("Hr Register the LBT with the Iridium system.",       ctrm);
         cputs("Hs Query SIM card's ICCID & MSISDN numbers.",        ctrm);
      } 
   }
}

/*------------------------------------------------------------------------*/
/* function to exectute a (destructive) test of the RAM                   */
/*------------------------------------------------------------------------*/
/**
   This function writes a test pattern to RAM and rereads the RAM to verify
   the expected test pattern.  This process involves the destruction of all
   data in the segment of memory with addresses in the range
   [Apf9FarRamLo:Apf9FarRamHi].

   This function returns a the number of errors detected.
*/
static unsigned long int RamTest(void)
{
   char buf[80];
   unsigned long int i,addr,nErrors=0;
   
   /* initialize the block size for reads/writes */
   const unsigned long BlockSize=0x4000;

   /* initialize a pointer to the far RAM */
   far unsigned char *farPointer = (far unsigned char *)Apf9FarRamLo;

   /* write the test pattern block by block */
   for(addr=Apf9FarRamLo; addr<=Apf9FarRamHi; addr+=BlockSize)
	{
      /* compute the current block number */
      unsigned int block=(addr-Apf9FarRamLo)/BlockSize;
      unsigned long int high=((addr+BlockSize)>Apf9FarRamHi)?Apf9FarRamHi:(addr+BlockSize-1);

      csnprintf(buf,sizeof(buf),"Writing test pattern: (block %02u) "
                "[0x%05lx:0x%05lx].",block,addr,high);
      pputs(&conio,buf,2,trm);

      /* loop thru each address in the block */
      for (i=0; i<BlockSize; i++)
		{
         /* compute the index relative to the beginning for far RAM */
         unsigned long int indx=(addr-Apf9FarRamLo) + i;

         /* pet the watchdog */
         WatchDog();
         
         /* prevent overindexing errors */
         if ((addr+i)>Apf9FarRamHi) break;
         
         /* write the test pattern into the current address */
         farPointer[indx] = (unsigned char)((i + 10 + block) & 0xff);
		}
	}

   /* read the test pattern block by block */
   for(addr=Apf9FarRamLo; addr<=Apf9FarRamHi; addr+=BlockSize)
	{
      /* compute the current block number */
      unsigned int block=(addr-Apf9FarRamLo)/BlockSize;
      unsigned long int high=((addr+BlockSize)>Apf9FarRamHi)?Apf9FarRamHi:(addr+BlockSize-1);
       
      csnprintf(buf,sizeof(buf),"Reading test pattern: (block %02u) "
                "[0x%05lx:0x%05lx].",block,addr,high);
      pputs(&conio,buf,2,trm);
      
      for (i=0; i<BlockSize; i++)
		{
         /* compute the index relative to the beginning for far RAM */
         unsigned long int indx=(addr-Apf9FarRamLo) + i;

         /* pet the watchdog */
         WatchDog();
         
         /* prevent overindexing errors */
         if ((addr+i)>Apf9FarRamHi) break;

         /* verify the test pattern */
         if (farPointer[indx] != (unsigned char)((i + 10 + block) & 0xff))
			{
            /* indicate failure and count the number of errors */
            ++nErrors;

            csnprintf(buf,sizeof(buf),"Error at addr=0x%05lx: wr=%02x, rd=%02x\n",
                      (unsigned long)(addr+i),(unsigned char)((i+10+block)&0xff),
                      (unsigned char)farPointer[indx]);
            pputs(&conio,buf,2,trm);
			}
		}
	}
   
   return nErrors;
}

/*------------------------------------------------------------------------*/
/* agent to exercise the Seascan PT module                                */
/*------------------------------------------------------------------------*/
/**
   This function implements a user interface for exercising functions of the
   Seascan PT module.
*/
static void SeascanAgent(void)

{
   unsigned char key;
   char buf[80];

   /* define error message for  out-of-range entries */
   static cc SeascanError[] = "ERROR: Attempt to query Seascan failed.";

   /* define the current time and the time-out period */
   const time_t To=time(NULL), TimeOut=60;

   /* pet the watch dog */
   WatchDog();

   /* flush the console's Rx buffer */
   conio.ioflush();

   /* wait for the next keyboard-hit */
   while ((key=kbdhit())<=0)
   {
      /* arrange for the APF9 to be put in hybernate mode */
      if (!ConioActive() || difftime(time(NULL),To)>TimeOut)
      {
         cputs(" (timeout)",ctrm);
         key=CR; break;
      }
   }

   /* acknowledge key entry by spacing over */
   if (key!=CR) conio.putb(' ');
      
   switch (tolower(key))
   {
     
      /* gateway mode */
      case 'g':
      {
         /* notify entery into Seascan TD gateway mode */
         cputs("Opening Seascan gateway.",ctrm);
         
         /* enter into isus gateway mode */
         SeascanGateway();
         
         break;
      }
      
      /* measure the voltage and current while the Seacan pump is running */
      case 'm':
      {
         unsigned char V,A;

         /* give the user some feedback */
         cputs("Measuring Seascan power consumption.",ctrm);
         
         /* measure the power consumption while the Seascan is running */
         if (CtdPower(&V,&A,60)>0)
         {
            /* convert counts to engineering units */
            float a=Amps(A), v=Volts(V), w=a*v;

            /* create the report */
            csnprintf(buf,sizeof(buf),"Seascan Power consumption: "
                     "%0.3fVolts * %0.3fAmps = %0.2fWatts.",v,a,w);
            pputs(&conio,buf,2,trm);
         }
         else cputs(SeascanError,ctrm);

         break;
      }
      
      /* get the Seascan serial number */
      case 'n':
      {
         char SeascanSerno[SERNOSIZE];

         /* query the Seascan for serial number */
         if (SeascanSerialNumber(SeascanSerno) > 0)
         {
            csnprintf(buf,sizeof(buf),"Seascan Serial Number: [%s]",SeascanSerno);
            pputs(&conio,buf,2,trm);
         }
         else cputs(SeascanError,ctrm);         
 
         break;
      }

      /* Seascan quick-pressure measurement */
      case 'p':
      {
         float p;

         if (SeascanGetP(&p)>0)
         {
            csnprintf(buf,sizeof(buf),"Seascan pressure: %1.2f decibars",p);
            pputs(&conio,buf,2,trm);
         }
         else cputs(SeascanError,ctrm);

         break;
      }
     
      /* Seascan PT sample */
      case 't':
      {
         float p,t;
         
         if (SeascanGetPt(&p,&t)>0)
         {
            csnprintf(buf,sizeof(buf),"Seascan P,T:    "
                     "%7.2f decibars, %7.4f degC",p,t);
            pputs(&conio,buf,2,trm);
         }
         else cputs(SeascanError,ctrm);
         
         break;
      }

      /* print the menu */
      case '?': cputs("",ctrm);
            
      /* the default response is to print the menu */
      default:  
      {
         cputs("Menu of Seascan PT module functions.",             ctrm);
         cputs("?  Print this menu.",                              ctrm);
         cputs("Sg Open Seascan Gateway.",                         ctrm);
         cputs("Sm Measure power consumption by Seascan.",         ctrm);
         cputs("Sn Display Seascan serial number.",                ctrm);
         cputs("Sp Get Seascan pressure.",                         ctrm);
         cputs("St Get Seascan P & T.",                            ctrm);
      } 
   } 
}

/*------------------------------------------------------------------------*/
/* Open a serial gateway to the Seascan TD                                */
/*------------------------------------------------------------------------*/
/**
   This function opens a gateway to the Seascan TD.  It allows commands
   (#A<cr> or -) by sending console input characters to the Seascan port.  
   The Seascan expects any returned character to be echoed back.
   Tx characters are displayed in {}.  Rx characters are displayed in [].
*/
static void SeascanGateway(void)
{
   unsigned char byte,ibyte; int n;
   int stat;
   
   /* define the current time and the time-out period */
   time_t To=time(NULL), TimeOut=120;

   /* pet the watch dog and flush the console IO buffers */
   WatchDog(); conio.ioflush();

   /* notify entery into isus gateway mode */
   cputs("Opening Seascan gateway.  Hit <ESC> key to exit.",ctrm);

      /* enable IO from Seascan TD serial port */
   CtdEnableIo(); CtdAssertWakePin();  //CtdAssertTxPin();
   
   Wait(1000);
   /* Flush the IO queues again in case break received */
   if (ctdio.ioflush) {Wait(10); ctdio.ioflush(); Wait(10);}
   
   /* polling loop to ferry traffic between isus and the console */
   for(;;)
   {
      /* pet the watch dog */
      WatchDog();
   
      /* enable interrupts CTD and 28mA serial ports */
      TXD0=1; RI0=0; ERI0=1; RI1=0; ERI1=1;

      /* check for input from the console */
      if ((byte=kbdhit_CRLF())>0) /* call routine for CR/LF */
      {
         putchar('{');
         putchar_CRLF(byte);  /* call routine that puts CR/LF */
         putchar('}');
         /* break out of the polling loop */
         if (byte==ESC) {SeascanExitCmdMode(); break;}
         
         /* transfer the byte to isus and restart the timeout period */
         ctdio.putb(byte); Wait(5); To=time(NULL);
      
      }
      
      /* check for input from Seascan TD and echo back */
      for (n=0; n<100; n++)
      {
         if ( (stat = ctdio.getb(&ibyte)) >0 )
         {
            /* echo the byte to the console */
            putchar('[');
            putchar_CRLF(ibyte); /* call routine that puts CR/LF */
            putchar(']');
            /* reset the timeout reference */
            To=time(NULL);
	    /* echo the byte to the Seascan TD */
            Wait(50);ctdio.putb(ibyte); 
         }
      }

      /* exit gateway mode on console disconnection or timeout */
      if (!ConioActive() || difftime(time(NULL),To)>TimeOut)
      {
         cputs(" (timeout)",ctrm); SeascanExitCmdMode(); break;
      }
   }

   /* disable interrupts for the CTD interface (XA serial port 0) */
   RI0=0; ERI0=0;
   
   /* announce exit from gateway mode */
   cputs(" Exiting Seascan gateway.",ctrm);

   /* flush the console's and Seascan IO buffers */
   conio.ioflush(); ctdio.ioflush(); 
   CtdClearWakePin(); 
}


/*------------------------------------------------------------------------*/
/* function to print the mission parameters to the console                */
/*------------------------------------------------------------------------*/
/**
   This function writes the mission parameters to the console.
*/
static void ShowParameters(void)
{
   char buf[80];
   #pragma strings
   const char tunits[]="("TUnits")";
   const char minutes[]="(Minutes)";
   const char seconds[]="(Seconds)";
   const char decibars[]="(Decibars)";
   const char counts[]="(Counts)";
   #pragma strings code

   #define print(buf) pputs(&conio,buf,2,trm);
   
   /* compute the signature of the mission program */
   mission.crc = Crc16Bit((unsigned char *)(&mission), sizeof(mission)-sizeof(mission.crc));
   
   /* write the firmware revision and the float id to the console */
   csnprintf(buf,sizeof(buf),"QUEphone version %06lx  sn %04d",FwRev,mission.FloatId); print(buf);
   csnprintf(buf,sizeof(buf),"User: %-41s",mission.user); print(buf);   
   if (ShowPwd)
   {
      //csnprintf(buf,sizeof(buf),"Pwd:  %s  0x%04x",mission.pwd,
      //         Crc16Bit((unsigned char *)mission.pwd,strlen(mission.pwd)));
      csnprintf(buf,sizeof(buf),"Pwd:  %s ",mission.pwd);
      print(buf); ShowPwd=0;
   }
   else
   {
      //csnprintf(buf,sizeof(buf),"Pwd:  0x%04x",
      //          Crc16Bit((unsigned char *)mission.pwd,strlen(mission.pwd)));
      csnprintf(buf,sizeof(buf),"Pwd:  %s", mission.pwd);//HM Pwd is changed to sys name, e.g., Q001
      print(buf);
   }
   
   csnprintf(buf,sizeof(buf),"Pri:  %-42s Mhp",                                  mission.at); print(buf);
   csnprintf(buf,sizeof(buf),"Alt:  %-42s Mha",                                  mission.alt); print(buf);
   
   if (inRange(0,mission.ToD,Day))
   {
      csnprintf(buf,sizeof(buf),"  %04ld ToD for down-time expiration. %-10s  Mtc",mission.ToD/TQuantum,tunits); print(buf);
   }
   else 
   {
      csnprintf(buf,sizeof(buf),"INACTV ToD for down-time expiration. %-10s  Mtc",tunits); print(buf);
   }
 
   csnprintf(buf,sizeof(buf)," %05ld Down time. %-10s                     Mtd",  mission.TimeDown/TQuantum,tunits); print(buf);
   csnprintf(buf,sizeof(buf)," %05ld Up time. %-10s                       Mtu",  mission.TimeUp/TQuantum,tunits); print(buf);
   csnprintf(buf,sizeof(buf)," %05ld Ascent time-out. %-10s               Mta",  mission.TimeOutAscent/TQuantum,tunits); print(buf);
   csnprintf(buf,sizeof(buf)," %05ld Deep-profile descent time. %-10s     Mtj",  mission.TimeDeepProfileDescent/TQuantum,tunits); print(buf);
   csnprintf(buf,sizeof(buf)," %05ld Park descent time. %-10s             Mtk",  mission.TimeParkDescent/TQuantum,tunits); print(buf);
   csnprintf(buf,sizeof(buf)," %05ld Mission prelude. %-10s               Mtp",  mission.TimePrelude/TQuantum,tunits); print(buf);
   csnprintf(buf,sizeof(buf)," %05ld Telemetry retry interval. %-10s      Mhr",  mission.TimeTelemetryRetry/Min,minutes); print(buf);
   csnprintf(buf,sizeof(buf)," %05ld Host-connect time-out. %-10s         Mht",  mission.ConnectTimeOut,seconds); print(buf);
   csnprintf(buf,sizeof(buf)," %5.0f Continuous profile activation. %-10s Mc",   mission.PressureCP,decibars); print(buf);
   csnprintf(buf,sizeof(buf)," %5.0f Park pressure. %-10s                 Mk",   mission.PressurePark,decibars); print(buf);
   csnprintf(buf,sizeof(buf)," %5.0f Deep-profile pressure. %-10s         Mj",   mission.PressureProfile,decibars); print(buf);
   csnprintf(buf,sizeof(buf),"   %03d Park piston position. %-10s          Mbp", mission.PistonParkPosition,counts); print(buf);
   csnprintf(buf,sizeof(buf),"   %03d Compensator hyper-retraction. %-10s  Mbh", mission.PistonParkHyperRetraction,counts); print(buf);
   csnprintf(buf,sizeof(buf),"   %03d Deep-profile piston position. %-10s  Mbj", mission.PistonDeepProfilePosition,counts); print(buf);
   csnprintf(buf,sizeof(buf),"   %03d Ascent buoyancy nudge. %-10s         Mbn", mission.PistonBuoyancyNudge,counts); print(buf);
   csnprintf(buf,sizeof(buf),"   %03d Initial buoyancy nudge. %-10s        Mbi", mission.PistonInitialBuoyancyNudge,counts); print(buf);
   csnprintf(buf,sizeof(buf),"   %03d Park-n-profile cycle length. %-10s   Mn",  mission.PnpCycleLength,ns); print(buf);
   csnprintf(buf,sizeof(buf),"   %03d Maximum air bladder pressure. %-10s  Mfb", mission.MaxAirBladder,counts); print(buf);
   csnprintf(buf,sizeof(buf),"   %03d OK vacuum threshold. %-10s           Mfv", mission.OkVacuumCount,counts); print(buf);
   csnprintf(buf,sizeof(buf),"   %03d Piston full extension. %-10s         Mff", mission.PistonFullExtension,counts); print(buf);
   csnprintf(buf,sizeof(buf),"   %03d Piston storage position. %-10s       Mfs", mission.PistonStoragePosition,counts); print(buf);
   csnprintf(buf,sizeof(buf)," %5u Logging verbosity. [0-5]                  D", debuglevel); print(buf);
   csnprintf(buf,sizeof(buf),"  %04x DebugBits.                                D",debugbits); print(buf);
   csnprintf(buf,sizeof(buf),"  %04x Mission signature (hex).",                   mission.crc); print(buf);

   #undef print
}

/*------------------------------------------------------------------------*/
/* function to notify the user of a mission sanity check violation        */
/*------------------------------------------------------------------------*/
/*
   This function is called by AnalyzeMission() to notify the user of a
   mission sanity check violation.
*/
static int warn(cc *expr, cc *why)
{
   if (!expr) cputs("warn(): NULL function argument.",ctrm);
   else
   {
      cputs("Sanity check violated: ","");
      cputs(expr,ctrm);

      if (why && *why)
      {
         cputs("   ","(");
         cputs(why,")");
         cputs(ctrm,ctrm);
      }
   }
   
   return 1;
}
