#ifndef APF9_H
#define APF9_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: apf9.c,v 1.22 2007/04/24 01:43:05 swift Exp $
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
 * $Log: apf9.c,v $
 * Revision 1.22  2007/04/24 01:43:05  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.21  2006/08/17 21:17:48  swift
 * MOdifications to allow better logging control.
 *
 * Revision 1.20  2006/04/21 13:45:38  swift
 * Changed copyright attribute.
 *
 * Revision 1.19  2005/06/28 18:58:54  swift
 * Validate MaxLogSize to control maximum size of logfile.
 *
 * Revision 1.18  2005/06/22 19:25:17  swift
 * Rework of the RTC watchdog alarm and reiniatialization of TXD0.
 *
 * Revision 1.17  2005/06/21 14:35:19  swift
 * Reduced manipulation of TXD0 and TXD1.
 *
 * Revision 1.16  2005/06/17 16:30:27  swift
 * Implement a watch-dog alarm to prevent power-down exceptions from killing a float.
 *
 * Revision 1.15  2005/06/14 18:56:44  swift
 * Implement a watch-dog alarm to prevent power-down exceptions from killing a float.
 *
 * Revision 1.14  2005/05/13 23:24:57  swift
 * Modifications to increase the baud rate of the 20ma serial loop from
 * 4800baud to 9600baud.
 *
 * Revision 1.13  2005/02/22 21:55:35  swift
 * Eliminated the SetAlarm() call in the initialization code...causes bugs.
 *
 * Revision 1.12  2005/01/15 00:51:29  swift
 * Added code in Apf9Init() to set the alarm for an hour after each start-up to
 * protect against the possibility that the APF9 may shutdown uncleanly.
 *
 * Revision 1.11  2005/01/06 00:49:40  swift
 * Added interrupt control to SetBaud().
 *
 * Revision 1.10  2004/12/29 23:02:17  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.9  2004/04/26 21:42:55  swift
 * Added function SetBaud() to set the buad rate of the XA serial ports.
 *
 * Revision 1.8  2004/04/15 23:17:31  swift
 * Changed initialization state of TXD1 to eliminate stray characters on start-up.
 *
 * Revision 1.7  2004/04/14 20:48:21  swift
 * Revision E of the APF9 made some hardware changes to earlier versions.  The
 * PSD935 was replaced by a PSD835 and the 20mA serial loop was changed to be a
 * real serial IO port.
 *
 * Revision 1.6  2004/04/14 16:35:48  swift
 * Fixed a syntax error in calling Com2Disable();
 *
 * Revision 1.5  2003/12/07 22:08:11  swift
 * Debugged and fixed the APF9's annoying habit of generating garbage
 * characters during power-down.  It turned out to be an artifact of an RC
 * circuit somewhere in the hardware.
 *
 * Revision 1.4  2003/11/20 18:59:34  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.3  2003/11/12 22:33:14  swift
 * Another round of refactorization and API enhancement.
 *
 * Revision 1.2  2003/07/19 22:54:42  swift
 * Modifications to reflect refactorization and remodeling of the
 * Psd835 ports.
 *
 * Revision 1.1  2003/07/06 00:05:56  swift
 * Initial revision
 * \end{verbatim}
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define apf9ChangeLog "$RCSfile: apf9.c,v $ $Revision: 1.22 $ $Date: 2007/04/24 01:43:05 $"

#include <conio.h>
#include <xa.h>

/* XA-G3 I/O port definitions */
static unsigned char bd1          @ 0x20;
static bit BD1_LSB                @ 0x100;
static bit BD1_MSB                @ 0x107;
static bit RTC_DQ                 @ 0x38E; // p1.6
static bit EE_SDA                 @ 0x38F; // p1.7
static bit ADC_DOUT               @ 0x39c; // p3.4

/* macro to restart the watchdog timer */
#define WatchDog() {EA=0; WFEED1=0xa5; WFEED2=0x5a; EA=1;}

/* lowest address for far RAM */
extern const unsigned long Apf9FarRamLo;

/* highest address of far RAM */
extern const unsigned long Apf9FarRamHi;

/* starting address of the program stack */
extern unsigned int Apf9StackBase;

/* maximum size of the program stack */
extern const int Apf9StackSize;

/* define reserve required to prevent activation of stack-overflow assertions */
extern const int Apf9StackReserve;

/* macro to perform a stack-check computation */
#define StackOk() (Apf9StackSize-Apf9StackLen()>Apf9StackReserve)

extern bit timer0;
extern persistent far long dzero;

/* function prototypes */
int           AirPumpRun(time_t seconds, unsigned char *VoltCnt, unsigned char *AmpCnt);
int           AirValveOpen(void);
int           AirValveClose(void);
int           Apf9Init(void);
void          Apf9PowerOff(time_t AlarmSec);
int           Apf9StackLen(void);
unsigned int  Apf9StackPeek(unsigned int *StackAddr);
int           Apf9Tp1(unsigned int millisec,unsigned int Hz);
time_t        Apf9WakeTime(void);
unsigned char Apf9WakeUpByRtc(void);
int           Ds2404Read(unsigned int addr,unsigned int bytes,void *buf);
int           Ds2404Write(unsigned int addr,unsigned int bytes,const void *buf);
int           HDrive(int dir);
int           MagSwitchReset(void);
int           MagSwitchToggled(void);
int           SeimacXmit(const unsigned char *msg,unsigned int N);
void          SetBaud(unsigned int baud);                 
void          Wait(unsigned int millisec);
void          WaitTicks(unsigned char tics);

#endif /* APF9_H */

#include <math.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <xa.h>
#include <intrpt.h>
#include <ds2404.h>
#include <psd835.h>
#include <serial.h>
#include <assert.h>
#include <logger.h>
#include <lt1598ad.h>
#include <apf9com.h>
#include <apf9icom.h>

/* lowest address for far RAM */
const unsigned long Apf9FarRamLo=0x10000;

/* highest address of far RAM */
const unsigned long Apf9FarRamHi=0x82fff; 

/* starting address of the program heap */
unsigned int Apf9HeapBase;
 
/* ending address of the program heap */
const unsigned int Apf9HeapHi = 0xffff;

/* starting address of the program stack */
unsigned int Apf9StackBase;

/* maximum size of the program stack */
const int Apf9StackSize = 0x1000;

/* define reserve required to prevent activation of stack-overflow assertions */
const int Apf9StackReserve = 256;

/* define a static variable to hold the DS2404 status at powerup */
static unsigned char Ds2404StatusAtPowerUp=(unsigned char)(-1);

/* define a variable to hold the wakeup time */
static time_t WakeTime;

/* prototypes of static functions */
static unsigned char DS2404Rd(void);
static void Ds2404Wr(unsigned char b);

/* declarations to external RAM */
extern unsigned char _Hbss;
extern unsigned char _Lbss;
extern unsigned char _Bdata;
extern unsigned char _Ldata;
extern unsigned char _Hstrings;
extern unsigned char _Lheap;

/* initialize interrupt vector for timer0 */
ROM_VECTOR(IV_T0, t0_intr, IV_PSW);       
bit timer0; interrupt void t0_intr(void) {timer0=1;}

/* initialize interrupt vector for division-by-zero exception */
ROM_VECTOR(IV_DIVZERO, divzero_intr, IV_PSW);
persistent far long dzero; interrupt void divzero_intr(void) {++dzero;}

/*------------------------------------------------------------------------*/
/* function to run the air pump for a specified length of time            */
/*------------------------------------------------------------------------*/
/**
   This function activates the air-pump motor for a specified length of
   time.  The battery voltage and current are measured just prior to turning
   off the air pump.

      \begin{verbatim}
      input:
         seconds....The length of time to run the air pump.

      output:
         VoltCnt....The 8-bit AD measurement of the battery voltage just
                    prior to turning off the air pump.
         AmpCnt.....The 8-bit AD meeasurement of the battery current just
                    prior to turning off the air pump.

         This function returns a positive number on success and zero on
         failure.  A negative return value indicates an exceptional
         condition.
      \end{verbatim}
*/
int AirPumpRun(time_t seconds, unsigned char *VoltCnt, unsigned char *AmpCnt)
{
   /* turn on the air pump motor */
   Psd835PortBSet(AIR_PUMP_ON);

   /* let the air pump run for the specified number of seconds */
   Wait(seconds * 1000);

   /* measure the 8-bit voltage count */
   if (VoltCnt) *VoltCnt = BatVoltsAd8();

   /* measure the 8-bit current count */
   if (AmpCnt)  *AmpCnt = BatAmpsAd8();

   /* turn off the air pump motor */
   Psd835PortBClear(AIR_PUMP_ON);

   return 1;
}

/*------------------------------------------------------------------------*/
/* function to open the air valve                                         */
/*------------------------------------------------------------------------*/
/**
   This function applies a 500mA current source to pins 1 and 2 of APF9
   connector H3.  The current will source from pin 1 and sink to pin 2.  The
   air valve should be connected to H3 so that this function will have the
   effect of opening the solenoid air valve.
*/
int AirValveOpen(void)
{
   /* clear both inputs */ 
   Psd835PortGClear(AIR_OPEN|AIR_CLOSE); Wait(2);

   /* apply a 60 millisecond current pulse to the air-valve solenoid */
   Psd835PortGSet(AIR_OPEN); Wait(60); Psd835PortGClear(AIR_OPEN);
   
   return 1;     
}

/*------------------------------------------------------------------------*/
/* function to close the air valve                                        */
/*------------------------------------------------------------------------*/
/**
   This function applies a 500mA current source to pins 1 and 2 of APF9
   connector H3.  The current will source from pin 2 and sink to pin 1.  The
   air valve should be connected to H3 so that this function will have the
   effect of closing the solenoid air valve.
*/
int AirValveClose(void)
{
   /* clear both inputs */ 
   Psd835PortGClear(AIR_OPEN|AIR_CLOSE); Wait(2);
   
   /* apply a 60 millisecond current pulse to the air-valve solenoid */
   Psd835PortGSet(AIR_CLOSE); Wait(60); Psd835PortGClear(AIR_CLOSE);

   return 1;     
}

/*------------------------------------------------------------------------*/
/* function to initialize the APF9 at startup                             */
/*------------------------------------------------------------------------*/
/**
   This function initializes the APF9's ports, registers, and RAM.  It
   should be called immediately upon program startup before performing any
   function that requires the use of the PSD or XA ports.

   If successful this function returns a positive number.  Otherwise zero is
   returned.  Behavior will be undefined if this function fails.
*/
int Apf9Init(void)
{
   /* define the logging signature */
   static cc FuncName[] = "Apf9Init()";
   
   int status=1;
   unsigned char *ptrRam;
   unsigned char code *ptrCode;
   unsigned int i, cnt;
   time_t now;
   
   /* allow interrupts */
   PSWH = 0x80;          

   /* initialize the APF9's PSD935 */
   Psd835Init();
   
   /* inintialize the DUARTs */
   InitDUart1(); InitDUart2();

   IPA0 = 0x11;                     // set timer 0, ex0 interrupt priority
   IPA1 = 1;                        // set timer 1, ex1 interrupt priority
   IPA2 = 0;                        // set timer 2 interrupt priority 
   IPA4 = 1;                        // set RX0 interrupt priority
   IPA5 = 1;                        // set RX1 interrupt priority

   // serial ports 
   TMOD = 0;                        // timer 0 and 1 16 bit auto reload 
   S0CON = 0x50;                    // uart 0 mode 1 
   S1CON = 0x50;                    // uart 1 mode 1 
   TXD0=0; RI0=0; ERI0=0; ETI0=0;
   TXD1=1; RI1=0; ERI1=0; ETI1=0;
   RTH1 = TH1 = (unsigned char)0xff; // msb
   RTL1 = TL1 = (unsigned char)0xfa; // lsb
   TR1 = 1;                          // start baud rate timer
   TH0 = RTH0 = 0xdc;                // 9216 ticks = 0.01 sec, 65536 - 9216 = 0xdc00
   TL0 = RTL0 = 0;
   ET0 = 1;
   TR0 = 1;
   ei();
   Wait(300);                       // wait for 2nd MAX 823
   
   cnt = (unsigned int)&_Hstrings - (unsigned int)&_Ldata; // copy data and strings
   ptrRam = (unsigned char *)&_Ldata;
   ptrCode = (unsigned char code *)&_Bdata;
   for(i=0; i<cnt; i++) ptrRam[i] = ptrCode[i];
   cnt = (unsigned int)&_Hbss - (unsigned int)&_Lbss;              // clear BSS psect
   ptrRam = (unsigned char *)&_Lbss;
   for(i=0; i<cnt; i++) ptrRam[i] = 0;
   
   Apf9StackBase = (unsigned int)((unsigned char *)&_Ldata);
   Apf9HeapBase = (unsigned int)((unsigned char *)&_Lheap);

   /* validate the logstream */
   if (fstreamok(LogStream())<=0) logstream=NULL;

   /* validate the maximum size of the logfile */
   if (MaxLogSize<5120L || MaxLogSize>61440L) MaxLogSize=40960L;

   /* validate the debuglevel */
   if (debuglevel>5) debugbits=2;
   
   /* disable all serial ports */
   ModemDisable(); GpsDisable(); Com1Disable(); Com2Disable();
   
   /* get the startup status of the DS2404 RTC */
   Ds2404StatusAtPowerUp = Ds2404StatusByte();
   
   /* initialize the DS2404 RTC */
   if (Ds2404Init()<=0) status=0;

   /* initialize the wakeup time */
   WakeTime=itimer();
   
   /* validate the RTC */
   if ((now=time(NULL))<0) {RtcSet(0,0); now=0;}

   /* set a watchdog alarm in case of a power-down exception */
   RtcAlarmSet(now+Hour,0);

   /* check for wake-up by the watchdog (RTC) alarm signal */
   if ((Apf9WakeUpByRtc()&Ds2404RtfMask))
   {
      static cc msg[]="Warning: Wake-up initiated by watch-dog alarm.\n";
      LogEntry(FuncName,msg);
   }
   
   /* check for user-initiated wake-up */
   else if (debuglevel>=2 && !Apf9WakeUpByRtc())
   {
      static cc msg[]="Asynchronous wake-up detected "
         "(ie., wake-up not initiated by alarm signal).\n";
      LogEntry(FuncName,msg);
   }

   /* check for normal wake-up by the interval-timer alarm */
   else if (debuglevel>=3 && (Apf9WakeUpByRtc()&Ds2404ItfMask))
   {
      static cc msg[]="Wake-up initiated by interval-timer alarm signal.\n";
      LogEntry(FuncName,msg);
   }
	
	
	Com1Enable(9600);

   
   /* pet the watchdog */
   WatchDog();

   return status;
}

/*------------------------------------------------------------------------*/
/* function to turn off power                                             */
/*------------------------------------------------------------------------*/
/**
   This function powers down the APF9 controller after setting the alarm for
   the next wake-up.  If the sleep period is not in the closed interval
   [5sec,7Hr] then this function will reset the sleep period.

      \begin{verbatim}
      input:
         sleepsec ... This is the number of seconds in the future for which
                      the interval-timer alarm will be set.  The DS2404 RTC
                      will wake up the APF9 when the sleep period expires.
                      If the sleep period is not in the closed interval
                      [5sec,7Hr] then this function will reset the sleep
                      period.
      \end{verbatim}
*/
void Apf9PowerOff(time_t AlarmSec)
{
   /* define the logging signature */
   static cc FuncName[] = "Apf9PowerOff()";

   /* compute the sleep time */
   time_t sleep = (AlarmSec-itimer());

   /* validate the sleep time */
   if (sleep<5L || sleep>7*Hour)
   {
      /* create the format for the logentry */
      static cc format[]="Sleep period (%ldsec) not in "
         "valid range [5sec,7Hr].  Resetting to %s.\n";
      
      /* reset the sleep period */
      AlarmSec = itimer() + ((sleep<5L) ? 5L : Hour);

      /* make the log-entry */
      LogEntry(FuncName,format,sleep,(sleep<5L)?"5sec":"1Hr");
   }

   /* set the sleep for the next wake-up and reset the watchdog alarm */
   SetAlarm(AlarmSec); RtcAlarmSet(time(NULL)+7*Hour+Min,0);
   
   /* pause for 1/4 second and then cut the power */
   Wait(250); di(); Psd835PortAClear(LATCH_PWR); Wait(1000);
}

/*------------------------------------------------------------------------*/
/* function to compute the current stack length                           */
/*------------------------------------------------------------------------*/
/**
   This function computes and returns the number of bytes currently on the
   program stack.  If the value returned by this function exceeds the stack
   size then a stack overflow has occurred.
*/
int Apf9StackLen(void)
{
   unsigned int stackaddr;

   /* compute the current stack length */
   int stacklen = Apf9StackBase - Apf9StackPeek(&stackaddr);

   /* assert that the stacklength is non-negative */
   assert(stacklen>=0);
   
   return stacklen;
}

/*------------------------------------------------------------------------*/
/* function to get the address of the top of the stack                    */
/*------------------------------------------------------------------------*/
/**
   This function reads the stack pointer from register r7 of the Phillips
   XA processor.  This allows trapping of stack overflows and it is
   recommended to use assertions to do so.

      \begin{verbatim}
      output:
         StackAddr ... This will contain the address of the top of the
                       stack. 

         On success, this function returns the address of the top of the
         stack.  If the function argument is NULL then the value StackBase
         is returned.
      \end{verbatim}
*/
unsigned int Apf9StackPeek(unsigned int *StackAddr) 
{
   /* define the logging signature */
   static cc FuncName[] = "Apf9StackPeek()";

   /* initialize the return value */
   unsigned int addr=Apf9StackBase;

   /* validate the function argument */
   if (!StackAddr)
   {
      static cc msg[]="NULL function argument.\n";
      LogEntry(FuncName,msg); 
   }
   
   /* read the stack address from register r7 */
   else
   {
      /* r7 is the stack pointer, *ptr is in r3 */
      asm("mov [r3], r7");

      /* assign the address to the return value */
      addr= *StackAddr;
   } 
   
   return addr;
}

/*------------------------------------------------------------------------*/
/* function to cycle the APF9's TP1 pin at specified frequency            */
/*------------------------------------------------------------------------*/
/**
   This function applies a square-wave of user-specified duration and
   frequency to the APF9's TP1 pin.  The frequency (Hz) must be in the range
   [8,65535] and the duration (msec) must be in the range [1,65535].
   
   Frequencies whose factors are not those of the number 460800
   [460800=(2^11)*(3^2)*(5^2)] will be generated with lesser precision
   especially for higher frequencies.  Frequencies (Hz) in the range
   [8,65535] that can be represented by Hz=(2^i)*(3^j)*(5^k) for i:[0,11];
   j:[0,2]; k:[0,2] will be generated most precisely.

   If successful, this function returns a positive number; otherwize zero is
   returned.
*/
int Apf9Tp1(unsigned int millisec, unsigned int Hz)
{
   int status=0;

   /* timer frequency */
   const unsigned long int TicsPerSec=921600;

   /* compute the number of timer-tics per half-cycle */
   const unsigned long int TicsPerHalfCycle=(TicsPerSec/(2*Hz));

   /* validate the function arguments */
   if (status=(TicsPerHalfCycle<=0xffff && millisec>0))
   {
      unsigned long int n;
      
      /* compute 2's-complement of the number of timer-tics per half-cycle */
      const unsigned int CTicsPerHalfCycle=0x10000UL-TicsPerHalfCycle;
   
      /* determine the number of half-cycles to execute */
      const unsigned long int N=2UL*Hz*millisec/1000UL;

      /* disable interrupts; disable timer overflow interrupt and stop timer */
      di(); ET0 = 0; TR0 = 0;

      /* set the timer: eg (400Hz); 1152 ticks = 1.25 msec, 65536 - 1152 = 0xfb80 */
      TH0 = RTH0 = (CTicsPerHalfCycle&0xff00)>>8; TL0 = RTL0 = (CTicsPerHalfCycle&0x00ff);
   
      /* enable timer overflow interrupt and start timer; enable interrupts */
      ET0 = 1; TR0 = 1; ei();

      /* execute the desired number of half-cycles */
      for (n=0; n<N; n++)
      {
         /* toggle the state of the TP1 pin */
         if (n%2) {Psd835PortESet(TP1);} else {Psd835PortEClear(TP1);}
      
         /* pause for a half-cycle */
         WatchDog(); for (timer0=0; !timer0;) {}
      }

      /* clear the TP1 pin */
      Psd835PortEClear(TP1);
      
      /* reset timer tic to 10msec: 9216 ticks = 0.01 sec, 65536 - 9216 = 0xdc00 */
      di(); TH0 = RTH0 = 0xdc; TL0 = RTL0 = 0; ei();
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to determine if the powerup was initiated by the RTC          */
/*------------------------------------------------------------------------*/
/**
   This function determines if the powerup was initiated by the DS2404 real
   time clock.  To do this, it examines the status register to see if any
   alarms flags are asserted.

   This function returns a positive value if the DS2404 RTC was determined
   to have initiated the wake-up.  If the wake-up was asynchronous (ie.,
   user initiated) or if an error occurred then this function returns zero.
*/
unsigned char Apf9WakeUpByRtc(void)
{
   /* initialize the return value */
   int status=0;

   /* stack-check assertion */
   assert(StackOk());

   /* validate the DS2404 status flags at startup */
   if (Ds2404StatusAtPowerUp!=(unsigned char)(-1))
   {
      /* extract just the bits that relate to the alarm flags */
      status = Ds2404StatusAtPowerUp & (Ds2404RtfMask | Ds2404ItfMask);
   } 

   return status; 
} 

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
time_t Apf9WakeTime(void)
{
   return WakeTime;
}

/*------------------------------------------------------------------------*/
/* function to read DS2404 memory pages                                   */
/*------------------------------------------------------------------------*/
/**
   This function reads bytes from the DS2404 memory pages.

      \begin{verbatim}
      input:
         addr....The SRAM address where reading should begin.  This address
                 must be less than 0x0220.
         bytes...The number of bytes to be read.  Reads across page
                 boundaries are not allowed by the DS2404 and therefore
                 the values of addr and bytes must satisfy the
                 condition: addr%32 + bytes < 32.
         buf.....A buffer to contain the bytes read from the SRAM.
                 
      output:
         This function returns a positive number to indicate success, zero
         to indicate failure, and a negative number to indicate a problem
         with one or more of the function arguments.
      \end{verbatim}
*/
int Ds2404Read(unsigned int addr,unsigned int bytes,void *buf)
{
   /* define the logging signature */
   static cc FuncName[] = "Ds2404Read()";

   int status=-1;

   /* stack-check assertion */
   assert(StackOk());

   /* pet the watchdog timer */
   WatchDog();

   /* make sure that addr is a 2-byte quantity */
   assert(sizeof(addr)==2);

   /* validate the buffer from which to write */
   if (!buf)
   {
      /* create the message */
      static cc msg[]="Invalid buffer: buf==NULL.\n";

      /* make the logentry */
      LogEntry(FuncName,msg);
   }
   
   /* validate the beginning address */
   else if (addr>=0x0220)
   {
      /* create the message */
      static cc format[]="Address (0x%04x) exceeds memory size.\n";
      
      /* make the logentry */
      LogEntry(FuncName,format,addr);

      /* indicate failure */
      status=0;
   }

   /* make sure the write attempt won't cross a page boundary */
   else if ((addr%32 + bytes) > 32)
   {
      /* create the message */
      static cc format[]="Ending address (0x%04x) over-runs page boundary (0x%04x).\n";
      
      /* make the logentry */
      LogEntry(FuncName,format,(addr%32 + bytes - 1),(addr - addr%32 + 32));

      /* indicate failure */
      status=0;
   }

   else 
   {
      unsigned int i;
      
      /* extract the LSB and the MSB from the address */
      const unsigned char lsb=addr&0xff, msb=(addr>>8);

      /* enable DS2404 communications */
      Psd835PortESet(RTC_RST);

      /* write the command into the scratch pad that reads the specified register */
      Ds2404Wr(0xf0); Ds2404Wr(lsb); Ds2404Wr(msb);

      /* read the specified number of bytes from the specified address */
      for (i=0; i<bytes; i++) ((unsigned char *)buf)[i] = DS2404Rd();

      /* disable DS2404 communications */
      Psd835PortEClear(RTC_RST);

      /* indicate success */
      status=1;
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to read a single byte from the DS2404 RTC                     */
/*------------------------------------------------------------------------*/
/**
   This function reads and returns a single byte from the DS2404 RTC.  
*/
static unsigned char DS2404Rd(void)        
{
   unsigned char i;

   /* pet the watchdog timer */
   WatchDog();

   /* configure for I/0 */
   RTC_DQ = 1;            

   /* write each bit in turn from the DS2404 starting with the lsb */
   for (bd1=0, i=0; i<8; i++)
   {
      /* set the clock signal high */
      Psd835PortASet(RTC_CLK); 

      /* right-shift before clocking the next bit from the DS2404 */
      bd1 = bd1 >> 1;

      /* data is output on the falling edge */ 
      Psd835PortAClear(RTC_CLK);
      
      BD1_MSB = RTC_DQ;
   }
   
   return(bd1);
}

/*------------------------------------------------------------------------*/
/* function to write a single byte to the DS2404 RTC                      */
/*------------------------------------------------------------------------*/
/**
   This function writes a single byte into the DS2404 RTC.
*/
static void Ds2404Wr(unsigned char b)
{                             
   unsigned char i;

   /* pet the watchdog */
   WatchDog();

   /* write each bit in turn to the DS2404 starting with the lsb */
   for (bd1=b, i=0; i<8; i++)
   {
      RTC_DQ = BD1_LSB;          // output bit

      /* clock the current bit into the DS2404 */
      Psd835PortAClear(RTC_CLK); Psd835PortASet(RTC_CLK);

      /* right-shift the next bit to be clocked into the DS2404 */ 
      bd1 = bd1 >> 1;
   }
   
   RTC_DQ = 1;
}

/*------------------------------------------------------------------------*/
/* function to write to DS2404 memory pages                               */
/*------------------------------------------------------------------------*/
/**
   This function writes bytes to the DS2404 memory pages.

      \begin{verbatim}
      input:
         addr....The SRAM address where writing should begin.  This address
                 must be less than 0x0220.
         bytes...The number of bytes to be written.  Writes across page
                 boundaries are not allowed by the DS2404 and therefore
                 the values of addr and bytes must satisfy the
                 condition: addr%32 + bytes < 32.
         buf.....A buffer that contains the bytes to write to the SRAM.
                 
      output:
         This function returns a positive number to indicate success, zero
         to indicate failure, and a negative number to indicate a problem
         with one or more of the function arguments.
      \end{verbatim}
*/
int Ds2404Write(unsigned int addr,unsigned int bytes,const void *buf)
{
   /* define the logging signature */
   static cc FuncName[] = "Ds2404Write()";

   int status=-1;

   /* stack-check assertion */
   assert(StackOk());

   /* make sure that addr is a 2-byte quantity */
   assert(sizeof(addr)==2);

   /* validate the buffer from which to write */
   if (!buf)
   {
      /* create the message */
      static cc msg[]="Invalid buffer: buf==NULL.\n";

      /* make the logentry */
      LogEntry(FuncName,msg);
   }
   
   /* validate the beginning address */
   else if (addr>=0x0220)
   {
      /* create the message */
      static cc format[]="Address (0x%04x) exceeds memory size.\n";

      /* make the logentry */
      LogEntry(FuncName,format,addr);

      /* indicate failure */
      status=0;
   }

   /* make sure the write attempt won't cross a page boundary */
   else if ((addr%32 + bytes) > 32)
   {
      /* create the message */
      static cc format[]="Ending address (0x%04x) over-runs "
         "page boundary (0x%04x).\n";
      
      /* make the logentry */
      LogEntry(FuncName,format,(addr%32 + bytes - 1), (addr - addr%32 + 32));

      /* indicate failure */
      status=0;
   }
   
   else
   { 
      unsigned int i; unsigned char EsReg;

      /* extract the LSB and the MSB from the address */
      const unsigned char lsb=addr&0xff, msb=(addr>>8);
  
      /* enable DS2404 communications */
      Psd835PortESet(RTC_RST);

      /* write the command into the scratch pad that reads the specified register */
      Ds2404Wr(0x0f); Ds2404Wr(lsb); Ds2404Wr(msb);

      /* write the bytes into the scratch pad */
      for (i=0; i<bytes; i++) Ds2404Wr(((unsigned char *)buf)[i]);

      /* disable DS2404 communications, wait a millisecond, and re-enable  */
      Psd835PortEClear(RTC_RST); Wait(1); Psd835PortESet(RTC_RST);        

      /* E/S register required for authorization - read from scratch pad */
      Ds2404Wr(0xaa); DS2404Rd(); DS2404Rd(); EsReg = DS2404Rd();
 
      /* disable DS2404 communications, wait a millisecond, and re-enable  */
      Psd835PortEClear(RTC_RST); Wait(1); Psd835PortESet(RTC_RST);        

      /* execute copy-scratchpad command */
      Ds2404Wr(0x55); Ds2404Wr(lsb); Ds2404Wr(msb); Ds2404Wr(EsReg); Wait(1); 

      /* disable communications with the DS2404  */
      Psd835PortEClear(RTC_RST);

      /* indicate success */
      status=1;
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to control the state of the APF9 H-drive                      */
/*------------------------------------------------------------------------*/
/*
   This function places the APF9 H-drive in one of three states.  Actually,
   the H-drive has four possible states but, for DC motor control, two of
   these states are (almost) degenerate.  The effect of the H-drive is to
   connect either ground or the battery directly to pins 1 and 2 of
   connector H11 of the APF9.

      \begin{verbatim}
      input:
         dir ... The DC motor is controlled by the sign of this function
                 argument.  Positive values cause the DC motor to turn one
                 direction and negative values make the motor turn the
                 opposite diection.  Zero connects both ends of the motor
                 winding to ground which has the effect of braking the DC
                 motor so that it strongly resists turning in either
                 direction.  The relationship between input and the output
                 at connector H11 is summarized in the following truth
                 table.
         
                                  dir  H11-1 H11-2
                                  ----------------
                                   0    Grnd  Grnd
                                   +    VBat  Grnd
                                   -    Grnd  VBat
                                  n/a   VBat  VBat

                 The APF9 has the ability to simultaneously connect the
                 battery to pins 1 and 2 of connector H11.  This has the
                 same braking effect on a DC motor as connecting both pins
                 to ground.
                 
      output:
         This function returns a positive value on success and zero on
         failure.  
      \end{verbatim}
*/
int HDrive(int dir)
{
   /* put the H-drive in a known state with both inputs low */
   Psd835PortGClear(PISTON_OUT|PISTON_IN);

   /* set the H-drive for positive polarity */
   if (dir>0) {Psd835PortGSet(PISTON_OUT);}
   
   /* set the H-drive for negative polarity */
   else if (dir<0) {Psd835PortGSet(PISTON_IN);}

   return 1;
}

/*------------------------------------------------------------------------*/
/* reset the flip-flop that records the magnetic switch toggle            */
/*------------------------------------------------------------------------*/
/**
   This function resets the flip-flop that records toggles of the magnetic
   switch.  This function returns a positive value on success or zero on
   failure. 
*/
int MagSwitchReset(void)
{
   /* reset the flip-flop that records the magnetic switch toggle */
   Psd835PortASet(RESET_MAGSWITCH); Wait(2); Psd835PortAClear(RESET_MAGSWITCH);

   return 1;
}

/*------------------------------------------------------------------------*/
/* function to determine if the magnetic reset switch has been toggled    */
/*------------------------------------------------------------------------*/
/**
   This function tests to see if the magnetic reset switch has been toggled
   by the user.  The APF9 has an RC circuit composed of R12=100K and
   C12=2.2uF so that the RC time constant is 1 second.  The magnetic switch
   has to be held open from more than 1 second or so before the APF9 (via
   the 4013 flip-flop) will recognise the transistion.  A positive value is
   returned if the magswitch was toggled and zero is return if not.
*/
int MagSwitchToggled(void)
{
   return (PORTBRD & MAGSWITCH_MOVE) ? 1 : 0;
}

/*------------------------------------------------------------------------*/
/* APF9 heap manager                                                      */
/*------------------------------------------------------------------------*/
/**
  This function allocates blocks of memory from the heap to be used for
  dynamic memory allocation.

     \begin{verbatim}
        input:
           size....The number of bytes to be reserved from the heap.

        output:
           If sufficient heap space is available to fulfill the request,
           this function returns a pointer to the beginning of the newly
           allocated block.  If insufficient heapspace is available then
           this function returns the value: (void *)(-1);
     \end{verbatim}
*/
void *sbrk(int size)
{
   /* define the logging signature */
   static cc FuncName[] = "sbrk()";

    /* static variable to point to the top of the heap */
   static void *HeapTop=(unsigned char *)&_Lheap;

   /* initialize the return value */
   unsigned char *cp=HeapTop;

   /* stack-check assertion */
   assert(StackOk());

   /* allocate more space from the heap only if size is positive */
	if (size>0)
   {
      /* check if enough heap space exists to fulfill the request */
		if ((Apf9HeapHi - (unsigned int)HeapTop) <= size ||
          (unsigned int)HeapTop + size <= (unsigned int)HeapTop)
      {
         /* create the message */
         static cc format[]="Request for %d bytes exceeds available heap "
            " space.  [base: 0x%04x, top: 0x%04x, high: 0x%04x]\n";

         /* make the logentry */
         LogEntry(FuncName,format,size,Apf9HeapBase,HeapTop,Apf9HeapHi);

         /* reinitialize the return value */
         cp = (void *)(-1);
      }
      
      /* reset the heap pointer */
      else HeapTop = cp + size;
   }

   /* the debuglevel sets the verbosity of the logging */
   if (debuglevel>=4)
   {
      /* create the message */
      static cc format[]="Heap: [base: 0x%04x, top: 0x%04x, high: 0x%04x]\n";
      
      /* log heap statistics */
      LogEntry(FuncName,format,Apf9HeapBase,HeapTop,Apf9HeapHi);
   }
   
	return cp;
}

/*------------------------------------------------------------------------*/
/* function to transmit a single message via the SEIMAC PTT               */
/*------------------------------------------------------------------------*/
/**
   This function transmits a single message using the SEIMAC SmartCat PTT.
   A well-formed message starts with the 3-byte preamble (0xfffe2f),
   1-nibble length indicator, 5-nibbles of argos root id, and 1-32 bytes of
   data. 

      \begin{verbatim}
      input:
         msg...A pointer to the argos message to transmit.  This message
               should start with the preamble, then the argos id, and then
               the data bytes.

         N.....The number of bytes in the msg buffer including preamble,
               argos id, and data bytes.

      output:
         This function returns a positive value on success.  Zero is
         returned if the message length is invalid (less than 7 bytes).  
         A negative return value indicates a null function argument.
      \end{verbatim}
*/
int SeimacXmit(const unsigned char *msg,unsigned int N)
{
   /* define the logging signature */
   static cc FuncName[] = "SeimacXmit()";

   int status=-1;

   /* validate the pointer to the argos message */
   if (!msg)
   {
      /* create the message */
      static cc msg[]="NULL function argument.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }
   
   /* validate the message length */
   else if (N<=7)
   {
      /* create the message */
      static cc msg[]="Invalid argos message length.\n";

      /* log the message */
      LogEntry(FuncName,msg); status=0;
   } 
   
   else 
   {
      int i,n,tics; 

      /* reset the return value */
      status=1; 
   
      /* turn on the switching regulator and warm the circuit for 400msec */
      Psd835PortASet(PTT_PON); Wait(400);

      /* turn on the oscillator and stabilize for 100 msec*/
      Psd835PortASet(PTT_PLL_ON); Wait(100);

      /* disable interrupts while setting up the timer */
      di();
   
      /* disable timer overflow interrupt and stop timer */
      ET0 = 0; TR0 = 0;

      /* 1152 ticks = 1.25 msec, 65536 - 1152 = 0xfb80 */
      TH0 = RTH0 = 0xfb; TL0 = RTL0 = 0x80;

      /* reset the timer flag */
      timer0 = 0;
   
      /* enable timer overflow interrupt and start timer */
      ET0 = 1; TR0 = 1;							

      /* enable interrupts */
      ei();

      /* delay for 1.25msec */
      for (timer0=0; !timer0;) {}

      /* turn on the transmitter */
      Psd835PortASet(PTT_TX_ON);		

      /* leave carrier on for 160msec */
      for (tics=0; tics<128; tics++)
      {
         /* pet the watchdog */
         WatchDog();

         /* pause for 1.25msec */
         for (timer0=0; !timer0;) {}
      }

      /* loop through all bytes in the current message */
      for (n=0; n<N; n++)
      {
         /* pet the watchdog */
         WatchDog();
      
         /* get next byte in message */
         bd1 = msg[n];

         /* loop through the 8 bits of the current byte */
         for (i=0; i<8; i++)
         {
            /* select the sense of the first half-bit */
            if(BD1_MSB) PORTAWR = POSITIVE_PHASE; else PORTAWR = NEGATIVE_PHASE;

            /* transmit the first half-bit for 1.25msec */
            for (timer0=0; !timer0;) {}
         
            /* select the sense of the second half-bit */
            if(BD1_MSB) PORTAWR = NEGATIVE_PHASE; else PORTAWR = POSITIVE_PHASE;

            /* transmit the second half-bit for 1.25msec */
            for (timer0=0; !timer0;) {}

            /* shift the next bit to the MSB */
            bd1 = bd1 << 1;				
         }
      }

      /* clear PTT_PHASE and PTT_PHASE\ */
      Psd835PortAClear(PTT_TX_ON);

      /* turn off the oscillator */
      Psd835PortAClear(PTT_PLL_ON);

      /* turn power off */
      Psd835PortAClear(PTT_PON);

      /* reset timer tic to 10msec: 9216 ticks = 0.01 sec, 65536 - 9216 = 0xdc00 */
      di(); TH0 = RTH0 = 0xdc; TL0 = RTL0 = 0; ei();
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to set the baud rate for the XA uart                          */
/*------------------------------------------------------------------------*/
/**
   This function sets the baud rate for XA serial ports 0 and 1.  Timer1 is
   used as the baud rate generator.
*/
void SetBaud(unsigned int baud)                 
{
   unsigned int n, reload;

   /* stack-check assertion */
   assert(StackOk());

   di();
   n = 57600 / baud;                               // 76800 = osc / 64
   reload = 65536 - n;
   RTH1 = TH1 = (unsigned char)((reload >> 8) & 0xff);      // msb
   RTL1 = TL1 = (unsigned char)(reload & 0xff);             // lsb
   ei();
}

/*------------------------------------------------------------------------*/
/* pause program execution for a specified number of clock tics           */
/*------------------------------------------------------------------------*/
/**
   This function pauses program execution for a specified number of clock
   tics (1 tic = 1/921600 sec).  
*/
void WaitTicks(unsigned char tics) 
{
   int load;

   load = 256 - tics;             // each tick is (4.0 / 3686400) seconds
   di();
   ET0 = 0;                       // stop timer
   TR0 = 0;
   TH0 = 0xff;                    // load value into timer 0 
   TL0 = (unsigned char)load;
   timer0 = 0;                    // reset flag 
   ET0 = 1;                       // enable timer overflow interrupt 
   TR0 = 1;                       // start timer 
   ei();
   while(timer0 == 0) {};
}

/*------------------------------------------------------------------------*/
/* pause program execution for a specified number of milliseconds         */
/*------------------------------------------------------------------------*/
/**
   This function pauses program execution for a specified number of
   milliseconds. 
*/
void Wait(unsigned int millisec)        
{
   while(millisec>0)
   {
      di();
      ET0 = 0;                    // stop timer
      TR0 = 0;
      TH0 = 0xfc;                 // load value into timer 0 
      TL0 = 0x66;
      timer0 = 0;                 // reset flag 
      ET0 = 1;                    // enable timer overflow interrupt 
      TR0 = 1;                    // start timer 
      WFEED1 = 0xa5;
      WFEED2 = 0x5a;
      ei();
      while(timer0 == 0) IDL = 1; 
      --millisec;
   }
}
