#ifndef DESCENT_H
#define DESCENT_H (0x0010U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: descent.c,v 1.16.2.1 2008/09/11 19:58:56 dbliudnikas Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
 * $Log: descent.c,v $
 * Revision 1.16.2.1  2008/09/11 19:58:56  dbliudnikas
 * Replace SBE41 with Seascan TD: status.
 *
 * Revision 1.16  2008/07/14 16:54:24  swift
 * Implement compensator hyper-retraction mechanism for parking N2
 * floats at mid-water pressures.
 *
 * Revision 1.15  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.14  2006/10/11 21:00:34  swift
 * Integrated the new flashio file system.
 *
 * Revision 1.13  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.12  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.11  2005/10/11 20:45:29  swift
 * Added a flag for 8-bit profile counter overflow (for use with SBD floats).
 *
 * Revision 1.10  2005/10/03 16:11:25  swift
 * Change extension of data from 'prf' to 'msg'.
 *
 * Revision 1.9  2005/06/20 18:52:58  swift
 * Change handling of the surface pressure data so that the actual value is telemetered.
 *
 * Revision 1.8  2005/02/22 21:03:50  swift
 * Added code to define the profile path name.
 *
 * Revision 1.7  2005/01/06 00:54:57  swift
 * Added code to initialize prf_path.
 *
 * Revision 1.6  2004/12/29 23:11:27  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.5  2004/04/26 23:32:53  swift
 * Moved an include directive to get it out of the header section.
 *
 * Revision 1.4  2004/03/26 16:31:40  swift
 * Added criteria for limiting the surface pressure offset to be within a
 * 20 decibar deadband.
 *
 * Revision 1.3  2003/11/20 18:59:47  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.2  2003/11/12 22:41:22  swift
 * Modifications to use GetP() rather than Sbe41GetP().
 *
 * Revision 1.1  2003/09/11 22:05:01  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define DescentChangeLog "$RCSfile: descent.c,v $ $Revision: 1.16.2.1 $ $Date: 2008/09/11 19:58:56 $"
/* define the number of accumulated detections*/
extern persistent long int accum_dtx; //HM accumulated detections per profile

/* function prototypes for statically linked functions */
int Descent(void);
int DescentInit(void);
int DescentTerminate(void);

#endif /* DESCENT_H */

#include <control.h>
#include <config.h>
#include <logger.h>
#include <seascan.h>
#include <ds2404.h>
#include <eeprom.h>
#include <engine.h>
#include <unistd.h>
#include <apf9.h>
#include <stdio.h>
#include <flashio.h>
#include <profile.h>
#include <lt1598ad.h>
#include <apf9com.h>

#include "quecom.h" //CJ
persistent long int accum_dtx = 0;  //HM

/*------------------------------------------------------------------------*/
/* execute tasks during the descent phase of the mission cycle            */
/*------------------------------------------------------------------------*/
/**
   This function executes regularly scheduled tasks during the descent phase
   of the mission cycle.  A positive value is returned on success; otherwise
   zero is returned.
*/
int Descent(void)
{
   /* define the logging signature */
   static cc FuncName[] = "Descent()";

   int err,status=1;
   float p;

   /* measure the current pressure */
   if ((err=GetP(&p))<=0)
   {
      /* create the message */
      static cc format[]="Pressure measurement failed [%d].\n";

      /* make the logentry */
      LogEntry(FuncName,format,err);

      /* set a status bit */
      vitals.status|=SeascanPFail;
   }
   else
   {
	
      /* verify the logging verbosity */
      if (debuglevel>=2 || (debugbits&DESCENT_H))
      {
         /* create the message */
         static cc format[]="Press: %0.1f dB\n";

         /* make the logentry */
         LogEntry(FuncName,format,p);
      }
		
      //CJ QUEPHONE MESSAGE - Descending
      //QuecomSetDSC(p); //CJ does not use //11/15/2010 HM

      /* check criteria for early correction of compressee hyper-retraction */
      if (mission.PistonParkHyperRetraction && p>=MinN2ParkPressure &&
          mission.PistonParkPosition!=PistonPosition())
      {
         /* extend to the park piston position */
         PistonMoveAbs(mission.PistonParkPosition);
      }
   }
   
   /* check if the current pressure should be stored */
   if (vitals.ParkDescentPCnt<ParkDescentPMax)
   {
      /* initialize the pressure with a sentinel value */
      unsigned char P=0xfe;

      /* round the pressure to the nearest bar */
      if (err>0) P = (p>0) ? (unsigned int)(p/10.0 + 0.5) : 0;

      /* store the pressure (bars) in the array */
      vitals.ParkDescentP[vitals.ParkDescentPCnt]=P; vitals.ParkDescentPCnt++;
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to initialize the descent phase of the profile cycle          */
/*------------------------------------------------------------------------*/
/**
   This function initializes the descent phase of the mission cycle.  A
   positive value is returned on success; otherwise zero is returned.    
*/
int DescentInit(void)
{
   /* define the logging signature */
   static cc FuncName[] = "DescentInit()";

   int status=1;
   FILE *fp;
   float p=0;
   time_t now=itimer();
   
   /* reset the interval timer */
   IntervalTimerSet(0,0);
   
   /* set the state variable */
   StateSet(DESCENT);
     
   /* increment the profile number */
   status=PrfIdIncrement();
   accum_dtx = 0L; //HM reset # of detections=0 before profile begins
   //MesgInc   = 0;  //HM used in control.c
      
   /* create the file to contain the profile data */
   snprintf(prf_path,sizeof(prf_path),"%04d.%03d.msg",mission.FloatId,PrfIdGet());
   
   /* make sure the file system is valid and not full */
   if (!(fp=fopen(prf_path,"w")))
   {
      /* create the message */
      static cc msg[]="Reformatting file system.\n";

      /* make the logentry */
      LogEntry(FuncName,msg);

      /* format the file system */
      LogClose(); fcloseall(); fformat();

      /* open the log file */
      LogOpen(log_path,'a');
   }
   else fclose(fp);

   /* make a log entry */
   if (debuglevel>=2 || (debugbits&DESCENT_H))
   {
      /* create the message */
      static cc format[]="%s profile %d initiated at %ldsec.\n";

      /* make the logentry */
      LogEntry(FuncName,format,(DeepProfile()>0)?"Deep":"Park",PrfIdGet(),now);
   }
   
   /* make sure the sequence points satisfy sanity constraints */
   SequencePointsValidate();
   
   /* get the surface pressure reading just prior to descent */
   if (GetP(&p)<=0) p=vitals.SurfacePressure;

   /* log the surface pressure */
   if (debuglevel>=2 || (debugbits&DESCENT_H))
   {
      /* create the message */
      static cc format[]="Surface press: %0.1fdB.\n";

      /* make the logentry */
      LogEntry(FuncName,format,p);
   }
      
   /* initialize the engineering data */
   InitVitals(); 
   
   /* save the surface pressure reading */
   vitals.SurfacePressure = p;
   
   /* check if an 8-bit argos profile counter will overflow */
   if (PrfIdGet()>255) vitals.status|=PrfIdOverflow;

   /* open the air valve twice */
   sleep(2); AirValveOpen(); sleep(2); AirValveOpen();

   /* move the piston to the park position */
   PistonMoveAbs(mission.PistonParkPosition);
   
   /* check for N2 compressee hyper-retraction */
   if (mission.PistonParkHyperRetraction)
   {
      PistonMoveRel(-mission.PistonParkHyperRetraction);
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to terminate the descent phase of the mission cycle           */
/*------------------------------------------------------------------------*/
/**
   This function terminates the descent phase of the mission cycle.  A
   positive value is returned on success; otherwise zero is returned.
*/
int DescentTerminate(void)
{
   /* correct the compressee hyper-retraction */
   if (mission.PistonParkHyperRetraction && mission.PistonParkPosition!=PistonPosition())
   {
      /* extend to the park piston position */
      PistonMoveAbs(mission.PistonParkPosition);
   }

   return Descent();
}

