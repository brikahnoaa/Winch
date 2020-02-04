#ifndef LBT9522_H
#define LBT9522_H (0x0100U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: lbt9522.c,v 1.23.2.1 2008/09/11 20:03:35 dbliudnikas Exp $
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
 * $Log: lbt9522.c,v $
 * Revision 1.23.2.1  2008/09/11 20:03:35  dbliudnikas
 * Update for Apf9i test setup with U.S.Robitic modem, so does not fail on mission start.
 *
 * Revision 1.23  2008/07/14 16:56:20  swift
 * Fixed a bug in the function to query the LBT for signal strength.
 *
 * Revision 1.22  2007/06/08 02:14:30  swift
 * Added a logentry to IrModemRegister() that warns when the modem is not a
 * 9522A.  Added delays to resolve a race condition.
 *
 * Revision 1.21  2007/05/09 19:12:28  swift
 * Added a logentry to IrModemRegister() that warns when the modem is not a 9522A.
 *
 * Revision 1.20  2007/05/08 18:10:39  swift
 * Added attribution just below the copyright in the main comment section.
 *
 * Revision 1.19  2007/03/26 20:47:22  swift
 * Implemented meachanism for storing and retrieving ICCID/MSISDN numbers on
 * SIM cards.
 *
 * Revision 1.18  2006/08/17 21:17:59  swift
 * Modifications to allow better logging control.
 *
 * Revision 1.17  2006/05/15 18:54:49  swift
 * Changed the AT command to set the fixed baud rate at 19200.  The old AT spec
 * was incomplete and did not correctly describe the AT+IPR command.
 *
 * Revision 1.16  2006/04/21 13:45:42  swift
 * Changed copyright attribute.
 *
 * Revision 1.15  2006/03/24 23:39:02  swift
 * Eliminated IrModemDefibrillate() because the LBT9522a nonresponse problem
 * was found and fixed.
 *
 * Revision 1.14  2006/02/28 16:34:23  swift
 * Eliminated the modem defibrillation kludge because the fundamental problem
 * with nonresponsive LBT9522a modems was found and fixed.
 *
 * Revision 1.13  2006/02/22 21:53:29  swift
 * Added functions to query the LBT for firmware revision, model, and IMEI number.
 *
 * Revision 1.12  2006/02/03 00:22:52  swift
 * Fixed a buffer over-run bug in use of pgets().
 *
 * Revision 1.11  2006/01/31 19:25:52  swift
 * Added LBT9522 debibrillation kludge to prevent nonresponsive modems.
 *
 * Revision 1.10  2005/07/12 18:18:29  swift
 * Minor rework of power-cycling of LBT.
 *
 * Revision 1.9  2005/07/07 19:46:19  swift
 * Changed handling of the DTR signal to work around problems with the LBT9522.
 *
 * Revision 1.8  2005/07/07 15:08:13  swift
 * Fixed a bug caused by 1-second time descretization.
 *
 * Revision 1.7  2005/06/27 17:05:07  swift
 * Replaced references to 'Motorola IRIDIUM' to 'IRIDIUM' to account for
 * changes in manufacturer of the Daytona.
 *
 * Revision 1.6  2005/06/21 14:11:44  swift
 * Reduced power-cycling wait periods to 1 second.
 *
 * Revision 1.5  2005/06/14 19:02:44  swift
 * Added features to configure the LBT.
 *
 * Revision 1.4  2005/05/01 14:30:43  swift
 * Minor adjustments to LBT power-up bug work-around.
 *
 * Revision 1.3  2005/04/30 17:33:28  swift
 * Improvements to the work-around for the power-up bug of Sebring and Daytona LBTs.
 *
 * Revision 1.2  2005/04/29 19:39:14  swift
 * Implemented power-cycling work-around for LBT9522 no-response bug.
 *
 * Revision 1.1  2005/02/22 21:43:07  swift
 * Eliminated use of Motorola 9500 phone in favor of LBT9522.
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define lbt9522ChangeLog "$RCSfile: lbt9522.c,v $  $Revision: 1.23.2.1 $   $Date: 2008/09/11 20:03:35 $"

#include <serial.h>

int IrModemAttention(const struct SerialPort *port);
int IrModemConfigure(const struct SerialPort *port);
int IrModemFwRev(const struct SerialPort *port, char *FwRev, size_t size);
int IrModemIccid(const struct SerialPort *port,char *iccid,char *msisdn,size_t size);
int IrModemIccidSet(const struct SerialPort *port,const char *iccid,const char *msisdn);
int IrModemImei(const struct SerialPort *port, char *imei, size_t size);
int IrModemModel(const struct SerialPort *port, char *model, size_t size);
int IrModemRegister(const struct SerialPort *port);
int IrSignalStrength(const struct SerialPort *port);
int IrSkySearch(const struct SerialPort *port);

#endif /* LBT9522_H */
 
#include <assert.h>
#include <chat.h>
#include <logger.h>
#include <modem.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* declare functions with external linkage */
time_t itimer(void);
void   ModemEnable(unsigned int BaudRate);
void   ModemDisable(void);
int    snprintf(char *str, size_t size, const char *format, ...);
void   Wait(unsigned int millisec);

/*------------------------------------------------------------------------*/
/* function to power-up the modem and get its attention                   */
/*------------------------------------------------------------------------*/
/**
   This function powers-up the iridium modem and ensures that it will
   respond to AT commands.  It was created because experience shows that
   under some circumstances the LBT must be power-cycled before it will
   respond to commands.  On success (ie., the modem responds to AT
   commands), then this function returns a positive value.  On failure (ie.,
   the modem fails to respond to AT commands), then this function returns
   zero.  A negative return value indicates an invalid serial port.
*/
int IrModemAttention(const struct SerialPort *port)
{
   /* define the logging signature */
   static cc FuncName[] = "IrModemAttention()";

   int i,j,status = -1;

   if (!port) 
   {
      /* create the message */
      static cc msg[]="NULL serial port.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* fault-tolerance loop to get the LBT9522's attention */
   else for (status=0,i=1; !status && i<=4; i++)
   {
      /* allow for autobaud detection */
      for (j=0; j<3; j++) {port->putb('\r'); Wait(100);}

      for (j=0; j<3; j++)
      {
         /* get the LBT9522's attention */
         if (chat(port,"AT","OK",2,"\r")>0) {status=1; break;}
      }

      /* cycle power if modem does not respond */
      if (!status && i<4) {ModemDisable(); sleep(i*15); ModemEnable(19200);}
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to configure the LBT9522                                      */
/*------------------------------------------------------------------------*/
int IrModemConfigure(const struct SerialPort *port)
{
   /* define the logging signature */
   static cc FuncName[] = "IrModemConfigure()";

   /* initialize the return value */
   int status=-1;

   if (!port) 
   {
      /* create the message */
      static cc msg[]="NULL serial port.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   else if ((status=IrModemAttention(port))<=0)
   {
      /* create the message */
      static cc msg[]="Modem not responding.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   else
   {
      /* set the timeout period (sec) for chat sessions */
      const time_t timeout = 5;
      
      /* reinitialize the return value */
      status=1;

      /* initialize the modem configuration */
      if (modem_initialize(port)>0)
      {
         /* check if the modem is an iridium LBT */
         if (chat(port,"AT I4","IRIDIUM",timeout,"\r")>0)
         {
            /* set the baud rate to be 19200 (fixed) */
            if (chat(port,"AT +IPR=6,0","OK",timeout,"\r")>0)
            {
               /* create the message */
               static cc msg[]="Modem configured for fixed (19200) baud rate.\n";

               /* log the message */
               if (debuglevel>=2 || (debugbits&LBT9522_H)) LogEntry(FuncName,msg);
            }
            else
            {
               /* create the message */
               static cc msg[]="Attempt to configure modem for fixed baud rate failed.\n";

               /* log the message */
               LogEntry(FuncName,msg);

               status=0;
            }
         }

         /* store the configuration in the modem's NVRAM */
         if (chat(port,"AT &W0 &Y0","OK",timeout,"\r")>0)
         {
            /* create the message */
            static cc msg[]="Configuration saved to modem's NVRAM.\n";
    
            /* log the message */
            if (debuglevel>=2 || (debugbits&LBT9522_H)) LogEntry(FuncName,msg);
         }
         else 
         {
            /* create the message */
            static cc msg[]="Attempt to save configuration to modem's NVRAM failed.\n";
    
            /* log the message */
            LogEntry(FuncName,msg);

            /* status=0; Not a failure condition for USRobotics modem test simulation setup */
         }
      }
      
      else 
      {
         /* create the message */
         static cc msg[]="Attempt to initialize modem failed.\n";
    
         /* log the message */
         LogEntry(FuncName,msg);

         status=0;
      }
   }
   
   if (status>0)
   {
      /* create the message */
      static cc msg[]="Modem configuration successful.\n";
    
      /* log the message */
      if (debuglevel>=2 || (debugbits&LBT9522_H)) LogEntry(FuncName,msg);
   }
   else
   {
      /* create the message */
      static cc msg[]="Modem configuration failed.\n";
    
      /* log the message */
      LogEntry(FuncName,msg);
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to query the LBT for its firmware revision                    */
/*------------------------------------------------------------------------*/
/**
   This function queries the LBT for its firmware.

      \begin{verbatim}
      input:

         port....A structure that contains pointers to machine dependent
                 primitive IO functions.  See the comment section of the
                 SerialPort structure for details.  The function checks to
                 be sure this pointer is not NULL.
         size....The size of the 'FwRev' buffer used to store the firmware
                 revision.  This buffer must be at least 16 bytes long.

      output:

         FeRev...The firmware revision of the LBT is stored in this
                 buffer.

         This function returns a positive value on success and a zero on
         failure.  A negative negative value indicates an invalid argument.
     \end{verbatim}
*/
int IrModemFwRev(const struct SerialPort *port, char *FwRev, size_t size)
{
   /* define the logging signature */
   static cc FuncName[] = "IrModemFwRev()";

   /* initialize return value */
   int status=-1;

   /* validate the port */
   if (!port) 
   {
      /* create the message */
      static cc msg[]="NULL serial port.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* validate the buffer */
   else if (!FwRev || size<16)
   {
      static cc msg[]="FwRev buffer NULL or too small.\n";
      LogEntry(FuncName,msg);
   }

   else if ((status=IrModemAttention(port))<=0)
   {
      /* create the message */
      static cc msg[]="Modem not responding.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }
 
   else
   {
      char *p,buf[64];

      /* define the key that introduces the firmware revision */
      const char *key="Call Processor Version: ";

      /* compute the length of the key */
      const int len=strlen(key);

      /* reinitialize return value */
      status=0;
      
      /* flush the IO port buffers */
      usleep(50000U); assert(port->ioflush && port->ioflush()>0);

      /* transmit the command to the LBT */
      if (chat(port,"AT+CGMR","AT+CGMR",5,"\r")<=0)
      {
         static cc msg[]="LBT not responding.\n"; LogEntry(FuncName,msg);
      }

      /* read each line of output */
      else while (status<=0 && pgets(port,buf,sizeof(buf),5,"\r\n")>0)
      {
         /* scan the current line for the key */
         if (strlen(buf)>0 && (p=strstr(buf,key)))
         {
            /* copy the FwRev number to the output buffer */
            strncpy(FwRev,buf+len,size); FwRev[size-1]=0; status=1; break;
         }
      }
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to query the LBT for the ICCID and MSISDN numbers             */
/*------------------------------------------------------------------------*/
/**
   This function queries the LBT for its ICCID number (ie., SIM card serial
   number) and its MSISDN number (ie., SIM card phone number) .

      \begin{verbatim}
      input:

         port....A structure that contains pointers to machine dependent
                 primitive IO functions.  See the comment section of the
                 SerialPort structure for details.  The function checks to
                 be sure this pointer is not NULL.

         size....The size of the 'iccid' and 'msisdn' buffers used to store
                 the ICCID and MSISDN numbers.  These buffers must be at
                 least 20 bytes long.

      output:

         iccid....The 19-digit ICCID number of the sim card is stored in
                  this buffer.  Also known as the SIM card serial number.

         msisdn...The 12-digit MSISDN number of the sim card is stored in
                  this buffer.  Also known as the SIM card phone number.

         This function returns a positive value on success or zero if an
         invalid ICCID string was read from the SIM card.  A negative
         value indicates that an exception was encountered.
     \end{verbatim}
*/
int IrModemIccid(const struct SerialPort *port,char *iccid,char *msisdn,size_t size)
{
   /* define the logging signature */
   static cc FuncName[] = "IrModemIccid()";

   /* initialize return value */
   int status=-1;

   if (iccid) *iccid=0; if (msisdn) *msisdn=0;
   
   /* validate the port */
   if (!port) 
   {
      /* create the message */
      static cc msg[]="NULL serial port.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* validate the buffer */
   else if (!iccid || size<20)
   {
      static cc msg[]="ICCID buffer NULL or too small.\n";
      LogEntry(FuncName,msg);
   }

   /* induce the modem into a communicative state */
   else if ((status=IrModemAttention(port))<=0)
   {
      /* create the message */
      static cc msg[]="Modem not responding.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* validate that the modem is a 9522{A} */
   else if (chat(port,"AT+CGMM","9522",2,"\r")<=0)
   {
      static cc msg[]="Modem's model is not 9522{A}; Can't access SIM card.\n";
      LogEntry(FuncName,msg); status=-3;
   }
   
   else
   {
      /* define some work space */
      char *p,*q,buf[64]; int i,err,len;

      /* define the expected response string */
      const char *CpbrRsp="+CPBR:101,\""; const int CpbrRspLen=strlen(CpbrRsp);

      /* define quantities needed to implement the timeout mechanism */
      const time_t To=time(NULL), timeout=70;

      /* reinitialize return value */
      status=-2; sleep(1);

      /* select the SIM phone book */
      for (err=0; err<=0 && difftime(time(NULL),To)<timeout;)
      {
         /* flush the IO port buffers */
         assert(port->ioflush && port->ioflush()>0);

         /* execute the AT command to select the SIM phone book */
         pputs(port,"AT+CPBS=\"SM\"",1,"\r");

         /* retry loop for response analysis */
         for (err=0; pgets(port,buf,sizeof(buf)-1,3,"\r\n")>0;)
         {
            /* an ERROR response means the SIM phone book isn't available yet */
            if (strstr(buf,"ERROR")) {sleep(5); break;}

            /* an OK response means the SIM phone book is ready */
            else if (strstr(buf,"OK")) {err=1; break;}
         }
      }

      /* check if the SIM phone book was found */
      if (err<=0)
      {
         static cc msg[]="Attempt to access SIM card failed.\n";
         LogEntry(FuncName,msg); status=0;
      }

      /* retry loop to extract the SIM card serial number */
      else for (i=0; status<0 && i<2; i++)
      {
         /* flush the IO port buffers */
         assert(port->ioflush && port->ioflush()>0);

         /* execute the AT command to extract the first SIM phone book entry */
         pputs(port,"AT+CPBR=101",1,"\r");

         /* retry loop for response analysis */
         while (pgets(port,buf,sizeof(buf)-1,2,"\r\n")>0)
         {
            /* locate the start and end of the ICCID (ie., SIM serial number) */
            if ((p=strstr(buf,CpbrRsp)) && (q=strchr(p+CpbrRspLen,'"')))
            {
               /* terminate and extract the ICCID */
               (*q)=0; snprintf(iccid,size,"%s",p+CpbrRspLen);

               /* locate the start and end of the MSISDN */
               if (msisdn && (p=strstr(++q,",129,\"")) && (q=strchr(p+6,'"')))
               {
                  /* terminate and extract the MSISDN */
                  (*q)=0; snprintf(msisdn,size,"%s",p+6);
               }

               /* validate the length of the ICCID string */
               if ((len=strlen(iccid))!=19)
               {
                  static cc format[]="Invalid ICCID length(%d): \"%s\"\n";
                  LogEntry(FuncName,format,len,iccid); status=0; break; 
               }
               else status=1;

               break;
            }
         }
      }
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to write the ICCID/MSISDN numbers into the SIM phone book     */
/*------------------------------------------------------------------------*/
/**
   This function writes the ICCID number (ie., SIM card serial number) and
   the MSISDN number (ie., SIM card phone number) into the SIM phone book.

      \begin{verbatim}
      input:

         port....A structure that contains pointers to machine dependent
                 primitive IO functions.  See the comment section of the
                 SerialPort structure for details.  The function checks to
                 be sure this pointer is not NULL.

         iccid....The 19-digit ICCID number (aka., SIM card serial number)
                  of the sim card to be stored in the SIM's phone book.

         msisdn...The 12-digit MSISDN number (aka., SIM card phone number)
                  of the sim card is stored in this buffer.

      output:

         This function returns a positive value on success or zero if the
         attempt to write to the SIM phone book failed.  A negative value
         indicates that an exception was encountered.
      \end{verbatim}
*/
int IrModemIccidSet(const struct SerialPort *port,const char *iccid,const char *msisdn)
{
   /* define the logging signature */
   static cc FuncName[] = "IrModemIccidSet()";

   /* initialize return value */
   int status=-1;

   /* validate the port */
   if (!port) 
   {
      /* create the message */
      static cc msg[]="NULL serial port.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* validate the ICCID string */
   else if (!iccid || strlen(iccid)!=19 || strspn(iccid,"0123456789")!=19)
   {
      static cc msg[]="NULL or invalid ICCID string.\n";
      LogEntry(FuncName,msg);
   }

   /* validate the MSISDN string */
   else if (!msisdn || strlen(msisdn)!=12 || strspn(msisdn,"0123456789")!=12)
   {
      static cc msg[]="NULL or invalid MSISDN string.\n";
      LogEntry(FuncName,msg);
   }

   /* induce the modem into a communicative state */
   else if ((status=IrModemAttention(port))<=0)
   {
      /* create the message */
      static cc msg[]="Modem not responding.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* validate that the modem is a 9522{A} */
   else if (chat(port,"AT+CGMM","9522",2,"\r")<=0)
   {
      static cc msg[]="Modem's model is not 9522{A}; Can't access SIM card.\n";
      LogEntry(FuncName,msg); status=-3;
   }

   else
   {
      /* define some work space */
      char cmd[64],buf[64]; int i,err;

      /* define quantities needed to implement the timeout mechanism */
      const time_t To=time(NULL), timeout=70;

      /* create AT command to write the ICCID/MSISDN to the SIM phone book */
      snprintf(cmd,sizeof(cmd),"AT+CPBW=101,\"%s\",129,\"%s\"",iccid,msisdn);
               
      /* reinitialize return value */
      status=-2; sleep(1);

      /* select the SIM phone book */
      for (err=0; err<=0 && difftime(time(NULL),To)<timeout;)
      {
         /* flush the IO port buffers */
         assert(port->ioflush && port->ioflush()>0);

         /* execute the AT command to select the SIM phone book */
         pputs(port,"AT+CPBS=\"SM\"",1,"\r");

         /* retry loop for response analysis */
         for (err=0; pgets(port,buf,sizeof(buf)-1,3,"\r\n")>0;)
         {
            /* an ERROR response means the SIM phone book isn't available yet */
            if (strstr(buf,"ERROR")) {sleep(5); break;}

            /* an OK response means the SIM phone book is ready */
            else if (strstr(buf,"OK")) {err=1; break;}
         }
      }

      /* check if the SIM phone book was found */
      if (err<=0)
      {
         static cc msg[]="Attempt to access SIM card failed.\n";
         LogEntry(FuncName,msg); status=0;
      }

      /* retry loop to extract the SIM card serial number */
      else for (status=0,i=0; i<2; i++)
      {
         if (chat(port,cmd,"OK",2,"\r")>0) {status=1; break;}
         else
         {
            static cc msg[]="Attempt to write ICCID/MSISDN to SIM phone book failed.\n";
            LogEntry(FuncName,msg);
         }
      }
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to query the LBT for the IMEI number                          */
/*------------------------------------------------------------------------*/
/**
   This function queries the LBT for its IMEI number.

      \begin{verbatim}
      input:

         port....A structure that contains pointers to machine dependent
                 primitive IO functions.  See the comment section of the
                 SerialPort structure for details.  The function checks to
                 be sure this pointer is not NULL.
         size....The size of the 'imei' buffer used to store the IMEI
                 number.  This buffer must be at least 16 bytes long.

      output:

         imei....The 15-digit IMEI number of the LBT is stored in this
                 buffer.

         This function returns a positive value on success and a zero on
         failure.  A negative negative value indicates an invalid argument.
     \end{verbatim}
*/
int IrModemImei(const struct SerialPort *port, char *imei, size_t size)
{
   /* define the logging signature */
   static cc FuncName[] = "IrModemImei()";

   /* initialize return value */
   int status=-1;

   /* validate the port */
   if (!port) 
   {
      /* create the message */
      static cc msg[]="NULL serial port.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* validate the buffer */
   else if (!imei || size<16)
   {
      static cc msg[]="IMEI buffer NULL or too small.\n";
      LogEntry(FuncName,msg);
   }
   
   else if ((status=IrModemAttention(port))<=0)
   {
      /* create the message */
      static cc msg[]="Modem not responding.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   else
   {
      char buf[32];

      /* reinitialize return value */
      status=0;
      
      /* flush the IO port buffers */
      usleep(50000U); assert(port->ioflush && port->ioflush()>0);

      /* transmit the command to the LBT */
      if (chat(port,"AT+CGSN","AT+CGSN",5,"\r")<=0)
      {
         static cc msg[]="LBT not responding.\n"; LogEntry(FuncName,msg);
      }

      /* read each line of output */
      else while (status<=0 && pgets(port,buf,sizeof(buf),5,"\r\n")>0)
      {
         /* scan the current line for a 15 digit number */
         if (strlen(buf)>0 && strspn(buf,"0123456789")==15)
         {
            /* copy the IMEI number to the output buffer */
            strncpy(imei,buf,size); imei[size-1]=0; status=1; break;
         }
      }
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to query the LBT for the IMEI number                          */
/*------------------------------------------------------------------------*/
/**
   This function queries the LBT for its IMEI number.

      \begin{verbatim}
      input:

         port....A structure that contains pointers to machine dependent
                 primitive IO functions.  See the comment section of the
                 SerialPort structure for details.  The function checks to
                 be sure this pointer is not NULL.
         size....The size of the 'model' buffer used to store the LBT model
                 identifier.  This buffer must be at least 16 bytes long.

      output:

         model....The LBT model identifier.

         This function returns a positive value on success and a zero on
         failure.  A negative negative value indicates an invalid argument.
     \end{verbatim}
*/
int IrModemModel(const struct SerialPort *port, char *model, size_t size)
{
   /* define the logging signature */
   static cc FuncName[] = "IrModemModel()";

   /* initialize return value */
   int status=-1;

   /* validate the port */
   if (!port) 
   {
      /* create the message */
      static cc msg[]="NULL serial port.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* validate the buffer */
   else if (!model || size<16)
   {
      static cc msg[]="Model buffer NULL or too small.\n";
      LogEntry(FuncName,msg);
   }
  
   else if ((status=IrModemAttention(port))<=0)
   {
      /* create the message */
      static cc msg[]="Modem not responding.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }
 
   else
   {
      char buf[32];

      /* reinitialize return value */
      status=0;
      
      /* flush the IO port buffers */
      usleep(50000U); assert(port->ioflush && port->ioflush()>0);

      /* transmit the command to the LBT */
      if (chat(port,"AT+CGMM","AT+CGMM",5,"\r")<=0)
      {
         static cc msg[]="LBT not responding.\n"; LogEntry(FuncName,msg);
      }

      /* read each line of output */
      else while (status<=0 && pgets(port,buf,sizeof(buf),5,"\r\n")>0)
      {
         /* scan the current line for the model number */
         if (strlen(buf)>0 && strcspn(buf," \t\r\n")>0)
         {
            /* copy the model to the output buffer */
            strncpy(model,buf,size); model[size-1]=0; status=1; break;
         }
      }
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to register the modem with the iridium system                 */
/*------------------------------------------------------------------------*/
/**
   This function attempts to register the LBT with the iridium system.  On
   success, this function returns a positive value.  Zero is returned if the
   registration attempt failed.  A negative return value indicates an
   invalid serial port.
*/
int IrModemRegister(const struct SerialPort *port)
{
   /* define the logging signature */
   static cc FuncName[] = "IrModemRegister()";

   /* initialize the return value */
   int status=-1;

   if (!port) 
   {
      /* create the message */
      static cc msg[]="NULL serial port.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   else if ((status=IrModemAttention(port))<=0)
   {
      /* create the message */
      static cc msg[]="Modem not responding.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }
   
   /* check if the modem is an iridium modem */
   else if (chat(port,"AT I4","IRIDIUM",10,"\r")>0)
   {
      /* initialize parameters of the timeout mechanism */
      const time_t TimeOut=120, To=time(NULL);

      /* reinitialize the return value */
      status=0;

      do
      {
         /* flush the serial port buffers */
         if (port->ioflush) port->ioflush();

         /* request the LBT9522's registration status */
         if (pputs(port,"AT+CREG?",2,"\r")>0)
         {
            char *p,buf[32];

            /* read the next line from the serial port */
            while (pgets(port,buf,sizeof(buf)-1,2,"\r\n")>0)
            {
               if (debuglevel>=3 || (debugbits&LBT9522_H)) 
               {
                  static cc format[]="Received: %s\n";
                  LogEntry(FuncName,format,buf);
               }

               /* search for the second field of the expected response string */
               if ((p=strstr(buf,"+CREG:")) && (p=strchr(p,',')))
               {
                  /* get the value of the second field */
                  int s = atoi(++p);

                  /* check for registration */
                  if (s==1 || s==5) {status=1; break;}
               }
            }

            /* pause before retry */
            sleep(5);
         }
      }

      /* check termination criteria */
      while (!status && difftime(time(NULL),To)<TimeOut);
   }

   /* make a logentry that the modem is not an LBT */
   else if (debuglevel>=3 || (debugbits&LBT9522_H)) 
   {
      static cc msg[]="Modem is not an Iridium LBT.\n";
      LogEntry(FuncName,msg); 
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to query the iridium modem for signal strength                */
/*------------------------------------------------------------------------*/
/**
   This function queries the Iridium LBT for signal strength.  A time-out
   feature (2 minutes) is implemented to prevent this function waiting
   indefinitely for the LBT to respond.
   
      \begin{verbatim}
      input:

         port.........A structure that contains pointers to machine
                      dependent primitive IO functions.  See the comment
                      section of the SerialPort structure for details.  The
                      function checks to be sure this pointer is not NULL.
         
      output:

         This function returns the average of up to three responses to the
         +CSQ command.  A two second delay follows each response before the
         next +CSQ command is sent to the LBT.  On success, this function
         returns a non-negative number representing the average number of
         decibars (0-50) that would be visible on the LCD screen of an Iridium
         phone.  A negative return value indicates an exception was
         encountered. 
      \end{verbatim}
*/
int IrSignalStrength(const struct SerialPort *port)
{
   /* define the logging signature */
   static cc FuncName[] = "IrSignalStrength()";

   /* initialize the return value */
   int err=0,status=0;

   if (!port) 
   {
      /* create the message */
      static cc msg[]="NULL serial port.\n";

      /* log the message */
      LogEntry(FuncName,msg); status=-1;
   }
   
   else if ((err=IrModemAttention(port))<=0)
   {
      /* create the message */
      static cc msg[]="Modem not responding.\n";

      /* log the message */
      LogEntry(FuncName,msg); status=err;
   }

   /* check if the modem is an iridium modem */
   else if (chat(port,"AT I4","IRIDIUM",10,"\r")>0)
   {
      /* initialize parameters of the timeout mechanism */
      const time_t TimeOut=120, To=time(NULL);

      /* initialize averaging parameters */
      int n=0; status=0;

      /* pause before starting */
      sleep(1);
           
      do
      {
         /* pause before (re)try */
         sleep(1);

         /* flush the serial port buffers */
         if (port->ioflush) port->ioflush();

         /* request the LBT9522's registration status */
         if (pputs(port,"AT+CSQ",2,"\r")>0)
         {
            char *p,buf[32];

            /* read the next line from the serial port */
            while (pgets(port,buf,sizeof(buf)-1,5,"\n")>0)
            {
               if (debuglevel>=3 || (debugbits&LBT9522_H)) 
               {
                  static cc format[]="Received: %s\n";
                  LogEntry(FuncName,format,buf);
               }

               /* search for the expected response string */
               if ((p=strstr(buf,"+CSQ:")))
               {
                  /* get the value of the argument */
                  status+=atoi(p+5); n++;
               }
            }
         }
      }

      /* check termination criteria */
      while (n<3 && difftime(time(NULL),To)<TimeOut);

      /* compute the average signal strength */
      if (n) status=(status*10)/n;
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to search the sky for the iridium constellation               */
/*------------------------------------------------------------------------*/
/**
   This function implements a method for ensuring that the sky is visible.
   The LBT is powered up and an attempt is made to register with the iridium
   system.  The power is cycled up, down, and then back up again in order to
   work around a bug in the Sebring and Daytona LBTs.  After being powered
   down for a long time (days), most units will not respond to AT commands
   on the first power-up.  However, the modem will respond if the LBT is
   powered down for 2 seconds and then powered up a second time.
   
      \begin{verbatim}
      input:

         port.........A structure that contains pointers to machine
                      dependent primitive IO functions.  See the comment
                      section of the SerialPort structure for details.  The
                      function checks to be sure this pointer is not NULL.
         
      output:

         This function returns a positive value if the sky is visible (ie.,
         the LBT is able to register).  Zero is returned if the registration
         attempt failed.  A negative return value indicates an invalid
         serial port.
      \end{verbatim}
*/
int IrSkySearch(const struct SerialPort *port)
{
   /* define the logging signature */
   static cc FuncName[] = "IrSkySearch()";

   /* initialize the return value */
   int status=-1;

   if (!port) 
   {
      /* create the message */
      static cc msg[]="NULL serial port.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }
   
   else
   {
      /* enable the modem */
      ModemEnable(19200);

      /* use modem registration as proof of sky visibility */
      status=IrModemRegister(port); 
       
      /* deactivate the modem serial port */
      ModemDisable();
   }
   
   return status;
}

