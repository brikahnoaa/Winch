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
