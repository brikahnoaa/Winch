#ifndef GPS_H
#define GPS_H (0x0100U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: gps.c,v 1.14 2007/05/08 18:10:39 swift Exp $
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
 * $Log: gps.c,v $
 * Revision 1.14  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.13  2006/10/11 20:55:57  swift
 * Changed the timestamp-ing of almanac uploads to account for manually-induced
 * uploads.
 *
 * Revision 1.12  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.11  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.10  2006/01/06 23:23:44  swift
 * Improved time-skew checking.
 *
 * Revision 1.9  2005/08/06 17:19:35  swift
 * Add a logentry for magnet swipes during GPS acquisition.
 *
 * Revision 1.8  2005/07/05 21:59:43  swift
 * Fixed a minor formatting error in a logentry.
 *
 * Revision 1.7  2005/06/27 15:16:23  swift
 * Added a retry loop to make GPS fix acquisition more fault tolerant.
 *
 * Revision 1.6  2005/06/23 20:31:21  swift
 * Elminated an extraneous call to LogNmeaSentences().
 *
 * Revision 1.5  2005/06/20 18:55:23  swift
 * Changed location of call to LogNmeaSentences() so that logging happens even
 * if a fix was not obtained.
 *
 * Revision 1.4  2005/04/04 22:35:01  swift
 * Change to a one-time GPS configuration policy.
 *
 * Revision 1.3  2005/03/31 17:26:10  swift
 * Added facility to log NMEA sentences.
 *
 * Revision 1.2  2005/02/22 21:05:49  swift
 * Added function to compute the time relative to the Unix epoch.
 *
 * Revision 1.1  2005/01/18 18:49:44  swift
 * Module to manage GPS services for iridium APEX floats.
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define GpsChangeLog "$RCSfile: gps.c,v $ $Revision: 1.14 $ $Date: 2007/05/08 18:10:39 $"

#include <stdio.h>
#include <nmea.h>

/* prototypes for functions with external linkage */
int GpsServices(FILE *dest);
time_t UnixTime(const struct NmeaGpsFields *gps);
/* object to record the time-stamp of the most recent almanac */
extern persistent time_t AlmanacTimeStamp;
   
#endif /* GPS_H */

#include <apf9com.h>
#include <stdio.h>
#include <conio.h>
#include <apf9.h>
#include <apf9icom.h>
#include <config.h>
#include <control.h>
#include <ds2404.h>
#include <eeprom.h>
#include <garmin.h>
#include <logger.h>
#include <math.h>
#include <nmea.h>
#include <unistd.h>
#include <apf9.h>
#include <cmds.h>
#include "quecom.h" //CJ
#include <gps.h>

persistent time_t AlmanacTimeStamp;

/*------------------------------------------------------------------------*/
/* function to manage and execute GPS services                            */
/*------------------------------------------------------------------------*/
/**
   This function manages functionality associated with acquiring GPS fixes,
   maintaining a current almanac in the GPS, and correcting APF9 clock
   skew.  This function returns a positive value if a fix was acquired.  A
   zero or negative return value indicates failure.
*/
int GpsServices(FILE *dest)
{
   /* define the logging signature */
   static cc FuncName[] = "GpsServices()";

   /* initialize the return value */
   int status=0;

   /* define objects used when acquiring GPS fix */
   int i=0; const int NRetry=2;

   /* define objects used when acquiring GPS fix */
   time_t To,dT,GpsTime,timeout=300; struct NmeaGpsFields gpsfix;

   /* define the almanac upload triggers */
   const time_t MaxAlmanacAge=90*Day, MinAlmanacAge=45*Day; 

   /* compute the current age of the almanac */
   time_t AlmanacAge=difftime(time(NULL),AlmanacTimeStamp);
   //LogAdd("AlmanacTimeStamp=%ld, time(NULL)=%ld, diff=%ld\n",AlmanacTimeStamp, time(NULL),time(NULL)-AlmanacTimeStamp);//HM debug
   
   /* power-up the GPS engine */
   GpsEnable(4800); if (gps.ioflush) gps.ioflush();
   Wait(2000);//HM 12/29/2014

   /* check if the GPS almanac has aged-out */
   if (AlmanacAge<0 || AlmanacAge>MaxAlmanacAge) //Original
   //if(AlmanacAge<0 && AlmanacAge>MaxAlmanacAge) // WARN! debug LogWil put here to skip GPS Almanac
   {
      if (debuglevel>=2 || (debugbits&GPS_H))
      {
         static cc format[]="Replacing aged (%ld days) almanac.\n";
         LogEntry(FuncName,format,AlmanacAge/Day);
      }

      /* acquire new almanac */
      if (UpLoadGpsAlmanac(&gps)>0)
      {
         /* time-stamp the new almanac */
         AlmanacTimeStamp=time(NULL);

         if (debuglevel>=2 || (debugbits&GPS_H))
         {
            static cc msg[]="New almanac acquired.\n"; 
            LogEntry(FuncName,msg);
         }
      }
      else
      {
         static cc msg[]="GPS almanac acq failed.\n";
         LogEntry(FuncName,msg);
      }
   }
   else if (debuglevel>=2 || (debugbits&GPS_H)) 
   {
      static cc msg[]="GPS almanac is current.\n";
      LogEntry(FuncName,msg);
   }
   
   if (debuglevel>=3 || (debugbits&GPS_H)) //debuglevel was 2 before HM
   {
      static cc msg[]="Initiating GPS fix acq.\n";
      LogEntry(FuncName,msg);
   }

   /* retry loop for GPS fix acquisition */
   for (status=0,dT=0,To=time(NULL),i=1; i<=NRetry && status<=0; i++)
   {
      /* get the GPS fix */
      if (GetGpsFix(&gps,&gpsfix,timeout)>0)
      {
		
         /* indicate success in acquiring a fix */
         status=1; dT=(time_t)difftime(time(NULL),To);
            
         /* write the GPS fix data to the data file */ /*changed Fix: format 11/05/10 HM*/
         if (dest) fprintf(dest,"# GPS fix obtained in %ld seconds.\n"
                           "#    %8s  %7s  %2s/%2s/%4s %6s %4s\n"
                           "Fix: %9.4f %8.4f %02d/%02d/%04d %06ld %4d\n",
                           dT,"lon","lat","mm","dd","yyyy","hhmmss","nsat",
                           gpsfix.lon,gpsfix.lat,gpsfix.mon,gpsfix.day,
                           gpsfix.year,gpsfix.hhmmss,gpsfix.nsat);
      
         /* store the fix-acquisition time in engineering data */
         vitals.GpsFixTime=dT;

         /* write the GPS fix data to the logfile */
         if (debuglevel>=2 || (debugbits&GPS_H)) 
         {
            static cc fmt1[]="Profile %d GPS fix obtained in %ld sec.\n";
            //static cc fmt2[]="     %8s %7s %2s/%2s/%4s %6s %4s\n";
            static cc fmt3[]="Fix: %9.4f %8.4f %02d/%02d/%04d %06ld %4d\n";//increase res of lat and long HM

            /* write the fix to the log file */
            LogEntry(FuncName,fmt1,PrfIdGet(),dT);
            //LogEntry(FuncName,fmt2,"lon","lat","mm","dd","yyyy","hhmmss","nsat"); //HM
            LogEntry(FuncName,fmt3,gpsfix.lon,gpsfix.lat,gpsfix.mon,gpsfix.day,
                     gpsfix.year,gpsfix.hhmmss,gpsfix.nsat);
         }
      }
      else if (MagSwitchToggled())
      {
         static cc msg[]="GPS fix interrupted by swipe of mag-switch.\n";
         LogEntry(FuncName,msg);
         break;
      }
      
      else if (i<NRetry)
      {
         static cc msg[]="GPS fix not acquired after %lds; power-cycling the GPS.\n";
         dT=(time_t)difftime(time(NULL),To); LogEntry(FuncName,msg,dT);
          
         /* power-cycle the GPS engine */
         GpsDisable(); sleep(2); GpsEnable(4800); if (gps.ioflush) gps.ioflush();
      }
   }

   /* check for successful GPS acquisition */
   if (status>0)
   {
      /* get GPS time to protect against clock skew */
      if ((GpsTime=GetGpsTime(&gps,60))>0)
      {
		
         /* compute the clock skew */
         time_t RtcSkew=(time_t)difftime(time(NULL),GpsTime);//HM debug added (time_t)
		  
         /* store the clock skew in engineering data */
         vitals.RtcSkew=RtcSkew;

         /* check if clock skew should be corrected */
         if (fabs(RtcSkew)>Min/2)
         {
            static cc format[]="Excessive RTC skew (%lds) detected.  "
               "Resetting Apf9's RTC to %s";
            LogEntry(FuncName,format,RtcSkew,ctime(&GpsTime));
               
            /* reset the Apf9's RTC */
            if (RtcSet(GpsTime,0)>0)
            {
               time_t T=time(NULL);
               static cc format[]="Apf9's RTC now reads %s";
               LogEntry(FuncName,format,ctime(&T));
            }
            else
            {
               static cc msg[]="Apf9 RTC skew correction failed.\n";
               LogEntry(FuncName,msg);
            }

         }
         else if (debuglevel>=2 || (debugbits&GPS_H)) 
         {
            static cc msg[]="APF9 RTC skew (%lds) OK.\n";
            LogEntry(FuncName,msg,RtcSkew); 
         }
      }
      else {static cc msg[]="Apf9 RTC skew check aborted.\n"; LogEntry(FuncName,msg);}
      
      /* check criteria for replacement of almanac based on acquisition performance */
      if (dT>timeout && AlmanacAge>MinAlmanacAge)
      {
         if (debuglevel>=2 || (debugbits&GPS_H))
         {
            static cc format[]="Replacing almanac to speed future fix-acquisition.\n";
            LogEntry(FuncName,format,AlmanacAge/Day);
         }

         /* acquire new almanac */
         if (UpLoadGpsAlmanac(&gps)>0)
         {
            /* time-stamp the new almanac */
            AlmanacTimeStamp=time(NULL);

            if (debuglevel>=2 || (debugbits&GPS_H))
            {
               static cc msg[]="New almanac acquired.\n"; 
               LogEntry(FuncName,msg);
            }
         }
         else
         {
            static cc msg[]="GPS almanac acquisition failed.\n";
            LogEntry(FuncName,msg);
         }
      }
   }
   
   else
   {
      static cc format[]="GPS fix-acq failed after %lds.  "
         "Apf9 RTC skew check by-passed.\n";
      LogEntry(FuncName,format,dT);
      
      /* note the failed attempt in the data file */
      if (dest) fprintf(dest,"\n# Attempt to get GPS fix failed "
                        "after %lds.\n",dT);
   }

   /* log the nmea sentences */
   //LogNmeaSentences(&gps);  //HM Too much!
   
   /* power-down the GPS engine */
   GpsDisable();

   if (debuglevel>=2 || (debugbits&GPS_H)) 
   {
      static cc msg[]="GPS srvcs complete.\n";
      LogEntry(FuncName,msg);
   }

   GPSlon=gpsfix.lon; //HM GPSlon in quecom.c extern persistent float
   GPSlat=gpsfix.lat; //HM
   //CJ QUEPHONE MESSAGE - GPS
   //DspOn(); Wait(60000);
   //QuecomSetGPS(GpsTime, gpsfix.lon, gpsfix.lat);
	
   return status;
}

/*------------------------------------------------------------------------*/
/* function to compute time of fix relative to the unix epoch             */
/*------------------------------------------------------------------------*/
/**
   This function computes the time of the GPS fix relative to the Unix epoch.

      \begin{verbatim}
      input:
         gpsfix ... A NmeaGpsFields object that contains the GPS fix.  
      
      output:
         This function returns the time represented in the NmeaGpsFields
         object expressed as the number of seconds since the Unix epoch
         (00:00:00 Jan 1, 1970).  If this object is invalid or if the time
         represented is invalid then this function returns a negative
         number.
      \end{verbatim}
*/
time_t UnixTime(const struct NmeaGpsFields *gpsfix)
{
   /* define the logging signature */
   static cc FuncName[] = "UnixTime()";

   /* initialize the return value */
   time_t GpsSec=-1;

   /* validate the function argument */
   if (!gpsfix)
   {
      static cc msg[]="NULL function parameter.\n";
      LogEntry(FuncName,msg);
   }

   /* validate the date */
   else if (gpsfix->year<2000 || gpsfix->year>2100 ||
            gpsfix->mon<1     || gpsfix->mon>12    ||
            gpsfix->day<1     || gpsfix->day>31    ||
            gpsfix->hhmmss<0  || gpsfix->hhmmss>235959)
   {
      static cc format[]="Invalid GPS date/time: %02d/%02d/%04d %06ld\n";
      LogEntry(FuncName,format,gpsfix->mon,gpsfix->day,gpsfix->year,gpsfix->hhmmss);
   }
   
   else
   {
      /* compute the decimal encoded hours and minutes */
      long int hhmm=gpsfix->hhmmss/100;

      /* define a tm structure to contain the broken down date and time */
      struct tm date={0,0,0,0,0,0,0,0,0};

      /* initialize the elements of the broken down time */
      date.tm_sec=gpsfix->hhmmss-hhmm*100;
      date.tm_hour=hhmm/100;
      date.tm_min=hhmm-date.tm_hour*100;
      date.tm_mday=gpsfix->day;
      date.tm_mon=gpsfix->mon-1;
      date.tm_year=gpsfix->year-1900;
      
      /* compute the number of seconds in the epoch */
      GpsSec=mktime(&date);
   }
   
   return GpsSec;
}
