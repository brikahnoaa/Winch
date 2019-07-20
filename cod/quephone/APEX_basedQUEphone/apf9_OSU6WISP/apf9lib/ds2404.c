#ifndef DS2404_H
#define DS2404_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: ds2404.c,v 1.11 2007/04/24 01:43:05 swift Exp $
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
 * This compilation unit provides an interface to the Dallas Semiconductor
 * DS2404 EconoRAM Time Chip which implements a real time clock and an
 * interval timer.   These functions were written using the data sheet
 * (document number 110501) which was downloaded from the web URL:
 * www.maxim-ic.com
 *
 * \begin{verbatim}
 * $Log: ds2404.c,v $
 * Revision 1.11  2007/04/24 01:43:05  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.10  2006/04/21 13:45:38  swift
 * Changed copyright attribute.
 *
 * Revision 1.9  2005/06/14 18:56:44  swift
 * Implement a watch-dog alarm to prevent power-down exceptions from killing a float.
 *
 * Revision 1.8  2004/12/29 23:04:36  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.7  2003/11/20 18:59:35  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.6  2003/07/19 22:39:32  swift
 * Minor changes related to definitions of various time periods (hour, min,
 * day, year).
 *
 * Revision 1.5  2003/07/03 22:51:10  swift
 * Added 'trace()' to several functions.
 *
 * Revision 1.4  2003/06/10 00:11:13  swift
 * Modified RTC RAM test.
 *
 * Revision 1.3  2003/06/07 20:31:19  swift
 * Modifications to use difftime() to compute time differences.
 *
 * Revision 1.2  2003/06/04 23:24:32  swift
 * Implemented RTC alarm functionality.
 *
 * Revision 1.1  2003/06/03 14:43:37  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define Ds2404RtcChangeLog "$RCSfile: ds2404.c,v $  $Revision: 1.11 $   $Date: 2007/04/24 01:43:05 $"

#include <time.h>

/* declare the masks for the alarm flags of the DS2404's status register */
extern const unsigned char Ds2404RtfMask;
extern const unsigned char Ds2404ItfMask;

/* function prototypes */
int Ds2404Init(void);
int Ds2404RamTest(void);
int Ds2404Reset(void);
unsigned char Ds2404StatusByte(void);
time_t ialarm(void);
int IntervalAlarmCheck(void);
int IntervalAlarmDisable(void);
int IntervalAlarmEnable(void);
int IntervalAlarmGet(time_t *sec,unsigned char *tics);
int IntervalAlarmSet(time_t sec,unsigned char tics);
int IntervalTimerGet(time_t *sec,unsigned char *tics);
int IntervalTimerSet(time_t sec,unsigned char tics);
time_t itimer(void);
int RtcAlarmCheck(void);
int RtcAlarmDisable(void);
int RtcAlarmEnable(void);
int RtcAlarmGet(time_t *sec,unsigned char *tics);
int RtcAlarmSet(time_t sec,unsigned char tics);
int RtcGet(time_t *sec,unsigned char *tics);
int RtcSet(time_t sec,unsigned char tics);
int SetAlarm(time_t sec);
time_t time(time_t *sec);

/* define some constants of nature */
#define Year (31536000L)
#define Day  (86400L)
#define Hour (3600L)
#define Min  (60L)

#endif /* DS2404_H */

#include <logger.h>
#include <assert.h>

/* function prototypes for local functions */
int Ds2404Read(unsigned int addr,unsigned int bytes,void *buf);
int Ds2404Write(unsigned int addr,unsigned int bytes,const void *buf);

/* define DS2404 register locations */
static const unsigned int StatusReg        = 0x0200;
static const unsigned int ControlReg       = 0x0201;
static const unsigned int RtcReg           = 0x0202;
static const unsigned int IntervalTimerReg = 0x0207;
static const unsigned int RtcAlarmReg      = 0x0210;
static const unsigned int IntervalAlarmReg = 0x0215;

/* define the status register alarm flags */
const unsigned char Ds2404RtfMask = 0x01;
const unsigned char Ds2404ItfMask = 0x02;

/* define status register activation masks */
#define RTE 0xf7  /* 11110111b */
#define ITE 0xef  /* 11101111b */
#define CCE 0xdf  /* 11011111b */

/* define control register activation masks */
#define OSC  0x10
#define AUTO 0x20
#define STOP 0x40

/*------------------------------------------------------------------------*/
/* function to initialize the DS2404 real-time-clock                      */
/*------------------------------------------------------------------------*/
/**
   This function initializes the DS2404 real-time-clock so that all three
   timers (rtc, interval-timer, cycle-counter) are running and interrupts
   are disabled for the rtc, interval-timer, and cycle-counter.  The values
   of the rtc and interval-timer registers as well as their respective alarm
   registers are not modified.

   This function returns a positive value on success and zero otherwise.
*/
int Ds2404Init(void)
{
   /* define the logging signature */
   static cc FuncName[] = "Ds2404Init()";

   /* initialize the return value */
   int status=1;
   
   /* set the interval-timer for manual control and turn on the oscillator */
   const unsigned char creg = OSC & ~AUTO & ~STOP;
   
   /* disable CCE interrupts, enable RTC,ITE interrupts */
   const unsigned char sreg = ~CCE | (RTE & ITE);
   
   /* initialize the status register of the DS2404 */
   if (Ds2404Write(StatusReg,1,&sreg)<=0)
   {
      /* create the message */
      static cc msg[]="Attempt to write the "
         "DS2404 status register failed.\n";

      /* log the read error */      
      LogEntry(FuncName,msg);
      
      /* indicate failure */
      status=0;
   }

   /* write the control byte to the control register */
   if (Ds2404Write(ControlReg,1,&creg)<=0) 
   {
      /* create the message */
      static cc msg[]="Attempt to write the "
         "DS2404 control register failed.\n";
      
      /* log the write error */
      LogEntry(FuncName,msg);

      /* indicate failure */
      status=0;
   }

   return status;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
int Ds2404Reset(void)
{
   /* define the logging signature */
   static cc FuncName[] = "Ds2404Reset()";

   int status=1;

   if (Ds2404Init()<=0) status=0;
   else if (IntervalTimerSet(0,0)<=0) status=0;
   else if (IntervalAlarmSet(Hour,0)<=0) status=0;
   else if (RtcAlarmSet((time_t)(-1),(time_t)(-1))<=0) status=0;

   if (status<=0)
   {
      /* create the message */
      static cc msg[]="Attempt to reset the DS2404 failed.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
int Ds2404RamTest(void)
{
   /* define the logging signature */
   static cc FuncName[] = "Ds2404Test()";

   unsigned int addr, j;
   unsigned int nError=0;
   unsigned char tics, buf[32];
   time_t now;
      
   /* initialize the return value and get the current calendar time */
   int status = (RtcGet(&now,&tics)<=0) ? 0 : 1;

   for(addr=0x000; addr<=0x200 && status>0; addr+=0x20)
   {
      /* create a test pattern to write to ram */
      for(j=0; j<32; j++) buf[j] = (unsigned char)(addr/0x20 + j);

      /* write the test pattern to RAM */
      if (Ds2404Write(addr, 32, buf)<=0)
      {
         /* create the message */
         static cc msg[]="Attempt to write test pattern to RAM failed.\n";

         /* log the message */
         LogEntry(FuncName,msg);

         /* indicate failure */
         status=0;
      }
   }

   for(addr=0x000; addr<=0x1e0 && status>0; addr+=0x20)
   {
      /* reinitialize the buffer before reading from RAM */
      for(j=0; j<32; j++) buf[j]=0;
      
      /* read the test pattern back from RAM */
      if (Ds2404Read(addr, 32, buf)<=0)
      {
         /* create the message */
         static cc msg[]="Attempt to read test pattern from RAM failed.\n";

         /* log the message */
         LogEntry(FuncName,msg);
            
         /* indicate failure */
         status=0;
      }
      
      /* verify the test pattern */
      for(j=0; j<32; j++)
      {
         /* check the current byte against the expected test pattern */
         if (buf[j] != (unsigned char)(addr/0x20 + j))
         {
            /* create the message */
            static cc format[]="Error at addr=0x%03x: wr=%02x, rd=%02x\n";

            /* log the message */
            LogEntry(FuncName,format,addr+j,(int)(addr/0x20 + j), (int)buf[j]);

            status=0; ++nError;
         }
      }
   }

   /* reinitialize the DS2404 and restore the calendar time */
   if (Ds2404Reset()<=0 || RtcSet(now,tics)<=0) 
   {
      /* create the message */
      static cc msg[]="Attempt to reinitialize the DS2404 failed.\n";

      /* log the message */
      LogEntry(FuncName,msg);

      /* indicate failure */
      status=0;
   }

   if (status)
   {
      if (!nError)
      {
         /* create the message */
         static cc msg[]="DS2404 RAM test passed.\n";
         
         /* log the message */
         LogEntry(FuncName,msg);
      }
      
      else
      {
         /* create the message */
         static cc format[]="DS2404 RAM test failed with %d errors.\n";
         
         /* log the message */
         LogEntry(FuncName,format,nError);

         /* indicate failure */
         status=0;
      }
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to return the DS2404 status byte                              */
/*------------------------------------------------------------------------*/
/**
   This function reads the status byte from the DS2404's status register.
   It is important to note that the DS2404 clears the alarm flags after each
   read.

   On success, this function returns the value of the DS2404's status
   register.  If the attempt to read the status register failed then this
   function returns (unsigned char)(-1).
*/
unsigned char Ds2404StatusByte(void) 
{
   /* initialize the function's return status */
   unsigned char status=(unsigned char)(-1);

   /* read the status register and extract the interval-timer alarm flag */
   if (Ds2404Read(StatusReg,1,&status)<=0) {status=(unsigned char)(-1);}

   return status;
}

/*------------------------------------------------------------------------*/
/* function to read the DS2404 interval-timer alarm register              */
/*------------------------------------------------------------------------*/
/**
   This function reads DS2404 interval-timer alarm register.  On success,
   the value of the interval-timer register is returned.  Failure is
   signaled with a return value of (time_t)(-1).
*/
time_t ialarm(void)
{
   time_t inow;
   unsigned char tics;

   /* read the interval alarm register */
   if (IntervalAlarmGet(&inow,&tics)<=0) inow=(time_t)(-1);

   return inow;
}

/*------------------------------------------------------------------------*/
/* function to deterimine if the DS2404 interval-timer alarm is ringing   */
/*------------------------------------------------------------------------*/
/**
   This function determines whether or not the DS2404's interval-timer alarm
   flag (within the status register) is asserted.  If it is asserted then
   the interval-timer alarm has expired...the alarm is ringing.  Refer to
   p5-7 of the data sheet referenced above.

   This function returns a positive number if the interval-timer alarm flag
   is asserted and zero if the flag is not asserted.  A negative return
   value indicates a failure occurred when reading the status register.
*/
int IntervalAlarmCheck(void)
{
   unsigned char Status;

   /* initialize the function's return status */
   int status=-1;

   /* read the status register and extract the interval-timer alarm flag */
   if (Ds2404Read(StatusReg,1,&Status)>0)
   {
      status = (Status&Ds2404ItfMask) ? 1 : 0;
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to disable the interval-timer alarm                           */
/*------------------------------------------------------------------------*/
/**
   This function disables the DS2404's interval-timer interrupt.  Refer to
   p5-8 of the DS2404 data sheet (referenced above).

   This function returns a positive number if the command to disable the
   interval-timer was successfully written to the DS2404 and zero otherwise.
*/
int IntervalAlarmDisable(void)
{
   /* define the logging signature */
   static cc FuncName[] = "IntervalAlarmDisable()";

   unsigned char sreg;
   
   /* initialize return value */
   int status=1;

   /* read the status register */
   if (Ds2404Read(StatusReg,1,&sreg)<=0)
   {
      /* create the message */
      static cc msg[]="Attempt to read DS2404 status register failed.\n";

      /* log the message */
      LogEntry(FuncName,msg);

      /* indicate failure */
      status=0;
   }
   else
   {
      /* disable the interval-timer interrupt */
      sreg |= ~ITE;

      /* write the status register */
      if (Ds2404Write(StatusReg,1,&sreg)<=0)
      {
         /* create the message */
         static cc msg[]="Attempt to write DS2404 status register failed.\n";

         /* log the read error */      
         LogEntry(FuncName,msg);

         /* indicate failure */
         status=0;
      }
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to enable the interval-timer alarm                           */
/*------------------------------------------------------------------------*/
/**
   This function enables the DS2404's interval-timer interrupt.  Refer to
   p5-8 of the DS2404 data sheet (referenced above).

   This function returns a positive number if the command to enable the
   interval-timer was successfully written to the DS2404 and zero otherwise.
*/
int IntervalAlarmEnable(void)
{
   /* define the logging signature */
   static cc FuncName[] = "IntervalAlarmEnable()";

   unsigned char sreg;
   
   /* initialize return value */
   int status=1;

   /* read the status register */
   if (Ds2404Read(StatusReg,1,&sreg)<=0)
   {
      /* create the message */
      static cc msg[]="Attempt to read DS2404 status register failed.\n";

      /* log the read error */      
      LogEntry(FuncName,msg);

      /* indicate failure */
      status=0;
   }
   else
   {
      /* enable the interval-timer interrupt */
      sreg &= ITE;

      /* write the status register */
      if (Ds2404Write(StatusReg,1,&sreg)<=0)
      {
         /* create the message */
         static cc msg[]="Attempt to write DS2404 status register failed.\n";

         /* log the read error */      
         LogEntry(FuncName,msg);

         /* indicate failure */
         status=0;
      }
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to read the interval-timer alarm setting from the DS2402      */
/*------------------------------------------------------------------------*/
/**
   This function reads the time setting from the DS2404's interval-timer
   alarm register.  The interval-timer is a 5-byte binary counter that
   increments 256 times per second.  The least significant byte (lsb) is a
   count of fractional seconds (1/256 resolution) and the most significant
   4-bytes are a count of seconds.  When the interval-timer register equals
   the interval-timer alarm register then the alarm is activated.  If the
   control register is set properly then the alarm flag in the status
   register is asserted and an interrupt is generated.  See p5-8 of the
   DS2404 data sheet (referenced above).

      \begin{verbatim}
      output:

         sec....The number of seconds stored in the upper 4-bytes of the
                interval-timer alarm register.

         tics...The number of tics (1/256 seconds) stored in the lsb of the
                interval-timer alarm register.

         This function returns a positive number if at least the upper
         4-bytes of the interval-timer alarm register were successfully
         read.  A return value of 1 indicates that only the seconds portion
         of the register was read.  A return value of 2 indicates that both
         the seconds and tics were successfully read from the register.
         This function returns zero if it failed to read the seconds portion
         of the register in which case both of the function arguments will
         also be set to zero.  This function returns a negative number if
         either of the function arguments are invalid (NULL).
      \end{verbatim}
*/
int IntervalAlarmGet(time_t *sec,unsigned char *tics)
{
   /* define the logging signature */
   static cc FuncName[] = "IntervalAlarmGet()";

   /* initialize the function's return status */
   int status=-1;

   /* validate integer size */
   assert(sizeof(time_t)==4);
   
   /* validate the function parameters */
   if (!sec || !tics)
   {
      /* create the message */
      static cc msg[]="NULL function argument.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* read the seconds from the interval-timer alarm register */
   else if (Ds2404Read(IntervalAlarmReg+1,4,sec)>0)
   {
      /* reinitialize the status to indicate success */
      status=1;

      /* read the tics from the interval-timer alarm register */
      if (Ds2404Read(IntervalAlarmReg,1,tics)>0) status=2; else (*tics)=0;
   }

   else
   {
      /* initialize the return value and the function parameters */      
      status=0; (*sec)=0; (*tics)=0;

      /* log the read error */
      if (debuglevel>=2)
      {
         /* create the message */
         static cc msg[]="Attempt to read DS2404 register failed.\n";

         /* log the message */
         LogEntry(FuncName,msg);
      }
   }
      
   return status;
}

/*------------------------------------------------------------------------*/
/* function to write the interval-timer alarm setting to the DS2402       */
/*------------------------------------------------------------------------*/
/**
   This function writes the time setting to the DS2404's interval-timer
   alarm register.  The interval-timer is a 5-byte binary counter that
   increments 256 times per second.  The least significant byte (lsb) is a
   count of fractional seconds (1/256 resolution) and the most significant
   4-bytes are a count of seconds.  When the interval-timer register equals
   the interval-timer alarm register then the alarm is activated.  If the
   control register is set properly then the alarm flag in the status
   register is asserted and an interrupt is generated.  See p5-8 of the
   DS2404 data sheet (referenced above).

      \begin{verbatim}
      output:
         sec....The number of seconds to write to the upper 4-bytes of the
                interval-timer alarm register.

         tics...The number of tics (1/256 seconds) to write to the lsb of
                the interval-timer alarm register.

         This function returns a positive number if interval-timer alarm
         register was successfully written otherwise zero is returned.
      \end{verbatim}
*/
int IntervalAlarmSet(time_t sec,unsigned char tics)
{
   /* define the logging signature */
   static cc FuncName[] = "IntervalAlarmSet()";

   /* initialize the function's return status */
   int status=1;

   /* validate integer size */
   assert(sizeof(time_t)==4);

   /* write the seconds to the interval-timer alarm register */
   if (Ds2404Write(IntervalAlarmReg+1,4,&sec)<=0)
   {
      /* create the message */
      static cc msg[]="Attempt to write seconds to DS2404 register failed.\n";

      /* log the message */
      LogEntry(FuncName,msg);
      
      /* indicate failure */
      status=0;
   }
   
   /* write the tics to the interval-timer alarm register */
   else if (Ds2404Write(IntervalAlarmReg,1,&tics)<=0) 
   {
      /* create the message */
      static cc msg[]="Attempt to write tics to DS2404 register failed.\n";

      /* log the message */
      LogEntry(FuncName,msg);
      
      /* indicate failure */
      status=0;
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to read the interval-timer register from the DS2402           */
/*------------------------------------------------------------------------*/
/**
   This function reads the time setting from the DS2404's interval-timer
   register.  The interval-timer is a 5-byte binary counter that increments
   256 times per second.  The least significant byte (lsb) is a count of
   fractional seconds (1/256 resolution) and the most significant 4-bytes
   are a count of seconds.  See p5-8 of the DS2404 data sheet (referenced
   above).

      \begin{verbatim}
      output:

         sec....The number of seconds stored in the upper 4-bytes of the
                interval-timer register.

         tics...The number of tics (1/256 seconds) stored in the lsb of the
                interval-timer register.

         This function returns a positive number if at least the upper
         4-bytes of the interval-timer register were successfully read.  A
         return value of 1 indicates that only the seconds portion of the
         register was read.  A return value of 2 indicates that both the
         seconds and tics were successfully read from the register.  This
         function returns zero if it failed to read the seconds portion of
         the register in which case both of the function arguments will also
         be set to zero.  This function returns a negative number if either
         of the function arguments are invalid (NULL).
      \end{verbatim}
*/
int IntervalTimerGet(time_t *sec,unsigned char *tics)
{
   /* define the logging signature */
   static cc FuncName[] = "IntervalTimerGet()";

   /* initialize the function's return status */
   int status=-1;

   /* validate integer size */
   assert(sizeof(time_t)==4);

   /* validate the function parameters */
   if (!sec || !tics) 
   {
      /* create the message */
      static cc msg[]="NULL function argument.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* read the seconds from the interval-timer register */
   else if (Ds2404Read(IntervalTimerReg+1,4,sec)>0)
   {
      /* reinitialize the status to indicate success */
      status=1;

      /* read the tics from the interval-timer register */
      if (Ds2404Read(IntervalTimerReg,1,tics)>0) status=2; else (*tics)=0;
   }
 
   else
   {
      /* initialize the return value and the function parameters */      
      status=0; (*sec)=0; (*tics)=0;

      /* log the read error */
      if (debuglevel>=2)
      {
         /* create the message */
         static cc msg[]="Attempt to read DS2404 register failed.\n";

         /* log the message */
         LogEntry(FuncName,msg);
      }
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to write the interval-timer setting to the DS2402             */
/*------------------------------------------------------------------------*/
/**
   This function writes the time setting to the DS2404's interval-timer
   register.  The interval-timer is a 5-byte binary counter that increments
   256 times per second.  The least significant byte (lsb) is a count of
   fractional seconds (1/256 resolution) and the most significant 4-bytes
   are a count of seconds.  When the interval-timer register equals the
   interval-timer alarm register then the alarm is activated.  If the
   control register is set properly then the alarm flag in the status
   register is asserted and an interrupt is generated.  See p5-8 of the
   DS2404 data sheet (referenced above).

      \begin{verbatim}
      output:

         sec....The number of seconds to write to the upper 4-bytes of the
                interval-timer register.

         tics...The number of tics (1/256 seconds) to write to the lsb of
                the interval-timer register.

         This function returns a positive number if interval-timer register
         was successfully written otherwise zero is returned.
      \end{verbatim}
*/
int IntervalTimerSet(time_t sec,unsigned char tics)
{
   /* define the logging signature */
   static cc FuncName[] = "IntervalTimerSet()";

   /* initialize the function's return status */
   int status=1;

   /* validate integer size */
   assert(sizeof(time_t)==4);

   /* write the seconds to the interval-timer register */
   if (Ds2404Write(IntervalTimerReg+1,4,&sec)<=0)
   {
      /* create the message */
      static cc msg[]="Attempt to write seconds to DS2404 register failed.\n";

      /* log the error */
      LogEntry(FuncName,msg);

      /* indicate failure */
      status=0;
   }
   
   /* write the tics to the interval-timer register */
   else if (Ds2404Write(IntervalTimerReg,1,&tics)<=0) 
   {
      /* create the message */
      static cc msg[]="Attempt to write tics to DS2404 register failed.\n";

      /* log the error */
      LogEntry(FuncName,msg);
      
      /* indicate failure */
      status=0;
   }
      
   return status;
}

/*------------------------------------------------------------------------*/
/* function to read the DS2404 interval-timer                             */
/*------------------------------------------------------------------------*/
/**
   This function reads DS2404 interval-timer register.  On success, the
   value of the interval-timer register is returned.  Failure is signaled
   with a return value of (time_t)(-1).
*/
time_t itimer(void)
{
   time_t inow;
   unsigned char tics;

   /* read the interval timer register */
   if (IntervalTimerGet(&inow,&tics)<=0) inow=(time_t)(-1);

   return inow;
}

/*------------------------------------------------------------------------*/
/* function to deterimine if the DS2404 real-time-clock alarm is ringing  */
/*------------------------------------------------------------------------*/
/**
   This function determines whether or not the DS2404's real-time-clock alarm
   flag (within the status register) is asserted.  If it is asserted then
   the real-time-clock alarm has expired...the alarm is ringing.  Refer to
   p5-7 of the data sheet referenced above.

   This function returns a positive number if the real-time-clock alarm flag
   is asserted and zero if the flag is not asserted.  A negative return
   value indicates a failure occurred when reading the status register.
*/
int RtcAlarmCheck(void)
{
   unsigned char Status;

   /* initialize the function's return status */
   int status=-1;

   /* read the status register and extract the real-time-clock alarm flag */
   if (Ds2404Read(StatusReg,1,&Status)>0)
   {
      status = (Status&Ds2404RtfMask) ? 1 : 0;
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to disable the real-time-clock alarm                          */
/*------------------------------------------------------------------------*/
/**
   This function disables the DS2404's real-time-clock interrupt.  Refer to
   p5-8 of the DS2404 data sheet (referenced above).

   This function returns a positive number if the command to disable the
   real-time-clock was successfully written to the DS2404 and zero otherwise.
*/
int RtcAlarmDisable(void)
{
   /* define the logging signature */
   static cc FuncName[] = "RtcAlarmDisable()";

   unsigned char sreg;
   
   /* initialize return value */
   int status=1;

   /* read the status register */
   if (Ds2404Read(StatusReg,1,&sreg)<=0)
   {
      /* create the message */
      static cc msg[]="Attempt to read DS2404 status register failed.\n";

      /* log the read error */      
      LogEntry(FuncName,msg);

      /* indicate failure */
      status=0;
   }
   else
   {
      /* disable the real-time-clock interrupt */
      sreg |= ~RTE;

      /* write the status register */
      if (Ds2404Write(StatusReg,1,&sreg)<=0)
      {
         /* create the message */
         static cc msg[]="Attempt to write DS2404 status register failed.\n";

         /* log the read error */      
         LogEntry(FuncName,msg);
   
         /* indicate failure */
         status=0;
      }
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to enable the real-time-clock alarm                           */
/*------------------------------------------------------------------------*/
/**
   This function enables the DS2404's real-time-clock interrupt.  Refer to
   p5-8 of the DS2404 data sheet (referenced above).

   This function returns a positive number if the command to enable the
   real-time-clock was successfully written to the DS2404 and zero otherwise.
*/
int RtcAlarmEnable(void)
{
   /* define the logging signature */
   static cc FuncName[] = "RtcAlarmEnable()";

   unsigned char sreg;
   
   /* initialize return value */
   int status=1;

   /* read the status register */
   if (Ds2404Read(StatusReg,1,&sreg)<=0)
   {
      /* create the message */
      static cc msg[]="Attempt to read DS2404 status register failed.\n";
      
      /* log the message */
      LogEntry(FuncName,msg);

      /* indicate failure */
      status=0;
   }
   else
   {
      /* enable the real-time-clock interrupt */
      sreg &= RTE;

      /* write the status register */
      if (Ds2404Write(StatusReg,1,&sreg)<=0)
      {
         /* create the message */
         static cc msg[]="Attempt to read DS2404 status register failed.\n";
      
         /* log the message */
         LogEntry(FuncName,msg);
         
         /* indicate failure */
         status=0;
      }
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to read the real-time-clock alarm setting from the DS2402     */
/*------------------------------------------------------------------------*/
/**
   This function reads the time setting from the DS2404's real-time-clock
   alarm register.  The real-time-clock is a 5-byte binary counter that
   increments 256 times per second.  The least significant byte (lsb) is a
   count of fractional seconds (1/256 resolution) and the most significant
   4-bytes are a count of seconds.  When the real-time-clock register equals
   the real-time-clock alarm register then the alarm is activated.  If the
   control register is set properly then the alarm flag in the status
   register is asserted and an interrupt is generated.  See p5-8 of the
   DS2404 data sheet (referenced above).

      \begin{verbatim}
      output:
         sec....The number of seconds stored in the upper 4-bytes of the
                real-time-clock alarm register.

         tics...The number of tics (1/256 seconds) stored in the lsb of the
                real-time-clock alarm register.

         This function returns a positive number if at least the upper
         4-bytes of the real-time-clock alarm register were successfully
         read.  A return value of 1 indicates that only the seconds portion
         of the register was read.  A return value of 2 indicates that both
         the seconds and tics were successfully read from the register.
         This function returns zero if it failed to read the seconds portion
         of the register in which case both of the function arguments will
         also be set to zero.  This function returns a negative number if
         either of the function arguments are invalid (NULL).
      \end{verbatim}
*/

int RtcAlarmGet(time_t *sec,unsigned char *tics)
{
   /* define the logging signature */
   static cc FuncName[] = "RtcAlarmGet()";

   /* initialize the function's return status */
   int status=-1;

   /* validate integer size */
   assert(sizeof(time_t)==4);
   
   /* validate the function parameters */
   if (!sec || !tics) 
   {
      /* create the message */
      static cc msg[]="NULL function argument.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* read the seconds from the real-time-clock alarm register */
   else if (Ds2404Read(RtcAlarmReg+1,4,sec)>0)
   {
      /* reinitialize the status to indicate success */
      status=1;

      /* read the tics from the real-time-clock alarm register */
      if (Ds2404Read(RtcAlarmReg,1,tics)>0) status=2; else (*tics)=0;
   }

   else
   {
      /* initialize the return value and the function parameters */      
      status=0; (*sec)=0; (*tics)=0;

      if (debuglevel>=2)
      {
         /* create the message */
         static cc msg[]="Attempt to read DS2404 register failed.\n";

         /* log the read error */         
         LogEntry(FuncName,msg);
      }
   }
      
   return status;
}

/*------------------------------------------------------------------------*/
/* function to write the real-time-clock alarm setting to the DS2402      */
/*------------------------------------------------------------------------*/
/**
   This function writes the time setting to the DS2404's real-time-clock
   alarm register.  The real-time-clock is a 5-byte binary counter that
   increments 256 times per second.  The least significant byte (lsb) is a
   count of fractional seconds (1/256 resolution) and the most significant
   4-bytes are a count of seconds.  When the real-time-clock register equals
   the real-time-clock alarm register then the alarm is activated.  If the
   control register is set properly then the alarm flag in the status
   register is asserted and an interrupt is generated.  See p5-8 of the
   DS2404 data sheet (referenced above).

      \begin{verbatim}
      output:
         sec....The number of seconds to write to the upper 4-bytes of the
                real-time-clock alarm register.

         tics...The number of tics (1/256 seconds) to write to the lsb of
                the real-time-clock alarm register.

         This function returns a positive number if real-time-clock alarm
         register was successfully written otherwise zero is returned.
      \end{verbatim}
*/
int RtcAlarmSet(time_t sec,unsigned char tics)
{
   /* define the logging signature */
   static cc FuncName[] = "RtcAlarmSet()";

   /* initialize the function's return status */
   int status=1;

   /* validate integer size */
   assert(sizeof(time_t)==4);

   /* write the seconds to the real-time-clock alarm register */
   if (Ds2404Write(RtcAlarmReg+1,4,&sec)<=0)
   {
      /* create the message */
      static cc msg[]="Attempt to write seconds to DS2404 register failed.\n";

      /* log the message */
      LogEntry(FuncName,msg);
      
      /* indicate failure */
      status=0;
   }
   
   /* write the tics to the real-time-clock alarm register */
   else if (Ds2404Write(RtcAlarmReg,1,&tics)<=0) 
   {
      /* create the message */
      static cc msg[]=   "Attempt to write tics to DS2404 register failed.\n";
      
      /* log the error */
      LogEntry(FuncName,msg);
      
      /* indicate failure */
      status=0;
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to read the real-time clock register from the DS2402          */
/*------------------------------------------------------------------------*/
/**
   This function reads the time setting from the DS2404's real-time clock
   register.  The real-time clock is a 5-byte binary counter that increments
   256 times per second.  The least significant byte (lsb) is a count of
   fractional seconds (1/256 resolution) and the most significant 4-bytes
   are a count of seconds.  See p5-8 of the DS2404 data sheet (referenced
   above).

      \begin{verbatim}
      output:
         sec....The number of seconds stored in the upper 4-bytes of the
                real-time clock register.  The reference time is arbitrary
                and user-defined.  To be POSIX and UNIX compliant, this
                should be equal to the number of seconds since Jan 1, 1970
                at 00:00:00 GMT.

         tics...The number of tics (1/256 seconds) stored in the lsb of the
                real-time clock register.

         This function returns a positive number if at least the upper
         4-bytes of the real-time clock register were successfully read.  A
         return value of 1 indicates that only the seconds portion of the
         register was read.  A return value of 2 indicates that both the
         seconds and tics were successfully read from the register.  This
         function returns zero if it failed to read the seconds portion of
         the register in which case both of the function arguments will also
         be set to zero.  This function returns a negative number if either
         of the function arguments are invalid (NULL).
      \end{verbatim}
*/
int RtcGet(time_t *sec,unsigned char *tics)
{
   /* define the logging signature */
   static cc FuncName[] = "RtcGet()";

   /* initialize the function's return status */
   int status=-1;

   /* validate integer size */
   assert(sizeof(time_t)==4);
   
   /* validate the function parameters */
   if (!sec || !tics) 
   {
      /* create the message */
      static cc msg[]="NULL function argument.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* read the seconds from the real-time clock register */
   else if (Ds2404Read(RtcReg+1,4,sec)>0)
   {
      /* reinitialize the status to indicate success */
      status=1;

      /* read the tics from the real-time clock register */
      if (Ds2404Read(RtcReg,1,tics)>0) status=2; else (*tics)=0;
   } 
 
   else
   {
      /* initialize the return value and the function parameters */      
      status=0; (*sec)=0; (*tics)=0;

      /* log the read error */
      if (debuglevel>=2)
      {
         /* create the message */
         static cc msg[]="Attempt to read DS2404 register failed.\n";

         /* log the read error */         
         LogEntry(FuncName,msg);
      }
   }
      
   return status;
}

/*------------------------------------------------------------------------*/
/* function to write the real-time clock setting to the DS2402             */
/*------------------------------------------------------------------------*/
/**
   This function writes the time setting to the DS2404's real-time clock
   register.  The real-time clock is a 5-byte binary counter that increments
   256 times per second.  The least significant byte (lsb) is a count of
   fractional seconds (1/256 resolution) and the most significant 4-bytes
   are a count of seconds. See p5-8 of the DS2404 data sheet (referenced
   above).

      \begin{verbatim}
      output:
         sec....The number of seconds to write to the upper 4-bytes of the
                real-time clock register.  The reference time is arbitrary
                and user-defined.  To be POSIX and UNIX compliant, this
                should be equal to the number of seconds since Jan 1, 1970
                at 00:00:00 GMT.

         tics...The number of tics (1/256 seconds) to write to the lsb of
                the real-time clock register.

         This function returns a positive number if real-time clock register
         was successfully written otherwise zero is returned.
      \end{verbatim}
*/
int RtcSet(time_t sec,unsigned char tics)
{
   /* define the logging signature */
   static cc FuncName[] = "RtcSet()";

   /* initialize the function's return status */
   int status=1;

   /* validate integer size */
   assert(sizeof(time_t)==4);
   
   /* write the seconds to the real-time clock register */
   if (Ds2404Write(RtcReg+1,4,&sec)<=0)
   {
      /* create the message */
      static cc msg[]="Attempt to write seconds to DS2404 register failed.\n";
      
      /* log the error */
      LogEntry(FuncName,msg);
      
      /* indicate failure */
      status=0;
   }
   
   /* write the tics to the real-time clock register */
   else if (Ds2404Write(RtcReg,1,&tics)<=0) 
   {
      /* create the message */
      static cc msg[]="Attempt to write tics to DS2404 register failed.\n";
      
      /* log the error */
      LogEntry(FuncName,msg);
      
      /* indicate failure */
      status=0;
   }
      
   return status;
}

/*------------------------------------------------------------------------*/
/* function to activate the interval alarm                                */
/*------------------------------------------------------------------------*/
/**
   This function sets and activates an alarm that will generate an interrupt
   when the alarm expires.  Various sanity checks are done to ensure that
   the 32-bit interval-timer will not roll over before the alarm expires.
   The alarm periods are also limited to the closed interval
   [MinAlarmSec, MaxAlarmSec] as a failsafe against a float inadvertently
   committing suicide.  The interval-alarm interrupt is also enabled so that
   when the alarm expires, an interrupt will be generated.

      \begin{verbatim}
      input:
         alarm ... The time (seconds) when the alarm is supposed to expire.
                   When the value of the interval-timer register equals the
                   value of the interval-alarm register then the alarm
                   expires and an interrupt is generated (unless the
                   interval alarm is subsequently disabled).

      output:
         This function returns a positive value on success and zero otherwise.
      \end{verbatim}
*/
int SetAlarm(time_t alarm)
{
   /* define the logging signature */
   static cc FuncName[] = "SetAlarm()";

   unsigned char tics;
   time_t now,alarmcheck=-1;
   int tries=0,status;

   /* define the minimum and the maximum alarm periods */
   const time_t MinAlarmSec=5, MaxAlarmSec=7*Hour;
   
   do
   {
      /* initialize return value */
      status=1;
      
      /* get the current time from the interval timer */
      if (IntervalTimerGet(&now,&tics)<=0)
      {
         /* create the message */
         static cc msg[]="Attempt to read interval timer failed.\n";
         
         /* log the message */
         LogEntry(FuncName,msg);
         
         /* indicate failure */
         status=0;
      }

      /* validate the interval-timer */
      if (now<0 || (now+MaxAlarmSec)<now)
      {
         /* create the message */
         static cc msg[]="Resetting interval-timer to 0 sec.\n";
         
         /* log the message */
         LogEntry(FuncName,msg);
         
         /* reset the interval-time to zero */
         if (IntervalTimerSet((now=0),0)<=0)
         {
            /* create the message */
            static cc msg[]="Attempt to reset interval timer failed.\n";
            
            /* log the message */
            LogEntry(FuncName,msg);

            /* indicate failure */
            status=0;
         }
      }

      /* sanity check of alarm time */
      if (difftime(alarm,now)<MinAlarmSec)
      {
         /* create the message */
         static cc format[]="Alarm time (%ld sec) less than %ld sec "
            "after interval-timer (%ld sec).  Resetting to %ld sec.\n";
            
         /* log the message */
         LogEntry(FuncName,format,alarm,MinAlarmSec,now,now+MinAlarmSec);

         /* reset the alarm time */
         alarm = now + MinAlarmSec;
      }

      /* sanity check of alarm time */
      else if (difftime(alarm,now)>MaxAlarmSec)
      {
         /* create the message */
         static cc format[]="Alarm time (%ld sec) more than %ld sec "
                  "after interval-timer (%ld sec).  Resetting to %ld sec.\n";

         /* log the message */
         LogEntry(FuncName,format,alarm,MaxAlarmSec,now,now+MaxAlarmSec);
         
         /* reset the alarm time */
         alarm = now + MaxAlarmSec;
      }

      /* write the alarm to the DS2404 and then read it back */
      if (IntervalAlarmSet(alarm,0)<=0 || IntervalAlarmGet(&alarmcheck,&tics)<=0)
      {
         /* create the message */
         static cc msg[]="Attempt to set interval alarm failed.\n";
         
         /* log the message */
         LogEntry(FuncName,msg);

         /* indicate failure */
         status=0;
      }

      /* verify that the alarm is properly set */
      else if (alarm!=alarmcheck)
      {
         /* create the message */
         static cc format[]="Verification of interval alarm "
                  "failed: %ld sec != %ld sec.\n";

         /* log the message */
         LogEntry(FuncName,format,alarm,alarmcheck);

         /* indicate failure */
         status=0;
      }
      
      /* log the current interval-timer value and alarm */
      else if (debuglevel>=3)
      {
         /* create the message */
         static cc format[]="Success: itimer=%ld sec, ialarm=%ld sec\n";
         
         /* log the message */
         LogEntry(FuncName,format,now,alarm);
      }
      
   }
   while (status<=0 && (tries++)<3);

   /* enable interval-alarm interrupts */
   if (IntervalAlarmEnable()<=0)
   {
      /* create the message */
      static cc msg[]="Attempted to enable interval alarm failed.\n";
      
      /* log the message */
      LogEntry(FuncName,msg);

      /* indicate failure */
      status=0;
   }
   
   return status; 
}

/*------------------------------------------------------------------------*/
/* function to implement the ANSI C time() function using the DS2404 RTC  */
/*------------------------------------------------------------------------*/
/**
   This function returns the value of the DS2404's RTC register.  This
   counter-register contains the number of seconds since a reference time
   that is arbitrary and user-defined.  To be POSIX and UNIX compliant, this
   should be equal to the number of seconds since Jan 1, 1970 at 00:00:00
   GMT.  This function is intended to implement the behavior and function of
   the ANSI C standard library function.

      \begin{verbatim} output:
         sec ... If this pointer is non-NULL then it will be initialized
                 with the value in the RTC counter register.  If NULL then
                 this function parameter is safely ignored.  

         On success, this function returns the value stored in the value in
         the RTC counter register.  On error, this function returns the
         special value: (time_t)(-1).
      \end{verbatim}
*/
time_t time(time_t *sec)
{
   /* define the logging signature */
   static cc FuncName[] = "time()";

   time_t T; unsigned char tic;

   /* read the RTC register */
   if (RtcGet(&T,&tic)<=0)
   {
      /* create the message */
      static cc msg[]="Attempt to read RTC failed.\n";

      /* note the error in the log */
      LogEntry(FuncName,msg);

      /* initialize the according to the ANSI standard */
      T=(time_t)(-1);
   }

   /* initialize the function parameter */
   if (sec) {*sec = T;} 

   return T;
}

