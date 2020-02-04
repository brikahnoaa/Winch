#ifndef APF9ICOM_H
#define APF9ICOM_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: apf9icom.c,v 1.15 2007/04/24 01:43:05 swift Exp $
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
 * $Log: apf9icom.c,v $
 * Revision 1.15  2007/04/24 01:43:05  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.14  2006/04/21 13:45:38  swift
 * Changed copyright attribute.
 *
 * Revision 1.13  2006/03/24 23:37:55  swift
 * Eliminated conditional 3s pause in favor of unconditional 3 sec pause after
 * enable.
 *
 * Revision 1.12  2006/03/24 23:05:29  swift
 * Eliminated firmware kludge to fix nonresponsive LBT9522a modems.  This
 * problem was solved correctly with a hardware fix.
 *
 * Revision 1.11  2006/03/15 15:59:57  swift
 * Eliminate persistent trait of DUART_OPR variable.
 *
 * Revision 1.10  2006/02/28 16:35:48  swift
 * Implemented a kludge to fix nonresponsive LBT9522a modems.  After a hardware
 * fix is implemented and tested, this kludge should be eliminated.
 *
 * Revision 1.9  2006/01/16 18:34:31  swift
 * Fixed problems with implementation of na() and na_().
 *
 * Revision 1.8  2006/01/06 23:08:54  swift
 * Added dtr(), and dsr() primitives to the SerialPort interface for the modem port.
 *
 * Revision 1.7  2005/07/07 19:45:04  swift
 * Assert the DTR line when the iridum module is powered up.
 *
 * Revision 1.6  2005/06/14 18:57:19  swift
 * Modifications to initialize low-level UART hardware before VALID line is active.
 *
 * Revision 1.5  2005/02/22 21:56:51  swift
 * Clean-up and fix-up of GPS serial port code.
 *
 * Revision 1.4  2005/01/15 00:52:03  swift
 * Added a time delay of 500ms to GpsEnable() to allow time for the GPS to boot
 * up.
 *
 * Revision 1.3  2005/01/06 00:51:13  swift
 * Fixed bugs in DTR/DSR control.
 *
 * Revision 1.2  2004/12/30 17:09:20  swift
 * Added functions to manipulate pin 7 of the GPS port and pins 2,3 of the RF
 * switch port.
 *
 * Revision 1.1  2004/12/29 23:03:32  swift
 * Module for Iridium and GPS serial ports.
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define apf9icomChangeLog "$RCSfile: apf9icom.c,v $ $Revision: 1.15 $ $Date: 2007/04/24 01:43:05 $"

#include <serial.h>

/* prototypes for functions with external linkage */
void   GpsDisable(void);
void   GpsEnable(unsigned int BaudRate);
size_t GpsFifoLen(void);
size_t GpsFifoSize(void);
int    GpsPin7Assert(void);
int    GpsPin7Clear(void);
void   InitDUart1(void);
int    ModemCd(void);
void   ModemDisable(void);
int    ModemDsr(void);
int    ModemDtrAssert(void);
int    ModemDtrClear(void);
void   ModemEnable(unsigned int BaudRate);
size_t ModemFifoLen(void);
size_t ModemFifoSize(void);
int    RfSwitchPin2Assert(void);
int    RfSwitchPin2Clear(void);
int    RfSwitchPin3Assert(void);
int    RfSwitchPin3Clear(void);

/* extern references to expansion serial ports */
extern const struct SerialPort modem,gps;

#endif /* APF9ICOM_H */

#include <apf9.h>
#include <assert.h>
#include <fifo.h>
#include <intrpt.h>
#include <logger.h>
#include <psd835.h>
#include <xa.h>

/* prototypes for functions with static linkage */
static unsigned char GetBaudCode(unsigned int baud);
static int GpsGetb(unsigned char *byte);
static int GpsIflush(void);
static int GpsPutb(unsigned char c);
static int ModemGetb(unsigned char *byte);
static int ModemIflush(void);
static int ModemPutb(unsigned char c);
static int ModemDtr(int state);
static int na(void);
static int na_(int state);

/* define a fifo buffer for the Modem serial port */
persistent static far unsigned char ModemFifoBuf[2048];

/* define a Fifo object for the Modem serial port */
static struct Fifo ModemFifo ={ModemFifoBuf, sizeof(ModemFifoBuf), 0, 0, 0, 0};
 
/* define a serial port for the Modem interface */
const struct SerialPort modem = {ModemGetb, ModemPutb, ModemIflush, ModemIflush, na, na, ModemCd, na_, na, ModemDtr, ModemDsr};

/* define a fifo buffer for the Gps serial port */
persistent static far unsigned char GpsFifoBuf[1024];
 
/* define a Fifo object for the Gps serial port */
static struct Fifo GpsFifo ={GpsFifoBuf, sizeof(GpsFifoBuf), 0, 0, 0, 0};

/* define a serial port for the Gps interface */
const struct SerialPort gps = {GpsGetb, GpsPutb, GpsIflush, GpsIflush, na, na, na, na_, na, na_, na};

/* define a mirror of the DUART's output register (OPR) */
static unsigned char DUART_OPR=0xff;

/* addresses for DUART read-registers */
static unsigned char DUART_MRA  @ 0xef0; // READ, WRITE
static unsigned char DUART_SRA  @ 0xef1;
static unsigned char DUART_RXA  @ 0xef3;
static unsigned char DUART_IPCR @ 0xef4;
static unsigned char DUART_ISR  @ 0xef5;
static unsigned char DUART_CTU  @ 0xef6;
static unsigned char DUART_CTL  @ 0xef7;
static unsigned char DUART_MRB  @ 0xef8; // READ, WRITE
static unsigned char DUART_SRB  @ 0xef9;
static unsigned char DUART_RXB  @ 0xefb;
static unsigned char DUART_IPR  @ 0xefd; // read

/* addresses for DUART write-registers */
static unsigned char DUART_CSRA @ 0xef1;
static unsigned char DUART_CRA  @ 0xef2;
static unsigned char DUART_TXA  @ 0xef3;
static unsigned char DUART_ACR  @ 0xef4;
static unsigned char DUART_IMR  @ 0xef5;
static unsigned char DUART_CTPU @ 0xef6;
static unsigned char DUART_CTPL @ 0xef7;
static unsigned char DUART_CSRB @ 0xef9;
static unsigned char DUART_CRB  @ 0xefa;
static unsigned char DUART_TXB  @ 0xefb;
static unsigned char DUART_OPCR @ 0xefd; // output configuration
static unsigned char DUART_SOPR @ 0xefe; // set ouput bits
static unsigned char DUART_ROPR @ 0xeff; // reset output bits
 
/* initialize the interrupt vector for DUART1 (Iridium/GPS ports) */
ROM_VECTOR(IV_EX0, TelIntr, IV_PSW);

/*------------------------------------------------------------------------*/
/* function to get the baud-rate code for a given baud rate               */
/*------------------------------------------------------------------------*/
/**
   This function returns the baud-rate code from Table 5 of the data sheet
   for the SC28L92 data sheet.  The return values are correct only if
   MR0[0]=0 and ACR[7]=1 so this function depends on particular values
   specified in ModemEnable() and GpsEnable().
*/
static unsigned char GetBaudCode(unsigned int baud)
{
   /* define the logging signature */
   static cc FuncName[] = "GetBaudCode()";

   unsigned char c;
   
   switch(baud)
   {
      case   300: {c = 0x44; break;}
      case   600: {c = 0x55; break;}
      case  1200: {c = 0x66; break;}
      case  2400: {c = 0x88; break;}
      case  4800: {c = 0x99; break;}
      case  9600: {c = 0xbb; break;}
      case 19200: {c = 0xcc; break;}
      default:
      {
         /* create the message */
         static cc format[]="Unsupported baud rate: %u\n";
         
         /* log the message */
         LogEntry(FuncName,format,baud);

         /* set the baud to be 9600 */
         c=0xbb;
      }
   }
   
   return c;
}

/*------------------------------------------------------------------------*/
/* function to return number of bytes currently stored in the GPS fifo    */
/*------------------------------------------------------------------------*/
/**
   This function returns the number of bytes currently stored in the GPS
   fifo.  The returned value should be regarded as merely a lower-bound
   estimate since interrupt-driven input from the GPS port can (without
   notice) increase the number of stored bytes.
*/
size_t GpsFifoLen(void)
{
   return GpsFifo.length;
}

/*------------------------------------------------------------------------*/
/* function to return the maximum number of bytes the GPS fifo can store  */
/*------------------------------------------------------------------------*/
/**
   This function returns the maximum number of bytes that the GPS fifo can
   store without overflowing.  
*/
size_t GpsFifoSize(void)
{
   return GpsFifo.size;
}

/*------------------------------------------------------------------------*/
/* function to disable and power-down the gps serial port                 */
/*------------------------------------------------------------------------*/
/**
   This function disables and powers-down the gps serial port.
*/
void GpsDisable(void)
{
   /* stack-check assertion */
   assert(StackOk());

   /* pet the watchdog */
   WatchDog();

   EX0=0;
   DUART_CRB = 0xa;                 // disable tx and rx
   DUART_CRB = 0x20;                // reset receiver
   DUART_CRB = 0xa;                 // delay between misc commands
   DUART_CRB = 0x30;                // reset transmitter
   DUART_CRB = 0xa;                 // delay between misc commands
   DUART_CRB = 0x40;                // reset error status

   /* disable GPS power circuitry and Tx-enable line */
   DUART_SOPR = 0x06; DUART_OPR |= 0x06;
}

/*------------------------------------------------------------------------*/
/* function to enable and power-up the gps serial port                    */
/*------------------------------------------------------------------------*/
/**
   This function enables and powers-up the gps serial port.
*/
void GpsEnable(unsigned int BaudRate)
{
   /* stack-check assertion */
   assert(StackOk());

   /* pet the watchdog */
   WatchDog();
   
   /* Gps initialization */
   DUART_CRB = 0xa;                 // disable tx and rx
   DUART_CRB = 0x20;                // reset receiver
   DUART_CRB = 0xa;                 // delay between misc commands
   DUART_CRB = 0x30;                // reset transmitter
   DUART_CRB = 0xa;                 // delay between misc commands
   DUART_CRB = 0x40;                // reset error status
   DUART_CRB = 0xa;                 // delay between misc commands
   DUART_CRB = 0xc0;                // disable timeout mode
   DUART_CRB = 0xa;                 // delay between misc commands
   DUART_CRB = 0xb0;                // set MR pointer to 0
   DUART_MRB = 0;                   // mode register 0
   DUART_MRB = 0x13;                // mode register 1: 8 data bits, no parity
   DUART_MRB = 7;                   // mode register 2: one stop bit
   DUART_CSRB = GetBaudCode(BaudRate);
   DUART_CRB = 0x55;                // enable TX and RX

   /* activate the GPS power circuitry */
   DUART_ROPR = 0x02; DUART_OPR &= ~(0x02); Wait(1);

   /* activate the Tx-enable gate */
   DUART_ROPR = 0x06; DUART_OPR &= ~(0x06); EX0=1; Wait(500);

   flush(&GpsFifo);
}

/*------------------------------------------------------------------------*/
/* function to read a byte from the Gps FIFO queue                        */
/*------------------------------------------------------------------------*/
/**
   This function reads one byte from the FIFO queue of the Gps serial
   port.  

      \begin{verbatim}
      output:
         byte...The next byte from the Gps FIFO queue.

         This function returns a positive number on success and zero on
         failure.  A negative return value indicates an exceptional
         condition such as a NULL function argument.
      \end{verbatim}
*/
static int GpsGetb(unsigned char *byte)
{
   /* define the logging signature */
   static cc FuncName[] = "GpsGetb()";

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
      status=pop(&GpsFifo,byte);
      
      /* enable interrupts */
      ei();
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to flush the Gps's Rx FIFO queue                              */
/*------------------------------------------------------------------------*/
/**
   This function flushes the Rx FIFO queue of the Gps serial port.  This
   function returns a positive value on success and zero on failure.  A
   negative return value indicates an invalid or corrupt Fifo object.
*/
static int GpsIflush(void)
{
   int status=0;
   
   /* stack-check assertion */
   assert(StackOk());

   /* pet the watch dog */
   WatchDog();

   /* flush the Gps serial port's fifo buffer */
   di(); status=flush(&GpsFifo); ei();
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to assert pin 7 of the APF9's GPS header (H18)                */
/*------------------------------------------------------------------------*/
/**
   This function asserts pin 7 of the APF9's GPS header (H18); pin 7 of H18
   will measure +3.3V.  It returns a positive value that, at present, has no
   meaning.
*/
int GpsPin7Assert(void)
{
   /* reset the 0x20 bit of the SOPR register */
   DUART_SOPR = 0x20; DUART_OPR |= 0x20;

   return 1;
}

/*------------------------------------------------------------------------*/
/* function to clear pin 7 of the APF9's RF switch header (H19)          */
/*------------------------------------------------------------------------*/
/**
   This function clears pin 7 of the APF9's RF switch header (H19); pin 7 of
   H18 will measure 0V.  It returns a positive value that, at present,
   has no meaning.
*/
int GpsPin7Clear(void)
{
   /* reset the 0x20 bit of the ROPR register */
   DUART_ROPR = 0x20; DUART_OPR &= ~(0x20);

   return 1;
}

/*------------------------------------------------------------------------*/
/* function to write a byte to the Gps serial port                        */
/*------------------------------------------------------------------------*/
/**
   This function writes a byte to the Gps serial port.  This function
   returns the number of bytes written to the serial port.
*/ 
static int GpsPutb(unsigned char c)
{
   /* initialize the return value */
   int status = 0;
   
   /* stack-check assertion */
   assert(StackOk());
      
   /* pet the watch dog */
   WatchDog();

   /* check the status register to see if Tx is ready */
   if (DUART_SRB&0x04) {DUART_TXB=c; status=1;}
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to initialize DUART1 of the APF9                              */
/*------------------------------------------------------------------------*/
void InitDUart1(void)
{
   /* disable communications via Modem and Gps */
   EX0=0;
   
   /* BRG set 2, timer (not used): clk / 16 */
   DUART_ACR = 0xf0;

   /* enable RX interrupt both UARTS */
   DUART_IMR = 0x22;

   /* initialize all of the UART's output pins */
   DUART_SOPR = 0xff; DUART_OPR = 0xff;
}

/*------------------------------------------------------------------------*/
/* function to return the state of the DCD line of modem                  */
/*------------------------------------------------------------------------*/
/**
   This function retuns the state of the DCD line of the modem serial port.
   If DCD is asserted, pin 7 of H15 will measure +5V. This function returns
   1 if DCD is asserted; zero is returned if DCD is cleared.
*/
int ModemCd(void)
{
   /* read the input port control register */
   int status = (DUART_IPCR&0x02) ? 0 : 1;

   return status;
}

/*------------------------------------------------------------------------*/
/* function to disable and power-down the modem serial port               */
/*------------------------------------------------------------------------*/
/**
   This function disables and powers-down the modem serial port.
*/
void ModemDisable(void)
{
   /* stack-check assertion */
   assert(StackOk());

   /* pet the watchdog */
   WatchDog();
   
   DUART_CRA = 0xa;  // disable tx and rx
   DUART_CRA = 0x20; // reset receiver
   DUART_CRA = 0xa;  // delay between misc commands
   DUART_CRA = 0x30; // reset transmitter
   DUART_CRA = 0xa;  // delay between misc commands
   DUART_CRA = 0x40; // reset error status
   
   /* power-down modem port */
   Psd835PortAClear(IRIDIUM_PON); Wait(250); ModemDtrAssert();
 }

/*------------------------------------------------------------------------*/
/* function to return the state of the DSR line of modem                  */
/*------------------------------------------------------------------------*/
/**
   This function retuns the state of the DSR line of the modem serial port.
   If DSR is asserted, pin 4 of H15 will measure +5V. This function returns
   1 if DSR is asserted; zero is returned if DSR is cleared.
*/
int ModemDsr(void)
{
   /* read the input port control register */
   int status = (DUART_IPCR&0x01) ? 0 : 1;

   return status;
}

/*------------------------------------------------------------------------*/
/* function to assert the DTR line of the modem serial port               */
/*------------------------------------------------------------------------*/
/**
   This function asserts the DTR line of the modem serial port; pin 6 of H17
   will measure +5V.  A positive value is returned but the return value has
   no meaning at this time.  
*/
int ModemDtrAssert(void)
{
   /* set the 0x01 bit of the SOPR register */
   DUART_SOPR = 0x01; DUART_OPR |= 0x01;

   return 1;
}

/*------------------------------------------------------------------------*/
/* function to clear the DTR line of the com 1 serial port                */
/*------------------------------------------------------------------------*/
/**
   This function clears the DTR line of the modem serial port; pin 6 of H17
   will measure -5V.  A positive value is returned but the return value has
   no meaning at this time.
*/
int ModemDtrClear(void)
{
   /* reset the 0x01 bit of the ROPR register */
   DUART_ROPR = 0x01; DUART_OPR &= ~(0x01);

   return 1;
}

/*------------------------------------------------------------------------*/
/* function to enable and power-up the modem serial port                  */
/*------------------------------------------------------------------------*/
/**
   This function enables and powers-up the modem serial port.
*/
void ModemEnable(unsigned int BaudRate)
{
   /* stack-check assertion */
   assert(StackOk());

   /* pet the watchdog */
   WatchDog();
   
   /* Modem initialization */
   DUART_CRA = 0xa;                 // disable tx and rx
   DUART_CRA = 0x20;                // reset receiver
   DUART_CRA = 0xa;                 // delay between misc commands
   DUART_CRA = 0x30;                // reset transmitter
   DUART_CRA = 0xa;                 // delay between misc commands
   DUART_CRA = 0x40;                // reset error status
   DUART_CRA = 0xa;                 // delay between misc commands
   DUART_CRA = 0xc0;                // disable timeout mode
   DUART_CRA = 0xa;                 // delay between misc commands
   DUART_CRA = 0xb0;                // set MR pointer to 0
   DUART_MRA = 0;                   // mode register 0
   DUART_MRA = 0x13;                // mode register 1: 8 data bits, no parity
   DUART_MRA = 7;                   // mode register 2: one stop bit
   DUART_CSRA = GetBaudCode(BaudRate);
   DUART_CRA = 0x55;                // enable TX and RX

   /* make sure the DTR line is asserted when the port is powered up */
   ModemDtrAssert();
   
   /* power-up the modem port */
   Psd835PortASet(IRIDIUM_PON);

   /* allow time for 2.7s reset LBT EXT_ON_OFF pin and enable interrupts */
   Wait(2595); EX0=1; Wait(5);

   /* flush the modem fifo */
   flush(&ModemFifo);
}

/*------------------------------------------------------------------------*/
/* function to return number of bytes currently stored in the modem fifo  */
/*------------------------------------------------------------------------*/
/**
   This function returns the number of bytes currently stored in the modem
   fifo.  The returned value should be regarded as merely a lower-bound
   estimate since interrupt-driven input from the modem port can (without
   notice) increase the number of stored bytes.
*/
size_t ModemFifoLen(void)
{
   return ModemFifo.length;
}

/*------------------------------------------------------------------------*/
/* function to return the maximum number of bytes the Modem fifo can store  */
/*------------------------------------------------------------------------*/
/**
   This function returns the maximum number of bytes that the Modem fifo can
   store without overflowing.  
*/
size_t ModemFifoSize(void)
{
   return ModemFifo.size;
}

/*------------------------------------------------------------------------*/
/* function to read a byte from the Modem FIFO queue                       */
/*------------------------------------------------------------------------*/
/**
   This function reads one byte from the FIFO queue of the Modem serial
   port.  

      \begin{verbatim}
      output:
         byte...The next byte from the Modem FIFO queue.

         This function returns a positive number on success and zero on
         failure.  A negative return value indicates an exceptional
         condition such as a NULL function argument.
      \end{verbatim}
*/
static int ModemGetb(unsigned char *byte)
{
   /* define the logging signature */
   static cc FuncName[] = "ModemGetb()";

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
      status=pop(&ModemFifo,byte);
      
      /* enable interrupts */
      ei();
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to flush the Modem's Rx FIFO queue                            */
/*------------------------------------------------------------------------*/
/**
   This function flushes the Rx FIFO queue of the Modem serial port.  This
   function returns a positive value on success and zero on failure.  A
   negative return value indicates an invalid or corrupt Fifo object.
*/
static int ModemIflush(void)
{
   int status=0;
   
   /* stack-check assertion */
   assert(StackOk());

   /* pet the watch dog */
   WatchDog();

   /* flush the Modem serial port's fifo buffer */
   di(); status=flush(&ModemFifo); ei();
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to write a byte to the Modem serial port                      */
/*------------------------------------------------------------------------*/
/**
   This function writes a byte to the Modem serial port.  This function
   returns the number of bytes written to the serial port.
*/ 
static int ModemPutb(unsigned char c)
{
   /* initialize the return value */
   int status = 0;
   
   /* stack-check assertion */
   assert(StackOk());
      
   /* pet the watch dog */
   WatchDog();

   /* check the status register to see if Tx is ready */
   if (DUART_SRA&0x04) {DUART_TXA=c; status=1;}
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to assert/clear the RTS signal of Modem                       */
/*------------------------------------------------------------------------*/
/**
   This function is used to assert or clear the RTS signal for Modem.  To
   assert the RTS line then set the argument to be positive.  To clear the
   RTS line then set the argument to be zero or negative.  This function
   returns a positive value on success; otherwise zero is returned.
*/
static int ModemDtr(int state)
{
   int status = (state>0) ? ModemDtrAssert() : ModemDtrClear();

   return status;
}

/*------------------------------------------------------------------------*/
/* function to indicate that a serial port abstractionis NOT AVAILABLE    */
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
/* function to indicate that a serial port abstractionis NOT AVAILABLE    */
/*------------------------------------------------------------------------*/
/**
   This function is used to fulfill the requirements of the serial port
   abstraction, In the case where the serial port lacks the ability to
   perform a given function, this function should be substituted in the
   initialization of the SerialPort object.
*/
static int na_(int state)
{
   return -1;
}

/*------------------------------------------------------------------------*/
/* function to assert pin 2 of the APF9's RF switch header (H19)          */
/*------------------------------------------------------------------------*/
/**
   This function asserts pin 2 of the APF9's RF switch header (H19).  It
   returns a positive value that, at present, has no meaning.
*/
int RfSwitchPin2Assert(void)
{
   /* set the 0x08 bit of the ROPR register */
   DUART_ROPR = 0x08; DUART_OPR &= ~(0x08);

   return 1;
}

/*------------------------------------------------------------------------*/
/* function to clear pin 2 of the APF9's RF switch header (H19)          */
/*------------------------------------------------------------------------*/
/**
   This function clears pin 2 of the APF9's RF switch header (H19).  It
   returns a positive value that, at present, has no meaning.
*/
int RfSwitchPin2Clear(void)
{
   /* reset the 0x08 bit of the SOPR register */
   DUART_SOPR = 0x08; DUART_OPR |= 0x08;

   return 1;
}

/*------------------------------------------------------------------------*/
/* function to assert pin 3 of the APF9's RF switch header (H19)          */
/*------------------------------------------------------------------------*/
/**
   This function asserts pin 3 of the APF9's RF switch header (H19).  It
   returns a positive value that, at present, has no meaning.
*/
int RfSwitchPin3Assert(void)
{
   /* set the 0x10 bit of the ROPR register */
   DUART_ROPR = 0x10; DUART_OPR &= ~(0x10);

   return 1;
}

/*------------------------------------------------------------------------*/
/* function to clear pin 3 of the APF9's RF switch header (H19)          */
/*------------------------------------------------------------------------*/
/**
   This function clears pin 3 of the APF9's RF switch header (H19).  It
   returns a positive value that, at present, has no meaning.
*/
int RfSwitchPin3Clear(void)
{
   /* reset the 0x10 bit of the SOPR register */
   DUART_SOPR = 0x10; DUART_OPR |= 0x10;

   return 1;
}

/*------------------------------------------------------------------------*/
/* interrupt handler for the expansion serial ports gps \& telemetry      */
/*------------------------------------------------------------------------*/
/**
   This is an interrupt handler for the telemetry and GPS serial ports of
   the APF9 controller.
*/
interrupt void TelIntr(void)
{
   /* define some bit masks (see SC28L92 data sheet) */
   #define RXRDYA 0x02
   #define RXRDYB 0x20

   /* read the SC28L92's interrupt status register */
   unsigned char byte,status = DUART_ISR;

   /* read the byte from modem if it's available */
   if (status&RXRDYA) {byte=DUART_RXA; push(&ModemFifo,byte);}

   /* read the byte from gps if it's available */
   if (status&RXRDYB) {byte=DUART_RXB; push(&GpsFifo,byte);}
   
   #undef RXRDYA 
   #undef RXRDYB 
}
