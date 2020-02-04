#ifndef MODEM_H
#define MODEM_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: modem.c,v 1.6 2007/04/24 01:43:29 swift Exp $
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
 * $Log: modem.c,v $
 * Revision 1.6  2007/04/24 01:43:29  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.5  2007/03/26 20:43:21  swift
 * Added prototype for snprintf() and removed include directive for snprintf.h.
 *
 * Revision 1.4  2006/10/11 21:02:59  swift
 * Added a toggle of the DTR signal prior to each connection attempt.
 *
 * Revision 1.3  2005/06/27 17:05:02  swift
 * Replaced references to 'Motorola IRIDIUM' to 'IRIDIUM' to account for
 * changes in manufacturer of the Daytona.
 *
 * Revision 1.2  2005/06/14 19:03:33  swift
 * Changed linkage of modem_initialize from static to external.
 *
 * Revision 1.1  2004/12/29 23:08:42  swift
 * Module to manage communications via modem.
 *
 * Revision 1.5  2002/12/31 16:58:29  swift
 * Eliminated the reference to the unistd.h header file.
 *
 * Revision 1.4  2002/10/21 18:24:04  swift
 * Reduce the number of connect tries from 5 to 3.
 *
 * Revision 1.3  2002/10/11 13:59:16  swift
 * Minor rework of the modem_initialize() function.
 *
 * Revision 1.2  2002/10/08 23:49:23  swift
 * Modifications for const correctness.  Added documentation.  Combined header file
 * into source file.
 *
 * Revision 1.1  2002/05/07 22:16:08  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define ModemChangeLog "$RCSfile: modem.c,v $  $Revision: 1.6 $   $Date: 2007/04/24 01:43:29 $"

#include <serial.h>

/* prototypes for external functions */
int connect(const struct SerialPort *port,const char *dialstring,time_t sec);
int hangup(const struct SerialPort *port);
int modem_initialize(const struct SerialPort *port);

#endif /* MODEM_H */

#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <chat.h>
#include <logger.h>

/* prototypes for local functions */
static int modem_connect(const struct SerialPort *port,const char *dialstring,time_t sec);
static int modem_hangup(const struct SerialPort *port);
static int modem_restore_factory_configuration(const struct SerialPort *port);
static int m9500cbst(const struct SerialPort *port);

/* prototypes for functions with external linkage */
int snprintf(char *str, size_t size, const char *format, ...);

/* time-out period (seconds) for communication with the modem */
const time_t timeout=5;

/* Motorola's ISU AT Command Reference indicates max command length of 128 bytes */
#define MAXCMDLEN 128

/* prototypes for functions with external linkage */
void Wait(unsigned int millisec);

/*------------------------------------------------------------------------*/
/* function to connect to a host computer                                 */
/*------------------------------------------------------------------------*/
/**
   This function attempts to establish a modem-to-modem connection with the
   remote host computer.  Multiple attempts are made to establish a
   connection up to a maximum number of retries.
    
      \begin{verbatim}
      input:

         port.........A structure that contains pointers to machine
                      dependent primitive IO functions.  See the comment
                      section of the SerialPort structure for details.  The
                      function checks to be sure this pointer is not NULL.

         dialstring...The modem command that generates the tones to connect
                      to the remote computer.  Example: ATDT2065436697.

         sec..........The number of seconds allowed to make the connection
                      before this function abandons the connection attempt.
  
      output:

         This function returns a positive number (equal to the number of
         tries) if the exchange was successful.  A negative number (whose
         absolute value is equal to the number of tries) is returned if a
         connection could not be established.
         
      \end{verbatim}

*/
int connect(const struct SerialPort *port,const char *dialstring,time_t sec)
{
   /* define the logging signature */
   static cc FuncName[] = "connect()";
   
   int i,status=0;
      
   /* validate the SerialPort object */
   if (!port)
   {
      /* create the message */
      static cc msg[]="NULL serial port.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* validate the dialstring */
   else if (!dialstring)
   {
      /* create the message */
      static cc msg[]="NULL dial string.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   else
   {
      /* define the number of tries allowed */
      const int NRetries=2;
      
      for (status=0, i=1; i<=NRetries; i++)
      {
         /* toggle the DTR line */
         if (port->dtr) {port->dtr(0);  Wait(250); port->dtr(1); Wait(250);}

         /* write the dialstring to the modem */
         if (modem_connect(port,dialstring,sec)>0) {status=i; break;}
         else status=-i;
      }

      /* make a log entry that the connection attempt failed */
      if (status<0)
      {
         /* create the message */
         static cc format[]="Aborted after %d failed attempts.\n";

         /* log the message */
         LogEntry(FuncName,format,NRetries);
      }
      
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to hang-up the modem to break the connection with the host    */
/*------------------------------------------------------------------------*/
/**
   This function attempts to hang-up the modem in order to break the
   connection with the host.  Multiple attempts are made to hang-up the
   modem up to a maximum number of retries.
   
      \begin{verbatim}
      input:

         port.......A structure that contains pointers to machine dependent
                    primitive IO functions.  See the comment section of the
                    SerialPort structure for details.  The function checks
                    to be sure this pointer is not NULL.

      output:

         This function returns a positive number if the hang-up was
         confirmed.  Zero is returned if the confirmation failed.  A
         negative number is returned if the function parameters were
         determined to be ill-defined.
         
      \end{verbatim}

   written by Dana Swift
*/
int hangup(const struct SerialPort *port)
{
   /* define the logging signature */
   static cc FuncName[] = "hangup()";
   
   int status=-1;
      
   /* validate the SerialPort object */
   if (!port)
   {
      /* create the message */
      static cc msg[]="NULL serial port.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   else
   {
      int i;
      
      for (status=0,i=0; i<3; i++)
      {
         if (modem_hangup(port)>0) {status=1; break;}
      }
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to connect the modem to a remote computer                     */
/*------------------------------------------------------------------------*/
/**
  This function connects the modem to the remote computer according to the
  dial-string.  Motorola's ISU AT Command Reference (SSP-ISU-CPSW-USER-005
  Version 1.3) was used as the guide document.
   
      \begin{verbatim}
      input:

         port.........A structure that contains pointers to machine
                      dependent primitive IO functions.  See the comment
                      section of the SerialPort structure for details.  The
                      function checks to be sure this pointer is not NULL.

         dialstring...The modem command that generates the tones to connect
                      to the remote computer.  Example: ATDT2065436697.

         sec..........The number of seconds allowed to make the connection
                      before this function abandons the connection attempt.

      output:

         This function returns a positive number if the exchange was
         successful.  Zero is returned if the exchange failed.  A negative
         number is returned if the function parameters were determined to be
         ill-defined. 
         
      \end{verbatim}

   written by Dana Swift
*/
static int modem_connect(const struct SerialPort *port,const char *dialstring,time_t sec)
{
   /* define the logging signature */
   static cc FuncName[] = "modem_connect()";
   
   int status=-1;
   
   /* check the length of the dialstring */
   if (dialstring && strlen(dialstring)>MAXCMDLEN-1)
   {
      /* create the message */
      static cc format[]="Warning...dialstring (%s) longer than %d bytes.\n";

      /* log the message */
      LogEntry(FuncName,format,dialstring,MAXCMDLEN-1);
   }
      
   /* validate the SerialPort object */
   if (!port)
   {
      /* create the message */
      static cc msg[]="NULL serial port.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* restore the modem's factory defaults */
   else if ((status=modem_initialize(port))<=0)
   {
      /* create the message */
      static cc msg[]="Modem configuration failed.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* validate the dialstring */
   else if (!dialstring)
   {
      /* create the message */
      static cc msg[]="NULL dial string.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* send the escape string and get the modem's response */
   else if (chat(port,dialstring,"CONNECT",sec,"\r")<=0)
   {
      /* create the message */
      static cc format[]="Modem command [%s] failed.\n";
      
      /* make a log entry */
      LogEntry(FuncName,format,dialstring);

      /* indicate failure */
      status=0;
   }

   /* set return value to indicate success */
   else status=1;

   return status;
}

/*------------------------------------------------------------------------*/
/* function to initialize modem using AT command string                   */
/*------------------------------------------------------------------------*/
/**
   This function initializes the modem with a specified command string.
   Motorola's ISU AT Command Reference (SSP-ISU-CPSW-USER-005 Version 1.3)
   was used as the guide document.
   
      \begin{verbatim}
      input:

         port.......A structure that contains pointers to machine dependent
                    primitive IO functions.  See the comment section of the
                    SerialPort structure for details.  The function checks
                    to be sure this pointer is not NULL.

      output:

         This function returns a positive number if the exchange was
         successful.  Zero is returned if the exchange failed.  A negative
         number is returned if the function parameters were determined to be
         ill-defined. 
         
      \end{verbatim}

   written by Dana Swift
*/
static int modem_hangup(const struct SerialPort *port)
{
   /* define the logging signature */
   static cc FuncName[] = "modem_hangup()";
   
   int status=-1;
      
   /* validate the SerialPort object */
   if (!port) 
   {
      /* create the message */
      static cc msg[]="NULL serial port.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   else
   {
      /* a longer timeout is needed when hanging up the phone */
      const time_t timeout=10;

      /* define the hang-up command string */
      const char *cmd = "+++~~~ATH0";
      
      /* set an assertion on the maximum allowed length of the command string */
      assert(strlen(cmd)<MAXCMDLEN);

      /* re-initialize return value */
      status=1;
      
      /* send the command string and get the modem's response */
      if (chat(port,cmd,"OK",timeout,"\r")<=0)
      {
         /* create the message */
         static cc format[]="Modem command [%s] failed.\n";

         /* make a log entry */
         LogEntry(FuncName,format,cmd);

         /* indicate failure */
         status=0;
      } 
      else if (debuglevel>=5)
      {
         /* create the message */
         static cc msg[]="Success.\n";

         /* log the message */
         LogEntry(FuncName,msg);
      }
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to initialize modem using AT command string                   */
/*------------------------------------------------------------------------*/
/**
   This function initializes the modem with a specified command string.
   Motorola's ISU AT Command Reference (SSP-ISU-CPSW-USER-005 Version 1.3)
   was used as the guide document.
   
      \begin{verbatim}
      input:

         port.......A structure that contains pointers to machine dependent
                    primitive IO functions.  See the comment section of the
                    SerialPort structure for details.  The function checks
                    to be sure this pointer is not NULL.

      output:

         This function returns a positive number if the exchange was
         successful.  Zero is returned if the exchange failed.  A negative
         number is returned if the function parameters were determined to be
         ill-defined. 
         
      \end{verbatim}

   written by Dana Swift
*/
int modem_initialize(const struct SerialPort *port)
{
   /* define the logging signature */
   static cc FuncName[] = "modem_initialize()";
   
   int status=-1;
      
   /* validate the SerialPort object */
   if (!port)
   {
      /* create the message */
      static cc msg[]="NULL serial port.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* restore the modem's factory defaults */
   else if ((status=modem_restore_factory_configuration(port))<=0)
   {
      /* create the message */
      static cc msg[]="Unable to restore modem's factory defaults.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }
    
   else
   {
      /* specify initialization command string */
      const char *cmd = "AT &C1 &D2 &K0 &R1 &S1 E1 Q0 S0=0 S7=45 S10=100 V1 X4";

      /* set an assertion on the maximum allowed length of the command string */
      assert(strlen(cmd)<MAXCMDLEN);
      
      /* re-initialize the return value of this function */
      status=1;

      /* send the command string and get the modem's response */
      if (chat(port,cmd,"OK",timeout,"\r")<=0)
      {
         /* create the message */
         static cc format[]="Modem command [%s] failed.\n";
      
         /* make a log entry */
         LogEntry(FuncName,format,cmd);

         /* indicate failure */
         status=0;
      } 

      else if (chat(port,"AT I4","IRIDIUM",timeout,"\r")>0 && m9500cbst(port)<=0)
      {
         /* create the message */
         static cc msg[]="Selection of bearer service type failed.\n";

         /* log the message */
         LogEntry(FuncName,msg);

         /* indicate failure */
         status=0;
      }

      else if (debuglevel>=5)
      {
         /* create the message */
         static cc msg[]="Success.\n";

         /* log the message */
         LogEntry(FuncName,msg);
      }
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to restore the modem's factory configuration                  */
/*------------------------------------------------------------------------*/
/**
   This function uses the modem AT\&F command function to restore the modem
   to the factory default configuration.  Motorola's ISU AT Command
   Reference (SSP-ISU-CPSW-USER-005 Version 1.3) was used as the guide
   document. 
   
      \begin{verbatim}
      input:

         port.......A structure that contains pointers to machine dependent
                    primitive IO functions.  See the comment section of the
                    SerialPort structure for details.  The function checks
                    to be sure this pointer is not NULL.

      output:

         This function returns a positive number if the exchange was
         successful.  Zero is returned if the exchange failed.  A negative
         number is returned if the function parameters were determined to be
         ill-defined. 
         
      \end{verbatim}

   written by Dana Swift
*/
static int modem_restore_factory_configuration(const struct SerialPort *port)
{
   /* define the logging signature */
   static cc FuncName[] = "modem_restore_factory_configuration()";
   
   int status=-1;
      
   /* validate the SerialPort object */
   if (!port)
   {
      /* create the message */
      static cc msg[]="NULL serial port.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   else
   {
      /* define the command */
      const char *cmd = "AT&F0";
      
      /* set an assertion on the maximum allowed length of the command string */
      assert(strlen(cmd)<MAXCMDLEN); 

      /* re-initialize the return value of this function */
      status=1;

      /* send the command to restore factory defaults and get response */
      if (chat(port,cmd,"OK",timeout,"\r")<=0)
      {
         /* create the message */
         static cc format[]="Modem command [%s] failed.\n";
         
         /* make a log entry */
         LogEntry(FuncName,format,cmd);

         /* indicate failure */
         status=0;
      } 
      else if (debuglevel>=5)
      {
         /* create the message */
         static cc msg[]="Success.\n";

         /* log the message */
         LogEntry(FuncName,msg);
      }
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to set the bearer service type                                */
/*------------------------------------------------------------------------*/
/**
   This function selects the Bearer Service Type to be 4800 baud on the
   remote computer.  I have found this command to be necessary in order for
   logins to be successful.  Motorola's ISU AT Command Reference
   (SSP-ISU-CPSW-USER-005 Version 1.3) was used as the guide document.
   
      \begin{verbatim}
      input:

         port.......A structure that contains pointers to machine dependent
                    primitive IO functions.  See the comment section of the
                    SerialPort structure for details.  The function checks
                    to be sure this pointer is not NULL.

      output:

         This function returns a positive number if the exchange was
         successful.  Zero is returned if the exchange failed.  A negative
         number is returned if the function parameters were determined to be
         ill-defined. 
         
      \end{verbatim}

   written by Dana Swift
*/
static int m9500cbst(const struct SerialPort *port)
{
   /* define the logging signature */
   static cc FuncName[] = "m9500cbst()";
   
   int status=-1;
      
   /* validate the SerialPort object */
   if (!port)
   {
      /* create the message */
      static cc msg[]="NULL serial port.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   else
   {
      /* define the command */
      const char *cmd = "AT +CBST=6,0,1";
      
      /* set an assertion on the maximum allowed length of the command string */
      assert(strlen(cmd)<MAXCMDLEN); 

      /* re-initialize the return value of this function */
      status=1;

      /* send the command to restore factory defaults and get response */
      if (chat(port,cmd,"OK",timeout,"\r")<=0)
      {
         /* create the message */
         static cc format[]="Modem command [%s] failed.\n";
         
         /* make a log entry */
         LogEntry(FuncName,format,cmd); 

         /* indicate failure */
         status=0;
      } 
      else if (debuglevel>=5)
      {
         /* create the message */
         static cc msg[]="Success.\n";

         /* log the message */
         LogEntry(FuncName,msg);
      }
   }
   
   return status;
}

