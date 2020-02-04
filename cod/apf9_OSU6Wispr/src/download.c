#ifndef DOWNLOAD_H
#define DOWNLOAD_H (0x0100U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: download.c,v 1.7 2007/05/08 18:10:39 swift Exp $
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
 * $Log: download.c,v $
 * Revision 1.7  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.6  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.5  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.4  2005/06/27 15:15:37  swift
 * Added an additional bit of modem control if the download failed.
 *
 * Revision 1.3  2005/06/14 19:02:09  swift
 * Changed the return value of the download manager.
 *
 * Revision 1.2  2005/02/22 21:04:39  swift
 * Remove the temporary mission config file.
 *
 * Revision 1.1  2004/12/29 23:11:27  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define downloadChangeLog "$RCSfile: download.c,v $ $Revision: 1.7 $ $Date: 2007/05/08 18:10:39 $"
 
#include <serial.h>

int DownLoadMissionCfg(const struct SerialPort *modem,int NRetries);

#endif /* DOWNLOAD_H */

#include <logger.h>
#include <unistd.h>
#include <clogin.h>
#include <chat.h>
#include <rx.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <config.h>

#ifdef _XA_
   #include <apf9.h>
   #include <apf9icom.h>
#else
   #define WatchDog()
   #define StackOk() 1
#endif /* _XA_ */

/* prototypes for functions with static linkage */
static int RxConfig(const struct SerialPort *modem);

/* define the command to download via xmodem */
static const char *sxcmd = "sx -a ";

/*------------------------------------------------------------------------*/
/* function to download the mission configuration from the remote host    */
/*------------------------------------------------------------------------*/
/**
   This function is designed to download the mission configuration file from
   the remote host.  The strategy for dealing with poor or broken
   connections with the remote host is to allow for multiple retries of the
   download session.  Breaks in the connection are automatically detected,
   the connection re-established, and the download reinitiated.  Attempts to
   download the configuration file from the remote host continue until the
   download is successful or else the number of retries is exhausted.

      \begin{verbatim}
      input:
         modem......A structure that contains pointers to machine dependent
                    primitive IO functions.  See the comment section of the
                    SerialPort structure for details.  The function checks to
                    be sure this pointer is not NULL.

      output:
         If the configuration file was successfully downloaded, this
         function returns a positive number indicating the attempts required
         to download the configuration file.  Zero is returned if the
         configuration file was not downloaded.  A negative return value
         indicates either an invalid serial port or unimplemented modem
         carrier-detect functionality.
      \end{verbatim}
*/
int DownLoadMissionCfg(const struct SerialPort *modem,int NRetries)
{
   /* function name for log entries */
   static cc FuncName[] = "DownLoadMissionCfg()";
   
   int i,status=-1;
               
   /* pet the watch dog */
   WatchDog(); 

   /* validate the port */
   if (!modem)
   {
      /* create the message */
      static cc msg[]="NULL serial port.\n";

      /* make the logentry */
      LogEntry(FuncName,msg);
   }
   
   /* loop to download mission configuration from remote host */
   else for (status=0, i=1; i<=NRetries; i++)
   {
      /* make sure a login session exists before the xfer */
      if (CLogin(modem)>0)
      {
         /* download the configuration file from the host computer */            
         if ((status=RxConfig(modem))>0) break;

         /* make log entry that the existing configuration is retained */
         else
         {
            /* create the message */
            static cc format[]="Keeping previous version of \"%s\".\n";

            /* make the logentry */
            LogEntry(FuncName,format,config_path); 
         }
      }

      /* make log entry that the mission configuration wasn't downloaded */
      else
      {
         /* create the message */
         static cc msg[]="Connect/login failed.  Mission "
            "configuration not downloaded from remote host.\n";

         /* make the logentry */
         LogEntry(FuncName,msg);

         /* hang up the modem */
         ModemDtrClear(); if (i<NRetries) sleep(5);
      }
   }

   if (status==0) 
   {
      /* create the message */
      static cc format[]="Aborting download: Retry limit [%u] exceeded.\n";

      /* make the logentry */
      LogEntry(FuncName,format,NRetries);
   }
   
      
   return (status>0) ? i : status;
}

/*------------------------------------------------------------------------*/
/* function to download the mission configuration file from the host      */
/*------------------------------------------------------------------------*/
/**
   This function downloads the mission configuration file from the remote
   host.
  
      \begin{verbatim}
      output:
         This function returns a positive value if successful, zero if it
         fails, and a negative value if exceptions were detected in the
         function's arguments.
      \end{verbatim}
*/
static int RxConfig(const struct SerialPort *modem)
{
   /* function name for log entries */
   static cc FuncName[] = "RxConfig()";

   int i,status=-1;

   /* set the number of retries */
   const int NRetries=2;
            
   /* pet the watch dog */
   WatchDog(); 

   /* validate the port */
   if (!modem)
   {
      /* create the message */
      static cc msg[]="NULL serial port.\n";

      /* make the logentry */
      LogEntry(FuncName,msg);
   }
   
   /* check if carrier-dectect enabled and CD line not asserted */
   else if (modem->cd && !modem->cd())
   {
      /* create the message */
      static cc msg[]="No carrier detected.\n";

      /* make the logentry */
      LogEntry(FuncName,msg);

      /* indicate failure */
      status=0;
   }

   /* make three attempts to download the mission configuration file */
   else for (status=0,i=1; i<=NRetries && status<=0; i++)
   {
      /* define a temporary buffer to hold filenames and commands */
      char cmd[64];

      /* define the command timeout period */
      const time_t timeout=30;
      
      /* create a temporary file name to download the config file */
      const char *tmp="RxConfig.tmp";
      
      /* define a FILE pointer for downloading the config file */
      FILE *dest=NULL;
         
      /* pet the watch dog */
      WatchDog(); 

      /* check if carrier-dectect enabled and CD line not asserted */
      if (modem->cd && !modem->cd())
      {
         /* create the message */
         static cc msg[]="No carrier detected.\n";

         /* make the logentry */
         LogEntry(FuncName,msg);

         break;
      }
      
      /* open a temporary file for writing */
      if (!(dest=fopen(tmp,"w"))) 
      {
         /* create the message */
         static cc format[]="Unable to open \"%s\" for writing [errno:%d].\n";

         /* make the logentry */
         LogEntry(FuncName,format,tmp,errno);
      }
      
      else 
      {
         /* turn off the binary mode of the xmodem transfer */
         BinMode=0;

         /* create the host's command to upload the config file */
         snprintf(cmd,sizeof(cmd)-1,"%s %s",sxcmd,config_path);

         /* execute the command on the host */
         if ((status=chat(modem,cmd,cmd,timeout,"\n"))>0)
         {
            /* log the download attempt */
            if (debuglevel>=2 || (debugbits&DOWNLOAD_H))
            {
               /* create the message */
               static cc format[]="Downloading \"%s\" from host.\n";

               /* make the logentry */
               LogEntry(FuncName,format,config_path);
            }
            
            /* delay to allow the remote host time to set up the transfer */
            sleep(2); if (modem->iflush) modem->iflush();
            
            /* download the config file to the temporary file */
            if ((status=Rx(modem,dest))<=0)
            {
               /* create the message */
               static cc msg[]="Download failed.\n";

               /* make the logentry */
               LogEntry(FuncName,msg);
            }
            
            /* make a log entry that the download was successful */
            else if (debuglevel>=2 || (debugbits&DOWNLOAD_H))
            {
               /* create the message */
               static cc msg[]="Download successful.\n";

               /* make the logentry */
               LogEntry(FuncName,msg);
            }
         }

         /* make a log entry that the chat session failed */
         else
         {
            /* create the message */
            static cc format[]="Failed attempt to execute \"%s\" on the host.\n";

            /* make the logentry */
            LogEntry(FuncName,format,cmd);
         }
   
         /* close the temporary file */
         fclose(dest);

         /* if the transfer was successful then rename it to the config pathname */
         if (status>0)
         {
            /* delete the config file prior to renaming the temporary file */
            if (remove(config_path)<0)
            {
               /* create the message */
               static cc format[]="Attempt to delete \"%s\" failed [errno=%d].\n";

               /* make the logentry */
               LogEntry(FuncName,format,config_path,errno);
            }

            /* rename the temporary file to the config pathname */
            if (rename(tmp,config_path)<0)
            {
               /* create the message */
               static cc format[]="Attempt to rename \"%s\" file to \"%s\" failed [errno=%d].\n";

               /* make a log entry that the attempt to move the temporary file failed */
               LogEntry(FuncName,format,tmp,config_path,errno);

               /* indicate failure */
               status=0;
            }
         }
      }

      /* remove the temporary file if it exists */
      remove(tmp);
   }
   
   return status;
}
