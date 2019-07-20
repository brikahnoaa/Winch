#ifndef CLOGIN_H
#define CLOGIN_H (0x0100U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: clogin.c,v 1.7 2007/05/08 18:10:39 swift Exp $
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
 * $Log: clogin.c,v $
 * Revision 1.7  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.6  2006/10/11 20:57:10  swift
 * Added a toggle of the DTR signal just before each connection attempt.
 *
 * Revision 1.5  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.4  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.3  2005/03/30 20:25:22  swift
 * Fixed a typo.
 *
 * Revision 1.2  2005/01/06 00:52:06  swift
 * Fixed problems with DTR control of modem.
 *
 * Revision 1.1  2004/12/29 23:11:26  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define cloginChangeLog "$RCSfile: clogin.c,v $ $Revision: 1.7 $ $Date: 2007/05/08 18:10:39 $"

#include <serial.h>

int CLogin(const struct SerialPort *modem);

#endif /* CLOGIN_H */

#include <logger.h>
#include <modem.h>
#include <config.h>
#include <stdlib.h>
#include <control.h>
#include <login.h>

#ifdef _XA_
   #include <apf9.h>
   #include <apf9icom.h>
#else
   #define WatchDog()
   #define StackOk() 1
#endif /* _XA_ */

/* store engineering data in persistent far ram */
extern persistent far struct EngineeringData vitals;

/*------------------------------------------------------------------------*/
/* function to establish a connection/login session with a remote host    */
/*------------------------------------------------------------------------*/
/**
   This function checks for a current connection/login session with a remote
   host and establishes a new one if one does not exist.  This function
   relies on correct operation of the modem's carrier detect (CD) signal.
   If the CD signal is asserted then a current connection is indicated and
   the function immediately returns.  If the CD signal is not asserted then
   this function establishes a session by first connecting to the remote
   host's modem and then logging into the remote host.  If successful, the
   net result will be an active communications session with the remote host.

      \begin{verbatim}
      input:
         modem......A structure that contains pointers to machine dependent
                    primitive IO functions.  See the comment section of the
                    SerialPort structure for details.  The function checks to
                    be sure this pointer is not NULL.

      output:
         This function returns a positive value if an active communications
         session with the remote host exists when this function returns.  If
         an session could not be established then this function returns
         zero.  A negative return value indicates either an invalid serial
         port or unimplemented modem carrier-detect functionality.
      \end{verbatim}
*/
int CLogin(const struct SerialPort *modem)
{
   /* function name for log entries */
   static cc FuncName[] = "CLogin()";

   int status=-1;

   /* pet the watchdog */
   WatchDog(); 

   /* validate the port */
   if (!modem) 
   {
      /* create the message */
      static cc msg[]="NULL serial port.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* validate the modem's CD functionality */
   else if (!modem->cd)
   {
      /* create the message */
      static cc msg[]="Nonfunctional modem CD signal.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* check if a connection already exists */
   else if (modem->cd()) {status=1;}

   /* connect and login to remote host */
   else
   {
      /* record beginning of connect-time */
      time_t Tc,To=time(NULL);

      /* make a log entry about connecting to primary host */
      if (debuglevel>=2 || (debugbits&CLOGIN_H))
      {
         /* create the message */
         static cc msg[]="Connecting to primary host.\n";

         /* log the message */
         LogEntry(FuncName,msg);
      }
      
      /* toggle the DTR line */
      ModemDtrClear();  Wait(500); ModemDtrAssert(); Wait(1000);//both waits changed from 250 HM 12/10/2014 

      /* connect to the primary host */
      if ((status=connect(modem,mission.at,mission.ConnectTimeOut))<=0)
      {
         /* make a log entry about connecting to alternate host */
         if (debuglevel>=2 || (debugbits&CLOGIN_H))
         {
            /* create the message */
            static cc format[]="Attempt to connect to primary host "
               "failed after %d tries.  Connect to alternate host.\n";

            /* log the message */
            LogEntry(FuncName,format,abs(status));
         }
         
         /* pet the watch dog */
         WatchDog(); 

         /* connect to the alternate host */
         status=connect(modem,mission.alt,mission.ConnectTimeOut);
      }

      /* pet the watchdog */
      WatchDog(); 

      /* timestamp when the connection was established */
      Tc=time(NULL);

      /* check if the connection was successfully established */
      if (status>0)
      {
         /* accumulate the number of connection attempts made to the remote host */
         vitals.ConnectionAttempts+=status;

         /* increment the number of connections made to the remote host */
         vitals.Connections++;
          
         /* log the amount of time required to establish the connection */
         if (debuglevel>=2 || (debugbits&CLOGIN_H))
         {
            /* create the message */
            static cc format[]="Connection %d established in %0.0f sec.\n";

            /* log the message */
            LogEntry(FuncName,format,vitals.Connections,difftime(Tc,To));
         }
      }
      else {ModemDtrClear(); Wait(250);}
      
      /* make a log entry if the connection attempt failed */
      if (status<=0)
      {
         /* create the message */
         static cc msg[]="Connecting to primary and alternate hosts failed - aborting.\n";

         /* log the message */
         LogEntry(FuncName,msg);
      }
      
      /* login to the host computer */
      else if ((status=login(modem,mission.user,mission.pwd))<=0)
      {
         /* create the message */
         static cc msg[]="Attempt to login to host failed.\n";

         /* log the message */
         LogEntry(FuncName,msg);
      }

      /* make a log entry that the login attempt was successful */
      else if (debuglevel>=2 || (debugbits&CLOGIN_H))
      {
         /* create the message */
         static cc format[]="Logged to host.  [in %0.0f seconds]\n";
 
         /* log the message */
         LogEntry(FuncName,format,difftime(time(NULL),Tc));
      }
   }

   return status;
}
