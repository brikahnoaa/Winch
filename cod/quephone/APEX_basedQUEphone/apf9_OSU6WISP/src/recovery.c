#ifndef RECOVERY_H
#define RECOVERY_H (0x0008U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: recovery.c,v 1.11 2007/05/08 18:10:39 swift Exp $
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
 * $Log: recovery.c,v $
 * Revision 1.11  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.10  2007/03/26 20:46:06  swift
 * Fixed a legacy bug in Recovery() that removed all files in storage after
 * each telemetry cycle.  With RAMIO storage this was necessary due to space
 * limitations.  However, with flash storage, this is definitely undesirable.
 *
 * Revision 1.9  2006/10/11 21:01:08  swift
 * Implemented the module-wise logging facility.
 *
 * Revision 1.8  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.7  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.6  2005/08/08 15:01:59  swift
 * Fixed a bug in the determinating whether to resume the mission prelude or
 * a regular profile cycle.
 *
 * Revision 1.5  2005/08/06 21:42:19  swift
 * Modifications to properly handle recover mode during the mission prelude as
 * well as during the mission.
 *
 * Revision 1.4  2005/06/27 15:13:41  swift
 * Fixed some erroneous comments.
 *
 * Revision 1.3  2005/02/22 21:08:56  swift
 * Added code to acquire a GPS fix.
 *
 * Revision 1.2  2005/01/06 00:58:42  swift
 * Initialized prf_path[].
 *
 * Revision 1.1  2004/12/29 23:11:27  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define recoveryChangeLog "$RCSfile: recovery.c,v $ $Revision: 1.11 $ $Date: 2007/05/08 18:10:39 $"

/* function prototypes */
int Recovery(void);
int RecoveryInit(void);
int RecoveryTerminate(void);

#endif /* RECOVERY_H */

#include <control.h>
#include <eeprom.h>
#include <engine.h>
#include <gps.h>
#include <logger.h>
#include <lt1598ad.h>
#include <prelude.h>
#include <descent.h>
#include <flashio.h>
#include <string.h>
#include <telemetr.h>
#include <time.h>
#include <control.h>

#ifdef _XA_
   #include <apf9.h>
#else
   #define WatchDog()
   #define StackOk() 1
#endif /* _XA_ */

persistent static enum State StateOnExit;

/*------------------------------------------------------------------------*/
/* implement recovery mode functionality                                  */
/*------------------------------------------------------------------------*/
/**
   This function implements a recovery mode for iridium drifters.  
*/
int Recovery(void)
{
   /* initialize the return value */
   int i,n,status=0;

   /* local objects for logfile creation */
   char buf[12]; time_t T;

   /* initialize the flag to download the mission configuration */
   vitals.status |= DownLoadCfg;
  
	//DspOn();//HM debug
	
   /* make sure the piston is fully extended */
   if (PistonPosition()<mission.PistonFullExtension)
   {
      /* put the float into its maximum buoyancy state */
      PistonMoveAbs(mission.PistonFullExtension);
   }

   /* close all streams */
   LogClose(); fcloseall();
   
   /* encode the date/time */
   T=time(NULL); strftime(buf,sizeof(buf),"%y%m%d%H%M",gmtime(&T));

   /* create the recovery filename */
   snprintf(prf_path,sizeof(prf_path),"%04d.%s",mission.FloatId,buf);

   /* create the logfile name */
   snprintf(log_path,sizeof(log_path),"%04d.%s.log",mission.FloatId,buf);

   /* allow 3 tries to open the recovery file */
   for (i=0; i<3 && LogOpen(log_path,'w')<=0; i++) {fformat();}
   
   /* execute telemetry */
   if ((status=Telemetry())>0) {vitals.ConnectionAttempts=0; vitals.Connections=0;}

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
/* function to exit recovery mode and re-enter the mission in progress    */
/*------------------------------------------------------------------------*/
int RecoveryTerminate(void)
{
   /* initialize the return value */
   int status=1;

   /* exit to mission prelude or else descent phase */
   if (StateOnExit==PRELUDE) PreludeInit(); else DescentInit();

   return status;
}

/*------------------------------------------------------------------------*/
/* function to initialize a cycle of the recovery mode                    */
/*------------------------------------------------------------------------*/
int RecoveryInit(void)
{
   /* initialize the return value */
   int status=1;

   /* get the current state of the mission */
   enum State state = StateGet();

   /* initialize StateOnExit only if not already in recovery mode */
   if (state!=RECOVERY)
   {
      /* determine which state to enter upon exiting recovery mode */
      StateOnExit = (state>=DESCENT && state<=TELEMETRY) ? DESCENT : PRELUDE;
   }
   
   /* set the state to the mission prelude */
   StateSet(RECOVERY);

   /* intialize telemetry statistics */
   vitals.ConnectionAttempts=0; vitals.Connections=0;
      
   return status;
}
