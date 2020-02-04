#ifndef TELEMETR_H
#define TELEMETR_H (0x0100U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: telemetr.c,v 1.25.2.1 2008/09/11 19:58:15 dbliudnikas Exp $
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
 * $Log: telemetr.c,v $
 * Revision 1.25.2.1  2008/09/11 19:58:15  dbliudnikas
 * Replace SBE41 with Seascan TD: Remove salinity display.
 *
 * Revision 1.25  2008/07/14 16:58:01  swift
 * Implemented 32-bit status words.
 *
 * Revision 1.24  2007/10/05 22:31:00  swift
 * Impose pneumatic inflation limits.
 *
 * Revision 1.23  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.22  2007/01/15 19:19:01  swift
 * Changed handling of air-valve solenoid signals to reduce bladder inflation
 * time.
 *
 * Revision 1.21  2006/10/11 20:53:55  swift
 * Modifications for compatibility with the new flashio file system.
 *
 * Revision 1.20  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.19  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.18  2005/09/23 14:45:22  swift
 * Minor changes to formatting of the float-id and profile-id.
 *
 * Revision 1.17  2005/09/02 22:20:26  swift
 * Added status bit-mask and logentries for shallow-water trap.
 *
 * Revision 1.16  2005/07/05 21:44:12  swift
 * Refactored air-bladder inflation algorithm into its own function.
 *
 * Revision 1.15  2005/06/28 19:23:56  swift
 * Fixed some minor formatting errors in logentries.
 *
 * Revision 1.14  2005/06/27 15:13:16  swift
 * Minor modifications to the air-bladder inflation algorithm.  Added
 * logentries of number of connections and connection attempts.  Added firmware
 * revisions to the logfile and profile data file.
 *
 * Revision 1.13  2005/06/23 20:30:53  swift
 * Implemented new air-bladder inflation algorithm to be more tolerant of wave fields.
 *
 * Revision 1.12  2005/06/22 19:28:20  swift
 * Rework the air-bladder inflation algorithm to introduce some hysterisis.
 *
 * Revision 1.11  2005/06/17 16:31:50  swift
 * Rework handling of writes by GpsServices() and WriteVitals() to the profile file.
 *
 * Revision 1.10  2005/06/14 18:58:28  swift
 * Change name of RegisterIrModem() to IrModemRegister().
 *
 * Revision 1.9  2005/02/22 21:42:12  swift
 * Eliminated GPS stub and incorporated actual GPS functionality.  Used
 * enhanced LBT9522 functionality for surface detection after the profile phase
 * terminates.
 *
 * Revision 1.8  2005/01/06 00:58:12  swift
 * Refactored some functionality to other source code modules.
 *
 * Revision 1.7  2004/12/29 23:11:27  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.6  2004/04/15 23:20:19  swift
 * Eliminated the piston full-extension count from argos messages with 28-bit ids.
 *
 * Revision 1.5  2004/04/14 20:42:33  swift
 * Added provisions to properly handle either 2-bit or 28-bit argos ids.
 *
 * Revision 1.4  2004/04/14 16:38:43  swift
 * Eliminated argos telemetry of the piston full-extension count if 28-bit
 * argos id is used.  This keeps the test message set to a single message.
 *
 * Revision 1.3  2003/11/20 18:59:47  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.2  2003/11/12 23:05:11  swift
 * Added descent marks to ARGOS message.
 *
 * Revision 1.1  2003/09/11 21:57:42  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define telemetryChangeLog "$RCSfile: telemetr.c,v $ $Revision: 1.25.2.1 $ $Date: 2008/09/11 19:58:15 $"

#include <stdio.h>
#include <serial.h>

/* prototypes for functions with external linkage */
int AirSystem(void);
int Telemetry(void);
int TelemetryInit(void);
int TelemetryTerminate(void);

#endif /* TELEMETR_H */

#include <apf9com.h>
#include <stdio.h>
#include <conio.h>
#include <apf9.h>
#include <apf9icom.h>
#include <assert.h>
#include <control.h>
#include <ctdio.h>
#include <descent.h>
#include <download.h>
#include <ds2404.h>
#include <eeprom.h>
#include <flashio.h>
#include <gps.h>
#include <limits.h>
#include <logger.h>
#include <login.h>
#include <lt1598ad.h>
#include <lbt9522.h>
#include <engine.h>
#include <nan.h>
#include <profile.h>
#include <seascan.h>
#include <string.h>
#include <upload.h>
#include <unistd.h>

#include "quecom.h" //CJ

/* prototypes for functions with static linkage */
static int WriteVitals(FILE *fp,const char *path);

/*------------------------------------------------------------------------*/
/* air-bladder inflation algorithm                                        */
/*------------------------------------------------------------------------*/
/**
   This function inflates the air bladder using fault-tolerant monitoring to
   avoid false-positives for bladder inflation.  The bladder pressure is
   checked N times to determine if it is below the threshold.  The first
   below-threshold measurement will induce the air pump to inflate the
   bladder.  The air pump is run in 1 second bursts followed by a 1 second
   pause before the bladder pressure is sampled.  The air pump will continue
   to run until N consecutive samples exceed the threshold pressure.  This
   function returns the number of pulses of the air pump.
*/
int AirSystem(void)
{
   /* function name for log entries */
   static cc FuncName[] = "AirSystem()";

   /* initialize the return value */
   int i, pulses=0; const int N=3;
	
   //CJ QUEPHONE MESSAGE - Air Bag Motor OFF
   //QuecomSetABS(1);

   /* loop to implement multiple checks of air bladder pressure */
   for (pulses=0,i=1; i<=N; i++) 
   {
      /* check if the current air bladder pressure is less than threshold */
      if ((vitals.AirBladderPressure=BarometerAd8())<=mission.MaxAirBladder)
      {
         /* exit loop to inflate bladder */
         pulses=1; break;
      }

      /* pause to sample a different part of the surface wave field */
      else if (i<N) sleep(3);
   }
   
   /* check to see if the air bladder should be inflated */
   if (pulses>0)
   {
      /* define variables to monitor air-system power consumption */
      unsigned char VCnt=0,ACnt=0;

      /* initialize termination parameters for the air-inflation algorithm */
      const time_t To=time(NULL), timeout=300, dT=1; time_t T=0; 

      /* loop until the air bladder exceeds the cut-off for N consecutive samples */
      for (pulses=0,i=0; i<N;)
      {
         /* close the air valve */
         if (!(pulses%10)) AirValveClose();

         /* pulse the air pump */
         AirPumpRun(dT,&VCnt,&ACnt); sleep(dT); T+=dT; pulses++;

         /* check pneumatic inflation limits */
         if (difftime(time(NULL),To)>=timeout) 
         {
            static cc format[]="Pneumatic inflation limits imposed. [pulses: %d, AirPumpSec: %ld]\n";
            LogEntry(FuncName,format,pulses,T); vitals.status |= AirSysLimit; break;
         }

         /* check if the air bladder pressure exceeds the cut-off */
         if ((vitals.AirBladderPressure=BarometerAd8())>mission.MaxAirBladder) i++;
         else i=0;
      }
      
      /* test whether to make a log entry */
      if (debuglevel>=2 || (debugbits&TELEMETR_H))
      {
         /* create the message */
         static cc format[]="BatV[%dcnt,%0.1fV]Amp[%dcnt,%0.1fmA]"
            "BrmtrP[%dcnt,%0.1f\"Hg]Run-Tm[%lds]\n";

         /* make a log entry of the power consumption of the air system */
         LogEntry(FuncName,format,VCnt,Volts(VCnt),ACnt,Amps(ACnt)*100,
                  vitals.AirBladderPressure,inHg(vitals.AirBladderPressure),T);
      }
   }
   else if (debuglevel>=2 || (debugbits&TELEMETR_H))
   {
      static cc format[]="Air-bladder inflation by-passed. Barmtr: [%dcnt, %0.1f\"Hg].\n";
      LogEntry(FuncName,format,vitals.AirBladderPressure,inHg(vitals.AirBladderPressure));
   }
	
   //Reset GoProfile=0 HM 10/18/2010
   ResetGoProfile();
	
   return pulses;
}

/*------------------------------------------------------------------------*/
/* execute tasks during the telemetry phase of the mission cycle          */
/*------------------------------------------------------------------------*/
/**
   This function executes regularly scheduled tasks during the telemetry phase
   of the mission cycle.  
*/
int Telemetry(void)
{
   /* function name for log entries */
   static cc FuncName[] = "Telemetry()";

   /* initialize the return value */
   int status=0;

   /* execute the air-bladder inflation algorithm */
   AirSystem();
   
   /* seek iridium satellite signals */
   if (IrSkySearch(&modem)>0)
   {
      FILE *dest=NULL;
      
      /* check if the profile file has already been uploaded */
      if (prf_path[0])
      {
         /* validate the file name and open the output file */
         if (fnameok(prf_path)<0 || !(dest=fopen(prf_path,"a")))
         {
            static cc format[]="Unable to open \"%s\" in append mode.\n";
            LogEntry(FuncName,format,prf_path);
         }
      }
             
      /* execute gps services */
      GpsServices(dest);
      
      /* activate the modem serial port */
      ModemEnable(19200);

      /* register the modem with the iridium system */
      if (IrModemRegister(&modem)>0)
      {
         /* format for logentry */
         static cc format[]="Tlmtry cycle done:PrfId=%d "
            "ConnAttmpts=%u Conn_St=%u\n";
         
         /* download the mission configuration from the remote host */
         //if (DownLoadMissionCfg(&modem,2)>0) vitals.status &= ~DownLoadCfg; LogWil 7/21/09

         /* write vital statistics and close the profile */
         if (dest) {WriteVitals(dest,prf_path); fclose(dest); dest=NULL;}

         /* upload logs and profiles to the remote host */
         status=UpLoad(&modem,3);  //second number = max number of reconnects

         /* logout of the remote host */
         //logout(&modem); //LogWil Should be hungup(modem) HM

         /* write the number connections and connection attempts */
         LogEntry(FuncName,format,PrfIdGet(),vitals.ConnectionAttempts,vitals.Connections); 
      }

      else
      {
         static cc msg[]="Iridium modem registration failed.\n";
         LogEntry(FuncName,msg); 
      }
      
      /* deactivate the modem serial port */
      ModemDisable();

      /* make sure the profile file is closed */
      if (dest) fclose(dest);
   }

   /* give the buoyancy pump another nudge */
   else
   {
      static cc msg[]="SkySearch failed - adding buoyancy.\n";
      LogEntry(FuncName,msg);
      
      PistonMoveRel(mission.PistonInitialBuoyancyNudge);
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to initialize the telemetry phase of the profile cycle        */
/*------------------------------------------------------------------------*/
/**
   This function initializes the telemetry phase of the mission cycle.     
*/
int TelemetryInit(void)
{
   /* function name for log entries */
   static cc FuncName[] = "TelemetryInit()";

   /* define some local objects */
   unsigned char VCnt,ACnt;

   /* initialize the function's return value */
   int status=1;

   /* set the state variable */
   StateSet(TELEMETRY);

   /* create the new logfile name */
   snprintf(log_path,sizeof(log_path)-1,"%04u.%03d.log",mission.FloatId,PrfIdGet()+1);

   /* close all open files and open the new logfile */
   LogClose(); fcloseall(); LogOpen(log_path,'w');
   
   /* make a log entry */
   if (debuglevel>=2 || (debugbits&TELEMETR_H))
   {
      /* create the message */
      static cc format[]="Profile %d. (Apf9i FwRev: %06lx)\n";

      /* make the logentry */
      LogEntry(FuncName,format,PrfIdGet(),FwRev);
   }

   /* close the air valve and run the air pump for 1 second */
   AirValveOpen(); AirPumpRun(1,&VCnt,&ACnt); AirValveClose(); 

   /* save the air pump current and volts */
   vitals.AirPumpVolts=VCnt; vitals.AirPumpAmps=ACnt;
   
   /* measure the air bladder pressure */
   vitals.AirBladderPressure=BarometerAd8();

   /* initialize the flag to download the mission configuration */
   vitals.status |= DownLoadCfg;
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to terminate the telemetry phase                              */
/*------------------------------------------------------------------------*/
int TelemetryTerminate(void)
{
   /* function name for log entries */
   static cc FuncName[] = "TelemetryTerminate()";

   int i,n,status=1;
   
   /* pet the watch dog */
   WatchDog(); 

   /* check if a mission configuration file was successfully downloaded */
   if (!(vitals.status&DownLoadCfg))
   {
      if (debuglevel>=2 || (debugbits&TELEMETR_H))
      {
         static cc msg[]="Parsing new mission config.\n";
         LogEntry(FuncName,msg);
      }
      
      /* process the configuration file */
      configure(&mission,config_path); 
   }
   else {static cc msg[]="By-passing mission config.\n"; LogEntry(FuncName,msg);}

   if (debuglevel>=2 || (debugbits&TELEMETR_H))
   {
      static cc msg[]="Reconditioning the file system.\n";
      LogEntry(FuncName,msg);
   }
         
   /* close all open files and reopen the logfile */
   LogClose(); fcloseall(); LogOpen(log_path,'a');

   /* check if the file system is full */
   if (fidnext()<0)
   {
      /* loop through each fid of the file system */
      for (i=0,n=0; i<MAXFILES && n<5; i++)
      {
         /* create a local buffer to hold file system entries */
         char fname[FILENAME_MAX+1];

         /* pet the watch dog */
         WatchDog(); 

         /* check if the current fid contains a file */
         if (fioName(i,fname,FILENAME_MAX)>0)
         {
            /* don't delete the mission configuration file */
            if (!strcmp(fname,config_path)) continue;
         
            /* don't delete the active log file */
            if (!strcmp(fname,log_path)) continue;

            /* delete the current file */
            remove(fname); n++;
         }
      }
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to write engineering data to the profile file                 */
/*------------------------------------------------------------------------*/
/**
   This function writes the engineering data to a stream.
*/
static int WriteVitals(FILE *fp,const char *path)
{
   /* function name for log entries */
   static cc FuncName[] = "WriteVitals()";

   int i,status=1;

   if (!fp)
   {
      static cc msg[]="NULL stream pointer.\n";
      LogEntry(FuncName,msg);
      status=-1;
   }

   else
   {
      if ((debuglevel>=2 || (debugbits&TELEMETR_H)) && path)
      {
         static cc format[]="Writing vitals to \"%s\".\n";
         LogEntry(FuncName,format,path);
      }

      fprintf(fp,"Apf9iFwRev=%06lx\n",FwRev);
      fprintf(fp,"ActiveBallastAdjustments=%u\n",vitals.ActiveBallastAdjustments);
      fprintf(fp,"AirBladderPressure=%u\n",vitals.AirBladderPressure);
      fprintf(fp,"AirPumpAmps=%u\n",vitals.AirPumpAmps);
      fprintf(fp,"AirPumpVolts=%u\n",vitals.AirPumpVolts);
      fprintf(fp,"BuoyancyPumpOnTime=%ld\n",vitals.BuoyancyPumpOnTime);
      fprintf(fp,"BuoyancyPumpAmps=%u\n",vitals.BuoyancyPumpAmps);
      fprintf(fp,"BuoyancyPumpVolts=%u\n",vitals.BuoyancyPumpVolts);
      fprintf(fp,"CurrentPistonPosition=%u\n",PistonPosition());
      fprintf(fp,"DeepProfilePistonPosition=%u\n",mission.PistonDeepProfilePosition);
      fprintf(fp,"GpsFixTime=%ld\n",vitals.GpsFixTime);
      fprintf(fp,"FlashErrorsCorrectable=%u\n",Tc58v64ErrorsCorrectable());
      fprintf(fp,"FlashErrorsUncorrectable=%u\n",Tc58v64ErrorsNoncorrectable());
      fprintf(fp,"FloatId=%04u\n",mission.FloatId);
      fprintf(fp,"ParkDescentPCnt=%u\n",vitals.ParkDescentPCnt);

      for (i=0; i<vitals.ParkDescentPCnt; i++)
      {
         fprintf(fp,"ParkDescentP[%d]=%u\n",i,vitals.ParkDescentP[i]);
      }

      fprintf(fp,"ParkPistonPosition=%u\n",mission.PistonParkPosition);
      fprintf(fp,"ParkObs={ %5.1fdbar, %8.3fC }\n",
              vitals.ParkObs.p,vitals.ParkObs.t);

      fprintf(fp,"ProfileId=%03d\n",PrfIdGet());
      fprintf(fp,"ObsIndex=%u\n",vitals.ObsIndex);
      fprintf(fp,"QuiescentAmps=%u\n",vitals.QuiescentAmps);
      fprintf(fp,"QuiescentVolts=%u\n",vitals.QuiescentVolts);
      fprintf(fp,"RtcSkew=%ld\n",vitals.RtcSkew);
      fprintf(fp,"SeascanAmps=%u\n",vitals.SeascanAmps);
      fprintf(fp,"SeascanVolts=%u\n",vitals.SeascanVolts);
      fprintf(fp,"SeascanStatus=0x%08x\n",vitals.SeascanStatus);
      fprintf(fp,"status=0x%08x\n",vitals.status);
      fprintf(fp,"SurfacePistonPosition=%u\n",vitals.SurfacePistonPosition);
      fprintf(fp,"SurfacePressure=%0.2f\n",vitals.SurfacePressure);
      fprintf(fp,"Vacuum=%u\n",vitals.Vacuum);
   }
   
   return status;
}
