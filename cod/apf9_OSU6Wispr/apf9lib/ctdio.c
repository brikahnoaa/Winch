#ifndef CTDIO_H
#define CTDIO_H (0x0008U)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: ctdio.c,v 1.26.2.1 2008/09/11 19:40:29 dbliudnikas Exp $
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
 * $Log: ctdio.c,v $
 * Revision 1.26.2.1  2008/09/11 19:40:29  dbliudnikas
 * Update for Webb development system: line length issue.
 *
 * Revision 1.26  2008/07/14 17:08:27  swift
 * Modify state of ModeControl pin.
 *
 * Revision 1.25  2007/04/24 01:43:05  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.24  2006/10/23 16:49:13  swift
 * Modifications to code-sequencing in CtdPtSample() to avoid premature
 * activation of sample.
 *
 * Revision 1.23  2006/10/11 21:07:18  swift
 * Changed handling of the mode-pin signal during CP mode.
 *
 * Revision 1.22  2006/08/17 21:17:48  swift
 * MOdifications to allow better logging control.
 *
 * Revision 1.21  2006/04/21 13:45:38  swift
 * Changed copyright attribute.
 *
 * Revision 1.20  2005/10/25 17:01:01  swift
 * Changes induced by SBE41CP-IDO.
 *
 * Revision 1.19  2005/08/17 21:53:57  swift
 * Minor modification of Tx pin states during STP samples and power samples.
 *
 * Revision 1.18  2005/08/14 23:37:56  swift
 * Changed timing for more robust P-only sampling.
 *
 * Revision 1.17  2005/08/12 19:49:39  swift
 * Fixed more timing issues with P-only sampling.
 *
 * Revision 1.16  2005/08/11 16:32:25  swift
 * Modifications to fix a timing-related bug when requesting a P-only
 * measurement while the ctd is in CP mode.
 *
 * Revision 1.15  2005/06/22 19:23:50  swift
 * Fixed bugs caused by reducing use of TXD0 manipulations.
 *
 * Revision 1.14  2005/06/21 14:35:18  swift
 * Reduced manipulation of TXD0 and TXD1.
 *
 * Revision 1.13  2005/05/13 23:24:57  swift
 * Modifications to increase the baud rate of the 20ma serial loop from
 * 4800baud to 9600baud.
 *
 * Revision 1.12  2005/01/06 00:48:30  swift
 * Alphabetized include directives and eliminated use of trace() functionality.
 *
 * Revision 1.11  2004/12/29 23:04:36  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.10  2004/04/26 21:40:15  swift
 * Moved SetBaud() to apf9.c module.
 *
 * Revision 1.9  2004/04/23 23:42:49  swift
 * Changes to work around deficiencies in the XA serial port implementation.
 *
 * Revision 1.8  2004/04/14 20:48:21  swift
 * Revision E of the APF9 made some hardware changes to earlier versions.  The
 * PSD935 was replaced by a PSD835 and the 20mA serial loop was changed to be a
 * real serial IO port.
 *
 * Revision 1.7  2003/12/20 19:44:34  swift
 * Increased the size of the CTD serial port fifo from 80 bytes to 32760 bytes
 * to allow large blocks of data to be received.  Added the CtdActiveIo()
 * function.
 *
 * Revision 1.6  2003/11/20 18:59:35  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.5  2003/11/12 22:36:53  swift
 * Added CtdPower() function.
 *
 * Revision 1.4  2003/07/19 22:38:26  swift
 * Modifications that reflect the refactorization and remodeling of the Psd835 ports.
 *
 * Revision 1.3  2003/07/06 00:07:56  swift
 * Changed 'SetBaud()' to have static linkage since this compilation unit is
 * the only one that needs to use this function.
 *
 * Revision 1.2  2003/07/05 21:21:29  swift
 * Added a function 'CtdInitiateSample()' to initiate a sample and receive the
 * response from the CTD serial port.
 *
 * Revision 1.1  2003/07/03 22:41:57  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define ctdioChangeLog "$RCSfile: ctdio.c,v $  $Revision: 1.26.2.1 $ $Date: 2008/09/11 19:40:29 $"

#include <serial.h>
#include <seascan.h>

/* function prototypes */
int CtdAssertModePin(void);
int CtdAssertWakePin(void);
int CtdAssertTxPin(void);
int CtdClearModePin(void);
int CtdClearTxPin(void);
int CtdClearWakePin(void);
int CtdActiveIo(time_t timeout);
int CtdEnableIo(void);
int CtdDisableIo(void);
int CtdPower(unsigned char *VoltCnt, unsigned char *AmpCnt, time_t timeout);

/* declare the console serial port */
extern struct SerialPort ctdio;

#endif /* CTDIO_H */

#include <apf9.h>
#include <assert.h>
#include <fifo.h>
#include <intrpt.h>
#include <logger.h>
#include <lt1598ad.h>
#include <math.h>
#include <psd835.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <xa.h>

#define ETX  0x03 /* ETX character */
#define NUL  0x00 /* NULL character */

/* function prototypes for functions with static linkage */
static int CtdGetb(unsigned char *byte);
static int CtdIflush(void);
static int na(void);
static int CtdPutb(unsigned char c);
static int CtdPutb_CRLF(unsigned char c); /* CtdPutb WITHOUT CR/LF translation */

/* define a fifo buffer for the CTD serial port (XA serial port 0) */
persistent static far unsigned char CtdFifoBuf[51200U];

/* define a Fifo object for the CTD serial port (XA serial port 0) */
struct Fifo CtdFifo = {CtdFifoBuf, sizeof(CtdFifoBuf), 0, 0, 0, 0};

/* define a serial port for the CTD interface */
/* Use the CtdPutb_CRLF which does not add the undesired CR to LF */
struct SerialPort ctdio = {CtdGetb, CtdPutb_CRLF, CtdIflush, CtdIflush, na, na, na};

/* define some ascii characters special meaning */
#define LF 0x0a /* line feed */
#define CR 0x0d /* carriage return */

/* initialize an interrupt vector for the CTD serial port (XA serial port 0) */
ROM_VECTOR(IV_RI0, CtdRxInterrupt, IV_PSW);

static bit RX_CTD @ 0x398; /* p3.0 */
static bit TX_CTD @ 0x399; /* p3.1 */

/*------------------------------------------------------------------------*/
/* function to assert the APF9's CTD PTS/FP mode-select pin               */
/*------------------------------------------------------------------------*/
/**
   This function asserts the APF9's CTD PTS/FP mode-select pin.  This
   function returns a positive value which has no meaning at this time.
*/
int CtdAssertModePin(void)
{
   int status=1;
      
   /* stack-check assertion */
   assert(StackOk());
   
   /* pet the watch dog */
   WatchDog();

   /* assert the mode-pin */
   Psd835PortBSet(CTD_PTS_FP);
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to assert the APF9's CTD power-control pin                    */
/*------------------------------------------------------------------------*/
/**
   This function asserts the APF9's CTD power-control pin.  This function
   returns a positive value which has no meaning at this time.
*/
int CtdAssertWakePin(void)
{
   int status=1;
      
   /* stack-check assertion */
   assert(StackOk());

   /* pet the watch dog */
   WatchDog();

   /* assert the wake-pin */
   Psd835PortBSet(CTD_PWR_CTRL);

   return status;
}
 
/*------------------------------------------------------------------------*/
/* function to assert the APF9's Tx pin on the CTD serial port            */
/*------------------------------------------------------------------------*/
/**
   This function asserts the APF9's Tx pin on the CTD serial port.  This
   function returns a positive value which has no meaning at this time.
*/
int CtdAssertTxPin(void)
{
   int status=1;
   
   TXD0=1; Wait(2);

   return status;
}

/*------------------------------------------------------------------------*/
/* function to clear the APF9's Tx pin on the CTD serial port             */
/*------------------------------------------------------------------------*/
/**
   This function asserts the APF9's Tx pin on the CTD serial port.  This
   function returns a positive value which has no meaning at this time.
*/
int CtdClearTxPin(void)
{
   int status=1;
   
   TXD0=0; Wait(2);

   return status;
}

/*------------------------------------------------------------------------*/
/* function to clear the APF9's CTD PTS/FP mode-select pin                */
/*------------------------------------------------------------------------*/
/**
   This function clears the APF9's CTD PTS/FP mode-select pin.  This
   function returns a positive value which has no meaning at this time.
*/
int CtdClearModePin(void)
{
   int status=1;
   
   /* stack-check assertion */
   assert(StackOk());

   /* pet the watch dog */
   WatchDog();

   /* clear the mode-pin */
   Psd835PortBClear(CTD_PTS_FP);

   return status;
}

/*------------------------------------------------------------------------*/
/* function to clear the APF9's CTD power-control pin                     */
/*------------------------------------------------------------------------*/
/**
   This function clears the APF9's CTD power-control pin.  This function
   returns a positive value which has no meaning at this time.
*/
int CtdClearWakePin(void)
{
   int status=1;

   /* stack-check assertion */
   assert(StackOk());

   /* clear the wake-pin */
   Psd835PortBClear(CTD_PWR_CTRL);

   return status;
}

/*------------------------------------------------------------------------*/
/* function to determine if CTD IO is active                              */
/*------------------------------------------------------------------------*/
/**
   This function determines if the CTD serial port is active.  The serial
   port is defined to be active if the length of the CtdFifo changes within
   a user-specified time-out period.

      \begin{verbatim}
      input:
         timeout ... The CTD serial port is determined to be active if the
                     length of the CtdFifo changes within this length of
                     time.

      output:
         This function returns a positive number if the CTD serial port is
         determined to be active and zero otherwise.
      \end{verbatim}
   
*/
int CtdActiveIo(time_t timeout)
{
   int status=0;
   const long int length=CtdFifo.length;
   const long int boc=CtdFifo.BufOverflowCount;
   const time_t To=time(NULL);
      
   while (difftime(time(NULL),To)<timeout) 
   {
      if (CtdFifo.length!=length || CtdFifo.BufOverflowCount!=boc) {status=1; break;}
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to disable communications with CTD serial port                */
/*------------------------------------------------------------------------*/
/**
   This function disables communications with the CTD serial port (at 9600
   baud) and enables communications with the console serial port (at 9600
   baud).  Unfortunately, the XA serial ports 0 and 1 are not independent of
   each other.  They both use a common baud rate generator which means that
   if the two serial ports must use different baud rates then the two serial
   ports must be effectively multiplexed so that they aren't both receiving
   data at the same time.
*/
int CtdDisableIo(void)
{
   /* stack-check assertion */
   assert(StackOk());

   /* pet the watch dog */
   WatchDog();

   /* disable interrupts for the CTD interface (XA serial port 0) */
    ERI0=0; TXD0=0; RI0=0; 

   /* enable interrupts for the 28ma interface (XA serial port 1) */
   RI1=0; ERI1=1;
   
   return 1;
}

/*------------------------------------------------------------------------*/
// /* function to enable communications with CTD serial port                 */
/*------------------------------------------------------------------------*/
/**
   This function enables communications with the CTD serial port (at 9600
   baud) and disables communications with the console serial port (at 9600
   baud).  Unfortunately, the XA serial ports 0 and 1 are not independent of
   each other.  They both use a common baud rate generator which means that
   if the two serial ports must use different baud rates then the two serial
   ports must be effectively multiplexed so that they aren't both receiving
   data at the same time.
*/
int CtdEnableIo(void)
{
   /* stack-check assertion */
   assert(StackOk());

   /* pet the watch dog */
   WatchDog();

   /* disable interrupts for the 28ma interface (XA serial port 1) */
   ERI1=0; RI1=0;
   
   /* enable interrupts for the CTD interface (XA serial port 0) */
   TXD0=1; RI0=0; ERI0=1;
   
   return 1;
}


/*------------------------------------------------------------------------*/
/* measure power consumption during a full PT sample                     */
/*------------------------------------------------------------------------*/
/**
   This function measures the power consumption by the CTD during a full PT
   sample.

      \begin{verbatim}
      output:
         This function returns a positive value on success and zero on
         failure.  A negative value indicates an exceptional error.
      \end{verbatim}
*/
int CtdPower(unsigned char *VoltCnt, unsigned char *AmpCnt, time_t timeout)
{
   /* define the logging signature */
   static cc FuncName[] = "CtdPower()";
   unsigned char ibyte; int n=0,nSamp=50,nMax=200;
   float V=0.0,A=0.0,W=0.0;
   char allAs[50];
   char allVs[50];
   char SeascanSerno[SERNOSIZE];
   char rspStr[MAXLEN+1];
   int rspLen = 0; 
   float pup;            /* power up pressure */
            
   /* initialize the return value */
   int stat=1,status=-1,statusExit=-1;
  
/* stack-check assertion */
   assert(StackOk());

   /* pet the watchdog */
   WatchDog();

   /* validate the function arguments */
   if (!VoltCnt || !AmpCnt)
   {
      /* create the message */
      static cc msg[]="NULL function argument.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   else
   {
      /* Wake up the Seascan.  Enter command mode.  */
      status = SeascanEnterCmdMode(SeascanSerno, &pup);
   
      if (status==SeascanOk)
      {            
         ctdio.putb('-');/* Send the command to get PT */
          
         /* make the volt and current measurements into the sample-cycle */
         for (n=0; n<nSamp; n++)
         {
            allVs[n]=BatVoltsAd8(); 
            allAs[n]=BatAmpsAd8();
            Wait(40);
         }
      }
      if (debuglevel >4)
      {
         /* create the message */
         static cc format[]="Raw VA [%d %d %d %d %d %d %d %d %d %d]\n";
          /* log the message */
         LogEntry(FuncName,format,allVs[0], allVs[1], allVs[2], allVs[3], allVs[4], allVs[5], allVs[6], allVs[7], allVs[8], allVs[9]);
         LogEntry(FuncName,format,allAs[0], allAs[1], allAs[2], allAs[3], allAs[4], allAs[5], allAs[6], allAs[7], allAs[8], allAs[9]);
         LogEntry(FuncName,format,allVs[10], allVs[11], allVs[12], allVs[13], allVs[14], allVs[15], allVs[16], allVs[17], allVs[18], allVs[19]);
         LogEntry(FuncName,format,allAs[10], allAs[11], allAs[12], allAs[13], allAs[14], allAs[15], allAs[16], allAs[17], allAs[18], allAs[19]);
         LogEntry(FuncName,format,allVs[20], allVs[21], allVs[22], allVs[23], allVs[24], allVs[25], allVs[26], allVs[27], allVs[28], allVs[29]);
         LogEntry(FuncName,format,allAs[20], allAs[21], allAs[22], allAs[32], allAs[24], allAs[25], allAs[26], allAs[27], allAs[28], allAs[29]);
         LogEntry(FuncName,format,allVs[30], allVs[31], allVs[32], allVs[33], allVs[34], allVs[35], allVs[36], allVs[37], allVs[38], allVs[39]);
         LogEntry(FuncName,format,allAs[30], allAs[31], allAs[32], allAs[33], allAs[34], allAs[35], allAs[36], allAs[37], allAs[38], allAs[39]);
         LogEntry(FuncName,format,allVs[40], allVs[41], allVs[42], allVs[43], allVs[44], allVs[45], allVs[46], allVs[47], allVs[48], allVs[49]);
         LogEntry(FuncName,format,allAs[40], allAs[41], allAs[42], allAs[43], allAs[44], allAs[45], allAs[46], allAs[47], allAs[48], allAs[49]);
     }
      
      /* Read characters and echo back until ETX is detected */
      /* Loop iteration and wait makes > 2 second response time */
      for (n=0; n<nMax; n++)
      {
         if ( (stat = ctdio.getb(&ibyte)) >0 )
         {
            /* got a byte, echo the byte to the Seascan TD */
            ctdio.putb(ibyte); 
            /* If etx, terminate the string and break out of loop */
            if (ibyte == ETX)  { rspStr[rspLen]=NUL; break;}
            /* store the byte in the response string */
            rspStr[rspLen] = ibyte; if (rspLen<MAXLEN) rspLen++;
          }
          Wait(10);
      }
            
      /* Power down the Seascan.  */
      statusExit = SeascanExitCmdMode();   
   }

   /* log the status */
   if (status<=0)
   {
      /* create the message */
      static cc msg[]="No response from CTD.\n";
      /* log the message */
      LogEntry(FuncName,msg);
   }
   else if (debuglevel>=4 || (debugbits&CTDIO_H))
   {
      /* create the message */
      static cc format[]="Raw PT [%s]\n";
      /* log the message */
      LogEntry(FuncName,format,rspStr);
   }
   
   W=0;
   for (n=0; n<nSamp; n++)
   {
      /* convert counts to engineering quantities */
      A=Amps(allAs[n]); V=Volts(allVs[n]);
      if (A*V > W )
      {
         W = A*V;
         (*VoltCnt) =  allVs[n];
         (*AmpCnt) = allAs[n];
      }
   }
   /* check if the power consumption is loggable */
   if (debuglevel>=4 || (debugbits&CTDIO_H))
   {
      /* create the message */
      static cc format[]="CTD Power consumption [%dVCnt %dACnt]: "
         "%0.3fVolts * %0.3fAmps = %0.2f %0.2fWatts.\n";
      /* convert counts to engineering quantities */
      A=Amps(*AmpCnt); V=Volts(*VoltCnt);
     /* log the power consumption */
      LogEntry(FuncName,format,*VoltCnt,*AmpCnt,V,A,V*A,W);
   }
   return status;
}

/*------------------------------------------------------------------------*/
/* interrupt handler for input on the CTD serial interface                */
/*------------------------------------------------------------------------*/
/*
   This is the interrupt handler for data received by the serial port
   connected to the CTD interface.  The CTD serial port uses a nonblocking
   unbuffered IO model.  This interrupt handler does nothing except push the
   byte into the serial port's FIFO queue.
*/
interrupt void CtdRxInterrupt(void)    
{
   unsigned char byte, stat;

   /* pet the watchdog */
   WatchDog();

   /* clear the rx-ready flag */
   RI0 = 0;

   /* read the byte from serial port 0 */
   byte = S0BUF & 0x7f;
 
   /* read and reset the status of serial port 0 */
   stat = S0STAT & 0xe; S0STAT=0;
   
   /* add the byte to the fifo */
   push(&CtdFifo,byte);
} 

/*------------------------------------------------------------------------*/
/* function to read a byte from the CTD FIFO queue                        */
/*------------------------------------------------------------------------*/
/**
   This function reads one byte from the FIFO queue of the CTD serial
   port.  

      \begin{verbatim}
      output:
         byte...The next byte from the CTD FIFO queue.

         This function returns a positive number on success and zero on
         failure.  A negative return value indicates an exceptional
         condition such as a NULL function argument.
      \end{verbatim}
*/
static int CtdGetb(unsigned char *byte)
{
   /* define the logging signature */
   static cc FuncName[] = "CtdGetb()";

   int status=-1;
   
   /* stack-check assertion */
   assert(StackOk());
   
   /* pet the watch dog */
   WatchDog();

   /* validate the function parameter */
   if (!byte) 
   {
      /* create the message */
      static cc msg[]="NULL function argument.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   else
   {
      /* initialize the return value */
      *byte=0;
      
      /* disable interrupts to prevent the fifo from changing state */
      di();

      /* pop the next byte out of the fifo queue */
      status=pop(&CtdFifo,byte);
      
      /* enable interrupts */
      ei();

      /* protect against interrupts being turned off too much */
      Wait(2);
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to flush the CTD's Rx FIFO queue                              */
/*------------------------------------------------------------------------*/
/**
   This function flushes the Rx FIFO queue of the CTD serial port.  This
   function returns a positive value on success and zero on failure.  A
   negative return value indicates an invalid or corrupt Fifo object.
*/
static int CtdIflush(void)
{
   int status=0;
   
   /* stack-check assertion */
   assert(StackOk());

   /* pet the watch dog */
   WatchDog();

   /* flush the CTD serial port's fifo buffer */
   di(); status=flush(&CtdFifo); ei();
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function                                                               */
/*------------------------------------------------------------------------*/
/**
   This function is used to fulfill the requirements of the serial port
   abstraction, In the case where the serial port lacks the ability to
   perform a given function, this function should be substituted in the
   initialization of the SerialPort object.
*/
static int na(void)
{
   return -1;
}

/*------------------------------------------------------------------------*/
/* function to write a byte to the                                        */
/*------------------------------------------------------------------------*/
/**
   This function writes a byte to the CTD serial port (XA serial port 0) and
   also performs LF to CR/LF translation.  This function returns the number
   of bytes written to the serial port.
*/ 
static int CtdPutb(unsigned char c)
{
   /* initialize the return value */
   int status = 1;
   
   /* stack-check assertion */
   assert(StackOk());
      
   /* pet the watch dog */
   WatchDog();

   /* translate LF to CR/LF */
   if(c==LF) {S0BUF=CR; Wait(2); status++;}

   /* write the byte to the serial port */
   S0BUF=c; Wait(2);

   return status;
}

/*------------------------------------------------------------------------*/
/* function to write a byte to the CTD serial port WITHOUT CR/LF translation */
/*------------------------------------------------------------------------*/
/**
   This function writes a byte to the CTD serial port (XA serial port 0).
   This function returns the number of bytes written to the serial port.  
   The LF to CR/LF translation (in regular CtdPutb) has been removed for
   Seascan TD so LF can be detected and echoed back as required.
*/ 
static int CtdPutb_CRLF(unsigned char c)
{
   /* initialize the return value */
   int status = 1;
   
   /* stack-check assertion */
   assert(StackOk());
      
   /* pet the watch dog */
   WatchDog();

   /* write the byte to the serial port */
   S0BUF=c; Wait(2);

   return status;
}
