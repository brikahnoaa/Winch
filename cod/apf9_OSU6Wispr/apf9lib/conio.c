#ifndef CONIO_H
#define CONIO_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: conio.c,v 1.21.2.1 2008/09/11 19:40:29 dbliudnikas Exp $
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
 * $Log: conio.c,v $
 * Revision 1.21.2.1  2008/09/11 19:40:29  dbliudnikas
 * Update for Webb development system: line length issue.
 *
 * Revision 1.21  2008/07/14 17:07:00  swift
 * Eliminate an un-needed wait to speed-up console IO.
 *
 * Revision 1.20  2007/04/24 01:43:05  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.19  2006/04/21 13:45:38  swift
 * Changed copyright attribute.
 *
 * Revision 1.18  2006/01/06 23:09:22  swift
 * Added a check to see if 20ma serial loop is active before printing a byte.
 * Avoiding the write attempt will save energy.
 *
 * Revision 1.17  2005/06/21 14:35:18  swift
 * Reduced manipulation of TXD0 and TXD1.
 *
 * Revision 1.16  2005/05/13 23:24:57  swift
 * Modifications to increase the baud rate of the 20ma serial loop from
 * 4800baud to 9600baud.
 *
 * Revision 1.15  2005/05/09 17:41:37  swift
 * Incorporated speed-ups to 20ma serial loop as suggested by John Dunlap.
 *
 * Revision 1.14  2005/01/06 00:48:30  swift
 * Alphabetized include directives and eliminated use of trace() functionality.
 *
 * Revision 1.13  2004/12/29 23:04:36  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.12  2004/07/14 22:49:02  swift
 * Added a getch() function to fulfill programming contract with the new stdio
 * library.
 *
 * Revision 1.11  2004/06/07 21:18:30  swift
 * Changed linkage specification of getb20ma() and putb20ma() from static to
 * extern for use by Iridium firmware.  Modified the return value of putb() to
 * satisfy the programming contract with Iridium firmware.
 *
 * Revision 1.10  2004/04/26 21:42:22  swift
 * Turned off recieve-interrupts of the 20mA serial interface during
 * character-transmits so that echos are ignored.
 *
 * Revision 1.9  2004/04/23 23:40:55  swift
 * Eliminated bit-banging code in putb20ma() and replaced it with a write to
 * the serial port.
 *
 * Revision 1.8  2004/04/14 20:48:21  swift
 * Revision E of the APF9 made some hardware changes to earlier versions.  The
 * PSD935 was replaced by a PSD835 and the 20mA serial loop was changed to be a
 * real serial IO port.
 *
 * Revision 1.7  2003/12/09 15:47:29  swift
 * Reversed changes referred to in revision 1.6 because interrupts were
 * disabled on the CTD serial port for long enough to lose bytes.
 *
 * Revision 1.6  2003/12/07 22:10:14  swift
 * Tracked down and fixed the bug that caused garbage characters to be written
 * to the log file during CTD operations.  It was fixed by disabling the Rx
 * interrupt of the CTD serial port during writes to the 20ma console port.
 *
 * Revision 1.5  2003/11/20 18:59:35  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.4  2003/11/12 22:36:22  swift
 * Added ConioActive(), ConioEnable(), and ConioDisable() funtions.
 *
 * Revision 1.3  2003/07/19 22:37:15  swift
 * Changes to better handle CR/LF issues.
 *
 * Revision 1.2  2003/07/03 22:41:18  swift
 * Major revisions.  This revision is not yet stable.
 *
 * Revision 1.1  2003/06/25 17:03:09  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define conioChangeLog "$RCSfile: conio.c,v $ $Revision: 1.21.2.1 $ $Date: 2008/09/11 19:40:29 $"

#include <serial.h>

/* function prototypes */
int ConioActive(void);
int ConioDisable(void);
int ConioEnable(void);
int getb(void); 
int getb20ma(unsigned char *byte); 
int kbdhit(void);
int putb(unsigned char byte);
int putb20ma(unsigned char byte);
void putch(char byte);
int kbdhit_CRLF(void);      /* kbdhit WITHOUT CR/LF translation */
void putch_CRLF(char byte); /* putch WITHOUT CR/LF translation */

/* declare variable to hold the number of breaks received */
extern volatile unsigned int ecnt1;

/* declare the console serial port */
extern struct SerialPort conio;

#endif /* CONIO_H */

#include <apf9.h>
#include <assert.h>
#include <fifo.h>
#include <intrpt.h>
#include <logger.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <xa.h>

/* define variable to hold the number of breaks received */
volatile unsigned int ecnt1; 

/* function prototypes for functions with static linkage */
static int iflush20ma(void); 
static int na(void); 

/* define a flag that is used for buffered input */
static bit ConsoleFifoLineReady = 0;

/* define a fifo buffer for the console terminal */
persistent static far unsigned char ConsoleFifoBuf[80];

/* define a Fifo object for the console terminal */
struct Fifo ConsoleFifo ={ConsoleFifoBuf, sizeof(ConsoleFifoBuf), 0, 0, 0, 0};

/* define a serial port for the 20ma interface */
struct SerialPort conio = {getb20ma, putb, iflush20ma, iflush20ma, na, na, na};

/* define some ascii characters special meaning */
#define LF   0x0a /* line feed */
#define CR   0x0d /* carriage return */
#define BKSP 0x08 /* backspace */
#define DEL  0x7f /* DEL */

/* initialize an interrupt vector for the 20ma console serial port */
ROM_VECTOR(IV_RI1, ConsoleRxInterrupt, IV_PSW);

/* XA-G3 I/O port definitions */
static bit RX_20MA @ 0x38C; /* p1.4 */
static bit TX_20MA @ 0x38D; /* p1.5 */

/*------------------------------------------------------------------------*/
/* function to disable console IO                                         */
/*------------------------------------------------------------------------*/
int ConioDisable(void)
{ 
   RI1=0; ERI1=0;

   return 1;
}

/*------------------------------------------------------------------------*/
/* function to enable console IO                                          */
/*------------------------------------------------------------------------*/
int ConioEnable(void)
{
   /* stack-check assertion */
   assert(StackOk());

   /* pet the watch dog */
   WatchDog();

   /* disable interrupts for the CTD interface (XA serial port 0) */
   RI0=0; ERI0=0;

   /* enable interrupts for the 20mA interface (XA serial port 1) */
   RI1=0; ERI1=1;

   return 1;
}

/*------------------------------------------------------------------------*/
/* function to determine if a computer is attached to the APFx controller */
/*------------------------------------------------------------------------*/
/**
   This function determines of the 20mA console serial current loop is
   active - that is, if the 20mA current loop is connected.
*/
int ConioActive(void)
{
   int i,n;

   for (n=0,i=0; i<10; i++) {Wait(1); if (RX_20MA) n++;}
   
   return (n>3) ? 1 : 0;
}

/*------------------------------------------------------------------------*/
/* interrupt handler for input on the 20ma serial interface               */
/*------------------------------------------------------------------------*/
/*
   This is the interrupt handler for data received by the serial port
   connected to the 20ma interface.  Console IO uses a buffered IO model
   where input are processed one line at a time.  Input on the console
   serial port is ignored if a complete line already exists in the FIFO
   queue.  
*/
interrupt void ConsoleRxInterrupt(void)    
{
   unsigned char byte, stat;

   /* clear the rx-ready flag */
   RI1 = 0;

   /* read the byte from the serial port */
   byte = S1BUF & 0x7f;

   /* read the status of serial port 1 */
   stat = S1STAT & 0xe;                  

   /* check if a break-signal was received */
   if((stat & 4) != 0) {++ecnt1; S1STAT = 0;}

   /* buffered IO model is used - ignore the byte if a line exists already */
   if (!ConsoleFifoLineReady)
   {
      switch(byte)
      {
         /* filter out the LFs from CR/LF pairs */
         case LF: {break;}

         /* backspace deletes the last byte fifo */
         case BKSP: {del(&ConsoleFifo); break;}
            
         /* DEL deletes the last byte from the fifo */            
         case DEL:  {del(&ConsoleFifo); break;}
            
         /* push the byte into the fifo */
         default:
         {
            /* add the byte to the fifo */
            push(&ConsoleFifo,byte);

            /* assert the flag that implements the buffered IO model */
            if (byte==CR) ConsoleFifoLineReady=1;
            
            break;
         } 
      } 
   }
} 

/*------------------------------------------------------------------------*/
/* function to detect if the keyboard was hit                             */
/*------------------------------------------------------------------------*/
/**
   This function uses the console FIFO buffer to determine if keyboard input
   is available.

      \begin{verbatim}
         output:
            If input is available, this function pops and returns the next
            byte from the FIFO queue.  Otherwise the status of pop() is
            returned.
      \end{verbatim}
*/
int kbdhit(void)
{
   unsigned char byte=0;
   int status;
      
   /* stack-check assertion */
   assert(StackOk());

   /* disable interrupts to prevent fifo changes */
   di();

   /* pop the next byte out of the fifo queue */
   status=pop(&ConsoleFifo,&byte);

   /* check buffer termination criteria */
   if (status<=0 || byte==CR) ConsoleFifoLineReady=0;
      
   /* enable interrupts */
   ei();

   /* transmit a LF if the incoming byte is a CR */
   if (status>0 && byte==CR) putb20ma(LF);

   /* protect against interrupts being turned off too much */
   Wait(5);

   /* return byte if successful, otherwise return the status */
   return (status>0) ? byte : status;
}

/*------------------------------------------------------------------------*/
/* function to detect if the keyboard was hit WITHOUT CR -> CRLF TX       */
/*------------------------------------------------------------------------*/
/**
   This function uses the console FIFO buffer to determine if keyboard input
   is available.  This function does not automatically send a LF after CR.
   This allows the Sserial output to reflect only actual characters being processed.

      \begin{verbatim}
         output:
            If input is available, this function pops and returns the next
            byte from the FIFO queue.  Otherwise the status of pop() is
            returned.
      \end{verbatim}
*/
int kbdhit_CRLF(void)
{
   unsigned char byte=0;
   int status;
      
   /* stack-check assertion */
   assert(StackOk());

   /* disable interrupts to prevent fifo changes */
   di();

   /* pop the next byte out of the fifo queue */
   status=pop(&ConsoleFifo,&byte);

   /* check buffer termination criteria */
   //if (status<=0 || byte==CR) ConsoleFifoLineReady=0;
   if (status<=0) ConsoleFifoLineReady=0;      
   /* enable interrupts */
   ei();

   /* transmit a LF if the incoming byte is a CR */
   //if (status>0 && byte==CR) putb20ma(LF);

   /* protect against interrupts being turned off too much */
   Wait(5);

   /* return byte if successful, otherwise return the status */
   return (status>0) ? byte : status;
}


/*------------------------------------------------------------------------*/
/* function to read a byte from the console FIFO queue                    */
/*------------------------------------------------------------------------*/
/**
   This function reads one byte from the FIFO queue of the console serial
   port.  A line-buffered IO model is implemented to allow for basic editing
   functions.

      \begin{verbatim}
      output:
         byte...The next byte from the console FIFO queue.

         This function returns a positive number on success and zero on
         failure.  A negative return value indicates an exceptional
         condition such as a NULL function argument.
      \end{verbatim}
*/
int getb20ma(unsigned char *byte)
{
   /* define the logging signature */
   static cc FuncName[] = "getb20ma()";

   int status=-1;
      
   /* stack-check assertion */
   assert(StackOk());

   /* validate the function parameter */
   if (!byte) 
   {
      /* create the message */
      static cc msg[]="NULL function argument.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* line buffered - don't read any bytes until a full line is available */
   else if (ConsoleFifoLineReady)
   {
      /* initialize the return value */
      *byte=0;
      
      /* disable interrupts to prevent the fifo from changing state */
      di();

      /* pop the next byte out of the fifo queue */
      status=pop(&ConsoleFifo,byte);

      /* check buffer termination criteria */
      if (status<=0 || (*byte)==CR) {ConsoleFifoLineReady=0;}
      
      /* enable interrupts */
      ei();

      /* transmit a LF if the incoming byte is a CR */
      if ((*byte)==CR) putb20ma(LF);

      /* return a CR if the FIFO buffer is empty */
      if (status<=0) (*byte)=CR;
      
      /* protect against interrupts being turned off too much */
      Wait(2);
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to return the next character from the console serial port     */
/*------------------------------------------------------------------------*/
/**
   This function reads the next character from the console serial port as an
   unsigned char cast to an int, or EOF on end of file or error.
*/
int getb(void)
{
   unsigned char byte;

   return (getb20ma(&byte)>0) ? byte : EOF;
}

/*------------------------------------------------------------------------*/
/* degenerate function to satisfy programming contract with stdio library */
/*------------------------------------------------------------------------*/
/**
   This function is used to satisfy the programming contract with the stdio
   library for reading input from stdin.  However, this function always
   returns EOF. The standard C library relies on a blocking IO model that is
   incompatible with safeguards built into APF9 firmware.  Use the
   SerialPort interface instead.
*/
int getch(void)
{
   return EOF;
}

/*------------------------------------------------------------------------*/
/* function to flush the console's Rx FIFO queue                          */
/*------------------------------------------------------------------------*/
/**
   This function flushes the Rx FIFO queue of the console serial port.  This
   function returns a positive value on success and zero on failure.  A
   negative return value indicates an invalid or corrupt Fifo object.
*/
static int iflush20ma(void)
{
   return flush(&ConsoleFifo);
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
/* function to write a single byte to the serial port                     */
/*------------------------------------------------------------------------*/
/**
   This function writes the character c, cast to an unsigned char, to the
   console serial port and returns a positive value on success or EOF on
   error.
*/
int putb(unsigned char byte)
{
   /* translate LF to CR, LF */
   if(byte == LF) putb20ma(CR);

   return (putb20ma((unsigned char)byte)>0) ? 1 : EOF;
}

/*------------------------------------------------------------------------*/
/* primitive used by printf() to write single character to 20ma interface */
/*------------------------------------------------------------------------*/
/**
   This function is used by the printf() family of functions to write output
   to the console serial port.  Aside from CR/LF translation, it's only
   action is to pass the byte to the serial port's low-level function:
   putb20ma().  
*/
void putch(char byte)        
{
   /* translate LF to CR, LF */
   if(byte == LF) putb20ma(CR);

   /* write the byte to the serial port */
   putb20ma(byte); 
}

/*------------------------------------------------------------------------*/
/* primitive used by printf() to write single character to 20ma interface */
/* WITHOUT CR/LF translation                                              */
/*------------------------------------------------------------------------*/
/**
   This function is used by the printf() family of functions to write output
   to the console serial port during certain serial output display.  
   It's only action is to pass the byte to the serial port's low-level 
   function: putb20ma().  The CR/LF translation (in regular putch) has been 
   removed so output reflects actual transmission.  
*/
void putch_CRLF(char byte)        
{
   /* write the byte to the serial port */
   putb20ma(byte); 
}

/*------------------------------------------------------------------------*/
/* function to write a single byte to the 20ma serial interface           */
/*------------------------------------------------------------------------*/
/**
   This function writes a single byte to the 20ma current loop interface.

      \begin{verbatim}
      input:
         byte ... The byte to be written to the 20ma serial interface.

      output:
         This function always returns a positive number.
      \end{verbatim}
*/
int putb20ma(unsigned char byte)
{
   /* initialize the return value */
   int i,status = 1;
   
   /* stack-check assertion */
   assert(StackOk());
      
   /* pet the watch dog */
   WatchDog();

   /* transmit the byte only if the 20ma serial loop is active */
   if (RX_20MA)
   {
      /* write the byte to the serial port */
      ERI1=0;     // disable RX1 interrupt while transmitting on current loop.
      EA=0;       // ==di(), disable all interrupts so next two lines are atomic
      S1BUF=byte; // put byte in TX1 buffer
      TI1=0;      // reset the TX1 rupt flag to turn off double buffering
      EA=1;        // ==ei(), enable all interrupts -- end of atomic
      for (i=0; i<4 && !TI1; i++) {Wait(1);} // wait for all TX1 bits to be fully sent
      RI1=0;      // clear RX1 interrupt flag
      ERI1=1;     // enable RX1 interrupts again
   }

   return status;
}
