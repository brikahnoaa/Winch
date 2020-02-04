#ifndef NMEA_H
#define NMEA_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: nmea.c,v 2.10 2007/04/24 01:43:29 swift Exp $
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
 * $Log: nmea.c,v $
 * Revision 2.10  2007/04/24 01:43:29  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 2.9  2006/03/15 15:58:45  swift
 * Implemented prescreening of NMEA messages in GetGpsFix() to speed up
 * processing and decrease time to get fix.
 *
 * Revision 2.8  2006/01/06 23:11:46  swift
 * Significant improvements made to the function that synchronizes the APF clock
 * to GPS time.
 *
 * Revision 2.7  2005/05/13 15:26:47  swift
 * Modified acceptance criteria to allow for a '2' in field 6 of the GGA nmea
 * string (this value is undocumented in the garmin manual).
 *
 * Revision 2.6  2005/04/30 15:09:22  swift
 * Fixed a bug in the mechanism to prevent overflow of the GPS fifo - reinitialize
 * the reference time for periodic IO flushes.
 *
 * Revision 2.5  2005/04/29 20:45:41  swift
 * Implement mechanism to prevent clogging or overflow of GPS fifo (periodic flushes).
 *
 * Revision 2.4  2005/02/22 21:51:28  swift
 * Modified GetGpsFix() to return the time needed to obtain the fix.
 *
 * Revision 2.3  2005/01/15 00:48:53  swift
 * Modified NMEA API for use with XA compiler for the APF9.
 *
 * Revision 2.2  2002/10/08 23:51:52  swift
 * Combined header file into source file.  Added hack to work around a firmware bug
 * in the Garmin GPS25 where latency exists between the number of satellites used and
 * the fix determination.
 *
 * Revision 2.1  2002/07/31 14:27:47  swift
 * Revision for use with Garmin GPS25 model gps engine.
 *
 * Revision 1.2  2002/07/30 18:27:10  swift
 * Modifications to use POSIX regex library rather than the GNU regex library.
 *
 * Revision 1.1  2002/05/07 22:19:55  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define NmeaChangeLog "$RCSfile: nmea.c,v $  $Revision: 2.10 $   $Date: 2007/04/24 01:43:29 $"

#include <serial.h> 

/*========================================================================*/
/* structure to contain data from the NMEA 0183 messages                  */
/*========================================================================*/
/**
   This structure is a repository in which to store data from NMEA 0183
   messages received from a GPS receiver.  The following description of the
   NMEA format was taken from Appendix E of the Trimble Lassen LP GPS System
   Designer Reference Manual (Part No 39264-00, Firmware 7.82, August 1999).

   \begin{verbatim}
   E.2 NMEA 0183 Message Format 
      
      The NMEA 0183 protocol covers a broad array of navigation data. This 
      broad array of information is separated into discrete messages which 
      convey a specific set of information.  The entire protocol encompasses 
      over 50 messages, but only a sub-set of these messages apply to a GPS 
      receiver like the Lassen LP GPS. The NMEA message structure is 
      described below. 
      
          \$IDMSG,D1,D2,D3,D4,.......,Dn*CS[CR][LF]  
      
          "\$"    The "\$" signifies the start of a message. 
      
           ID    The talker identification is a two letter mnemonic which 
                 describes the source of the navigation information. The GP 
                 identification signifies a GPS source. 
      
          MSG    The message identification is a three letter mnemonic which
                 describes the message content and the number and order of
                 the data fields.
      
          ","    Commas serve as delimiters for the data fields. 
      
           Dn    Each message contains multiple data fields (Dn) which are 
                 delimited by commas. 
      
           "*"   The asterisk serves as a checksum delimiter. 
      
            CS   The checksum field contains two ASCII characters which
                 indicate the hexadecimal value of the checksum.
      
      [CR][LF]   The carriage return [CR] and line feed [LF] combination 
                 terminate the message. 
      
      NMEA 0183 messages vary in length, but each message is limited to 79 
      characters or less.  This length limitation excludes the "\$" and the 
      [CR][LF]. The data field block, including delimiters, is limited to 74
      characters or less. 
   \end{verbatim}

   Written by Dana Swift
*/
struct NmeaGpsFields
{
   /** Latitude [-90, 90]. */
   float lat;
      
   /** Longitude (-180, 180]. */
   float lon;      

   /** UTC time of day [0, 24) (hours). */
   float hour;
      
   /** UTC day (DDMMYY) of the GPS fix. */
   long ddmmyy;   
      
   /** UTC time (HHMMSS) of the GPS fix. */
   long hhmmss;   

   /** GPS status: 'A'=ValidPosition, 'V'=NavReceiverWarning */   
   char status;

   /** GPS quality indicator: 0=NoGps, 1=Gps, 2=DGps. */   
   int quality;
      
   /** Number of satellites in use. */   
   int nsat;
      
   /** day of month of GPS fix [1, 31]. */
   int day;

   /** month of year of GPS fix [1, 12]. */   
   int mon;

   /** year of GPS fix */   
   int year;
};

/* declare an initialization NmeaGpsFields object */
extern const struct NmeaGpsFields zeros;

/* define the maximum allowed string length for a NMEA string */
#define MaxNmeaStrLen 82
 
/* function prototypes for public functions to read NMEA 0183 messages and data */
time_t        GetGpsFix(const struct SerialPort *port, struct NmeaGpsFields *gpsfix, time_t sec);
time_t        GetGpsTime(const struct SerialPort *port,  time_t sec);
void          init_nmea_struct(struct NmeaGpsFields *gpsfix);
unsigned char nmea_checksum(const char *msg,int start,int end);
int           nmea0183(const char *msg,struct NmeaGpsFields *gpsfix);

#endif /* NMEA_H */

#include <strtoul.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <ctype.h>
#include <extract.h>
#include <logger.h>
#include <regex.h>

#ifdef _XA_
   #include <apf9.h>
   #include <apf9icom.h>
#endif /* _XA_ */

/* declare local function prototypes */
static int gga(const char *msg,struct NmeaGpsFields *gpsfix);
static int rmc(const char *msg,struct NmeaGpsFields *gpsfix);

/* define the maximum allowed string length for a NMEA string */
static const int MaxNmeaDataStrLen = 74; 

/* define an initialization NmeaGpsFields object */
const struct NmeaGpsFields zeros={0,0,0,0,0,0,0,0,0,0,0};

/*------------------------------------------------------------------------*/
/* function to extract a GPS fix from NMEA messages                       */
/*------------------------------------------------------------------------*/
/**
   This function scans the serial port data stream for NMEA messages from
   which a GPS fix can be extracted.  
   
      \begin{verbatim}
      input:

         port.....A structure that contains pointers to machine dependent
                  primitive IO functions.  See the comment section of the
                  SerialPort structure for details.  The function checks to
                  be sure this pointer is not NULL.

         sec.....The maximum amount of time (measured in seconds) that this
                 function will attempt to read NMEA messages from the serial
                 port before returning to the calling function.
 
      output:

         gpsfix...A structure into which the GPS fix is written.  See the
                  NmeaGpsFields structure declaration for details.  This
                  function initializes the NmeaGpsFields object prior to
                  reading any NMEA messages.  It is not necessary for the
                  calling function to initialize this object.

         If a fix was successfully determined, this function returns a
         strictly positive value equal to the number of seconds required to
         obtain the fix. Otherwise a zero or negative value is returned.
      \end{verbatim}
*/
time_t GetGpsFix(const struct SerialPort *port, struct NmeaGpsFields *gpsfix, time_t sec)
{
   /* define the logging signature */
   static cc FuncName[] = "GetGpsFix()";

   /* initialize the function's return value */
   int status = -1;
           
   /* initialize the timeout period */
   time_t T,To,dT;
 
   /* validate the SerialPort object */
   if (!port) {static cc msg[]="NULL serial port.\n"; LogEntry(FuncName,msg);}

   /* flush IO buffers */
   else if (!port->ioflush || port->ioflush()<=0)
   {
      static cc msg[]="Unable to flush GPS IO buffers"; LogEntry(FuncName,msg);
   }
   
   /* validate the function parameter 'gpsfix' */
   else if (!gpsfix)
   {
      static cc msg[]="NULL pointer to the NmeaGpsFields structure.\n";
      LogEntry(FuncName,msg);
   }

   /* make sure the timeout period is non-negative */
   else if (sec<0)
   {
      static cc msg[]="Negative timeout period not allowed.\n";
      LogEntry(FuncName,msg);
   }
   
   else 
   {
      /* implement mechanism to prevent GPS fifo overflow or clogging */
      time_t LastIoFlush;
      
      /* define a buffer into which NMEA strings are written */
      char buf[MaxNmeaStrLen+1];
      
      /* initialize the timeout period */
      T=0; To=time(NULL); dT=sec; LastIoFlush=To;

      /* initialize the NmeaGpsFields structure */
      init_nmea_struct(gpsfix); 

      /* re-initialize the function's return value */
      status=0;
      
      /* initialize the mag-switch flip-flop */
      MagSwitchReset();

      do /* read the next string from the serial port */
      {
         /* read a string from the serial port */
         if (pgets(port,buf,MaxNmeaStrLen,dT,"\r\n")>0)
         {
            /* check if string should be logged */
            if (debuglevel>=3)
            {
               /* compute the length of the string */
               int i,len=strlen(buf);

               /* timestamp the log entry */
               {static cc msg[]="Received: "; LogEntry(FuncName,msg);}

               /* write the string to the log */
               for (i=0; i<len; i++)
               {
                  if (buf[i]=='\r') LogAdd("\\r");
                  else if (buf[i]=='\n') LogAdd("\\n");
                  else if (isprint(buf[i])) LogAdd("%c",buf[i]);
                  else LogAdd("[0x%02x]",buf[i]);
               }

               /* terminate the log entry */
               LogAdd("\n");
            }

            /* screen for GGA and RMC nmea strings */
            if (!strncmp(buf,"$GPGGA",6) || !strncmp(buf,"$GPRMC",6))
            {
               /* validate and parse the NMEA strings */
               if (nmea0183(buf,gpsfix) && gpsfix->status=='A' && 
                   (gpsfix->quality==1 || gpsfix->quality==2) &&
                   gpsfix->nsat>=3) 
               {
                  status=1;
               }
            }
         }

         /* compute the time remaining */
         T=time(NULL); if (T>=0 && To>=0) dT=(sec-(time_t)difftime(T,To)); else dT=0;

         /* periodically empty the fifo */
         if (difftime(T,LastIoFlush)>120) {port->ioflush(); LastIoFlush=T;}
      }
      
      /* check termination conditions */
      while (dT>0 && status<=0 && !MagSwitchToggled());
   }

   /* compute the time required to obtain the fix */
   dT = (T>=0 && To>=0 && difftime(T,To)>0) ? (time_t)difftime(T,To) : 1;

   /* return the status on failure else return the time to obtain the fix */
   return (status<=0) ? status : dT;
}

/*------------------------------------------------------------------------*/
/* function to extract the GPS time from the GGA NMEA sentence            */
/*------------------------------------------------------------------------*/
/**
   This function extracts the GPS time from the GGA NMEA sentence.  
   
      \begin{verbatim}
      input:

         port.....A structure that contains pointers to machine dependent
                  primitive IO functions.  See the comment section of the
                  SerialPort structure for details.  The function checks to
                  be sure this pointer is not NULL.

         sec.....The maximum amount of time (measured in seconds) that this
                 function will attempt to read NMEA messages from the serial
                 port before returning to the calling function.
 
      output:

         If the GPS time was successfully determined, this function returns
         the number of seconds since the UNIX epoch (Jan 1, 1970 00:00:00).
         Otherwise a zero or negative value is returned.
      \end{verbatim}
*/
time_t GetGpsTime(const struct SerialPort *port,  time_t sec)
{
   /* define the logging signature */
   static cc FuncName[] = "GetGpsTime()";

   /* initialize the function's return value */
   int status = -1;
           
   /* initialize the timeout period */
   time_t To=time(NULL),GpsSec=0;

   /* object to get a fix */
   struct NmeaGpsFields gpsfix; init_nmea_struct(&gpsfix);
   
   /* validate the SerialPort object */
   if (!port) {static cc msg[]="NULL serial port.\n"; LogEntry(FuncName,msg);}

   /* flush IO buffers */
   else if (!port->ioflush || port->ioflush()<=0)
   {
      static cc msg[]="Unable to flush GPS IO buffers.\n"; LogEntry(FuncName,msg);
   }

   /* make sure the timeout period is non-negative */
   else if (sec<0)
   {
      static cc msg[]="Negative timeout period not allowed.\n";
      LogEntry(FuncName,msg);
   }

   /* get a GPS fix */
   else if (GetGpsFix(port,&gpsfix,sec)<=0)
   {
      static cc msg[]="Unable to get GPS fix.\n";
      LogEntry(FuncName,msg); status=0;
   }

   else
   {
      /* define a buffer into which NMEA strings are written */
      char buf[MaxNmeaStrLen+1];
      
      /* initialize the timeout period */
      time_t T=0, dT=sec;

      /* synchronize with the NMEA-packets burst */
      int i; for (port->ioflush(), i=0; i<16; i++)
      {
         /* read and discard the current NMEA string */
         if (!pgets(port,buf,MaxNmeaStrLen,3,"\r\n")) break;
      }

      /* if synchronization failed then flush the port's IO buffers */
      if (i==16) port->ioflush();
      
      /* re-initialize the function's return value */
      status=0;

      do /* read the next string from the serial port */
      {
         /* read the next GGA NMEA string from the serial port */
         if (pgets(port,buf,MaxNmeaStrLen,dT,"\r\n")>0 && !strncmp(buf,"$GPGGA",6))
         {
            if (gga(buf,&gpsfix)>0)
            {
               /* compute the decimal encoded hours and minutes */
               long int hhmm=gpsfix.hhmmss/100;

               /* define a tm structure to contain the broken down date and time */
               struct tm date={0,0,0,0,0,0,0,0,0};

               /* initialize the elements of the broken down time */
               date.tm_sec=gpsfix.hhmmss-hhmm*100;
               date.tm_hour=hhmm/100;
               date.tm_min=hhmm-date.tm_hour*100;
               date.tm_mday=gpsfix.day;
               date.tm_mon=gpsfix.mon-1;
               date.tm_year=gpsfix.year-1900;
               
               /* compute the number of seconds in the epoch */
               GpsSec=mktime(&date);
               
               /* indicate success */
               status=1; break;
            }
         }
             
         /* compute the time remaining */
         T=time(NULL); if (T>=0 && To>=0) dT=(sec-(time_t)difftime(T,To)); else dT=0;
      }
      
      /* check termination conditions */
      while (dT>0 && status<=0);
   }

   return (status<=0) ? status : GpsSec;
}

/*------------------------------------------------------------------------*/
/* function to extract the GPS fix from a NMEA GGA message                */
/*------------------------------------------------------------------------*/
/**
   This function parses a NMEA GGA message to extract the GPS fix.  The
   following description of the NMEA ZDA message format was taken from
   Appendix E of the Trimble Lassen LP GPS System Designer Reference Manual
   (Part No 39264-00, Firmware 7.82, August 1999).

      \begin{verbatim}
      E.4.1    GGA - GPS Fix Data
      
      The GGA message includes time, position and fix related data for the
      GPS receiver.
      
        \$GP GGA,hhmmss,llll.lll,a,nnnnn.nnn,b,t,uu,v.v,w.w,M,x.x,M,y.y,zzzz*hh<CR><LF>
      
      Table E-3.    GGA - GPS Fix Data Message Parameters
         
         Field    Description
             1    UTC of Position
           2,3    Latitude, N (North) or S (South)
           4,5    Longitude, E (East) or W (West)
             6    GPS Quality Indicator: 0 = No GPS, 1 = GPS, 2 = DGPS
             7    Number of Satellites in Use
             8    Horizontal Dilution of Precision (HDOP)
          9,10    Antenna Altitude in Meters, M = Meters
         11,12    Geoidal Separation in Meters, M=Meters. Geoidal separation is the
                     difference between the WGS-84 earth ellipsoid and mean-sea-level.
            13    Age of Differential GPS Data. Time in seconds since the last Type 1 or 9
                     Update
            14    Differential Reference Station ID (0000 to 1023)
            hh    Checksum
      \end{verbatim}
 
   This function initializes the day, month, and year of the gpsfix object.
   The UTC stored in the message is not used.
   
      \begin{verbatim}
      input:
         msg......The NMEA GGA message.
   
      output:
         gpsfix...The quality, and nsat elements of this object are
                  initialized using data in the GGA message.
      \end{verbatim}

   This function returns one if the GGA message was successfully parsed,
   zero otherwise.
   
   Written by Dana Swift             
*/
static int gga(const char *msg,struct NmeaGpsFields *gpsfix)
{
   /* define the logging signature */
   static cc FuncName[] = "gga()";

   int status = 0;

   /* validate the function parameter 'msg' */
   if (!msg)
   {
      static cc msg[]="NULL pointer to the NMEA message.\n";
      LogEntry(FuncName,msg);
   }

   /* validate the function parameter 'gpsfix' */
   else if (!gpsfix)
   {
      static cc msg[]="NULL pointer to the NmeaGpsFields structure.\n";
      LogEntry(FuncName,msg);
   }

   else
   {
      int i,quality,nsat;
      long int hhmmss;
      const char *p=msg;

      /* initialize the GGA elements of the gpsfix */
      gpsfix->nsat=0; gpsfix->quality=0;

      /* write the GPGGA nmea sentence to the log file */
      if (debuglevel>=3) {static cc format[]="%s\n"; LogEntry(FuncName,format,p);}//debuglevel was 2 HM
      
      /* seek and verify the GGA identifier */
      i=strcspn(p,","); if (i<=0 || strncmp(p,"$GPGGA",i)) {goto Err;} else {p+=(i+1);}

      /* seek the UTC of the GPS fix */
      i=strcspn(p,","); if (i<=0) {goto Err;}

      /* extract the encoded UTC time */
      else {hhmmss=atol(extract(p,1,i));  p+=(i+1);}

      /* seek and discard the latitude */
      i=strcspn(p,","); if (i<=0) {goto Err;} else {p+=(i+1);}

      /* seek and discard the latitude's hemisphere */
      i=strcspn(p,","); if (i<=0) {goto Err;} else {p+=(i+1);}

      /* seek and discard the longitude */
      i=strcspn(p,","); if (i<=0) {goto Err;} else {p+=(i+1);}

      /* seek and discard the longitude's hemisphere */
      i=strcspn(p,","); if (i<=0) {goto Err;} else {p+=(i+1);}
      
      /* seek the GPS quality indicator */
      i=strcspn(p,","); if (i<=0) {goto Err;} else {quality=atoi(extract(p,1,i)); p+=(i+1);}

      /* seek the number of satellites in use */
      i=strcspn(p,","); if (i<=0) {goto Err;} else {nsat=atoi(extract(p,1,i)); p+=(i+1);}

      /* initialize the GGA elements of the gpsfix object */
      gpsfix->quality=quality; gpsfix->nsat=nsat; gpsfix->hhmmss=hhmmss; 
      
      status=1;
   }
      
   Err: /* muster point if message can't be parsed */

   return status;
}
   
/*------------------------------------------------------------------------*/
/* function to initialize the NMEA data structure with zeros              */
/*------------------------------------------------------------------------*/
/**
   This function initializes the NmeaGpsFields structure with zeros using
   the ANSI memset() function.

      \begin{verbatim}
      output:
      
         gpsfix...The NmeaGpsFields object to be initialized.
      \end{verbatim}

   written by Dana Swift
*/
void init_nmea_struct(struct NmeaGpsFields *gpsfix)
{
   memset(gpsfix,0,sizeof(struct NmeaGpsFields));
}

/*------------------------------------------------------------------------*/
/* function to parse a NEMA 0183 string for GPS data                      */
/*------------------------------------------------------------------------*/
/**
   This function is designed to parse a NMEA 0183 message and extract the
   GPS data from it.  The NMEA 0183 messages that are recognized are the
   following:

   \begin{verbatim}
      GGA: GPS fix data.
      GLL: Geographic position - latitude/longitude.
      GSA: GPS DOP and active satellites.
      GSV: GPS satellites in view.
      RMC: Recommended minimum specific GPS/Transit data.
      VTG: Track made good and ground speed.
      ZDA: Time and date.
   \end{verbatim}

   Although recognized as a NMEA message, not all of these messages are
   necessarily parsed. The following description of the NMEA format was
   taken from Appendix E of the Trimble Lassen LP GPS System Designer
   Reference Manual (Part No 39264-00, Firmware 7.82, August 1999).

   \begin{verbatim}
   E.2 NMEA 0183 Message Format 
      
      The NMEA 0183 protocol covers a broad array of navigation data. This 
      broad array of information is separated into discrete messages which 
      convey a specific set of information.  The entire protocol encompasses 
      over 50 messages, but only a sub-set of these messages apply to a GPS 
      receiver like the Lassen LP GPS. The NMEA message structure is 
      described below. 
      
          \$IDMSG,D1,D2,D3,D4,.......,Dn*CS[CR][LF]  
      
          "\$"    The "\$" signifies the start of a message. 
      
           ID    The talker identification is a two letter mnemonic which 
                 describes the source of the navigation information. The GP 
                 identification signifies a GPS source. 
      
          MSG    The message identification is a three letter mnemonic which
                 describes the message content and the number and order of
                 the data fields.
      
          ","    Commas serve as delimiters for the data fields. 
      
           Dn    Each message contains multiple data fields (Dn) which are 
                 delimited by commas. 
      
           "*"   The asterisk serves as a checksum delimiter. 
      
            CS   The checksum field contains two ASCII characters which
                 indicate the hexadecimal value of the checksum.
      
      [CR][LF]   The carriage return [CR] and line feed [LF] combination 
                 terminate the message. 
      
      NMEA 0183 messages vary in length, but each message is limited to 79 
      characters or less.  This length limitation excludes the "\$" and the 
      [CR][LF]. The data field block, including delimiters, is limited to 74
      characters or less. 
   \end{verbatim}

   This function uses a regex to verify that the input buffer (msg) contains
   a well-formed NMEA message as defined by the description above.  The
   checksum is verified as additional validation.
   
      \begin{verbatim}
      input:
      
         msg......NMEA 0183 formatted ascii message.

      output:
      
         gpsfix...A structure containing data relevant to the GPS fix
                  reported in the NMEA messages.
      \end{verbatim}

      This function returns one if the NMEA message was successfully parsed,
      zero otherwise.
      
   written by Dana Swift
*/
int nmea0183(const char *msg,struct NmeaGpsFields *gpsfix)
{
   /* define the logging signature */
   static cc FuncName[] = "nmea0183()";

   int status=0;
   
   /* validate the function parameter 'msg' */
   if (!msg)
   {
      static cc msg[]="NULL pointer to the NMEA message.\n";
      LogEntry(FuncName,msg);
   } 

   /* validate the function parameter 'gpsfix' */ 
   else if (!gpsfix)
   {
      static cc msg[]="NULL pointer to the NmeaGpsFields structure.\n";
      LogEntry(FuncName,msg);
   } 

   else
   {
      #define NSUB (5)
      enum {GGA,GLL,GSA,GSV,RMC,VTG,ZDA,NMSG}; const char *nmea[NMSG];
      regex_t regex; regmatch_t regs[NSUB+1];
      
      /* get the length of the NMEA string */
      int errcode,len = strlen(msg);

      /* construct the regex pattern string for files with message locks */
      const char *pattern = "^\\$(GP([A-Z]{3})(,([^*]+)))\\*([0-9A-F]{2})[ \r\n]*$";

      /* associate NMEA strings */
      nmea[GGA]="GGA"; nmea[GLL]="GLL"; nmea[GSA]="GSA"; nmea[GSV]="GSV";
      nmea[RMC]="RMC"; nmea[VTG]="VTG"; nmea[ZDA]="ZDA";
       
      /* re-initialize function's return value */
      status=1;

      /* compile the option pattern */
      assert(!regcomp(&regex,pattern,REG_EXTENDED|REG_NEWLINE));
      
      /* protect against segfaults */
      assert(NSUB==regex.re_nsub);
 
      if (len<=MaxNmeaStrLen && !(errcode=regexec(&regex,msg,regex.re_nsub+1,regs,0)))
      {
         /* compute the length of the data segment of the string */
         int NmeaDataStrLen = regs[3].rm_eo-regs[3].rm_so;

         /* extract the checksum from the NMEA message */
         int cs1 = strtoul(extract(msg,regs[5].rm_so+1,regs[5].rm_eo-regs[5].rm_so),NULL,16);

         /* compute the NMEA checksum of the data segment of the NMEA string */
         int cs2 = nmea_checksum(msg,regs[1].rm_so,regs[1].rm_eo);
         
         /* make sure the data string (including delimiters) doesn't exceed NMEA specs */
         if (cs1==cs2 && NmeaDataStrLen<=MaxNmeaDataStrLen) 
         {
            int msgid;
         
            /* search for the message identifier in the list of known identifiers */
            for (msgid=0; msgid<NMSG; msgid++) {if (!strncmp(nmea[msgid],msg+regs[2].rm_so,3)) break;}

            switch (msgid)
            {
               /* process a RMC message */
               case RMC: {status=rmc(msg,gpsfix); break;}

               /* process a GGA message */
               case GGA: {status=gga(msg,gpsfix); break;}
            }
         }
      }

      /* check for pathological regex conditions */
      else if (errcode!=REG_NOMATCH)
      {
         /* create format spec for log entry */
         static cc format[]="Exception in regexec(): %s\n";
         
         /* map the regex error code to an error string */
         char errbuf[128]; regerror(errcode,&regex,errbuf,128);

         /* print the regex error string */
         LogEntry(FuncName,format,errbuf); 
 
         /* indicate failure of the configuration attempt */
         status=0;
      }

      /* clean up the regex pattern buffer and registers */
      regfree(&regex);
   }
   
   return status;
   #undef NSUB
}

/*------------------------------------------------------------------------*/
/* function to compute the checksum of a NMEA message                     */
/*------------------------------------------------------------------------*/
/**
   This function computes the 8-bit checksum (ie, bitwise exclusive-or) of
   a NMEA message.  Each NMEA message has the form:
   
      \begin{verbatim}
      \$IDMSG,D1,D2,D3,D4,.......,Dn*CS[CR][LF]
      \end{verbatim}

   The checksum is computed by accumulating the bit-wise exclusive-or of all
   of the bytes inside the parentheses below:
    
      \begin{verbatim}
      \$(IDMSG,D1,D2,D3,D4,.......,Dn)*CS[CR][LF]
      \end{verbatim}

   That is, the checksum incorporates all bytes (including the delimiting
   commas) between the leading '\$' and the checksum delimiter, '*'.  But
   neither the '\$' nor the '*' are included in the checksum.
      
      \begin{verbatim}
      input:
         msg.....The NMEA message.
         
         start...The index of the first byte to include in the checksum
                 calculation.
                 
         end.....The index of the byte that terminates the checksum
                 calculation.
      \end{verbatim}

   This function returns the 8-bit checksum of the NMEA message.

   written by Dana Swift
*/
unsigned char nmea_checksum(const char *msg,int start,int end)
{
   int i;
   unsigned char checksum;

   for (checksum=0,i=start; i<end && msg[i]; i++)
   {
      checksum ^= msg[i];
   }

   return checksum;
}

/*------------------------------------------------------------------------*/
/* function to parse NMEA RMC messages                                    */
/*------------------------------------------------------------------------*/
/**
   This function parses a NMEA RMC message to extract the GPS fix.  The
   following desciption of the NMEA RMC message was taken from Appendix E of
   the Trimble Lassen LP GPS System Designer Reference Manual (Part No
   39264-00, Firmware 7.82, August 1999).

      \begin{verbatim}
      E.4.5 RMC - Recommended Minimum Specific GPS/Transit Data

      The RMC message contains the time, date, position, course, and speed
      data provided by the GPS navigation receiver.  A checksum is mandatory
      for this message and the transmission interval may not exceed 2
      seconds.  All data fields must be provided unless the data is
      temporarily unavailable.  Null fields may be used when data is
      temporarily unavailable.

      $GPRMC,hhmmss.s,A,ddmm.mmmm,a,dddmm.mmmm,a,x.x,x.x,ddmmyy,x.x,a*hh<CR><LF>

      Table E.7 RMC - Recommended Minimum Specific GPS/Transit Data
                      Message Parameters

         Field    Description
             1    UTC of position fix.
             2    Status: A=Valid, V=navigation receiver warning.
           3,4    Latitude, N (North) or S (South).
           5,6    Longitude, E (East) or W (West).
             7    Speed over the ground (SOG) in knots.
             8    Track made good in degrees true.
             9    Date: dd/mm/yy
         10,11    Magnetic variation in degrees, E=East/W=West.
            hh    Checksum
      \end{verbatim}

   This function initializes the gpsfix object.
   
      \begin{verbatim}
      input:
         msg......The NMEA RMC message.
   
      output:
         gpsfix...The lat, lon, hour, hhmmss, ddmmyy, status, day, mon, and
                  year elements of this object are initialized using data in
                  the GGA message.
      \end{verbatim}

   This function returns one if the GGA message was successfully parsed,
   zero otherwise.
   
   Written by Dana Swift             
*/
static int rmc(const char *msg,struct NmeaGpsFields *gpsfix)
{
   static cc FuncName[]="rmc()";
   
   int status = 0;

   /* validate the function parameter 'msg' */
   if (!msg)
   {
      static cc msg[]="NULL pointer to the NMEA message.\n";
      LogEntry(FuncName,msg);
   }

   /* validate the function parameter 'gpsfix' */
   else if (!gpsfix)
   {
      static cc msg[]="NULL pointer to the NmeaGpsFields structure.\n";
      LogEntry(FuncName,msg);
   }

   else
   {
      #define NSUB (15)
      regex_t regex;
      regmatch_t regs[NSUB+1];
      
      /* get the length of the NMEA string */
      int errcode,len = strlen(msg);

      #define TIME   ",(([0-9]{2})([0-9]{2})([0-9]{2}))"
      #define STATUS ",(A|V)"
      #define LAT    ",([0-9]{1,2})([0-9]{2}\\.[0-9]{1,4})"
      #define LATHEM ",(N|S)"
      #define LON    ",([0-9]{1,3})([0-9]{2}\\.[0-9]{1,4})"
      #define LONHEM ",(E|W)"
      #define SPEED  ",[0-9]{3}\\.[0-9]"
      #define COURSE ",[0-9]{3}\\.[0-9]"
      #define DATE   ",(([0-9]{2})([0-9]{2})([0-9]{2}))"
      #define CRC    "\\*[0-9A-F][0-9A-F]"
      
      /* construct the regex pattern string for files with message locks */
      const char *pattern = "^\\$GPRMC" TIME STATUS LAT LATHEM LON LONHEM 
                            SPEED COURSE DATE ".*[ \r\n]*$";

      /* compile the option pattern */
      assert(!regcomp(&regex,pattern,REG_EXTENDED|REG_NEWLINE));
      
      /* protect against segfaults */
      assert(NSUB==regex.re_nsub);
      
      if (len<=MaxNmeaStrLen && !(errcode=regexec(&regex,msg,regex.re_nsub+1,regs,0)))
      {
         /* get the UTC time components */
         int hh=atoi(extract(msg, regs[ 2].rm_so+1, regs[ 2].rm_eo-regs[ 2].rm_so));
         int mm=atoi(extract(msg, regs[ 3].rm_so+1, regs[ 3].rm_eo-regs[ 3].rm_so));
         int ss=atoi(extract(msg, regs[ 4].rm_so+1, regs[ 4].rm_eo-regs[ 4].rm_so));

         /* get the latitude components */
         int   latd=atoi(extract(msg, regs[6].rm_so+1, regs[6].rm_eo-regs[6].rm_so));
         float latm=atof(extract(msg, regs[7].rm_so+1, regs[7].rm_eo-regs[7].rm_so));
         char  lath=*(msg+regs[8].rm_so);

         /* get the longitude components */
         int   lond=atoi(extract(msg, regs[ 9].rm_so+1, regs[ 9].rm_eo-regs[ 9].rm_so));
         float lonm=atof(extract(msg, regs[10].rm_so+1, regs[10].rm_eo-regs[10].rm_so));
         char  lonh=*(msg+regs[11].rm_so);

         /* get the UTC date components */
         gpsfix->day =atoi(extract(msg, regs[13].rm_so+1, regs[13].rm_eo-regs[13].rm_so));
         gpsfix->mon =atoi(extract(msg, regs[14].rm_so+1, regs[14].rm_eo-regs[14].rm_so));
         gpsfix->year=atoi(extract(msg, regs[15].rm_so+1, regs[15].rm_eo-regs[15].rm_so))+2000;

         /* compute the latitude from its components */
         gpsfix->lat = latd + latm/60; if (lath=='S') gpsfix->lat *= -1;

         /* compute the longitude from its components */
         gpsfix->lon = lond + lonm/60; if (lonh=='W') gpsfix->lon *= -1;

         /* compute the hour of the day */
         gpsfix->hour = hh + mm/60.0 + ss/3600.0;
         
         /* get the status */
         gpsfix->status= *(msg+regs[5].rm_so);
         
         /* get the UTC time formatted as HHMMSS */
         gpsfix->hhmmss = atol(extract(msg, regs[ 1].rm_so+1, regs[ 1].rm_eo-regs[ 1].rm_so));

         /* get the UTC date formmated as DDMMYY */
         gpsfix->ddmmyy = atol(extract(msg, regs[12].rm_so+1, regs[12].rm_eo-regs[12].rm_so));

         /* indicate success */
         status=1;
      }
        
      /* check for pathological regex conditions */
      else if (errcode!=REG_NOMATCH)
      {
         /* create format spec for log entry */
         static cc format[]="Exception in regexec(): %s\n";
         
         /* map the regex error code to an error string */
         char errbuf[128]; regerror(errcode,&regex,errbuf,128);

         /* print the regex error string */
         LogEntry(FuncName,format,errbuf); 

         /* initialize function parameter */
         (*gpsfix)=zeros;
 
         /* indicate failure */
         status=0;
      }
 
      /* clean up the regex pattern buffer and registers */
      regfree(&regex);
   }
   
   return status;
   #undef NSUB
}
