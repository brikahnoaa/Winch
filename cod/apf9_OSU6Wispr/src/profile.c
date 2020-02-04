#ifndef PROFILE_H
#define PROFILE_H (0x0080U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: profile.c,v 1.26.2.2 2009/01/19 16:43:32 dbliudnikas Exp $
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
 * $Log: profile.c,v $
 * Revision 1.26.2.2  2009/01/19 16:43:32  dbliudnikas
 * Remove 5 minute energy saving wait in profile not applicable to Seascan (leftover from Sbe41CP mods).
 *
 * Revision 1.26.2.1  2008/09/11 20:09:24  dbliudnikas
 * Replace SBE41 with Seascan TD: status, no getobs, getp becomes getpt to ensure new data, no CP mode, no salinity.
 *
 * Revision 1.26  2008/07/14 16:57:34  swift
 * Implemented 32-bit status words.
 *
 * Revision 1.25  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.24  2006/12/06 15:54:48  swift
 * Added a LogEntry() if pressure acquisition failed for any reason.
 *
 * Revision 1.23  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.22  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.21  2005/10/03 16:11:25  swift
 * Change extension of data from 'prf' to 'msg'.
 *
 * Revision 1.20  2005/09/02 22:20:26  swift
 * Added status bit-mask and logentries for shallow-water trap.
 *
 * Revision 1.19  2005/08/14 23:42:09  swift
 * Modified the surface-detection algorithm to confirm a tentative surface
 * detection with a second pressure sample in order to ensure that the profile
 * will not be prematurely terminated.
 *
 * Revision 1.18  2005/08/11 20:15:34  swift
 * Added sanity checks on pressure to detect unreliable pressure data.
 *
 * Revision 1.17  2005/08/06 17:20:46  swift
 * Add profile termination time to output file.
 *
 * Revision 1.16  2005/07/05 21:44:56  swift
 * Minor modifications to format of data file.
 *
 * Revision 1.15  2005/06/20 18:54:35  swift
 * Eliminated bogus sanity check on AscentControl.SurfacePressure variable.
 * Added a sanity check on the valid pressure range reported by the pressure sensor.
 *
 * Revision 1.14  2005/06/17 16:32:25  swift
 * Fixed a typo of Sbe41cpPUnreliable flag.
 *
 * Revision 1.13  2005/06/14 18:59:19  swift
 * Add range check to reliability check of pressure data.
 *
 * Revision 1.12  2005/05/01 14:26:24  swift
 * Reduced the post surface-detection wait period from 15 minutes to 5 minutes.
 *
 * Revision 1.11  2005/02/22 21:07:50  swift
 * Moved code to open the profile file to 'descent.c'.
 *
 * Revision 1.10  2005/01/06 00:57:05  swift
 * Write the profile to the file system.
 *
 * Revision 1.9  2004/12/29 23:11:27  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.8  2004/06/07 21:14:39  swift
 * Fixed a bug that caused an array under-indexing error if the float was at
 * the surface at the end of the drift phase and the SBE41 reported a negative
 * pressure.
 *
 * Revision 1.7  2004/04/26 16:02:18  swift
 * Added an ascent control parameter called SurfacePressure to record the
 * surface reference.  The surface detection algorithm uses this reference.
 * The parameter 'SurfacePressure' in the engineering data records the pressure
 * measurement at the initiation of the descent phase.
 *
 * Revision 1.6  2004/04/14 16:39:31  swift
 * Included an external declaration of the pressure table.
 *
 * Revision 1.5  2004/03/26 16:34:53  swift
 * Changed the criteria for computing the near-surface time-out period of the
 * buoyancy pump to include the surface-pressure offset.
 *
 * Revision 1.4  2004/03/24 00:41:45  swift
 * Eliminated SurfaceDetect() from being executed twice.
 *
 * Revision 1.3  2003/11/20 18:59:47  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.2  2003/10/28 23:29:37  swift
 * Fixed a bug that caused the 'SbePUnreliable' bit to be incorrectly set.
 *
 * Revision 1.1  2003/09/11 21:54:37  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define profileChangeLog "$RCSfile: profile.c,v $ $Revision: 1.26.2.2 $ $Date: 2009/01/19 16:43:32 $"

#include <time.h>

/* definition of structure to contain an observation */
struct Obs {float p, t, s;};

/* function prototypes */
int    GetP(float *p);
time_t Profile(void);
int    ProfileInit(void);
int    ProfileTerminate(void);

/* define the number of elements in the pressure table */
extern const int pTableSize;
extern const float pTable[];

extern persistent far struct Obs obs[];
extern persistent far int pTableIndex;
extern persistent char prf_path[32];

#endif /* PROFILE_H */

#include <eeprom.h>
#include <logger.h>
#include <apf9.h>
#include <lt1598ad.h>
#include <engine.h>
#include <seascan.h>
#include <math.h>
#include <nan.h>
#include <telemetr.h>
#include <ds2404.h>
#include <limits.h>
#include <park.h>
#include <apf9com.h>
#include <apf9.h>
#include <control.h>

#include "quecom.h" //CJ
//extern long max_detect_per_read;  //HM 12262010

/* structure to preserve ascent-control parameters */
persistent static struct
{
      int    PistonPosition;
      int    InitialExtension;
      float  RefPressure;
      float  SurfacePressure;
      time_t RefTime;
      time_t TimeStamp;
      time_t TimeStampObs;
} AscentControl;

/* define where PTS samples should be taken */
const float pTable[] =
{
   2000.0, 1950.0, 1900.0, 1850.0, 1800.0, 1750.0, 1700.0, 1650.0, 1600.0, 1550.0,
   1500.0, 1450.0, 1400.0, 1350.0, 1300.0, 1250.0, 1200.0, 1150.0, 1100.0, 1050.0,
   1000.0,  950.0,  900.0,  850.0,  800.0,  750.0,  700.0,  650.0,  600.0,  550.0,
    500.0,  450.0,  400.0,  380.0,  360.0,  350.0,  340.0,  330.0,  320.0,  310.0,
    300.0,  290.0,  280.0,  270.0,  260.0,  250.0,  240.0,  230.0,  220.0,  210.0,
    200.0,  190.0,  180.0,  170.0,  160.0,  150.0,  140.0,  130.0,  120.0,  110.0,
    100.0,   90.0,   80.0,   70.0,   60.0,   50.0,   40.0,   30.0,   20.0,   10.0,
      6.0,    0.0
};
const int pTableSize = sizeof(pTable)/sizeof(float);

persistent far struct Obs obs[sizeof(pTable)/sizeof(float)];
persistent far int pTableIndex;
persistent char prf_path[32];

/* function prototypes for statically linked functions */
static int AscentControlAgent(float p);
static int SurfaceDetect(float p);

/*------------------------------------------------------------------------*/
/* agent to control the ascent rate of the float                          */
/*------------------------------------------------------------------------*/
/**
   This function controls the ascent-rate of the float by monitoring time
   and pressure and computing criteria for activating the buoyancy control
   engine.
*/
static int AscentControlAgent(float p)
{
   /* define the logging signature */
   static cc FuncName[] = "AscentControlAgent()";

   int status=1;

   /* define the maximum time of the piston nudge */
   time_t TimeOut = (p<(AscentControl.SurfacePressure+20)) ? 15 : 30;

   /* get the current mission time */
   const time_t t = itimer();

   //CJ QUEPHONE MESSAGE - Ascending
   //QuecomSetASC(p); //We do not use anymore 11/15/2010 HM

   if ((t-AscentControl.TimeStamp)>=5*Min)
   {
      float v;
            
      /* define the minimum allowed vertical ascent rate */
      const float Vmin=0.08;

      /* define the maximum believable vertical ascent rate */
      const float Vmax=0.30;

      /* note the time to limit execution to once every 5 minutes */
      AscentControl.TimeStamp=t;

      /* get and validate the current pressure */
      if (GetP(&p)>0)
      {
         /* compute the vertical ascent rate */
         v = -(p-AscentControl.RefPressure)/(t-AscentControl.RefTime);

         /* check for unreasonably high ascent rates */
         if (fabs(v)>Vmax) vitals.status|=SeascanPUnreliable;
      }
      else
      {
         /* create the message */
         static cc msg[]="GetP() failed.\n";

         /* make the logentry */
         LogEntry(FuncName,msg);
      }

      /* check timeout criteria for extending the piston */
      if (difftime(itimer(),AscentControl.TimeStampObs)>25*Min)
      {
         vitals.status|=Obs25Min;

         AscentControl.PistonPosition += mission.PistonBuoyancyNudge;

         if (debuglevel>=2 || (debugbits&PROFILE_H))
         {
            /* create the message */
            static cc format[]="Bouyancy nudge to %d (Obs25Min).\n";

            /* make the logentry */
            LogEntry(FuncName,format,AscentControl.PistonPosition);
         }
      }

      /* check Seascan unreliability criteria for extending the piston */
      else if (vitals.status&SeascanPUnreliable) 
      {
         AscentControl.PistonPosition += mission.PistonBuoyancyNudge;

         if (debuglevel>=2 || (debugbits&PROFILE_H))
         {
            /* create the message */
            static cc format[]="Bouyancy nudge to %d (SeascanPUnreliable).\n";

            /* make the logentry */
            LogEntry(FuncName,format,AscentControl.PistonPosition);
         }
      }

      /* check ascent-rate criteria for extending the piston */
      else if (v<=Vmin) 
      {
         AscentControl.PistonPosition += mission.PistonBuoyancyNudge;

         if (debuglevel>=2 || (debugbits&PROFILE_H))
         {
            /* create the message */
            static cc format[]="Bouyancy nudge to %d (v=%0.3fdB/sec).\n";
            
            /* make the logentry */
            LogEntry(FuncName,format,AscentControl.PistonPosition,v);
         }
      }

      /* save new references for pressure and time */
      if (!(vitals.status&SeascanPUnreliable))
      {
         AscentControl.RefTime=t;
         AscentControl.RefPressure=p;
      }
   }

   /* make sure that the new target does not exceed full extension */
   if (AscentControl.PistonPosition>mission.PistonFullExtension)
   {
      AscentControl.PistonPosition=mission.PistonFullExtension;
   }
      
   /* check if piston is at its target position */
   if (PistonPosition()!=AscentControl.PistonPosition)
   {
      unsigned char V=0xff,A=0xff;
      
      /* nudge the piston toward the target position */
      PistonMoveAbsWTO(AscentControl.PistonPosition,&V,&A,TimeOut);

      /* record power consumption during the initial extension period */
      if (PistonPosition()<=AscentControl.InitialExtension && V!=0xff && A!=0xff) 
      {
         vitals.BuoyancyPumpVolts=V; vitals.BuoyancyPumpAmps=A;
      }
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to query the Seascan PT module for a pressure measurment              */
/*------------------------------------------------------------------------*/
/**
   This function queries the Seascan PT module for a pressure measurement.  The mission
   status flags are also maintained.
*/
int GetP(float *p)
{
   /* define the logging signature */
   static cc FuncName[] = "GetP()";
   int status = -1;
   float plocal, tlocal;
   
   /* validate the function parameter */
   if (p)
   {
      /* query the Seascan for a pressure measurment */
      if ((status=SeascanGetPt(&plocal,&tlocal))!=SeascanOk)
      {
         static cc msg[]="SeascanPUnreliable: SeascanGetP()=%d\n";
         LogEntry(FuncName,msg,status);
         vitals.status |= SeascanPUnreliable;
      }
      *p = plocal;
      
      /* check for failure of the pressure measurement */
      if (status<SeascanOk) {vitals.status |= SeascanPFail;}

      /* make range checks on pressure */
      if (!((*p)>=-10 && (*p)<=2200))
      {
          /* create the message */
         static cc format[]="Pressure (%0.1fdB) out of range: [-10,2200].\n";
            
         /* make the logentry */
         LogEntry(FuncName,format,(*p));

         /* mark the pressure as unreliable */
         vitals.status |= SeascanPUnreliable;
      }
         
      /* maintan the mission status flags */
      if      (status==SeascanPedanticExceptn) {vitals.SeascanStatus |= 0x00000001UL;}
      else if (status==SeascanPedanticFail)    {vitals.SeascanStatus |= 0x00000002UL;}
      else if (status==SeascanRegexFail)       {vitals.SeascanStatus |= 0x00000004UL;}
      else if (status==SeascanNullArg)         {vitals.SeascanStatus |= 0x00000008UL;}
      else if (status==SeascanRegExceptn)      {vitals.SeascanStatus |= 0x00000010UL;}
      else if (status==SeascanNoResponse)      {vitals.SeascanStatus |= 0x00000020UL;}
   }

   return status;
}


/*------------------------------------------------------------------------*/
/* function to apply surface detection criteria                           */
/*------------------------------------------------------------------------*/
/**
   This function applies surface detection criteria in order to detect the
   even-oriented sequence point between the profile and telemetry phases of
   the mission cycle.

      \begin{verbatim}
      input:
         p....The current pressure (decibars) of the float.

      output:
         This function returns a positive value if the surface detection
         criteria have been satisfied; otherwise zero is returned.
      \end{verbatim}
*/
static int SurfaceDetect(float p)
{
   /* define the logging signature */
   static cc FuncName[] = "SurfaceDetect()";

   int err,status=0;
   
   /* check nonpedantic surface detection criteria */
   if (p<=4 || p<=(AscentControl.SurfacePressure+4))
   {
      /* a pause is necessary to ensure that the Seascan will wake-up */
      Wait(1000);
      
      /* check pedantic surface detection criteria */
      if ((err=GetP(&p))==SeascanOk && (p<=4 || p<=(AscentControl.SurfacePressure+4)))
      {
         /* record the piston position when surface detected */
         vitals.SurfacePistonPosition=PistonPosition();
      
         /* log the surface detection */
         if (debuglevel>=2 || (debugbits&PROFILE_H))
         {
            /* create the message */
            static cc format[]="SurfacePress:%0.1fdB Press:%0.1fdB PistonPos:%d\n";

            /* make the logentry */
            LogEntry(FuncName,format,AscentControl.SurfacePressure,p,vitals.SurfacePistonPosition);
         }
      
         /* if the pressure is unreliable or greater than 5dbar then move to full extension */
         if (p>5 || vitals.status&SeascanPUnreliable)
         {
            AscentControl.PistonPosition=mission.PistonFullExtension;
         }
      
         /* otherwise, add some buoyancy */
         else AscentControl.PistonPosition = vitals.SurfacePistonPosition +
                 mission.PistonInitialBuoyancyNudge;

         /* move the piston to the computed position */
         PistonMoveAbs(AscentControl.PistonPosition);
      
         /* indicate that the surface was detected */
         status=1;
      }

      /* check for notification of surface-detection failure */
      else if (debuglevel>=2 || (debugbits&PROFILE_H))
      {
         /* create the message */
         static cc format[]="Pedantic surface detection "
            "criteria failed: [err:%d] [p:%0.2f].\n";
               
         /* make the logentry */
         LogEntry(FuncName,format,err,p);
       }
   }
      
   /* check if the piston is fully extended before the surface is detected */
   else if (PistonPosition()>=mission.PistonFullExtension)
   {
      if ((debuglevel>=2 || (debugbits&PROFILE_H)) && !(vitals.status&PistonFullExt))
      {
         /* create the message */
         static cc msg[]="Piston fully extended before surface detected.\n";

         /* make the logentry */
         LogEntry(FuncName,msg);
      }
      
      /* assert a status bit that indicates the piston is at full extension */
      vitals.status|=PistonFullExt;
   }
	
	//if (status == 1) {
		//CJ QUEPHONE MESSAGE - Surface Detect
		//QuecomSetSDT();
	//}
		
   return status;
}

/*------------------------------------------------------------------------*/
/* execute tasks during the descent phase of the mission cycle            */
/*------------------------------------------------------------------------*/
/**
   This function executes regularly scheduled tasks during the profile phase
   of the mission cycle.  It returns the recommended sleep period.
*/
time_t Profile(void)
{
   /* define the logging signature */
   static cc FuncName[] = "Profile()";
   
   int err,sleep=HeartBeat;
   float p;
   //int	GoProfile; //HM
   
   if (GetP(&p)>0)
   {
      //CJ QUEPHONE MESSAGE - Profiling
      //QuecomSetPFL(p); //CJ does not use this 11/15/2010 HM
	
      /* execute surface detection criteria */
      if (SurfaceDetect(p)>0)
      {
         /* terminate the profile phase and initialize telemetry phase */
         ProfileTerminate(); TelemetryInit(); return 5*Min;
      }
          
      /* check sampling criteria */
      if (p<=pTable[pTableIndex]+1)      
      {
         float P,T;
         struct Obs o; o.p=nan(); o.t=nan(); o.s=nan(); /* salinity not applicable to Seascan */
         
         /* collect a full sample */
         /* Seascan is PT only */
         if ((err=GetPt(&P, &T))>0)
         {
            o.p = P;
            o.t = T;
            if (debuglevel>=2 || (debugbits&PROFILE_H))
            {
               /* create the message */
               static cc format[]="Sample %d at " 
                  "%0.1fdB for bin %d [%0.0fdB]. "
                  "PT:%0.1fdB %0.4fC\n";
 
               /* make the logentry */
               LogEntry(FuncName,format,vitals.ObsIndex,p,pTableIndex,
                        pTable[pTableIndex],o.p,o.t);
            }
 		    //Check if new DSP data are avail 
		    //if(DspPwrStatus) QuecomGetDTX(max_detect_per_read); //HM  
                                   
            /* assign the observation to the storage array; increment indexes */ 
            if (vitals.ObsIndex<pTableSize) {obs[vitals.ObsIndex]=o; ++vitals.ObsIndex;}

            /* reposition the table pointer */
            if (pTableIndex<pTableSize) ++pTableIndex;

            /* record a timestamp for this observation */
            AscentControl.TimeStampObs=itimer();
         }
         else
         {
            /* create the message */
            static cc format[]="GetObs()=%d.\n";
 
            /* make the logentry */
            LogEntry(FuncName,format,err);
         }
      }
   }
   else
   {
      /* create the message */
      static cc msg[]="GetP() failed.\n";
 
      /* make the logentry */
      LogEntry(FuncName,msg);
   }
   
   /* execute the ascent-rate control agent */
   AscentControlAgent(p);

   return sleep;
}

/*------------------------------------------------------------------------*/
/* function to initialize the profile phase of the profile cycle          */
/*------------------------------------------------------------------------*/
/**
   This function initializes the profile phase of the mission cycle.     
*/
int ProfileInit(void)
{
   /* define the logging signature */
   static cc FuncName[] = "ProfileInit()";

   int status=1;
   unsigned int i;
   float p;
   far unsigned char *b;
   const float MaxSurfPres=20;

   /* create an initialization object */
   struct Obs o; o.s=nan(); o.p=nan(); o.t=nan();
   
   /* set the state variable */
   StateSet(PROFILE);
   
   /* initialize the observation array */
   for (vitals.ObsIndex=0, i=0; i<pTableSize; i++) {obs[i]=o;}

   /* get the current pressure */
   if (GetP(&p)<=0) p=mission.PressureProfile;

   /* locate the pressure in the sample-bin table */
   for (pTableIndex=0; pTableIndex<(pTableSize-1) && pTable[pTableIndex]>p; pTableIndex++) {}

   /* make a log entry */
   if (debuglevel>=2 || (debugbits&PROFILE_H))
   {
      /* create the message */
      static cc format[]="PrfId:%03d Press:%0.1fdbar pTable[%d]:%0.0fdbar\n";

      /* make the logentry */
      LogEntry(FuncName,format,PrfIdGet(),p,pTableIndex,pTable[pTableIndex]);
   }
   
   /* initialize the ascent control structure */
   for (b=(far unsigned char *)(&AscentControl),i=0; i<sizeof(AscentControl); i++) b[i]=0;

   /* validate the surface pressure to use for ascent control and surface detect */
   if (vitals.SurfacePressure>MaxSurfPres) AscentControl.SurfacePressure = MaxSurfPres;
   else if (vitals.SurfacePressure<(-MaxSurfPres)) AscentControl.SurfacePressure = -MaxSurfPres;
   else AscentControl.SurfacePressure=vitals.SurfacePressure;

   /* initialize the reference pressure and the time stamps */
   AscentControl.RefPressure=p;
   AscentControl.RefTime=itimer();
   AscentControl.TimeStamp=AscentControl.RefTime+10*Min;
   AscentControl.TimeStampObs=AscentControl.RefTime;
   
   /* guard against shallow-water traps */
   if (p<=MaxSurfPres+4)
   {
      unsigned char V=0xff,A=0xff;

      if (debuglevel>=2 || (debugbits&PROFILE_H))
      {
         /* create the message */
         static cc format[]="Shallow water trap detected at %0.2fdB.\n";

         /* make the logentry */
         LogEntry(FuncName,format,p);
      }

      /* set the shallow-water-trap status bit */
      vitals.status |= ShallowWaterTrap;

      /* set the piston position to full extension */
      AscentControl.PistonPosition=mission.PistonFullExtension;
      
      /* nudge the piston toward the target position */
      PistonMoveAbsWTO(AscentControl.PistonPosition,&V,&A,LONG_MAX);

      /* record power consumption during the initial extension period */
      if (V!=0xff && A!=0xff) 
      {
         vitals.BuoyancyPumpVolts=V; vitals.BuoyancyPumpAmps=A;
      }
   }
   
   /* compute the target piston position for the initial piston extension */
   else AscentControl.PistonPosition = PistonPosition() + mission.PistonInitialBuoyancyNudge;

   /* make sure that the target piston position does not exceed full extension */
   if (AscentControl.PistonPosition>mission.PistonFullExtension)
   {
      AscentControl.PistonPosition=mission.PistonFullExtension;
   }

   /* record the initial piston extension for use with power measurements */
   AscentControl.InitialExtension=AscentControl.PistonPosition;

    /* initialize the CP-mode indicator */
    /* Not applicable to Seascan PT module */
   vitals.SeascanStatus &= ~SeascanCpActive;

   return status;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
int ProfileTerminate(void)
{
   static cc FuncName[]="ProfileTerminate()";
   
   /* initialize return value */
   int i,status=1; FILE *fp;

   /* validate the profile pathname */
   if (fnameok(prf_path)<=0) 
   {
      /* create the file to contain the profile data */
      snprintf(prf_path,15,"%04d.%03d.msg",mission.FloatId,PrfIdGet());
   }
   
   /* open the profile data file */
   if ((fp=fopen(prf_path,"a")))
   {
      /* write the date and time to the profile file */
      time_t t=time(NULL); fprintf(fp,"$ Profile %04u.%03d terminated: %s",
                                   mission.FloatId,PrfIdGet(),ctime(&t));

      /* write a header */
      fprintf(fp,"$ Discrete samples: %d\n",vitals.ObsIndex+1);

      /* write a column header */         
      fprintf(fp,"$ %7s %8s\n","p","t");

      /* write the park-level data */
      fprintf(fp,"  %7.2f %8.4f (Park Sample)\n",
              vitals.ParkObs.p,vitals.ParkObs.t);

      /* loop through each discrete observation */
      for (i=0; i<vitals.ObsIndex; i++)
      {
         /* write the observation to the data file */
         fprintf(fp,"  %7.2f %8.4f\n",obs[i].p,obs[i].t);
      }
      
      /* check if CP mode was activated during the profile */
      /* Removed - not applicable to Seascan */
      
      /* close the data file */
      fclose(fp);
   }

   /* log the failure */
   else
   {
      /* create the message */
      static cc format[]="Attempt to open \"%s\" failed [errno=%d].\n";

      /* make the logentry */
      LogEntry(FuncName,format,prf_path,errno);
   }
   
   return status;
}
