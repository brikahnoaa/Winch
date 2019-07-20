#ifndef PRELUDE_H
#define PRELUDE_H (0x0008U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: prelude.c,v 1.13 2007/05/08 18:10:39 swift Exp $
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
 * $Log: prelude.c,v $
 * Revision 1.13  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.12  2006/10/11 21:01:08  swift
 * Implemented the module-wise logging facility.
 *
 * Revision 1.11  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.10  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.9  2005/10/03 16:11:25  swift
 * Change extension of data from 'prf' to 'msg'.
 *
 * Revision 1.8  2005/08/06 21:42:19  swift
 * Modifications to properly handle recover mode during the mission prelude as
 * well as during the mission.
 *
 * Revision 1.7  2005/02/22 21:06:56  swift
 * Changed the name of the config logger function.
 *
 * Revision 1.6  2005/01/06 00:55:56  swift
 * Changed log_path length in snprintf().
 *
 * Revision 1.5  2004/12/29 23:11:27  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.4  2004/04/26 15:59:55  swift
 * Changed location of an include directive to get it out of the header secion.
 *
 * Revision 1.3  2003/11/20 18:59:47  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.2  2003/11/12 23:03:49  swift
 * Unset TestMsg bit of the status byte.
 *
 * Revision 1.1  2003/09/11 21:35:36  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define PreludeChangeLog "$RCSfile: prelude.c,v $ $Revision: 1.13 $ $Date: 2007/05/08 18:10:39 $"

/* function prototypes */
int Prelude(void);
int PreludeInit(void);
int PreludeTerminate(void);

#endif /* PRELUDE_H */

extern persistent int ReactivatedByE;

#include <config.h>
#include <control.h>
#include <ds2404.h>
#include <eeprom.h>
#include <engine.h>
#include <logger.h>
#include <recovery.h>
#include <telemetry.h>

/*------------------------------------------------------------------------*/
/* execute tasks during the mission prelude                               */
/*------------------------------------------------------------------------*/
/**
   This function executes regularly scheduled tasks during the mission
   prelude.
*/
int Prelude(void)
{
   /* function name for log entries */
   static cc FuncName[] = "Prelude()";

   /* initialize the return value */
   int status=0;

   /* create the message and make a log entry */
   static cc msg[]="Logging config and initiate telemetry.\n";

   /* log the message */
   LogEntry(FuncName,msg);

   /* log the mission parameters */
   LogConfiguration(&mission,"Mission");
   
   /* execute recovery functionality */
   status=Recovery();
   
   /* parse if mission configuration was successfully downloaded */
   if (!(vitals.status&DownLoadCfg)) {configure(&mission,config_path);}
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to initialize the mission prelude                             */
/*------------------------------------------------------------------------*/
/**
   This function initializes the mission prelude.
*/
int PreludeInit(void)
{
   /* function name for log entries */
   static cc FuncName[] = "PreludeInit()";

   /* initialize the return value */
   int status=1;

   /* store the vacuum locally before reinitializing engineering data */
   unsigned int vac=vitals.Vacuum;
      
   /* get the current time and the current profile id */
   time_t now=itimer(); int pid=PrfIdGet(); 

   /* set the state to the mission prelude */
   StateSet(PRELUDE);

	if(ReactivatedByE!=1) //HM 11/17/2010
		/* initialize the profile id to zero */
		{
		static cc format[]="Normal mission activation\n";
		status=PrfIdSet(0);
		LogEntry(FuncName,format,pid,now);
		}
		else //==1
		{
		static cc format[]="Mission reactivated remotely\n";
		LogEntry(FuncName,format);
		//ReactivatedByE=0; //Mission was reactivated  //HM 11/17/2010 //diabled 12/23/2014 HM
		}
		
   /* reset the interval timer */
   IntervalTimerSet(0,0);
   
   /* make a log entry */
   if (debuglevel>=2 || (debugbits&PRELUDE_H))
   {
      /* create the message */
      static cc format[]="Mission restarted. [pid=%d, itime=%ld]\n";

      /* make the logentry */
      LogEntry(FuncName,format,pid,now);
   }
           
   /* initialize the engineering data */
   InitVitals();

   /* reinitialize the vacuum */
   vitals.Vacuum=vac;
 
   /* move the piston to full extenstion */
   PistonMoveAbs(mission.PistonFullExtension);
   
   return status;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
int PreludeTerminate(void)
{
   /* initialize the return value */
   int status=1;

   /* create a log file name */
   snprintf(prf_path,sizeof(prf_path),"%04d.000.msg",mission.FloatId);

   /* initialize the telemetry phase */
   TelemetryInit();

   /* execute telemetry functions */
   status=Telemetry();
   
   return status;
}
