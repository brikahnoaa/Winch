#ifndef GARMIN_H
#define GARMIN_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: garmin.c,v 1.12 2007/04/24 01:43:29 swift Exp $
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
 * $Log: garmin.c,v $
 * Revision 1.12  2007/04/24 01:43:29  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.11  2006/12/06 15:53:37  swift
 * Modified PGRMC configuration string to account for changes in GPS15 firmware.
 *
 * Revision 1.10  2006/11/22 03:58:57  swift
 * Changed configuration of Garmin15 to use automatic mode.
 *
 * Revision 1.9  2006/02/03 00:24:16  swift
 * Rework the interactive configuration commands to improve robustness and
 * error detection.
 *
 * Revision 1.8  2006/01/31 19:28:00  swift
 * Implemented a work-around for a bug in the Garmin GPS15L-W firmware that
 * caused configuration problems.
 *
 * Revision 1.7  2006/01/06 23:15:53  swift
 * Fixed an incorrect warning message.
 *
 * Revision 1.6  2005/04/04 22:34:04  swift
 * Modifications to allow one-time configuration at beginning of mission.
 *
 * Revision 1.5  2005/02/22 21:49:16  swift
 * Added functions to enable/disable various NMEA sentences.  Added function to
 * update the almanac.
 *
 * Revision 1.4  2005/01/15 00:47:41  swift
 * Modified API for the Garmin GPS25 for use with the Garmin 15L.
 *
 * Revision 1.3  2002/12/31 16:58:29  swift
 * Eliminated the reference to the unistd.h header file.
 *
 * Revision 1.2  2002/10/08 23:33:24  swift
 * Fixed a formatting glitch in the comment section.
 *
 * Revision 1.1  2002/08/22 19:04:22  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define GarminChangeLog "$RCSfile: garmin.c,v $  $Revision: 1.12 $   $Date: 2007/04/24 01:43:29 $"

#include <serial.h>

/* function prototypes for global functions */
int    ConfigGarmin15(const struct SerialPort *port);
int    DisableNmeaSentence(const struct SerialPort *port,const char *id);
int    EnableNmeaSentence(const struct SerialPort *port,const char *id);
int    LogNmeaSentences(const struct SerialPort *port);
time_t UpLoadGpsAlmanac(const struct SerialPort *port);

#endif /* GARMIN_H */

#include <assert.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <logger.h>
#include <nmea.h>
#include <unistd.h>

#ifdef _XA_
   #include <apf9.h>
   #include <apf9icom.h>
#endif /* _XA_ */

/* function prototypes for local functions */
static int pgrmc(const struct SerialPort *port);
static int pgrmc1(const struct SerialPort *port);
static int pgrmo(const struct SerialPort *port,const char *msg,int mode);

/*------------------------------------------------------------------------*/
/* function to configure the Garmin GPS15 gps engine                      */
/*------------------------------------------------------------------------*/
/**
   This function configures the Garmin GPS15 OEM gps engine.  It does so by
   sending the PGRMC, PGRMC1, and PGRMO configuration strings to the gps
   engine.  For details refer to the technical specification: Garmin GPS 15H
   \& 15L Technical Specifications (Document No. 190-00266-00, Rev. B, Apr
   2004).
  
      \begin{verbatim}
      input:

         port.......A structure that contains pointers to machine dependent
                    primitive IO functions.  See the comment section of the
                    SerialPort structure for details.  The function checks
                    to be sure this pointer is not NULL.
 
      output:

         This function returns a positive value if successful, zero indicates
         failure, and -1 indicates an invalid function argument.
      \end{verbatim}

   written by Dana Swift
*/
int ConfigGarmin15(const struct SerialPort *port)
{
   /* define the logging signature */
   static cc FuncName[] = "ConfigGarmin15()";

   /* initialize this function's return value */
   int i,status=-1;
 
   /* validate the SerialPort object */
   if (!port) {static cc msg[]="NULL serial port.\n"; LogEntry(FuncName,msg);}

   /* flush IO buffers */
   else if (!port->ioflush || port->ioflush()<=0)
   {
      static cc msg[]="Unable to flush GPS IO buffers"; LogEntry(FuncName,msg);
   }

   /* configure the Garmin GPS15 using the PGRMC and PGRMC1 messages */
   else
   {
      /* make a log entry if the verbosity is high enough */
      if (debuglevel>=2)
      {
         static cc msg[]="Garmin GPS15 configuration initiated.\n";
         LogEntry(FuncName,msg);
      }

      for (status=0,i=0; i<3; i++)
      {
         /* disable all output */
         if (pgrmo(port,"",2)<=0)
         {
            static cc msg[]="Attempt to disable all NMEA sentences failed.\n";
            LogEntry(FuncName,msg);
         }
         else
         {
            /* pause for a second and then flush the IO queues */
            Wait(5000); port->ioflush(); 

            if (!GpsFifoLen()) {status=1; break;}
            else
            {
               static cc msg[]="Attempt to suspend output failed-retrying.\n";
               LogEntry(FuncName,msg);
            }
         }
      }
      
      /* configure the fix mode and baud rate */
      if (pgrmc(port)<=0)
      {
         static cc msg[]="Configuration of fix-mode and baud rate failed.\n";
         LogEntry(FuncName,msg);

         /* indicate failure */
         status=0;
      }

      /* configure the NMEA output frequency */
      else if (pgrmc1(port)<=0)
      {
         static cc msg[]="Configuration of NMEA output frequency failed.\n";
         LogEntry(FuncName,msg);

         /* indicate failure */
         status=0;
      }

      /* enable default NMEA messages */
      else if (pgrmo(port,"",4)<=0)
      {
         static cc msg[]="Attempt to enable default NMEA sentences failed.\n";
         LogEntry(FuncName,msg);

         /* indicate failure */
         status=0;
      }

      /* enable NMEA GGA sentence */
      else if (pgrmo(port,"GPGGA",1)<=0)
      {
         static cc msg[]="Attempt to enable NMEA GGA sentence failed.\n";
         LogEntry(FuncName,msg);

         /* indicate failure */
         status=0;
      }

      /* enable NMEA RMC sentence */
      else if (pgrmo(port,"GPRMC",1)<=0)
      {
         static cc msg[]="Attempt to enable NMEA RMC sentence failed.\n";
         LogEntry(FuncName,msg);

         /* indicate failure */
         status=0;
      }

      /* enable NMEA RMT sentence */
      else if (pgrmo(port,"PGRMT",1)<=0)
      {
         static cc msg[]="Attempt to enable PGRMT sentence failed.\n";
         LogEntry(FuncName,msg);

         /* indicate failure */
         status=0;
      }

      /* indicate success */
      else {status=1;} 
   }
   
   /* make a log entry if the configuration failed */
   if (status<=0)
   {
      static cc msg[]="Garmin GPS15 configuration failed.\n";
      LogEntry(FuncName,msg);
   }

   /* make a log entry if verbosity high enough */
   else if (debuglevel>=2)
   {
      static cc msg[]="Garmin GPS15 successfully configured.\n";
      LogEntry(FuncName,msg); 
   } 
   
   return status;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
int DisableNmeaSentence(const struct SerialPort *port,const char *id)
{
   return pgrmo(port,id,0);
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
int EnableNmeaSentence(const struct SerialPort *port,const char *id)
{
   return pgrmo(port,id,1);
}

/*------------------------------------------------------------------------*/
/* function to log NEMA sentences output from the Garmin GPS15            */
/*------------------------------------------------------------------------*/
/**
   This function was created as a way of providing information that could be
   used to monitor the health and efficacy of the antenna and GPS engine.
   It receives NMEA sentences from the GPS engine and writes them to the log
   file.  For more information on the NMEA sentences, refer to the Garmin
   documentation: Garmin GPS 15H \& 15L Technical Specifications (Document
   No. 190-00266-00, Rev. B, Apr 2004)
  
      \begin{verbatim}
      input:

         port.......A structure that contains pointers to machine dependent
                    primitive IO functions.  See the comment section of the
                    SerialPort structure for details.  The function checks
                    to be sure this pointer is not NULL.
 
      output:

         This function returns a positive value if successful, zero indicates
         failure, and -1 indicates an invalid function argument.
      \end{verbatim}

   written by Dana Swift
*/
int LogNmeaSentences(const struct SerialPort *port)
{
   /* define the logging signature */
   static cc FuncName[] = "LogNmeaSentences()";

   /* initialize this function's return value */
   int status=-1;
 
   /* validate the SerialPort object */
   if (!port) {static cc msg[]="NULL serial port.\n"; LogEntry(FuncName,msg);}

   /* flush IO buffers */
   else if (!port->ioflush || port->ioflush()<=0)
   {
      static cc msg[]="Unable to flush GPS IO buffers.\n"; LogEntry(FuncName,msg);
   }

   else
   {
      /* set the time-out periods for receiving a NMEA sentence from GPS engine */
      const time_t TimeOut=12, MinWatchTime=30, MaxWatchTime=90;

         /* define objects to store the reference time and watch-time */
      time_t To; float dT; int n;

      /* define a buffer to hold the NMEA strings read from the GPS engine */
      char buf[MaxNmeaStrLen+1];
      
      /* reinitialize the function's return value */
      status=0; 
      
      /* flush the IO port */
      port->ioflush();

      /* initialize watch-time variables and monitor fail-safe exit criteria */
      for (n=0, To=time(NULL), dT=0; dT<=MaxWatchTime && n<65; n++)
      {
         /* read a string from the serial port */
         if (pgets(port,buf,MaxNmeaStrLen,TimeOut,"\r\n")>0)
         {
            /* compute the length of the string */
            int i,len=strlen(buf);

               /* timestamp the log entry */
            {static cc format[]="%s"; LogEntry(FuncName,format,"");}

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
         
         /* check if the Rx buffer is in danger of overflowing */
         if (GpsFifoLen()>0.9*GpsFifoSize())
         {
            /* empty the buffer and resync to the output */
            port->iflush(); pgets(port,buf,MaxNmeaStrLen,TimeOut,"\r\n");
         }
         
         /* compute the length of the logging period so far */
         dT=difftime(time(NULL),To);

         /* check if the sensor status sentence was received */
         if (!strncmp(buf,"$PGRMT",6)) {status=1;}

         /* check exit criteria */
         if (status && dT>MinWatchTime) break;
      }
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to configure the Garmin GPS15 gps engine                      */
/*------------------------------------------------------------------------*/
/**
   This function configures the fix mode, altitude, earth-datum,
   differential mode, baud rate, velocity filter, PPS mode, and
   dead-reckoning time for the Garmin GPS15 gps engine.  The following
   information was taken from page 18 of the technical specification: Garmin
   GPS 15H \& 15L Technical Specifications (Document No. 190-00266-00,
   Rev. B, Apr 2004)

      \begin{verbatim}
      $PGRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<13>,<13>,<14>*HH\r\n

       <1> Fix mode, A=Automatic, 2=2D exclusively (host system must supply
              altitude).
       <2> Altitude above/below mean sea level, -1500.0 to 18000.0 meters.
       <3> Earth datim index.  Refer to technical specification for details.
       <4> User earth datum semi-mafor axis.  Refer to technical
              specification for details.
       <5> User earth datum inverse flattening factor.  Refer to technical
              specification for details.
       <6> User earth datum delta x earth centered coordinate.    
       <7> User earth datum delta y earth centered coordinate.    
       <8> User earth datum delta z earth centered coordinate.    
       <9> Differential mode, A=Automatic (output DGPS data when available,
              non-DGPS otherwise), D=differential exclusively (output only
              differential fixes).
      <10> NMEA baud rate, 1=1200, 2=2400, 3=4800, 4=9600, 5=19200, 6=300,
              7=600. 
      <11> Velocity filter, 0 = no filter, 1 = automatic filter, 2-255 =
              filter time constant (10 = 10 second filter).
      <12> PPS mode, 1=No PPS, 2 = 1Hz.
      <13> PPS pulse lenth, 0-48 = (n+1)*20msec. Example n=4 => 100 msec
              pulse.
      <14> Dead reckoning valid time 1-30 seconds.

        HH Hexidecimal checksum.
      \end{verbatim}

   The manual indicates that the <CR><LF> termination is required.
   Experimentation shows that this is untrue if the optional checksum is
   included.  In fact, including the <CR><LF> appears to trigger a firmware
   bug in the Garmin GPS15L-W.  The implementation herein works around this
   bug by not including the <CR><LF> termination.

   This function does not implement a parameterized configuration ability.
   The configuration string is hardwired into the source code.  Note also
   that the configuration changes are not effective until the next power-up
   cycle or external reset of the gps engine.
   
      \begin{verbatim}
      input:

         port.......A structure that contains pointers to machine dependent
                    primitive IO functions.  See the comment section of the
                    SerialPort structure for details.  The function checks
                    to be sure this pointer is not NULL.
 
      output:

         This function returns a positive value if successful, zero indicates
         failure, and -1 indicates an invalid function argument.
      \end{verbatim}

   written by Dana Swift
*/
static int pgrmc(const struct SerialPort *port)
{
   /* define the logging signature */
   static cc FuncName[] = "pgrmc()";

   /* initialize this function's return value */
   int status=-1;
 
   /* validate the SerialPort object */
   if (!port) {static cc msg[]="NULL serial port.\n"; LogEntry(FuncName,msg);}

   /* flush IO buffers */
   else if (!port->ioflush || port->ioflush()<=0)
   {
      static cc msg[]="Unable to flush GPS IO buffers.\n"; LogEntry(FuncName,msg);
   }

   else
   {
      /* specify the timeout period (seconds) for serial puts and gets */
      const time_t TimeOut=5; 
      
      /* specify the configuration command */
      const char *cmd = "PGRMC,A,,,,,,,,A,3,1,2,4,30";

      /* compute the NMEA checksum of the configuration command */
      unsigned char chksum = nmea_checksum(cmd,0,strlen(cmd));

      /* create the NMEA configuration command */
      char str[MaxNmeaStrLen+1],buf[MaxNmeaStrLen+1]; sprintf(buf,"$%s*%02X",cmd,chksum);

      /* reinitialize return value */
      status=0;

      /* make a log entry about writing the config string to the GPS engine */
      if (debuglevel>=3)
      {
         static cc format[]="Configuring Garmin GPS engine: %s\n";
         LogEntry(FuncName,format,buf);
      }
            
      /* flush the IO port */
      port->ioflush();

      /* write the configuration command to the Garmin GPS engine */
      if (pputs(port,buf,TimeOut,"")<=0) 
      {
         static cc format[]="Attempt to write configuration string failed: %s\n";
         LogEntry(FuncName,format,buf);
      } 

      /* read the configuration echo */
      else
      {
         /* set the reference for the timeout period */
         time_t T0=time(NULL);

         /* read strings from the GPS engine until the PGRMC message is found */
         do
         {
            /* read a string from the GPS serial port */
            pgets(port,str,sizeof(str)-1,5,"\r\n");
            
            /* check if current string is the PGRMC message */
            if (strstr(str,cmd)) status=1;

            if (debuglevel>=3)
            {
               static cc format[]="%s"; LogEntry(FuncName,format,"");
               LogAdd("%s %s\n",((status)?"Accept:":"Reject:"),str);
            }
         }

         /* keep reading strings until PGRMC message found or timeout expires */
         while (status<=0 && difftime(time(NULL),T0)<TimeOut);

         /* if status is false then the config attempt failed */
         if (!status)
         {
            static cc msg[]="Attempt to configure Garmin GPS15 failed.\n";  
            LogEntry(FuncName,msg);
         }
         
         /* make a log entry if the verbosity is high enough */
         else if (debuglevel>=3)
         {
            static cc format[]="Garmin GPS15 configured: %s\n";
            LogEntry(FuncName,format,str);
         }
      }
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to configure the Garmin GPS15 gps engine                      */
/*------------------------------------------------------------------------*/
/**
   This function configures the NMEA output rate, binary phase output mode,
   position pinning, DGPS config parameters, and NMEA mode.  The following
   information was taken from page 18 of the technical specification: Garmin
   GPS 15H \& 15L Technical Specifications (Document No. 190-00266-00,
   Rev. B, Apr 2004).

      \begin{verbatim}
      $PGRMC1,<1>,<2>,<3>,<4>,<5>,<6>,<7>*HH\r\n

       <1> NMEA output time: 1-900 seconds.
       <2> Binary phase output data, 1=off, 2=on.
       <3> Position pinning, 1=off, 2=on.
       <4> DGPS beacon frequency. Refer to technical specification for
              details.
       <5> DGPS beacon rate. Refer to technical specification for details.
       <6> DGPS beacon auto tune on station loss, 1=off, 2=on.
       <7> Activate NMEA 2.30 mode indicator, 1=off, 2=on.

        HH Hexidecimal checksum.
      \end{verbatim}

   The manual indicates that the <CR><LF> termination is required.
   Experimentation shows that this is untrue if the optional checksum is
   included.  In fact, including the <CR><LF> appears to trigger a firmware
   bug in the Garmin GPS15L-W.  The implementation herein works around this
   bug by not including the <CR><LF> termination.

   This function does not implement a parameterized configuration ability.
   The configuration string is hardwired into the source code.
   
      \begin{verbatim}
      input:

         port.......A structure that contains pointers to machine dependent
                    primitive IO functions.  See the comment section of the
                    SerialPort structure for details.  The function checks
                    to be sure this pointer is not NULL.
 
      output:

         This function returns a positive value if successful, zero indicates
         failure, and -1 indicates an invalid function argument.
      \end{verbatim}

   written by Dana Swift
*/
static int pgrmc1(const struct SerialPort *port)
{
   /* define the logging signature */
   static cc FuncName[] = "pgrmc1()";

   /* initialize this function's return value */
   int status=-1;
 
   /* validate the SerialPort object */
   if (!port) {static cc msg[]="NULL serial port.\n"; LogEntry(FuncName,msg);}

   /* flush IO buffers */
   else if (!port->ioflush || port->ioflush()<=0)
   {
      static cc msg[]="Unable to flush GPS IO buffers.\n"; LogEntry(FuncName,msg);
   }

   else
   {
      int len; char str[MaxNmeaStrLen+1],buf[MaxNmeaStrLen+1]; 
      
      /* specify the timeout period (seconds) for serial puts and gets */
      const time_t TimeOut=10; 
      
      /* specify the configuration command */
      const char *cmd = "PGRMC1,10,1,1,0.0,0,1,1,N,N";

      /* compute the NMEA checksum of the configuration command */
      unsigned char chksum = nmea_checksum(cmd,0,strlen(cmd));

      /* create the configuration command */
      sprintf(buf,"$%s*%02X",cmd,chksum); len=strlen(buf);

      /* reinitialize return value */
      status=0;

      /* make a log entry about writing the config string to the GPS engine */
      if (debuglevel>=3)
      {
         static cc format[]="Configuring Garmin GPS engine: %s\n";
         LogEntry(FuncName,format,buf);
      }
            
      /* flush the IO port */
      port->ioflush();

      /* write the configuration command to the Garmin GPS engine */
      if (pputs(port,buf,TimeOut,"")<=0) 
      { 
         static cc format[]="Attempt to write configuration string failed: %s\n";
         LogEntry(FuncName,format,buf);
      } 

      /* read the configuration echo */
      else
      {
         /* set the reference for the timeout period */
         time_t T0=time(NULL);

         /* read strings from the GPS engine until the PGRMC message is found */
         do
         {
            /* read a string from the GPS serial port */
            pgets(port,str,sizeof(str)-1,2,"\r\n");

            /* check if current string is the PGRMC message */
            if (strstr(str,cmd)) status=1;

            if (debuglevel>=3)
            {
               static cc format[]="%s"; LogEntry(FuncName,format,"");
               LogAdd("%s %s\n",((status)?"Accept:":"Reject:"),str);
            }
         }

         /* keep reading strings until PGRMC message found or timeout expires */
         while (status<=0 && difftime(time(NULL),T0)<TimeOut);

         /* if status is false then the config attempt failed */
         if (!status)
         {
            static cc msg[]="Attempt to configure Garmin GPS15 failed.\n";
            LogEntry(FuncName,msg);
         }
         
         /* make a log entry if the verbosity is high enough */
         else if (debuglevel>=3)
         {
            static cc msg[]="Garmin GPS15 configured: %s\n";
            LogEntry(FuncName,msg,str);
         }
      }
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to configure the Garmin GPS15 gps engine                      */
/*------------------------------------------------------------------------*/
/**
   This function configures which NMEA and Garmin sentences are enabled or
   disabled.  The following information was taken from page 18 of the
   technical specification: Garmin GPS 15H \& 15L Technical Specifications
   (Document No. 190-00266-00, Rev. B, Apr 2004).

      \begin{verbatim}
      $PGRMO,<1>,<2>*HH\r\n

       <1> Target sentence description (eg., PGRMT, GPGSV, etc).
       <2> Target sentence mode:
              0 = disable specific sentence.
              1 = enable specific sentence.
              2 = disable all output sentences.
              3 = enable all output sentences (except GPALM).
              
        HH Hexidecimal checksum.

        The following notes apply to the PGRMO input sentence:
           1) If the target sentence mode is '2' (disable all) or '3'
              (enable all), the target sentence description is not checked
              for validity.  In this case, an empty field is allowed (eg,
              $PGRMO,,3), or the descrition field may contain from 1 to 5
              characters.

           2) If the target sentence is '0' (disable) or '1' (enable), the
              target sentence description field must be an identifier for
              one of the sentences being output by the GPS15LP.

           3) If either the target sentence mode field or the target
              sentence description field is not valid, the PGRMO sentence
              will have no effect.

           4) $PGRMO,GPALM,1 will cause the sensor board to transmit all
              stored almanac information.  All other NMEA sentence
              transmission will be temporarily suspended.
      \end{verbatim}

   The manual indicates that the <CR><LF> termination is required.
   Experimentation shows that this is untrue if the optional checksum is
   included.  In fact, including the <CR><LF> appears to trigger a firmware
   bug in the Garmin GPS15L-W.  The implementation herein works around this
   bug by not including the <CR><LF> termination.

   This function implements a parameterized configuration ability.
   
      \begin{verbatim}
      input:

         port.......A structure that contains pointers to machine dependent
                    primitive IO functions.  See the comment section of the
                    SerialPort structure for details.  The function checks
                    to be sure this pointer is not NULL.
 
      output:

         This function returns a positive value if successful, zero indicates
         failure, and -1 indicates an invalid function argument.
      \end{verbatim}

   written by Dana Swift
*/
static int pgrmo(const struct SerialPort *port,const char *msg,int mode)
{
   /* define the logging signature */
   static cc FuncName[] = "pgrmo()";

   /* initialize this function's return value */
   int status=-1;
 
   /* validate the SerialPort object */
   if (!port)
   {
      static cc msg[]="NULL serial port.\n";
      LogEntry(FuncName,msg);
   }

   /* flush IO buffers */
   else if (!port->ioflush || port->ioflush()<=0)
   {
      static cc msg[]="Unable to flush GPS IO buffers.\n"; LogEntry(FuncName,msg);
   }

   else if (!msg)
   {
      static cc msg[]="NULL message identifier.\n";
      LogEntry(FuncName,msg);
   }
   
   else if (strlen(msg)>5)
   {
      static cc format[]="Invalid message identifier: %s\n";
      LogEntry(FuncName,format,msg);
   }

   else
   {
      unsigned char chksum;
      char str[MaxNmeaStrLen+1],cmd[MaxNmeaStrLen+1],buf[MaxNmeaStrLen+1];

      /* specify the timeout period (seconds) for serial puts and gets */
      const time_t TimeOut=10; 
     
      /* reinitialize return value */
      status=0;

      /* construct the configuration command */
      snprintf(cmd,sizeof(cmd),"PGRMO,%s,%d",msg,mode);
 
      /* compute the NMEA checksum of the configuration command */
      chksum = nmea_checksum(cmd,0,strlen(cmd));
      
      /* complete the configuration command */
      snprintf(buf,sizeof(buf),"$%s*%02X",cmd,chksum);
      
      /* make a log entry about writing the config string to the GPS engine */
      if (debuglevel>=3)
      {
         static cc format[]="Configuring Garmin GPS engine: %s\n";
         LogEntry(FuncName,format,buf);
      }
      
      /* flush the IO port */
      port->ioflush();
         
      /* write the configuration command to the Garmin GPS engine */
      if (pputs(port,buf,2,"")<=0) 
      { 
         static cc format[]="Attempt to write configuration string failed: %s\n";
         LogEntry(FuncName,format,buf);
      }
      
      /* read the configuration echo */
      else
      {
         /* set the reference for the timeout period */
         time_t T0=time(NULL);
         
         /* read strings from the GPS engine until the PGRMO message is found */
         do
         {
            /* read a string from the GPS serial port */
            pgets(port,str,sizeof(str)-1,TimeOut,"\r\n");
            
            /* check if current string is the PGRMO message */
            if (strstr(str,cmd)) status=1;

            if (debuglevel>=3)
            {
               static cc format[]="%s"; LogEntry(FuncName,format,"");
               LogAdd("%s %s\n",((status)?"Accept:":"Reject:"),str);
            }
         }

         /* keep reading strings until PGRMC message found or timeout expires */
         while (status<=0 && difftime(time(NULL),T0)<TimeOut);

         /* if status is false then the config attempt failed */
         if (!status)
         {
            static cc msg[]="Attempt to configure Garmin GPS15 failed.\n";
            LogEntry(FuncName,msg);
         }
         
         /* make a log entry if the verbosity is high enough */
         else if (debuglevel>=3)
         {
            static cc format[]="Garmin GPS15 configured: %s\n";
            LogEntry(FuncName,format,str);
         }
      }
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to upload the almanac                                         */
/*------------------------------------------------------------------------*/
/**
   This function allows time for the almanac to be uploaded into the GPS.
   It does nothing except pause for 15 minutes while the almanac is
   uploaded.  If successful, this function returns the number of seconds
   spent waiting for the upload.  Otherwise, this function returns a zero or
   negative value.
*/
time_t UpLoadGpsAlmanac(const struct SerialPort *port)
{
   /* define the logging signature */
   static cc FuncName[] = "UpLoadGpsAlmanac()";

   /* initialize the return value */
   time_t AwakeTime=-1;
   
   /* validate the SerialPort object */
   if (!port) {static cc msg[]="NULL serial port.\n"; LogEntry(FuncName,msg);}

   /* flush IO buffers */
   else if (!port->ioflush || port->ioflush()<=0)
   {
      static cc msg[]="Unable to flush GPS IO buffers.\n"; LogEntry(FuncName,msg);
   }
   
   else
   {
      /* record the starting time */
      time_t To=time(NULL);

      /* set the length of the pause-period */
      AwakeTime=900;
      
      if (debuglevel>=2)
      {
         static cc format[]="Pausing for %ld minutes to upload GPS almanac.\n";
         LogEntry(FuncName,format,AwakeTime/60);
      }

      /* initialize the mag-switch flip-flop */
      MagSwitchReset();

      do
      {
         /* check if NMEA strings are to be logged */
         if (debuglevel>=3)
         {
            /* define a buffer to hold the NMEA strings read from the GPS engine */
            char buf[MaxNmeaStrLen+1];

            /* read the next string from the port buffer */
            if (pgets(port,buf,sizeof(buf)-1,12,"\r\n")>0)
            {
               static cc format[]="%s\n"; LogEntry(FuncName,format,buf);
            }

            /* check if buffer overflow is imminent */
            if (GpsFifoLen()>(0.9*GpsFifoSize()))
            {
               /* flush and re-sync the buffer */
               port->ioflush(); pgets(port,buf,sizeof(buf),2,"\r\n");
            }
         }

         /* logging disabled so pause for a second before continuing */
         else sleep(1);
      }
      while (difftime(time(NULL),To)<AwakeTime && !MagSwitchToggled());

      if (debuglevel>=2)
      {
         static cc msg[]="Pause complete.\n"; LogEntry(FuncName,msg);
      }

      /* flush the GPS IO buffers */
      port->ioflush();
   }
   
   return AwakeTime;
}
