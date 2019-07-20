#ifndef APF9COM_H
#define APF9COM_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: apf9com.c,v 1.12 2008/07/14 17:06:13 swift Exp $
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
 * $Log: apf9com.c,v $
 * Revision 1.12  2008/07/14 17:06:13  swift
 * Increased the size of the IO buffers to facilitate Sbe41cp gateway mode.
 *
 * Revision 1.11  2007/06/08 02:16:56  swift
 * Fixed return values of some functions to report success.
 *
 * Revision 1.10  2007/04/24 01:43:05  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.9  2006/04/21 13:45:38  swift
 * Changed copyright attribute.
 *
 * Revision 1.8  2006/01/06 23:10:07  swift
 * Added rts(), cts(), dtr(), and dsr() primitives to the SerialPort interface.
 *
 * Revision 1.7  2005/06/14 18:57:19  swift
 * Modifications to initialize low-level UART hardware before VALID line is active.
 *
 * Revision 1.6  2005/01/06 00:50:42  swift
 * Fixed bugs in RTS/CTS control.
 *
 * Revision 1.5  2004/12/29 23:02:52  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.4  2004/07/14 22:49:28  swift
 * Modified SerialPort objects to be constant.
 *
 * Revision 1.3  2004/04/14 20:48:21  swift
 * Revision E of the APF9 made some hardware changes to earlier versions.  The
 * PSD935 was replaced by a PSD835 and the 20mA serial loop was changed to be a
 * real serial IO port.
 *
 * Revision 1.2  2003/11/20 18:59:35  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.1  2003/09/19 23:41:43  swift
 * Initial revision
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define apf9comChangeLog "$RCSfile: apf9com.c,v $ $Revision: 1.12 $ $Date: 2008/07/14 17:06:13 $"

#include <serial.h>

/* prototypes for functions with external linkage */
int  Com1Cts(void);
void Com1Disable(void);
void Com1Enable(unsigned int BaudRate);
int  Com1RtsAssert(void);
int  Com1RtsClear(void);
int  Com2Cts(void);
void Com2Disable(void);
void Com2Enable(unsigned int BaudRate);
int  Com2RtsAssert(void);
int  Com2RtsClear(void);
void InitDUart2(void);
void RMsg(void);

/* extern references to expansion serial ports */
extern const struct SerialPort com1,com2;

#endif /* APF9COM_H */

#include <xa.h>
#include <intrpt.h>
#include <logger.h>
#include <fifo.h>
#include <assert.h>
#include <apf9.h>
#include <psd835.h>

/* prototypes for functions with static linkage */
static int Com1(int Baudrate);
static int Com1Getb(unsigned char *byte);
static int Com1Iflush(void);
static int Com1Putb(unsigned char c);
static int Com1Rts(int state);
static int Com2(int Baudrate);
static int Com2Getb(unsigned char *byte);
static int Com2Iflush(void);
static int Com2Putb(unsigned char c);
static int Com2Rts(int state);
static unsigned char GetBaudCode(unsigned int baud);
static int na(void);

/* define a fifo buffer for the Com1 serial port */
persistent static far unsigned char Com1FifoBuf[10240U];

/* define a Fifo object for the Com1 serial port */
static struct Fifo Com1Fifo ={Com1FifoBuf, sizeof(Com1FifoBuf), 0, 0, 0, 0};

/* define a serial port for the Com1 interface */
const struct SerialPort com1 = {Com1Getb, Com1Putb, Com1Iflush, Com1Iflush, na, na, na, Com1Rts, Com1Cts, Com1, na};

/* define a fifo buffer for the Com2 serial port */
persistent static far unsigned char Com2FifoBuf[10240U];
 
/* define a Fifo object for the Com2 serial port */
static struct Fifo Com2Fifo ={Com2FifoBuf, sizeof(Com2FifoBuf), 0, 0, 0, 0};

/* define a serial port for the Com2 interface */
const struct SerialPort com2 = {Com2Getb, Com2Putb, Com2Iflush, Com2Iflush, na, na, na, Com2Rts, Com2Cts, Com2, na};

/* addresses for DUART read-registers */
static unsigned char DUART_MRA  @ 0xee0; // READ, WRITE
static unsigned char DUART_SRA  @ 0xee1;
static unsigned char DUART_RXA  @ 0xee3;
static unsigned char DUART_IPCR @ 0xee4;
static unsigned char DUART_ISR  @ 0xee5;
static unsigned char DUART_CTU  @ 0xee6;
static unsigned char DUART_CTL  @ 0xee7;
static unsigned char DUART_MRB  @ 0xee8; // READ, WRITE
static unsigned char DUART_SRB  @ 0xee9;
static unsigned char DUART_RXB  @ 0xeeb;
static unsigned char DUART_IPR  @ 0xeed; // read

/* addresses for DUART write-registers */
static unsigned char DUART_CSRA @ 0xee1;
static unsigned char DUART_CRA  @ 0xee2;
static unsigned char DUART_TXA  @ 0xee3;
static unsigned char DUART_ACR  @ 0xee4;
static unsigned char DUART_IMR  @ 0xee5;
static unsigned char DUART_CTPU @ 0xee6;
static unsigned char DUART_CTPL @ 0xee7;
static unsigned char DUART_CSRB @ 0xee9;
static unsigned char DUART_CRB  @ 0xeea;
static unsigned char DUART_TXB  @ 0xeeb;
static unsigned char DUART_OPCR @ 0xeed; // output configuration
static unsigned char DUART_SOPR @ 0xeee; // set ouput bits
static unsigned char DUART_ROPR @ 0xeef; // reset output bits

/* initialize the interrupt vector for DUART2 (serial expansion) */
ROM_VECTOR(IV_EX1, ComIntr, IV_PSW);

/*------------------------------------------------------------------------*/
/* interrupt handler for the expansion serial ports com 1 \& 2            */
/*------------------------------------------------------------------------*/
/**
   This is an interrupt handler for the expansion serial ports of the APF9
   controller. 
*/
interrupt void ComIntr(void)
{
   /* define some bit masks (see SC28L92 data sheet) */
   #define RXRDYA 0x02
   #define RXRDYB 0x20

   /* read the SC28L92's interrupt status register */
   unsigned char byte,status = DUART_ISR;

   /* read the byte from come if it's available */
   if (status&RXRDYA) {
		byte=DUART_RXA;
		push(&Com1Fifo,byte);
		
		//if (byte == '$') {
		//	RMsg();
		//}
	}

   /* read the byte from come if it's available */
   if (status&RXRDYB) {byte=DUART_RXB; push(&Com2Fifo,byte);}
   
   #undef RXRDYA 
   #undef RXRDYB 
}

/*------------------------------------------------------------------------*/
/* function to enable/disable Com1                                        */
/*------------------------------------------------------------------------*/
/**
   This function is used to enable or disable Com1.  To enable the com port
   then set the argument equal to the desired baud rate.  To disable the com
   port then set the argument less than or equal to zero.  This function
   returns a positive value on success; otherwise zero is returned.
*/
static int Com1(int BaudRate)
{
   if (BaudRate>0) Com1Enable(BaudRate);
   else Com1Disable();
   
   return 1;
}

/*------------------------------------------------------------------------*/
/* function to return the state of the CTS line of com1                   */
/*------------------------------------------------------------------------*/
/**
   This function retuns the state of the CTS line of the com1 serial port.
   If CTS is asserted, pin 4 of H15 will measure +5V. This function returns
   1 if CTS is asserted; zero is returned if CTS is cleared.
*/
int Com1Cts(void)
{
   /* read the input port control register */
   int status = (DUART_IPCR&0x01) ? 0 : 1;

   return status;
}

/*------------------------------------------------------------------------*/
/* function to disable and power-down the com1 serial port                */
/*------------------------------------------------------------------------*/
/**
   This function disables and powers-down the com1 serial port.
*/
void Com1Disable(void)
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

   /* power-down com1 */
   Psd835PortBClear(SERIAL1_ON); Wait(250); Com1RtsAssert();
}

/*------------------------------------------------------------------------*/
/* function to enable and power-up the com1 serial port                   */
/*------------------------------------------------------------------------*/
/**
   This function enables and powers-up the com1 serial port.
*/
void Com1Enable(unsigned int BaudRate)
{
   /* stack-check assertion */
   assert(StackOk());

   /* pet the watchdog */
   WatchDog();
   
   /* Com1 initialization */
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

   Com1RtsClear(); Psd835PortBSet(SERIAL1_ON); EX1=1; Wait(2);
   
   flush(&Com1Fifo);
}

/*------------------------------------------------------------------------*/
/* function to read a byte from the Com1 FIFO queue                       */
/*------------------------------------------------------------------------*/
/**
   This function reads one byte from the FIFO queue of the Com1 serial
   port.  

      \begin{verbatim}
      output:
         byte...The next byte from the Com1 FIFO queue.

         This function returns a positive number on success and zero on
         failure.  A negative return value indicates an exceptional
         condition such as a NULL function argument.
      \end{verbatim}
*/
static int Com1Getb(unsigned char *byte)
{
   /* define the logging signature */
   static cc FuncName[] = "Com1Getb()";

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
      status=pop(&Com1Fifo,byte);
      
      /* enable interrupts */
      ei();
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to flush the Com1's Rx FIFO queue                             */
/*------------------------------------------------------------------------*/
/**
   This function flushes the Rx FIFO queue of the Com1 serial port.  This
   function returns a positive value on success and zero on failure.  A
   negative return value indicates an invalid or corrupt Fifo object.
*/
static int Com1Iflush(void)
{
   int status=0;
   
   /* stack-check assertion */
   assert(StackOk());

   /* pet the watch dog */
   WatchDog();

   /* flush the Com1 serial port's fifo buffer */
   di(); status=flush(&Com1Fifo); ei();
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to write a byte to the Com1 serial port                       */
/*------------------------------------------------------------------------*/
/**
   This function writes a byte to the Com1 serial port.  This function
   returns the number of bytes written to the serial port.
*/ 
static int Com1Putb(unsigned char c)
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
/* function to assert the RTS line of the com1 serial port                */
/*------------------------------------------------------------------------*/
/**
   This function asserts the RTS line of the com1 serial port; pin 6 of H15
   will measure +5V.  A positive value is returned but the return value has
   no meaning at this time.  
*/
int Com1RtsAssert(void)
{
   /* set the 0x01 bit of the SOPR register */
   DUART_SOPR = 0x01;

   return 1;
}

/*------------------------------------------------------------------------*/
/* function to clear the RTS line of the com 1 serial port                */
/*------------------------------------------------------------------------*/
/**
   This function clears the RTS line of the com1 serial port; pin 6 of H15
   will measure -5V.  A positive value is returned but the return value has
   no meaning at this time.
*/
int Com1RtsClear(void)
{
   /* reset the 0x01 bit of the ROPR register */
   DUART_ROPR = 0x01;

   return 1;
}

/*------------------------------------------------------------------------*/
/* function to assert/clear the RTS signal of Com1                        */
/*------------------------------------------------------------------------*/
/**
   This function is used to assert or clear the RTS signal for Com1.  To
   assert the RTS line then set the argument to be positive.  To clear the
   RTS line then set the argument to be zero or negative.  This function
   returns a positive value on success; otherwise zero is returned.
*/
static int Com1Rts(int state)
{
   int status = (state>0) ? Com1RtsAssert() : Com1RtsClear();

   return status;
}

/*------------------------------------------------------------------------*/
/* function to enable/disable Com2                                        */
/*------------------------------------------------------------------------*/
/**
   This function is used to enable or disable Com2.  To enable the com port
   then set the argument equal to the desired baud rate.  To disable the com
   port then set the argument less than or equal to zero.  This function
   returns a positive value on success; otherwise zero is returned.
*/
static int Com2(int BaudRate)
{
   if (BaudRate>0) Com2Enable(BaudRate);
   else Com2Disable();
   
   return 1;
}

/*------------------------------------------------------------------------*/
/* function to return the state of the CTS line of com2                   */
/*------------------------------------------------------------------------*/
/**
   This function retuns the state of the CTS line of the com2 serial port.
   If CTS is asserted, pin 4 of H15 will measure +5V. This function returns
   1 if CTS is asserted; zero is returned if CTS is cleared.
*/
int Com2Cts(void)
{
   int status = (DUART_IPCR&0x02) ? 0 : 1;

   return status;
}

/*------------------------------------------------------------------------*/
/* function to disable and power-down the com2 serial port                */
/*------------------------------------------------------------------------*/
/**
   This function disables and powers-down the com2 serial port.
*/
void Com2Disable(void)
{
   /* stack-check assertion */
   assert(StackOk());

   /* pet the watchdog */
   WatchDog();
   
   DUART_CRB = 0xa;                 // disable tx and rx
   DUART_CRB = 0x20;                // reset receiver
   DUART_CRB = 0xa;                 // delay between misc commands
   DUART_CRB = 0x30;                // reset transmitter
   DUART_CRB = 0xa;                 // delay between misc commands
   DUART_CRB = 0x40;                // reset error status

   Psd835PortBClear(SERIAL2_ON); Wait(250); Com2RtsAssert();
}

/*------------------------------------------------------------------------*/
/* function to enable and power-up the com2 serial port                   */
/*------------------------------------------------------------------------*/
/**
   This function enables and powers-up the com2 serial port.
*/
void Com2Enable(unsigned int BaudRate)
{
   /* stack-check assertion */
   assert(StackOk());

   /* pet the watchdog */
   WatchDog();
   
   /* Com2 initialization */
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
   
   Com2RtsClear(); Psd835PortBSet(SERIAL2_ON); EX1=1; Wait(2);

   flush(&Com2Fifo);
}

/*------------------------------------------------------------------------*/
/* function to read a byte from the Com2 FIFO queue                       */
/*------------------------------------------------------------------------*/
/**
   This function reads one byte from the FIFO queue of the Com2 serial
   port.  

      \begin{verbatim}
      output:
         byte...The next byte from the Com2 FIFO queue.

         This function returns a positive number on success and zero on
         failure.  A negative return value indicates an exceptional
         condition such as a NULL function argument.
      \end{verbatim}
*/
static int Com2Getb(unsigned char *byte)
{
   /* define the logging signature */
   static cc FuncName[] = "Com2Getb()";

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
      status=pop(&Com2Fifo,byte);
      
      /* enable interrupts */
      ei();
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to flush the Com2's Rx FIFO queue                             */
/*------------------------------------------------------------------------*/
/**
   This function flushes the Rx FIFO queue of the Com2 serial port.  This
   function returns a positive value on success and zero on failure.  A
   negative return value indicates an invalid or corrupt Fifo object.
*/
static int Com2Iflush(void)
{
   int status=0;
   
   /* stack-check assertion */
   assert(StackOk());

   /* pet the watch dog */
   WatchDog();

   /* flush the Com2 serial port's fifo buffer */
   di(); status=flush(&Com2Fifo); ei();
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to write a byte to the Com2 serial port                       */
/*------------------------------------------------------------------------*/
/**
   This function writes a byte to the Com2 serial port.  This function
   returns the number of bytes written to the serial port.
*/ 
static int Com2Putb(unsigned char c)
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
/* function to assert/clear the RTS signal of Com2                        */
/*------------------------------------------------------------------------*/
/**
   This function is used to assert or clear the RTS signal for Com2.  To
   assert the RTS line then set the argument to be positive.  To clear the
   RTS line then set the argument to be zero or negative.  This function
   returns a positive value on success; otherwise zero is returned.
*/
static int Com2Rts(int state)
{
   int status = (state>0) ? Com2RtsAssert() : Com2RtsClear();

   return status;
}

/*------------------------------------------------------------------------*/
/* function to assert the RTS line of the com2 serial port                */
/*------------------------------------------------------------------------*/
/**
   This function asserts the RTS line of the com2 serial port; pin 6 of H16
   will measure +5V.  A positive value is returned but the return value has
   no meaning at this time.  
*/
int Com2RtsAssert(void)
{
   DUART_SOPR = 0x02;

   return 1;
}

/*------------------------------------------------------------------------*/
/* function to clear the RTS line of the com2 serial port                 */
/*------------------------------------------------------------------------*/
/**
   This function clears the RTS line of the com2 serial port; pin 6 of H16
   will measure -5V.  A positive value is returned but the return value has
   no meaning at this time.
*/
int Com2RtsClear(void)
{
   DUART_ROPR = 0x02;

   return 1;
}

/*------------------------------------------------------------------------*/
/* function to get the baud-rate code for a given baud rate               */
/*------------------------------------------------------------------------*/
/**
   This function returns the baud-rate code from Table 5 of the data sheet
   for the SC28L92 data sheet.  The return values are correct only if
   MR0[0]=0 and ACR[7]=1 so this function depends on particular values
   specified in Com1Enable() and Com2Enable().
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
/* function to initialize register values common to com1 and com2         */
/*------------------------------------------------------------------------*/
void InitDUart2(void)
{
   /* BRG set 2, timer (not used): clk / 16 */
   DUART_ACR = 0xf0;

   /* enable RX interrupt both UARTS */
   DUART_IMR = 0x22;

   /* clear all of the UART's output pins */
   DUART_SOPR = 0xff;
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

void RMsg(void) {
	printf("t");

	return;
}


