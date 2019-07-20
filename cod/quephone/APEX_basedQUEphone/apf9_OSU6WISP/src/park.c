#ifndef PARK_H
#define PARK_H (0x0020U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: park.c,v 1.20.2.1 2008/09/11 20:06:59 dbliudnikas Exp $
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
 * $Log: park.c,v $
 * Revision 1.20.2.1  2008/09/11 20:06:59  dbliudnikas
 * Replace SBE41 with Seascan TD: status, getobs becomes getpt (no salinity).
 *
 * Revision 1.20  2008/07/14 16:57:34  swift
 * Implemented 32-bit status words.
 *
 * Revision 1.19  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.18  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.17  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.16  2006/03/15 16:00:48  swift
 * Eliminate vestiges of ModemDefibrillator() calls by removing include directives.
 *
 * Revision 1.15  2006/02/28 18:43:22  swift
 * Eliminated the apf9icom.h header.
 *
 * Revision 1.14  2006/02/28 16:34:23  swift
 * Eliminated the modem defibrillation kludge because the fundamental problem
 * with nonresponsive LBT9522a modems was found and fixed.
 *
 * Revision 1.13  2006/01/31 19:44:43  swift
 * Added a missing include directive for apf9icom services.
 *
 * Revision 1.12  2006/01/31 19:25:05  swift
 * Added LBT defibrillation kludge.
 *
 * Revision 1.11  2005/09/02 22:21:31  swift
 * Reduced text to profile file.
 *
 * Revision 1.10  2005/08/17 21:56:22  swift
 * Changed logging verbosity of PT samples to reduce typical log file size.
 *
 * Revision 1.9  2005/08/06 17:20:03  swift
 * Modify the park PT report slightly.
 *
 * Revision 1.8  2005/06/27 15:14:18  swift
 * Added a logentry if the profile file couldn't be opened.
 *
 * Revision 1.7  2005/05/01 14:31:28  swift
 * Changed the Sbe41cpStatus masks for low-power TP sampling.
 *
 * Revision 1.6  2004/12/29 23:11:27  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.5  2004/05/03 18:06:34  swift
 * Added a 1 second pause in ParkTerminate() between CTD samples as required
 * for the CTD to wake up the second time.
 *
 * Revision 1.4  2004/04/26 15:55:18  swift
 * Changed location of an include directive to get it out of the header secion.
 *
 * Revision 1.3  2003/11/20 18:59:47  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.2  2003/11/12 23:02:10  swift
 * Modifications to logging.
 *
 * Revision 1.1  2003/09/11 21:34:13  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define ParkChangeLog "$RCSfile: park.c,v $ $Revision: 1.20.2.1 $ $Date: 2008/09/11 20:06:59 $"

/* function prototypes */
int GetPt(float *p, float *t);
int Park(void);
int ParkInit(void);
int ParkTerminate(void);

#endif /* PARK_H */

#include <control.h>
#include <logger.h>
#include <ds2404.h>
#include <engine.h>
#include <eeprom.h>
#include <lt1598ad.h>
#include <crc16bit.h>
#include <control.h>
#include <seascan.h>
#include <ctdio.h>
#include <nan.h>
#include <unistd.h>
#include <apf9com.h>
#include <apf9.h>

#include "quecom.h" //CJ

/*-------------------------------------------------------------------------------*/
/* function to query the Seascan Pressure Temperature module for a PT measurment */
/*-------------------------------------------------------------------------------*/
/**
   This function queries the Seascan PT module for a PT measurement.
   The mission status flags are also maintained.
*/
int GetPt(float *p, float *t)
{
   int status = -1;

   /* validate the function parameter */
   if (p && t)
   {
      /* query the Seascan for a PT measurment */
      if ((status=SeascanGetPt(p,t))<=0) {vitals.status |= SeascanPtFail;}
          
      /* maintan the mission status flags */
      if      (status==SeascanPedanticExceptn) {vitals.SeascanStatus |= 0x00000100UL;}
      else if (status==SeascanPedanticFail)    {vitals.SeascanStatus |= 0x00000200UL;}
      else if (status==SeascanRegexFail)       {vitals.SeascanStatus |= 0x00000400UL;}
      else if (status==SeascanNullArg)         {vitals.SeascanStatus |= 0x00000800UL;}
      else if (status==SeascanRegExceptn)      {vitals.SeascanStatus |= 0x00001000UL;}
      else if (status==SeascanNoResponse)      {vitals.SeascanStatus |= 0x00002000UL;}
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* execute tasks during the park phase of the mission cycle               */
/*------------------------------------------------------------------------*/
/**
   This function executes regularly scheduled tasks during the park phase
   of the mission cycle.  
*/
int Park(void)
{
   /* define the logging signature */
   static cc FuncName[] = "Park()";
   static long howoften;

   int errcode,status=1;
   float p;
   
   if ((errcode=GetP(&p))>0)
   {
      float P,T; int errcode;
      const float MaxErr=10;
      const float MaxP=mission.PressurePark+MaxErr;
      const float MinP=mission.PressurePark-MaxErr;

      /* collect the park-level temperature data */
      if ((errcode=GetPt(&P,&T))>0)
      {
         FILE *fp;

         /* check logging criteria */
         if (debuglevel>=3 || (debugbits&PARK_H))
         {
            /* create the log entry */
            static cc format[]="PT: %0.2f %0.4f\n";

            /* write the PT sample to the log file */
            LogEntry(FuncName,format,P,T);
         }
		  
         //CJ QUEPHONE MESSAGE - Parking Mode
         //QuecomSetPRK(p); //CJ does not use 11/15/2010 HM

         /* open the profile data file */
		if ((fp=fopen(prf_path,"a")))
		{
           time_t t=time(NULL),it=itimer();
            
           /* write the date and time */
           char buf[32]; strftime(buf,sizeof(buf),"%b %d %Y %H:%M:%S",gmtime(&t));
		   if(howoften % 6 == 0){//added to reduce the number of pressure and temp writings HM 4/8/2015
           /* write the PT sample to the profile file */
           fprintf(fp,"ParkPt:    %s %11ld %7ld %7.2f %7.4f\n",buf,t,it,P,T);
           }
           /* close the data file */
           fclose(fp);
		}
         else {static cc format[]="Open failed for: %s\n"; LogEntry(FuncName,format,prf_path);}
      }
      else
      {
         static cc format[] = "Low-power PT sample failed [err=%d].\n";
         LogEntry(FuncName,format,errcode);
      }

      /* check if the pressure is out-of-band on the low side */
      if (p<MinP)
      {
         /* maintain a record of out-of-band reports */
         if (vitals.ParkPOutOfBand<=0) --vitals.ParkPOutOfBand;
         else vitals.ParkPOutOfBand=-1;

         /* check active ballasting criteria */
         if (vitals.ParkPOutOfBand<=-3)
         {
            if (debuglevel>=2 || (debugbits&PARK_H))
            {
               /* create the message */
               static cc format[]="ParkPOutOfBand[%d, %0.1f dB]: retract piston.\n";

               /* make the logentry */
               LogEntry(FuncName,format,vitals.ParkPOutOfBand,p);
            }
            
            /* retract the piston one count to drive the float deeper */
            if (PistonMoveRel(-1)>0) vitals.ActiveBallastAdjustments++;

            /* reset the out-of-band counter */
            vitals.ParkPOutOfBand=0;
         }
      }

      /* check if the pressure is out-of-band on the high side */
      else if (p>MaxP)
      {
         /* maintain a record of out-of-band reports */
         if (vitals.ParkPOutOfBand>=0) ++vitals.ParkPOutOfBand;
         else vitals.ParkPOutOfBand=1;

         /* check active ballasting criteria */
         if (vitals.ParkPOutOfBand>=3)
         {
            if (debuglevel>=2 || (debugbits&PARK_H))
            {
               /* create the message */
               static cc format[]="ParkPOutOfBand[%d, %0.1f dB]: extend piston.\n";

               /* make the logentry */
               LogEntry(FuncName,format,vitals.ParkPOutOfBand,p);
            }
            
            /* extend the piston one count to drive the float shallower */
            if (PistonMoveRel(1)>0) vitals.ActiveBallastAdjustments++;

            /* reset the out-of-band counter */
            vitals.ParkPOutOfBand=0;
         }
      }

      /* pressure is in-band; reset the out-of-band counter*/
      else vitals.ParkPOutOfBand=0;

      if (debuglevel>=3 || (debugbits&PARK_H))
      {
         /* create the message */
         static cc format[]="ParkPOutOfBand[%d, %0.1f dB]\n";

         /* make the logentry */
         LogEntry(FuncName,format,vitals.ParkPOutOfBand,p);
      }
   }
   else
   {
      /* create the message */
      static cc format[]="Active ballast attempt aborted; "
         "pressure query failed. [err=%d]\n";

      /* make the logentry */
      LogEntry(FuncName,format,errcode);
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to initialize the park phase of the profile cycle             */
/*------------------------------------------------------------------------*/
/**
   This function initializes the park phase of the mission cycle.     
*/
int ParkInit(void)
{
   /* define the logging signature */
   static cc FuncName[] = "ParkInit()";

   int status=1;

   /* set the state variable */
   StateSet(PARK);
   
   /* make a log entry */
   if (debuglevel>=2 || (debugbits&PARK_H))
   {
      /* create the message */
      static cc msg[]="\n";

      /* make the logentry */
      LogEntry(FuncName,msg);
   }
   
   /* initialize the active ballasting record */
   vitals.ParkPOutOfBand=0;
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to terminate the park phase of the profile cycle              */
/*------------------------------------------------------------------------*/
/**
   This function terminates the park phase of the mission cycle.     
*/
int ParkTerminate(void)
{
   /* define the logging signature */
   static cc FuncName[] = "ParkTerminate()";

   int err,status=1;
   unsigned char V,A;
   struct Obs obs;
   float P,T;

   /* define the timeout for measuring the seascan PT module power consumption */
   const time_t TimeOut = 60;

   obs.p=nan(); obs.t=nan(); obs.s=nan(); /* salinity not supported by seascan */
   
   /* measure the internal vacuum at the park depth */
   vitals.Vacuum=BarometerAd8();

   /* measure the battery voltage and current */
   vitals.QuiescentVolts=BatVoltsAd8(); vitals.QuiescentAmps=BatAmpsAd8();

   /* copy the park piston position to the mission configuration */
   mission.PistonParkPosition=PistonPosition();
      
   /* recompute the signature of the mission configuration */
   mission.crc = Crc16Bit((unsigned char *)(&mission), sizeof(mission)-sizeof(mission.crc));

   /* measure the power consumption by the Seascan */
   CtdPower(&V,&A,TimeOut); vitals.SeascanVolts=V; vitals.SeascanAmps=A;
   
   /* make a log entry */
   if (debuglevel>=2 || (debugbits&PARK_H))
   {
      /* create the message */
      static cc format[]="Pist Pos:%d Vac:%d Vq:%d Aq:%d VSeascan:%d ASeascan:%d\n";

      /* make the logentry */
      LogEntry(FuncName,format,mission.PistonParkPosition,vitals.Vacuum,
               vitals.QuiescentVolts,vitals.QuiescentAmps,
               vitals.SeascanVolts,vitals.SeascanAmps);
   }
   
   /* necessary pause for Seascan PT module wakeup */
   else sleep(1);

   /* measure PT at the park depth */
   if ((err=GetPt(&P, &T)) < 0)
   {
      /* create the message */
      static cc format[]="GetPt()=%d\n";
      
      obs.p = P;
      obs.t = T;
      /* make the logentry */
      LogEntry(FuncName,format,err);
   }
   
   else if (debuglevel>=2 || (debugbits&PARK_H))
   {
      /* create the message */
      static cc format[]="PT: %0.1fdB %0.4fC\n";
      
      obs.p = P;
      obs.t = T;
      /* make the logentry */
      LogEntry(FuncName,format,obs.p,obs.t);
   }
   
   /* store the observation in the float vitals */
   vitals.ParkObs=obs;
   
   return status;
}

