#ifndef ENGINE_H
#define ENGINE_H (0x01f0U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: engine.c,v 1.11 2008/07/14 16:54:56 swift Exp $
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
 * $Log: engine.c,v $
 * Revision 1.11  2008/07/14 16:54:56  swift
 * Reduced timeout specification to 2 hours.
 *
 * Revision 1.10  2007/06/08 02:14:52  swift
 * Reduced maximum time-out to 7200 seconds.
 *
 * Revision 1.9  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.8  2006/10/11 21:01:08  swift
 * Implemented the module-wise logging facility.
 *
 * Revision 1.7  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.6  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.5  2004/12/29 23:11:27  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.4  2003/12/09 15:53:59  swift
 * Eliminated the Maxwell counter as a termination criterion.
 *
 * Revision 1.3  2003/11/20 18:59:47  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.2  2003/10/25 19:30:16  swift
 * Changed function arguments from 'unsigned char' to 'int' to avoid variable
 * overflow bugs.
 *
 * Revision 1.1  2003/09/10 18:12:31  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define engineChangeLog "$RCSfile: engine.c,v $  $Revision: 1.11 $   $Date: 2008/07/14 16:54:56 $"

#include <time.h>

/* function prototypes */
int PistonMoveAbs(int newPosition); 
int PistonMoveAbsWTO(int newPosition,unsigned char *Volt,
                     unsigned char *Amp, time_t timeout);
int PistonMoveRel(int counts); 
   
#endif /* ENGINE_H */

#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <logger.h>
#include <control.h>
#include <apf9.h>
#include <ds2404.h>
#include <lt1598ad.h>
#include <apf9com.h>
#include <stdio.h>
#include <conio.h>

#include "quecom.h" //CJ

#define EXTEND   1
#define RETRACT -1
#define BRAKE    0

/*------------------------------------------------------------------------*/
/* function to move the piston to a specified position                    */
/*------------------------------------------------------------------------*/
/**
   This function moves the piston to a specified position.

      \begin{verbatim}
      input:
         newPosition ... The new position to which the piston should be moved.

      output:
         This function returns a positive number if the piston position, on
         exit from this function, matches the user-specified piston
         position.  Zero is return otherwise.
      \end{verbatim}
*/
int PistonMoveAbs(int newPosition)
{
   return PistonMoveAbsWTO(newPosition,NULL,NULL,7200L);
}

/*------------------------------------------------------------------------*/
/* function to move the buoyancy engine's piston to a specific count      */
/*------------------------------------------------------------------------*/
/**
   This function moves the buoyancy engine's piston to a specific position
   as determined by the piston position sensor.  The piston is protected
   against crashing against either end-limit.  The piston is prevented from
   being retracted or extended past specified limits.  A dead-man's timeout
   is also implemented so that if the piston position does not change during
   a specified time-out interval, the attempt to reposition the piston is
   aborted.

      \begin{verbatim}
      input:
         newPosition ... The new position to which the piston should be moved.
         timeout.........The maximum time that the motor should be allowed
                         to run.
         
      output:

         Volt...The battery voltage (counts) as measured just before the
                motor was turned off.
         Amp....The battery current (counts) as measured just before the
                motor was turned off.
      
         This function returns a positive number if the piston position, on
         exit from this function, matches the user-specified piston
         position.  Zero is return otherwise.
      \end{verbatim}
*/
int PistonMoveAbsWTO(int newPosition,unsigned char *Volt,
                     unsigned char *Amp, time_t timeout)
{
   /* define the logging signature */
   static cc FuncName[] = "PistonMoveAbsWTO()";

   time_t pistonStopTime,dT;
   unsigned int VCnt=0xff,ACnt=0xff;

   /* initialize return value */
   int status=1;

   /* initialize the piston position */
   unsigned char pistonPosition=PistonPosition();
   
   /* get the start time */
   const time_t pistonStartTime=itimer(), TimeOut=40; //Timeout was 30 changed to 40 HM

   /* condition the new position to be in the range of unsigned int */
   if (newPosition<0) newPosition=0; else if (newPosition>255) newPosition=255;
   
   /* make a log entry */
   if (debuglevel>=2 || (debugbits&ENGINE_H))
   {
      /* create the message */
      static cc format[]="%03d->%03d"; //HM erased the first blank 

      /* make the logentry */
      LogEntry(FuncName,format,pistonPosition,newPosition);
   }
   
   /* initialize the mag-switch flip-flop */
   MagSwitchReset();
   
   /* check if the piston can be extended */
   if((newPosition>pistonPosition) && (pistonPosition<mission.PistonFullExtension))
   {
      /* record the current piston position */
      unsigned char Po=pistonPosition; time_t To=pistonStartTime;
      
      /* make sure the piston does not become over-extended */
      if(newPosition > mission.PistonFullExtension) newPosition = mission.PistonFullExtension;

      /* activate the buoyancy engine */
      //CJ QUEPHONE MESSAGE - Piston Motor ON //Move here HM
      EngineState=1;
	  QuecomSetPMA(EngineState);
	  
      HDrive(EXTEND);

      do
      {
         /* wait a bit and then measure the new piston position */
         Wait(500); pistonPosition = PistonPosition();

         /* implement a dead-man's timeout for piston movement */
         if (abs(pistonPosition-Po)>0)
         {
            /* record the new piston position and the time */
            Po=pistonPosition; To=itimer();

            /* write some user-feedback to the log */
            if (debuglevel>=3 || (debugbits&ENGINE_H)) LogAdd(" %03d",Po); //HM debuglevel was 2
         }

         /* compute quantities used in termination criteria */
         dT=(time_t)difftime(itimer(),To);
      }

      /* check criteria for terminating the piston extension */
      while ((pistonPosition<newPosition) && dT<TimeOut &&
             difftime(itimer(),pistonStartTime)<timeout &&
             !MagSwitchToggled());

      /* measure power consumption */
      VCnt=BatVoltsAd8(); ACnt=BatAmpsAd8();
      
      /* deactivate the buoyancy engine */
      HDrive(BRAKE);
      //QUEPHONE MESSAGE - Piston Motor OFF
      EngineState=0;
	  QuecomSetPMA(EngineState);
   }

   /* check if the piston can be retracted */
   else if((newPosition < pistonPosition) && (pistonPosition > mission.PistonFullRetraction))
   {
      /* record the current piston position */
      unsigned char Po=pistonPosition; time_t To=pistonStartTime;
      
      /* make sure the piston does not become overly-retracted */
      if(newPosition < mission.PistonFullRetraction) newPosition = mission.PistonFullRetraction;
      
      /* activate the buoyancy engine */
	  //QUEPHONE MESSAGE - Piston Motor ON
	  EngineState=1;
      QuecomSetPMA(EngineState);
  
      HDrive(RETRACT);

      do
      {
         /* wait a bit and then measure the new piston position */
         Wait(500); pistonPosition = PistonPosition();

         /* implement a dead-man's timeout for piston movement */
         if (abs(pistonPosition-Po)>0)
         {
            /* record new piston position and time */
            Po=pistonPosition; To=itimer();
            
            /* write some user-feedback to the log */
            if (debuglevel>=3 || (debugbits&ENGINE_H)) LogAdd(" %03d",Po); //HM debuglevel was 2
         } 
 
         /* compute quantities used in termination criteria */
         dT=(time_t)difftime(itimer(),To);
      }
      
      /* check criteria for terminating the piston retraction */
      while((pistonPosition>newPosition) && dT<TimeOut &&
             difftime(itimer(),pistonStartTime)<timeout &&
             !MagSwitchToggled());
      
      /* measure power consumption */
      VCnt=BatVoltsAd8(); ACnt=BatAmpsAd8();

      /* deactivate the buoyancy engine */
      HDrive(BRAKE);
      //QUEPHONE MESSAGE - Piston Motor OFF
      EngineState=0;
	  QuecomSetPMA(EngineState);
   }
   
   /* record the length of time that the buoyancy pump was running */
   pistonStopTime = itimer();

   /* keep a running tally of the pumping time */
   vitals.BuoyancyPumpOnTime += (pistonStopTime - pistonStartTime); 

   /* make a log entry */
   if (debuglevel>=2 || (debugbits&ENGINE_H))
   {
      /* compute the voltage from the volt-counts */
      float Volt = (VCnt!=0xff) ? Volts(VCnt) : 0;

      /* compute the current from the amp-counts */
      float Amp  = (ACnt!=0xff) ? Amps(ACnt)  : 0;
      
      LogAdd(" [%lds, %0.1fV, %0.3fA, CPT:%lds]\n",
             (pistonStopTime - pistonStartTime),Volt,Amp,
             vitals.BuoyancyPumpOnTime);
   }

   /* assign the power measurement to the function arguments */
   if (Volt) *Volt = VCnt; if (Amp) *Amp = ACnt;
   
   /* check if the request was successful */
   status = (abs(pistonPosition-newPosition)<=1) ? 1 : 0; 

   return status;
} 

/*------------------------------------------------------------------------*/
/* function to move the piston relative to its current location           */
/*------------------------------------------------------------------------*/
/**
   This function extends or retracts the buoyancy engine's piston by a
   specified number of counts.

      \begin{verbatim}
      input:
         counts ... The number of counts to move the piston relative to its
                    current location.  Positive values extend the piston and
                    negative values retract the piston.

      output:
         This function returns a positive number on success and zero on
         failure.  A negative return value indicates an exceptional
         condition.
      \end{verbatim}
*/
int PistonMoveRel(int counts)
{
   /* compute the new piston position */
   int newPosition = PistonPosition() + counts;

   /* make sure the piston position stays well conditioned */
   if (newPosition<0) newPosition=0; else if (newPosition>255) newPosition=255;

   /* move the piston to the new position */
   return PistonMoveAbsWTO(newPosition, NULL, NULL, 7200L);
}
