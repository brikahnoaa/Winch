#ifndef CONTROL_H
#define CONTROL_H (0x01f00U)

/*========================================================================*/
/* Design notes for float control firmware                                */
/*========================================================================*/
//DspStart() is added to replace complicated to power up WISPR, set GPS time
//set pre-amp gain.  HM Dec. 2015 
//Detections are checked at a fixed interval (AlarmInterval) during both 
//descent and profile.  HM Jan 2015.
//Found the AscentTimeOut is used in config.c for a different purpose by the 
//same name. Change it AscentTimeOutPeriod here. HM 12/17/2015
//Added QUEphone interfaces, QuecomGetGSP, QuecomGetDTX, DspOn and DspOff.  
//DSP power is turned on when depth exceeds DspPwrDpt or pressure activated.
//First apf9 sends real time clock and location to DSP.  Then it sends an 
//inquiry for the number of event detections to DSP board. During descent & 
//park, it asks every 10 min (AlarmInterval. During profile it is checked at 
//specific depths defined by CTD pressure table.  Oct, 2010 HM
//The DSP data processing is stopped by ASC command when it gets shallower than 
//DspPwrDpt.  A pulse is sent to DSP to turn off 15 seconds later. Oct, 2010 HM 
/**
   The APFx firmware design makes fundamental use of the concept of
   "sequence points" for controlling the flow of the profile cycle.  A
   sequence point is defined to be a point where one phase of the mission
   cycle transitions to the next phase.  Most of the sequence points are
   based on time but there are several sequence points that are event-based.
   Given a properly functioning APFx controller, the firmware guarantees the
   phase transition at each sequence point regardless of the health of any
   other float component.

   The schematics below illustrate four different parts of the mission
   cycle:

      \begin{verbatim}
      1. The pressure-activation phase.
      2. The mission prelude.
      3. A profile from the park level.
      4. A deep profile.
      \end{verbatim}

   Pressure-activation phase
   -------------------------

   The pressure-activation feature is an optional phase of the mission.  It
   was designed to accommodate requests from ship's crew to be able to deploy
   the float without being required to start it with a magnet.  One
   event-based sequence points is implemented that activates the mission
   prelude if the pressure exceeds the activation threshold.
       
   \begin{verbatim}
     |--------------------------- A -----------------------------------|
    -+----------------+-------------+----------------------------------+- Time
    P|                 .           .                                    .    
    r|                   .        .       Sequence Points                 .   
    e|                      .    .        -----------------------           .  
    s|                      B   .         B = Pressure-activation              .
    s|                                    
    u|
    r|
    e|
   \end{verbatim}

   
   Mission prelude                                                        
   ---------------

   The mission prelude is the time period between mission activation and the
   first descent.  The sequence point 'L' is time-based and is the
   transition between the mission prelude and the first descent.  The period
   of the mission prelude is user-defined.
   
   \begin{verbatim}
     |--------------------------- L -----------------------------------|
    -+-----------------------------------------------------------------+- Time
    P|                                                                  .    
    r|                                                 Sequence Points   .   
    e|                                                 ---------------    .  
    s|                                                 L = Prelude          .
    s|                                                                       
    u|
    r|
    e|
   \end{verbatim}
   


   Profile from park depth
   -----------------------

   The profile cycle for a shallow profile consists of four phases.
   
   \begin{verbatim}
     |--------------------------------------------------- C ----------------|     
     |------------------------------ P ---------------|                     |     
     |----------------- K -------------|              |                     |     
     |--- F -----|                     |           S  |                T    |     
    -+-----------+---------------------+-----------+-------------------+----+- Time
    P|.          |                     |         .                      .    
    r| .         |                     |       .       Sequence Points   .   
    e|  .        |                     |     .         ---------------    .  
    s|    .      |                     |   .           F = Descent          .
    s|       .   |                     | .             K = Park             
    u|           . . . . . . . . . . . .               S = SurfaceDetect
    r|                                                 P = Profile           
    e|                                                 T = Telemetry
     |                                                 C = Cycle
   \end{verbatim}                    
   


   Deep profile
   ------------
   
   Test for deep profile:
   (PnpCycleLength<254 && ((!(PrfId%PnpCycleLength)) || PrfId==1) ? Yes : No;

   The profile cycle for a deep profile consists of five phases.
   
   \begin{verbatim}
     |------------------------------------------------------ C -------------|     
     |--------------------------------- P ------------|                     |     
     |----------------- D --------|                   |                     |     
     |------- K -------------|    |                   |                     |     
     |--- F -----|           |    |                S  |                T    |     
    -+-----------+-----------+----+----------------+-------------------+------- Time
    P|.          |           |    |              .                      .    
    r| .         |           |    |            .       Sequence Points   .   
    e|  .        |           |    |          .         ---------------    .  
    s|    .      |           |    |        .           F = Descent          .
    s|       .   |           |    |      .             K = Park                        
    u|           . . . . . . .    |    .               Q = DeepProfile 
    r|                            |  .                 D = GoDeep
    e|                        .   |.                   S = SurfaceDetect
     |                           .                     P = Profile
     |                         .                       T = Telemetry 
     |                         Q                       C = Cycle                             
   \end{verbatim}


   The following schematic shows 3 important piston extensions.
     
   \begin{verbatim}
                    +---Full Extension (227 counts)    Full Retraction (9 counts)---+
                    |          Ballast piston position (19 counts)--+               |
                    |                                               |               |
                    V                                               V               V
     ---------------------------------------------------------------------------------------+
                                                                                            |
     -------------+---+-------------------------------------------+---+-----------+---+-----+
          +-------|   |-------------------------------------------|   |-----------|   |-----------+
         /|       |   |                                           |   |           |   |           |
     ---+ |       |   |                                           |   |           |   |           |
        | |       |   |                                           |   |           |   |           |
        | |     +-|   |-------------------------------------------|   |-----------|   |-+         |
        | |    (  |. .|                                           |. .|           |. .|  )        |
        | |     +-|   |-------------------------------------------|   |-----------|   |-+         |
        | |       +---+                                           +---+           +---+           |
     ---+ |       .   .                                               .               .           |
         \|       |   |                                               |               |           |
          +-------+---+-----------------------------------------------+---------------+-----------+
          .       |   |                                               |               |           .
       -->| 44mm  |<- |<------------------- 318mm ----------------------------------->|<- 56mm -->|
                                        (Total scope)                 |<----- 70mm -------------->|
   \end{verbatim}
*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: control.c,v 1.38.2.1 2008/09/11 20:01:37 dbliudnikas Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* Copyright University of Washington.   Written by Dana Swift.
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
 */
/** RCS log of revisions to the C source code.
 *
 * \begin{verbatim}
 * $Log: control.c,v $
 * Revision 1.38.2.1  2008/09/11 20:01:37  dbliudnikas
 * Replace SBE41 with Seascan TD.
 *
 * Revision 1.38  2008/07/14 16:53:30  swift
 * Increase length of status words from 16-bits to 32-bits.  Update
 * Sbe41cp firmware revision for new SBE controller.
 *
 * Revision 1.37  2007/10/05 22:31:41  swift
 * Change telemetry retry period to be a simple pause between cycles.
 *
 * Revision 1.36  2007/05/09 19:11:29  swift
 * Minor modifications to the code that handles the TimeOfDay feature.
 *
 * Revision 1.35  2007/05/08 18:10:16  swift
 * Added TimeOfDay feature to implement ability to schedule profiles to start
 * at a specified time of day.  Added attribution just below the copyright in
 * the main comment section.
 *
 * Revision 1.34  2007/05/07 21:04:08  swift
 * Rearranged the order in SelfTest() of GPS operations and LBT operations in
 * order to work around a 60-sec latency in SIM operations.
 *
 * Revision 1.33  2007/03/26 21:39:37  swift
 * Implemented retrieval of ICCID/MSISDN numbers from SIM card in SelfTest().
 *
 * Revision 1.32  2007/01/15 19:19:01  swift
 * Changed handling of air-valve solenoid signals to reduce bladder inflation
 * time.
 *
 * Revision 1.31  2006/12/27 16:02:53  swift
 * Changed expected firmware revision during SelfTest() of the Sbe41cp.
 *
 * Revision 1.30  2006/12/06 17:05:49  swift
 * Changed expected LBT firmware revision from ISO6001 to ISO6004.
 *
 * Revision 1.29  2006/11/25 00:10:05  swift
 * Added LogEntry() for undefined or uncaught state.
 *
 * Revision 1.28  2006/10/23 16:49:46  swift
 * Added missing include directive for flashio.h file.
 *
 * Revision 1.27  2006/10/13 18:54:06  swift
 * Added code in SelfTest() to reformat the flash file system.
 *
 * Revision 1.26  2006/10/11 20:59:24  swift
 * Integrated the new flashio file system and implemented the module-wise
 * debugging facility.
 *
 * Revision 1.25  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.24  2006/05/19 19:27:36  swift
 * Modifications to account for upgraded LBT9522A firmware.
 *
 * Revision 1.23  2006/05/15 18:55:34  swift
 * Changed SelfTest() to reflect new version of SBE41CP firmware 1.1c.
 *
 * Revision 1.22  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.21  2006/02/22 21:55:53  swift
 * Added biographical checks of the LBT to the self test.
 *
 * Revision 1.20  2006/01/13 20:40:18  swift
 * Modified expected version of SBE41CP firmware from 1.0c to 1.1a.
 *
 * Revision 1.19  2006/01/06 23:24:10  swift
 * Added GPS configuration to the SelfTest() function.
 *
 * Revision 1.18  2005/10/12 20:11:46  swift
 * Modifications to account for changes to SBE41CP API.
 *
 * Revision 1.17  2005/10/11 20:44:44  swift
 * Implemented pressure-activation of iridium floats.
 *
 * Revision 1.16  2005/09/02 22:20:26  swift
 * Added status bit-mask and logentries for shallow-water trap.
 *
 * Revision 1.15  2005/08/06 21:42:19  swift
 * Modifications to properly handle recover mode during the mission prelude as
 * well as during the mission.
 *
 * Revision 1.14  2005/07/05 22:00:14  swift
 * Added modem configuration to the self-test.
 *
 * Revision 1.13  2005/05/01 22:04:02  swift
 * Implement a feature to execute a deep profile immediately after the mission
 * prelude.
 *
 * Revision 1.12  2005/05/01 14:29:58  swift
 * Added two steps to the prelaunch self-test: Sbe41cp configuration and
 * Sbe41cp firmware validation.
 *
 * Revision 1.11  2005/02/22 21:03:06  swift
 * Minor changes to alarm times to account for HeartBeat.
 *
 * Revision 1.10  2005/01/06 00:54:04  swift
 * Modifications to prelude and telemetry phases.
 *
 * Revision 1.9  2004/12/29 23:11:27  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.8  2004/04/26 15:54:46  swift
 * Documentation correction for criteria used to determine a deep profile.
 *
 * Revision 1.7  2004/04/15 23:19:13  swift
 * Added mag-switch reset to LaunchMission().
 *
 * Revision 1.6  2004/04/14 20:43:02  swift
 * Modified the default mission parameters.
 *
 * Revision 1.5  2003/11/20 18:59:47  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.4  2003/11/12 22:40:58  swift
 * Added some diagnostic output.
 *
 * Revision 1.3  2003/09/13 14:42:58  swift
 * Added DescentTerminate() and GoDeepTerminate() to more cleanly implement the
 * mission sequencing model.
 *
 * Revision 1.2  2003/09/11 23:14:56  swift
 * Documentation added.
 *
 * Revision 1.1  2003/08/26 20:17:47  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define ControlChangeLog "$RCSfile: control.c,v $  $Revision: 1.38.2.1 $   $Date: 2010/11/11 20:01:37 $"

#include <time.h>
#include <profile.h>
#include "config.h"
#include <psd835.h>
#include <engine.h>//HM
#include "control.h"

/*------------------------------------------------------------------------*/
/* structure to contain engineering data for a single profile             */
/*------------------------------------------------------------------------*/
struct EngineeringData
{
   #define       ParkDescentPMax 9
   unsigned char ActiveBallastAdjustments;
   unsigned char AirBladderPressure;
   unsigned char AirPumpAmps;
   unsigned char AirPumpVolts;
   time_t        BuoyancyPumpOnTime;
   unsigned char BuoyancyPumpAmps;
   unsigned char BuoyancyPumpVolts;
   unsigned char ConnectionAttempts;
   unsigned char Connections;
   time_t        GpsFixTime;   
   unsigned char ParkDescentP[ParkDescentPMax];
   unsigned char ParkDescentPCnt;
   char          ParkPOutOfBand;
   struct Obs    ParkObs;
   unsigned int  ObsIndex;
   unsigned char QuiescentAmps;
   unsigned char QuiescentVolts;
   time_t        RtcSkew;
   unsigned char SeascanAmps;
   unsigned long SeascanStatus;
   unsigned char SeascanVolts;
   unsigned long status;
   unsigned char SurfacePistonPosition;
   float         SurfacePressure;
   unsigned char Vacuum;
};

/* definition of the 'status' bits in the engineering data above  */
/* Definitions not applicable to Seascan PT module left in as place holder only */
#define DeepPrf            0x00000001UL
#define ShallowWaterTrap   0x00000002UL
#define Obs25Min           0x00000004UL
#define PistonFullExt      0x00000008UL
#define AscentTimeOutPeriod 0x00000010UL
#define DownLoadCfg        0x00000020UL
#define BadSeqPnt          0x00000080UL
#define SeascanCpActive    0x00000100UL
#define SeascanPFail       0x00000200UL
#define SeascanPtFail      0x00000400UL
#define SeascanPtsFail     0x00000800UL
#define SeascanPUnreliable 0x00001000UL
#define AirSysLimit        0x00002000UL
#define WatchDogAlarm      0x00004000UL
#define PrfIdOverflow      0x00008000UL

/* define the various states that a float can be in */
enum State {UNDEFINED=-3, PACTIVATE, RECOVERY, INACTIVE, PRELUDE, DESCENT,
            PARK, GODEEP, PROFILE, TELEMETRY, EOS};

/* function prototypes */
int    DeepProfile(void);
void   InitVitals(void);
void   MissionControlAgent(void);
int    MissionLaunch(void);
int    MissionKill(void);
int    MissionValidate(void);
void   PowerOff(time_t AlarmSec);
int    SelfTest(void);
void   SequencePointsDisplay(void);
int    SequencePointsValidate(void);
time_t ToD(time_t Tref,time_t TimeDown, time_t DayMin);
void   DspOn(void);
void   DspOff(void);
void   DspStart(void);

/* store engineering data in persistent far ram */
extern persistent far struct EngineeringData vitals;
extern persistent int DspPwrStatus;
extern persistent int EngineState;
extern persistent int Command_exist;
extern persistent int RecoveryMode;
extern persistent int ReactivatedByE;
#endif /* CONTROL_H */

#include <eeprom.h>
#include <logger.h>
#include <crc16bit.h>
#include <ds2404.h>
#include <apf9.h>
#include <apf9icom.h>
#include <garmin.h>
#include <lbt9522.h>
#include <lt1598ad.h>
#include <engine.h>
#include <unistd.h>
#include <snprintf.h>
#include <seascan.h>
#include <nan.h>
#include <prelude.h>
#include <pactivat.h>
#include <descent.h>
#include <godeep.h>
#include <park.h>
#include <profile.h>
#include <telemetr.h>
#include <pactivat.h>
#include <recovery.h>
#include <string.h>
#include <tc58v64.h>
#include <flashio.h>
#include <stdlib.h>
#include <stdio.h>
//#include <gps.h>
#include "quecom.h" 
#include <config.h>

long max_detect_per_read  = 0L; // max number of detections per WISPR read (64 char/detection) HM 
time_t alarmDTX;//HM

//persistent long max_detect_per_read  = 0L; // max number of detections per WISPR read (64 char/detection) HM
//persistent int DspPwrDpt; 				   //DPS power on/off depth  
//int MesgInc  =0;      //HM
//extern persistent int      MesgInc; //HM
//int MesgInc;//HM
/*------------------------------------------------------------------------*/
/* structure to contain the time-based sequence points                    */
/*------------------------------------------------------------------------*/
persistent static struct
{
      time_t Descent;
      time_t Park;
      time_t GoDeep;
      time_t Profile;
      time_t Telemetry;
} SeqPoint;

/* define structure to contain engineering data */
persistent far struct EngineeringData vitals;
//int DspPwrDpt=10; //WISPR power ON/OFF depth in dB (m) HM
persistent int DspPwrStatus=0;
persistent int EngineState;
persistent int Command_exist;//HM 12/18/2014

/*------------------------------------------------------------------------*/
/* function to test if the current profile is a deep profile              */
/*------------------------------------------------------------------------*/
/**
   This function tests for a deep profile based on the current profile
   number.   The PnP mode is enabled only if the cycle length is in the
   closed interval [1,253].  Set PnpCycleLength=254 in order to disable PnP
   mode.  If PnP mode is enabled then this function returns a positive value
   if the current profile is a deep profile; otherwise zero is returned.
   The first profile is a deep profile as well as all profile ids that are
   evenly divisible by PnpCycleLength.
*/
int DeepProfile(void)
{
   /* initialize return status */
   int status=0;

   /* get the current profile id */
   int PrfId=PrfIdGet();

   /* check if PnP mode is enabled */
   if (mission.PnpCycleLength>0 && mission.PnpCycleLength<254) 
   {
      /* evaluate criteria for a deep profile */
      status=((!(PrfId%mission.PnpCycleLength)) || PrfId==1) ? 1 : 0;
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to initialize the engineering data                            */
/*------------------------------------------------------------------------*/
void InitVitals(void)
{
   unsigned int i;
   far unsigned char *b;
    
   /* initialize the engineering data to zeros */
   for (b=(far unsigned char *)&vitals, i=0; i<sizeof(vitals); i++) {b[i]=0;}

   /* initialize the error counters in the flash file system */
   Tc58v64ErrorsInit();
}

/*------------------------------------------------------------------------*/
/* main control loop and sequence point enforcement                       */
/*------------------------------------------------------------------------*/
/**
   This function constitutes the main control loop for detecting and
   enforcing phase transitions at sequence points.
*/
void MissionControlAgent(void)
{
   time_t Sleep;
   //time_t secs;
   int	HMDebug=0; //HM Nov2015
   float pres;
   //float DiskAvailable;
   time_t AlarmInterval=Hour/6L;    //HM Detection interval 5 or 10 min Nov2015
   //int skip;                      //HM

   /* define the logging signature */
   static cc FuncName[] = "MissionControlAgent";
  
   /* get the sequence time */
   time_t SeqTime = itimer();
   
   /* define the default alarm time */
   time_t alarm=itimer()+HeartBeat;

   /* get the current state of the mission */
   enum State state = StateGet();

   /*define the alarm interval HM 12172010*/
   //AlarmInterval=Qt_Hour; //HM
   //AlarmInterval=Ten_Min; //HM
   //skip = (int)(Half_Hour/AlarmInterval);//skip interval chking dpth in descent phase HM
   
   /* check if the watch-dog alarm indicates a power-down exception */
   if (Apf9WakeUpByRtc()&Ds2404RtfMask) vitals.status|=WatchDogAlarm;

   //debug here HM 11/30/09
   if(HMDebug){
		if(state==PARK || state==PROFILE ||state==DESCENT || state==GODEEP){
			static cc msg[]="State %d GoProfile=%d\n";
			/* make the logentry */
			LogEntry(FuncName,msg, state, GoProfile);
		} //end HM
   }
   /* apply criteria for sequence point at the end of the mission prelude */
   if (state==PRELUDE && SeqTime>=mission.TimePrelude)
   {
      /* initiate final telemetry before the mission starts */

      if (RecoveryMode == 0){ //If it is not in recovery mode, descend.  HM 12/18/2014
			static cc msg[]="Go descend after one last telemetry.\n";
         LogEntry(FuncName, msg);//HM debug
         /* terminate the telemetry phase and initiate the descent phase */
         //TelemetryTerminate(); 
         //DescentInit(); 	  //MesgInc=0;//HM
      }else if (RecoveryMode == 1 && Command_exist>=1){//command exist. Reconfigure. HM
         /* process the configuration file */
         configure(&mission,config_path);//HM
      }
	  
      PreludeTerminate(); DescentInit(); 
	   //MesgInc=0;//HM 12192010 Mesg counter
      /* arrange for a profile immediately after the mission prelude */
      SeqPoint.Descent   = (mission.TimeParkDescent>Hour) ? mission.TimeParkDescent : Hour;
      SeqPoint.Park      = SeqPoint.Descent + Hour;
      SeqPoint.GoDeep    = SeqPoint.Park + ((DeepProfile()>0) ? mission.TimeDeepProfileDescent : 0); 
      SeqPoint.Profile   = SeqPoint.GoDeep + mission.TimeOutAscent;
      SeqPoint.Telemetry = SeqPoint.GoDeep + mission.TimeUp;
    }
   
   /* apply criteria for sequence point at the end of the descent phase */
   else if (state==DESCENT && SeqTime>=SeqPoint.Descent)
   {
      /* terminate the descent phase */
      DescentTerminate();
	   GetP(&pres);
	   if(DspPwrStatus !=1  && pres>DspPwrDpt)	{
		/* create the message */
		static cc format[]="Descent-Park: p=%0.1fdB WISPR ON\n";
		/* make the logentry */
		LogEntry(FuncName,format, pres);		
		DspStart();		//Dec. 2015
		//if(EngineState==1)QuecomSetPMA(EngineState);
		//Wait(100);
		}

      /* initialize the park phase */
      ParkInit();
	  //MesgInc=0;//HM12192010 Reset Mesg counter
   }

   /* apply criteria for sequence point at the end of the park phase */
   else if (state==PARK && (SeqTime>=SeqPoint.Park || GoProfile==1))//HM added GoProfile
   {
      /* execute the tasks at the end of the park phase */
      ParkTerminate();

      /* sequence point depends whether this is a deep profile */
      (DeepProfile()>0) ? GoDeepInit() : ProfileInit();
	  alarmDTX=itimer();//HM
   }
   
   /* apply criteria for sequence point at the end of the deep-descent phase */
   else if (state==GODEEP && (SeqTime>=SeqPoint.GoDeep || GoProfile==1))
   {
      /* terminate the deep-descent phase */
      GoDeepTerminate(RETRACT);
      
      /* transition to the profile phase */
      ProfileInit();
   }
   
   /* apply criteria for sequence point at the end of the profile phase */
   else if (state==PROFILE && SeqTime>=SeqPoint.Profile)
   {
      /* create the message */
      static cc msg[]="Ascent time-out exceeded; p=%0.1fdB Abort profile WISPR OFF\n";

      /* record that the ascent period timed out */
      vitals.status|=AscentTimeOutPeriod;

      /* record the piston position */
      vitals.SurfacePistonPosition=PistonPosition();

      /* transition to the telemetry phase */
	  /* create the message */
	  GetP(&pres);
	  /* make the logentry */
      LogEntry(FuncName,msg, pres);
	  DspOff();//HM

      /* terminate the profile */
      ProfileTerminate(); 
      
      /* move the piston to full extension */
      PistonMoveAbs(mission.PistonFullExtension);

      TelemetryInit();
   }
   
   /* apply criteria for sequence point at the end of the telemetry phase */
   else if (state==TELEMETRY && SeqTime>=SeqPoint.Telemetry )
   {
      if (RecoveryMode != 1){ //It is not recovery mode, go descend.  HM 12/18/2014
      /* terminate the telemetry phase and initiate the descent phase */
      TelemetryTerminate(); DescentInit(); 	  //MesgInc=0;//HM
      }else if (RecoveryMode == 1 && Command_exist>=1){//Recovery mode, and command exist. Reconfigure. HM
       /* process the configuration file */
       configure(&mission,config_path);//HM
      }
   }

   
   /* mission state might have changed; read from EEPROM */
   switch (StateGet())
   {
      /* sleep for six hours at time in hybernate mode */
      //case INACTIVE:  {alarm=Apf9WakeTime()+6*Hour; DspOff(); break;}
      case INACTIVE:  {alarm=Apf9WakeTime()+6*Hour; break;}
	  //remove DspOff() to keep WISPR active

      case PACTIVATE: //Pressure activation
		{
         /* Make sure DPS power is ON*/
		 if(DspPwrStatus !=1  && pres>DspPwrDpt)	{
			/* make the logentry */
			/* create the message */
			static cc format[]="Pres Activation: WISPR ON\n";
			/* make the logentry */
			LogEntry(FuncName,format);
			DspStart();		//Dec. 2015
			}

			//Check if new WISPR data are avail HM
			QuecomGetDTX(max_detect_per_read);//HM	
		 
			/* execute the pressure-activation monitor */
			if (PActivate()>0) alarm = itimer()+HeartBeat;

			else
			{
				/* get the current mission time */
				SeqTime=itimer();
         
				/* compute the next regularly scheduled alarm-time */
				alarm = SeqTime - SeqTime%(2*Hour) + (2*Hour);
			}
         
         break;
		}
  
      case PRELUDE:
		{
		 
		/* execute the regularly scheduled tasks for the mission prelude */
         Prelude(); SeqTime=itimer()+HeartBeat;
		 DspPwrStatus=0; 	//added Dec2015
         
         /* compute the next regularly scheduled alarm-time */
         alarm = SeqTime + mission.TimeTelemetryRetry;

         /* ensure regularly scheduled alarm-time doesn't exceed the sequence point */
         if (alarm>mission.TimePrelude) alarm=mission.TimePrelude + HeartBeat;
         
         break;
		}

      case DESCENT: //This is the phase that WISPR WISPR power is on
		{
         /* Make sure WISPR power is ON and warm up for 20sec */
		 GetP(&pres);
		 if(DspPwrStatus !=1 && pres>DspPwrDpt) {
			static cc format[]="Descent p=%0.1fdB: Turn on WISPR\n";
			/* make the logentry */
			LogEntry(FuncName,format, pres);
			DspStart();
			}
			//Check if new WISPR data are avail 
		if(DspPwrStatus)QuecomGetDTX(max_detect_per_read);//HM 

		/* execute the regularly scheduled tasks for the descent phase */
		//Displays the phase and depth every 30 min
		//MesgInc++;							//HM
		//if(MesgInc % skip ==0) 
		Descent();   //HM
		SeqTime=itimer();
         
		/* compute the next regularly scheduled alarm-time */
		//alarm = SeqTime - SeqTime%Hour + Hour; //HM Original
		alarm = SeqTime - SeqTime%AlarmInterval + AlarmInterval; //HM changed to 10 min 11/05/10

		/* ensure regularly scheduled alarm-time doesn't exceed the sequence point */
        
		if (alarm>SeqPoint.Descent) alarm=SeqPoint.Descent + HeartBeat;
		break;
		}

	  case PARK:
		{
		 GetP(&pres);
		 if(DspPwrStatus !=1 && pres>DspPwrDpt) {
			/* make the logentry */
			static cc format[]="Park p=%0.1fdB: WISPR On\n";
			GetP(&pres);
			LogEntry(FuncName, format, pres);
			//if(EngineState==1)QuecomSetPMA(EngineState);
			DspStart();
			Wait(100);
			}

			//Check if new WISPR data are avail 
			QuecomGetDTX(max_detect_per_read); //HM
		 
			/* execute the regularly scheduled tasks for the park phase */
			/* execute the regularly scheduled tasks for the descent phase */
			//MesgInc++;							//HM
			//if(MesgInc % skip ==0) 
			Park();   //HM
			SeqTime=itimer();
          
			/* compute the next regularly scheduled alarm-time for WISPR*/
			//alarm = SeqTime - SeqTime%Hour + Hour; //Original
			//alarm = SeqTime - SeqTime%Half_Hour + Half_Hour; //HM
			alarm = SeqTime - SeqTime%AlarmInterval + AlarmInterval; //HM changed to 10 min 11/05/10
       
			/* ensure regularly scheduled alarm-time doesn't exceed the sequence point */
			if (alarm>SeqPoint.Park) alarm=SeqPoint.Park + HeartBeat;

			break;
		}

      case GODEEP:
		{

     	   //Check if new WISPR data are avail 
		   QuecomGetDTX(max_detect_per_read);

		   /* execute the regularly scheduled tasks for the deep-descent phase */
         if (GoDeep()>0) {SeqTime=itimer()+HeartBeat; alarm = SeqTime - SeqTime%(5*Min) + (5*Min);}

         /* transition to profile phase was detected; start profile in one heartbeat */
         else alarm = (itimer()+HeartBeat);
         
         /* ensure regularly scheduled alarm-time doesn't exceed the sequence point */
         if (alarm>SeqPoint.GoDeep) alarm=SeqPoint.GoDeep + HeartBeat;

         break;
		}
      
      /* execute the regularly scheduled tasks for the profile phase */
      case PROFILE:
      {
		   GetP(&pres);
		   //LogAdd("DspPwrDpt(%d)\n",DspPwrDpt); //Debug to check the DspPwrDpt still valid value  12/23/2014 HM
		   if(DspPwrStatus == 1 && pres < DspPwrDpt) {
			   /* create the message */
			   static cc format[]="Crossed WISPR-PWR-OFF depth p=%0.1fdB: WISPR OFF\n";
			   /* make the logentry */
			   LogEntry(FuncName,format, pres);
			   DspOff();
			}
			
		   //Check if new WISPR data are avail at AlarmInterval
		   //SeqTime=itimer();
		   //if(DspPwrStatus && alarmDTX<=SeqTime){
		   //  static cc format[]="DTX alarm\n";
		   //	  alarmDTX = SeqTime - SeqTime%AlarmInterval + AlarmInterval; //HM changed to 10 min 11/05/10		    
		   //  LogEntry(FuncName, format); 	
		   //  }
		   
		   if(DspPwrStatus){
		      QuecomGetDTX(max_detect_per_read);
	       }
		 
		   //This "if" statement was moved to SeaScan obs section in Profile.c . case==PROFILE 
		   //happens every 13 sec, which is too often and easily exceeds the max file size 
		   //limit (currently 40kB) and results in unsuccessful file TX.  In Profile, it is executed 
		   //at scheduled depths on the table for SeaScan P, T observations. -HM

		   /* execute the profile monitor */
           Sleep=Profile();
		 
           /* set the next alarm time based on Profile()'s recommended sleep period */
           alarm = itimer() + ((Sleep>=HeartBeat && Sleep<=15*Min) ? Sleep : HeartBeat);

           /* ensure regularly scheduled alarm-time doesn't exceed the sequence point */
           if (alarm>SeqPoint.Profile) alarm=SeqPoint.Profile + HeartBeat;
         
           break;
      }

      case TELEMETRY:
      {
		 //DspOff(); 	//making sure WISPR is off HM //Commented out Aug 8, 2015 HM	 
		 /* successful completion of telemetry is an event-based sequence point */
         //if (Telemetry()>0) {TelemetryTerminate(); DescentInit(); alarm=itimer()+HeartBeat;MesgInc=0;}
         if (Telemetry()>0) {TelemetryTerminate(); DescentInit(); alarm=itimer()+HeartBeat;}
         //if (Telemetry()>0 && RecoveryMode !=1) {//HM 12/17/2014
		 //   LogAdd("RecoveryMode %d\n", RecoveryMode);
         //   TelemetryTerminate(); DescentInit(); alarm=itimer()+HeartBeat;
         //}
         
         /* execute the regularly scheduled tasks for the telemetry phase */
         else
         {
            /* get the sequence time */ 
            SeqTime=itimer()+HeartBeat;
            
            /* compute the next regularly scheduled alarm-time */
            alarm = SeqTime + mission.TimeTelemetryRetry;
                          
            /* ensure regularly scheduled alarm-time doesn't exceed the sequence point */
            if (alarm>SeqPoint.Telemetry) alarm=SeqPoint.Telemetry + HeartBeat;
         }
         
         break;
      }

      case RECOVERY:
      {
         /* execute the regularly scheduled tasks for the recovery phase */
			//Wait (5000);
		 
		 //DspOff();//Making sure WISPR is off HM
		 Recovery();
        
         /* check if mission configuration was successfully downloaded */
		 //After ActivateRecovery(),Swift's original program reactivated the float when it 
		 //received a any new command.  The new program reactivates mission only when it 
		 //receives User(E#G######P####).
         if (!(vitals.status&DownLoadCfg))
         {
            configure(&mission,config_path);
            /* process the configuration file */
			LogAdd("ReactivatedByE=%d RecoveryMode=%d, state %d\n",ReactivatedByE, RecoveryMode, StateGet());
			if(ReactivatedByE==1){
   			   /* create the message */
   			   static cc msg[]="Reactivating mission by land command\n";
               /* terminate the recovery mode & reactivate the mission*/
   			   LogEntry(FuncName,msg); RecoveryTerminate(); alarm=itimer()+HeartBeat;
   			   RecoveryMode = 0;   //HM 12/28/2014
   			   ReactivatedByE = 0; //Mission was reactivated by E //HM 12/11/2014
			}else{
            /* activate recovery mode */
			RecoveryMode=1;
            fformat(); RecoveryInit();
			}	
			
		}
         else
         {
            /* get the sequence time */ 
            SeqTime=itimer()+HeartBeat; 
             
            /* compute the next regularly scheduled alarm-time */
            alarm = SeqTime + mission.TimeTelemetryRetry;
         }

         break;
      }
                            
      /* confused state; restart the mission */
      case UNDEFINED: default:
      {
         /* create a logentry */
         static cc msg[]="Logic error - uncaught state[%d].  Restarting mission.\n";
         LogEntry(FuncName,msg,StateGet());

         /* restart mission */
         PreludeInit(); alarm=itimer()+HeartBeat;
      }
   }

   /* set the alarm and go to sleep */
   PowerOff(alarm);
}

/*------------------------------------------------------------------------*/
/* function to launch a new mission                                       */
/*------------------------------------------------------------------------*/
/**
   This function launches the float on a new mission.
*/
int MissionLaunch(void)
{
   /* define the logging signature */
   static cc FuncName[] = "MissionLaunch()";

   int status=0;
   unsigned char VCnt,ACnt;
   
   /* reset the magnetic reset-switch */
   MagSwitchReset();

   /* open the air-valve so that self-test measures internal vacuum */
   AirValveOpen();
   
   /* run the air pump for 1 second as an audible startup signal */
   AirPumpRun(1,&VCnt,&ACnt); 

   /* save the air pump current and volts */
   vitals.AirPumpVolts=VCnt; vitals.AirPumpAmps=ACnt;

   /* open the air-valve again and wait a few seconds for bladder to deflate */
   AirValveOpen(); sleep(10);
      
   /* create a log file name */
   snprintf(log_path,sizeof(log_path),"%04d.000.log",mission.FloatId);

   /* format the file system and open the log file */
   LogClose(); fformat(); LogOpen(log_path,'w');

   /* run the self-test before the mission */
   if (SelfTest()<=0)
   {
      /* create the message */
      static cc msg[]="Self-test failed - aborting mission.\n";

      /* make the logentry */
      LogEntry(FuncName,msg);

      /* kill the mission */
      MissionKill();
   }
   
   /* initialize the mission prelude */
   else {status=PreludeInit();}
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to kill the current mission                                   */
/*------------------------------------------------------------------------*/
/**
   This function kills the current mission.
*/
int MissionKill(void)
{
   StateSet(INACTIVE);
   
   /* read the mission parameters from EEPROM */
   if (!MissionParametersRead(&mission))
   {
      /* copy the default mission to the active mission */
      mission=DefaultMission;
   }
   
}

/*------------------------------------------------------------------------*/
/* compute the mission signature to validate the mission stored in RAM    */
/*------------------------------------------------------------------------*/
/**
   This function computes the mission signature and compares it to the
   mission stored in RAM in order to validate the mission configuration.
   If the computed signature matches that stored in RAM then the mission is
   valid.  If the match fails then the mission is read from EEPROM.  If the
   mission stored in EEPROM is invalid then a default mission is assigned.

   This function returns a positive number on success and zero otherwise.
*/
int MissionValidate(void)
{
   /* define the logging signature */
   static cc FuncName[] = "MissionValidate()";

   int status=1;

   /* compute the mission signature */
   unsigned int crc = Crc16Bit((unsigned char *)(&mission), sizeof(mission)-sizeof(mission.crc));

   /* check if the computed signature matches the signature stored in the mission object */
   if (crc!=mission.crc)
   {
      /* create the message */
      static cc msg[]="Reading mission program from EEPROM.\n";

      /* make the logentry */
      LogEntry(FuncName,msg);

      /* read mission parameters from EEPROM */
      if (!MissionParametersRead(&mission))
      {
         /* create the message */
         static cc msg[]="Failed --- no mission stored in EEPROM.\n";

         /* make the logentry */
         LogEntry(FuncName,msg);

         /* indicate that the mission could not be read from EEPROM */
         status=0;
      }
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to properly power-down the APF9 controller                    */
/*------------------------------------------------------------------------*/
void PowerOff(time_t AlarmSec)
{
   /* reset the magnetic reset switch before powering down */
   MagSwitchReset();

   /* power down the APFx controller */
   Apf9PowerOff(AlarmSec);
}

/*------------------------------------------------------------------------*/
/* function to execute a self-test                                        */
/*------------------------------------------------------------------------*/
/**
   This function executes a self-test and returns a positive value if the
   self-test passed.  Zero is returned if the self-test failed.
*/
int SelfTest(void)
{
   /* define the logging signature */
   static cc FuncName[] = "SelfTest()";

   /* initialize the return value */
   int status = 1; float p; char buf[32],msisdn[32];
   
   /* define the Seascan serial number */
   char SeascanSerno[SERNOSIZE];

   /* define the recommended LBT model */
   const char *LbtModel="9522A";

   /* define the recommended LBT firmware revision */
   const char *LbtFwRev="IS06004";

   /* set the maximum log size to 40kb */
   //MaxLogSize=40960L;  //HM
   
   /* power-up the LBT early to work around 60-sec response for ICCID/MSISDN numbers */
   ModemEnable(19200);

   /* verify that the internal vacuum satisfies safe criteria */
   if ((vitals.Vacuum=BarometerAd8())>mission.OkVacuumCount)
   {
      /* create the message */
      static cc format[]="Failed: int P[%d,%0.1f\"Hg] "
         "exceeds thrshld[%d,%0.1f\"Hg].\n";

      /* make the logentry */
      LogEntry(FuncName,format,vitals.Vacuum,inHg(vitals.Vacuum),
               mission.OkVacuumCount,inHg(mission.OkVacuumCount));

      /* indicate failure */
      status=0;
   }
   else if (debuglevel>=2 || (debugbits&CONTROL_H))
   {
      /* create the message */
      static cc format[]="Passed: int P [%d,%0.1f\"Hg] <"
         "thrshld [%d,%0.1f\"Hg].\n";

      /* make the logentry */
      LogEntry(FuncName,format,vitals.Vacuum,inHg(vitals.Vacuum),
               mission.OkVacuumCount,inHg(mission.OkVacuumCount));
   }

   /* read the FLASH bad-block list from EEPROM */
   Tc58v64BadBlockListGet();

   /* check if the flash file system has been initialized */
   if (BadBlockCrc==Tc58v64BadBlockCrc())
   {
      static cc msg[]="Passed: FLASH file system has been initialized.\n";
      LogEntry(FuncName,msg);
   }
   else
   {
      static cc msg[]="Warning: FLASH file system has not been initialized.\n";
      LogEntry(FuncName,msg);
   }
   
   /* reformat the FLASH file system */
   if (fioFormat()>0)
   {
      static cc msg[]="Passed: FLASH file system has been reformatted.\n";
      LogEntry(FuncName,msg);
   }
   else
   {
      static cc msg[]="Failed: Attempt to reformat FLASH file system failed.\n";
      LogEntry(FuncName,msg);

      /* indicate failure */
      status=0;
   }

   /* verify successful Seaseascan PT module communications */
   if (GetP(&p)<=0)
   {
      /* create the message */
      static cc msg[]="Failed: Response not received from Seascan PT module.\n";

      /* make the logentry */
      LogEntry(FuncName,msg);

      /* indicate failure */
      status=0;
   }
   else if (debuglevel>=2 || (debugbits&CONTROL_H))
   {
      /* create the message */
      static cc msg[]="Passed: Response received from Seascan PT module.\n";

      /* make the logentry */
      LogEntry(FuncName,msg);
   }

   /* request the Seascan PT module serial number */
   if ( SeascanSerialNumber(SeascanSerno) == SeascanOk)
   {
      /* create the message */
      static cc format[]="Passed: Seascan Serial Number [%s] \n";    
      /* make the log entry */
      LogEntry(FuncName,format,SeascanSerno);
   }
   else
   {
      /* create the message */
      static cc format[]="Failed: Seascan Serial Number [%s] \n";    
      /* make the log entry */
      LogEntry(FuncName,format,SeascanSerno);
   }

   /* check the LBT model */
   if (IrModemModel(&modem,buf,sizeof(buf))<=0)
   {
      /* create the message */
      static cc msg[]="Warning: Query for LBT model failed.\n";
 
      /* make the logentry */
      LogEntry(FuncName,msg);
   }

   /* verify the LBT model */
   else if (strcmp(LbtModel,buf))
   {
      static cc format[]="Warning: Unexpected LBT model: [%s] != [%s] (expected).\n";
      LogEntry(FuncName,format,buf,LbtModel);
   }

   /* LBT model OK */
   else if (debuglevel>=2 || (debugbits&CONTROL_H))
   {
      /* create the log message */
      static cc format[]="Passed: LBT model: %s\n";
      
      /* make the log entry */
      LogEntry(FuncName,format,buf);
   }

   /* check the LBT firmware revision */
   if (IrModemFwRev(&modem,buf,sizeof(buf))<=0)
   {
      /* create the message */
      static cc msg[]="Warning: Query for LBT firmware revision failed.\n";
 
      /* make the logentry */
      LogEntry(FuncName,msg);
   }

   /* verify the LBT model */
   else if (strcmp(LbtFwRev,buf))
   {
      static cc format[]="Warning: Unexpected LBT firmare: [%s] != [%s] (expected).\n";
      LogEntry(FuncName,format,buf,LbtFwRev);
   }

   /* LBT firmware OK */
   else if (debuglevel>=2 || (debugbits&CONTROL_H))
   {
      /* create the log message */
      static cc format[]="Passed: LBT firmware revision: %s\n";

      /* make the log entry */
      LogEntry(FuncName,format,buf);
   }

   /* configure the LBT */
   if (IrModemConfigure(&modem)<=0)
   {
      /* create the message */
      static cc msg[]="Failed: Attempt to configure the LBT failed.\n";
 
      /* make the logentry */
      LogEntry(FuncName,msg);

      /* indicate failure */
      status=0;
   }
   else if (debuglevel>=2 || (debugbits&CONTROL_H))
   {
      /* create the message */
      static cc msg[]="Passed: LBT configuration was successful.\n";

      /* make the logentry */
      LogEntry(FuncName,msg);
   }

   /* check the LBT IMEI */
   if (IrModemImei(&modem,buf,sizeof(buf))<=0)
   {
      /* create the message */
      static cc msg[]="Warning: Query for LBT IMEI failed.\n";
 
      /* make the logentry */
      LogEntry(FuncName,msg);
   }

   /* LBT IMEI OK */
   else if (debuglevel>=2 || (debugbits&CONTROL_H))
   {
      /* create the log message */
      static cc format[]="Passed: LBT IMEI: %s\n";

      /* make the log entry */
      LogEntry(FuncName,format,buf);
   }

   /* query the SIM ICCID/MSISDN numbers */
   if (IrModemIccid(&modem,buf,msisdn,sizeof(buf))<=0)
   {
      /* create the message */
      static cc msg[]="Warning: Query for SIM card ICCID/MSISDN failed.\n";
 
      /* make the logentry */
      LogEntry(FuncName,msg);
   }

   /* SIM ICCID/MSISDN numbers OK */
   else if (debuglevel>=2 || (debugbits&CONTROL_H))
   {
      /* create the log message */
      static cc format[]="Passed: SIM card ICCID:%s  MSISDN:%s\n";

      /* make the log entry */
      LogEntry(FuncName,format,buf,msisdn);
   }

   /* power-down the LBT */
   ModemDisable();

   /* power-up the GPS */
   GpsEnable(4800);

   if (ConfigGarmin15(&gps)<=0)
   {
      /* create the message */
      static cc msg[]="Failed: Attempt to configure GPS failed.\n";
 
      /* make the logentry */
      LogEntry(FuncName,msg);

      /* indicate failure */
      status=0;
   }

   /* power-down the GPS */
   GpsDisable();
   
   sleep(1);

   return status;
}
 
/*------------------------------------------------------------------------*/
/* function to display the time-based sequence points                     */
/*------------------------------------------------------------------------*/
/**
   This function displays the time-based sequence points.
*/
void SequencePointsDisplay(void)
{
   char buf[256],*phase;
   enum State state = StateGet();

   /* create a string that describes the current mission state */
   switch (state)
   {
      case INACTIVE:  {phase="Hybernate."; break;}
      case PACTIVATE: {phase="Pressure-activation."; break;}
      case RECOVERY:  {phase="Recovery."; break;}
      case PRELUDE:   {phase="Mission Prelude."; break;}
      case DESCENT:   {phase="Park Descent."; break;}
      case PARK:      {phase="Park."; break;}
      case GODEEP:    {phase="Profile Descent."; break;}
      case PROFILE:   {phase="Profile."; break;}
      case TELEMETRY: {phase="Telemetry."; break;}
      case UNDEFINED: {phase="Undefined."; break;}
      default:        {phase="Indeterminant."; break;}
   }

   /* create a string with the mission time, alarm time, and mission phase */
   snprintf(buf,sizeof(buf),"Mission time: %ldsec   Alarm time: %ldsec\n"
            "Phase of mission cycle: %s",itimer(),ialarm(),phase);

   /* write the mission time, alarm time, and mission phase to the console */
   pputs(&conio,buf,2,"\n");
   
   /* create a report for an active mission */
   if (state>INACTIVE && state<EOS)
   {
      /* create a report for the mission prelude */
      if (state==PRELUDE) {snprintf(buf,sizeof(buf),"Sequence Point: Mission "
                                    "prelude at %ldsec.",mission.TimePrelude);}
   
      /* create a report for an active mission */
      else
      {
         snprintf(buf,sizeof(buf),"Sequence Points:\n"
                  "   Park descent:    %6ldsec\n"
                  "   Park:            %6ldsec\n" 
                  "   Profile descent: %6ldsec\n" 
                  "   Profile[%4s]:   %6ldsec\n" 
                  "   Telemetry:       %6ldsec",
                  SeqPoint.Descent,SeqPoint.Park,SeqPoint.GoDeep,
                  ((DeepProfile()>0)?"Deep":"Park"),SeqPoint.Profile,
                  SeqPoint.Telemetry);
      }
      
      /* write the report to the console */
      pputs(&conio,buf,2,"\n");
   }
}

/*------------------------------------------------------------------------*/
/* function to validate the sequence points                               */
/*------------------------------------------------------------------------*/
/**
   This function verifies that sequence points satisfies ordering
   constraints.  If the ordering constraints are violated, this function
   adjusts the times of the sequence points as necessary.
*/
int SequencePointsValidate(void)
{
   /* define the logging signature */
   static cc FuncName[] = "SequencePointsValidate()";

   /* initialize the return value */
   int status=1;
   
   /* initialize the mission down-time */
   time_t TimeDown=mission.TimeDown;

   /* check if down-time should end at a user-specified time-of-day */
   if (inRange(0,mission.ToD,Day)) TimeDown=ToD(time(NULL),mission.TimeDown,mission.ToD);
      
   /* compute the sequence points */
   SeqPoint.Descent   = mission.TimeParkDescent;
   SeqPoint.Park      = TimeDown - ((DeepProfile()>0) ? mission.TimeDeepProfileDescent : 0); 
   SeqPoint.GoDeep    = TimeDown;
   SeqPoint.Profile   = TimeDown + mission.TimeOutAscent;
   SeqPoint.Telemetry = TimeDown + mission.TimeUp;
   
   /* validate the Profile sequence point */
   if (SeqPoint.Profile>SeqPoint.Telemetry)
   {
      /* create the message */
      static cc format[]="Invalid sequence points: "
         "Profile[%ldsec]>Telemetry[%ldsec] (rectified).\n";

      /* make the logentry */
      LogEntry(FuncName,format,SeqPoint.Profile,SeqPoint.Telemetry);
      
      /* recompute valid sequence points */
      SeqPoint.Profile = TimeDown + mission.TimeUp/2; vitals.status|=BadSeqPnt;
   }
   
   /* validate the Park sequence point */
   if (SeqPoint.Park>SeqPoint.GoDeep)
   {
      /* create the message */
      static cc format[]="Invalid sequence points: "
         "Park[%ldsec]>GoDeep[%ldsec] (rectified).\n";

      /* make the logentry */
      LogEntry(FuncName,format,SeqPoint.Park,SeqPoint.GoDeep);
      
      /* recompute valid sequence points */
      SeqPoint.Park=TimeDown; vitals.status|=BadSeqPnt;
   }
   
   /* validate the Descent sequence point */
   if (SeqPoint.Descent>SeqPoint.Park)
   {
      /* create the message */
      static cc format[]="Invalid sequence points: "
         "Descent[%ldsec]>Park[%ldsec] (rectified).\n";

      /* make the logentry */
      LogEntry(FuncName,format,SeqPoint.Descent,SeqPoint.Park);
      
      /* recompute valid sequence points */
      SeqPoint.Descent = SeqPoint.Park/2; vitals.status|=BadSeqPnt;
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* adjust down-time to start profile at user specified time-of-day        */
/*------------------------------------------------------------------------*/
time_t ToD(time_t Tref,time_t TimeDown, time_t DaySec)
{
   /* validate the function arguments */
   if (TimeDown>=0 && inRange(0,DaySec,Day)) 
   {
      /* compute the estimated date/time for the start of the profile */
      time_t skew,daysec,t=Tref+TimeDown; struct tm T; T = *gmtime(&t);

      /* compute the seconds past midnight for the estimated start time */
      daysec=T.tm_sec + T.tm_min*Min + T.tm_hour*Hour;

      /* validate that the skew is less than one day */
      if (labs((skew=(DaySec-daysec)))<Day)
      {
         /* adjust down-time to end at specified seconds after midnight */
         TimeDown += skew; if (skew<0) TimeDown += Day;
      }
   }
   
   return TimeDown;
}
void DspOn(void)//Added function to power ON WISPR HM
{
	//LogAdd("Turn ON WISPR\n");
	Psd835PortASet(PTT_PLL_ON); Wait(10);   //tell WISPR to turn on HM 10-29/09
	//Psd835PortAClear(PTT_TX_ON);//keep the OFF line low
	Psd835PortAClear(PTT_PLL_ON); 
	DspPwrStatus=1;
	Wait(10000);WatchDog();  //tell WISPR to turn on HM 10-29/09
	Wait(7000);WatchDog();
	//Wait(3000);WatchDog(); //Additional wait added 9/19/2014 HM
}
void DspOff(void)//Added function to power OFF WISPR HM
{
    QuecomDspExit();
	//LogAdd("WISPR OFF\n");
	Wait(15000);//kill time until the WISPR program closes files and ends properly.
	Psd835PortASet(PTT_TX_ON); Wait(10); //Send a pulse to WISPR to turn off HM 10-29/09
    Psd835PortAClear(PTT_TX_ON); Wait(10); //WISPR to turn off HM 10-29/09
	//Psd835PortAClear(PTT_PLL_ON); //keep the ON line low
	DspPwrStatus =0;
}
void DspStart(void)
{
	float DiskAvailable;
	time_t secs;
    static cc FuncName[] = "MissionControlAgent";	
	int DSP_OFF = 1;
	if(DSP_OFF == 1){
		/* create the message */
		static cc msg[]="Data storage available %6.2f percent\n";
		/* make the logentry */
		DspOn();//this takes 17 seconds
		WatchDog();
		//Wait until disk space info becomes available from WISPR.  ~50 sec for Lexar 512GB HM 
		//This wait-duration depends on the size of CF card 
		Wait(41000);
		WatchDog();
		secs= time(NULL);
		QuecomSetGPS(secs, GPSlon, GPSlat);//sync time and get GPS position
		Wait(10500);
		WatchDog();
		if(debuglevel >1 || DSP_OFF == 1){
			static cc format2[]="GPS time sent to ext device\n";
			LogEntry(FuncName,format2);
			}
		//if(EngineState==1)QuecomSetPMA(EngineState);
		//Move the gain command here 7/13/2015
		QuecomSetNCM(Gain, DspPwrDpt);//Set Gain
		if(debuglevel >1 || DSP_OFF == 1){
			static cc format1[]="Gain=%d sent to ext device\n";
			LogEntry(FuncName,format1, Gain);
			}			
		Wait(1000);
				
		DiskAvailable=QuecomInqDisk(); //Inquire disk space available in %
		LogEntry(FuncName,msg,DiskAvailable);
		if(DiskAvailable <5.0){
			static cc msg[]="Warning! Disk < 5 percent. Make WISPR-ON depth to 9999m.\n";
			LogEntry(FuncName,msg);
			WatchDog();
			//DspPwrDpt = 9999;//Make the WISPR pwr ON depth large so that it never turns ON
			//DspOff();//turn off 
			//WatchDog();
		}
	}
}

