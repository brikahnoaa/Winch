#ifndef LT1598AD_H
#define LT1598AD_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: lt1598ad.c,v 1.6 2007/04/24 01:43:05 swift Exp $
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
 * This translation unit is an API to the APF9's LTC1598 AD convertor and
 * associated circuitry.  This API is based on the APF9 schematic (SeaBird
 * Document 32618, Rev A, Sheets 1,2,4,5) and on the data sheet for the
 * Linear Technology LTC1598 12-bit AD convertor.
 *
 * \begin{verbatim}
 * $Log: lt1598ad.c,v $
 * Revision 1.6  2007/04/24 01:43:05  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.5  2006/04/21 13:45:38  swift
 * Changed copyright attribute.
 *
 * Revision 1.4  2004/12/29 23:04:36  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.3  2004/04/14 20:48:21  swift
 * Revision E of the APF9 made some hardware changes to earlier versions.  The
 * PSD935 was replaced by a PSD835 and the 20mA serial loop was changed to be a
 * real serial IO port.
 *
 * Revision 1.2  2003/11/20 18:59:35  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.1  2003/11/12 22:30:48  swift
 * Initial revision
 *
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define apf9adChangeLog "$RCSfile: lt1598ad.c,v $ $Revision: 1.6 $ $Date: 2007/04/24 01:43:05 $"

/* function prototypes */
float         Amps(unsigned char AmpsAd8);
float         BatAmps(void);
unsigned char BatAmpsAd8(void);
float         BatVolts(void);
unsigned char BatVoltsAd8(void);
float         Barometer(void);
unsigned char BarometerAd8(void);
float         Com1Amps(void);
unsigned char Com1AmpsAd8(void);
float         Com2Amps(void);
unsigned char Com2AmpsAd8(void);
float         inHg(unsigned char inHgAd8);
unsigned char PistonPosition(void);
float         Volts(unsigned char VoltsAd8);

#endif /* LT1598AD_H */

#include <apf9.h>
#include <psd835.h>
#include <logger.h>

/* prototypes for static functions */
static unsigned int  Lt1598Ad12(unsigned char channel, unsigned int n);
static unsigned char Lt1598Ad8(unsigned char channel);
static unsigned int  Ltc1598(void);

/* AD converter channels */
static enum AdChannels {AIRPRESSURE,BATTERY,CURRENT,COM1,
                        COM2,PISTON_POSITION,NCHANNELS};

/*------------------------------------------------------------------------*/
/* function to convert APF9 8-bit AD current-counts to Amps               */
/*------------------------------------------------------------------------*/
/**
   This function converts APF9 8-bit AD current measurements to Amps.
   Refer to SWJ14:108,p151 for details.

       \begin{verbatim}
       input:
          AmpsAd8 .... The 8-bit AD current-counts to be converted to
                       electric current (Amps).

       output:
          This function returns the electric current (in Amps) that
          corresponds to the 8-bit AD count.
       \end{verbatim}
*/
float Amps(unsigned char AmpsAd8)
{
   return 16.5*AmpsAd8/4095;
}

/*------------------------------------------------------------------------*/
/* function to measure the barometric pressure internal to the float      */
/*------------------------------------------------------------------------*/
/**
   This function measures the absolute barometric pressure using the SenSym
   SDX15A2 pressure sensor on the APF9.  The sensitivity coefficent is
   computed analytically to be dPdV=22.36 in.Hg/V based on data sheets for
   the SenSym SDX15A2 pressure sensor plus the LT1789-1 instrument
   amplifier.  See SWJ14:79,p122-123 for the derivation that is based on the
   APF9's schematic.

   This function returns the internal barometric pressure (in.Hg) of the
   float. 
*/
float Barometer(void)
{
   /* sensitivity coefficient (in.Hg/V) */
   const float dPdV = 22.36; /* in.Hg/V */

   /* atmospheric pressure */
   const float Patm = 29.92; /* in.Hg */
   
   /* conversion for a 12-bit AD converter on a 3.3V system */
   float V = 3.3*Lt1598Ad12(AIRPRESSURE, 16)/4095;

   /* apply the sensitivity coefficient to the voltage */
   return dPdV*V - Patm;
}

/*------------------------------------------------------------------------*/
/* get the 8-bit AD conversion of the internal pressure sensor            */
/*------------------------------------------------------------------------*/
/**
   This function returns the 8-bit AD conversion of the internal SenSym
   SDX15A2 barometric pressure sensor.  The relationship between the
   pressure and the 8-bit AD counts is C = p*(dC/dP) where
   dC/dP=3.456cnt/in.Hg is the sensitivity coefficent.  Zero corresponds to
   an absolute barometric pressure of 0"Hg and 255 corresponds to an
   absolute barometric pressure of 74"Hg.  See SWJ14:79,p122-123 for the
   derivation that is based on the APF9's schematic.
*/
unsigned char BarometerAd8(void)
{
   /* get the AD conversion that has been decimated from 12-bits down to 8-bits */
   return Lt1598Ad8(AIRPRESSURE);
}

/*------------------------------------------------------------------------*/
/* function to measure the electric current consumption from the battery  */
/*------------------------------------------------------------------------*/
/**
   This function measures the total electric current being consumed from the
   batteries.  The return value is in Amps.  Refer to SWJ14:78,p121 for
   derivations of the equations used in this function.
*/
float BatAmps(void)
{
   /* conversion for a 12-bit AD converter on a 3.3V system */
   float amps = 3.3 * (float)Lt1598Ad12(CURRENT, 16) / 4095; 

   /* conversion for a 12-bit AD converter on a 3.3V system */
   return amps;
}

/*------------------------------------------------------------------------*/
/* function to measure the 8-bit AD conversion of the electric current    */
/*------------------------------------------------------------------------*/
/**
   This function measures the total electric current being consumed from the
   batteries.  The return value is in 8-bit AD counts.  Refer to
   SWJ14:78,p121 for derivations of relevant equations.  The relationship
   between electric current and 8-bit counts is: I = C*(dI/dC) where I is
   the electric current (mA), C is the 8-bit counts returned by this
   function, and dI/dC=12.94mA/count is the sensitivity.  The total range
   is [0mA, 3300mA].
*/
unsigned char BatAmpsAd8(void)
{
   return Lt1598Ad8(CURRENT);
}

/*------------------------------------------------------------------------*/
/* measure the battery voltage using APF9's 12-bit AD convertor           */
/*------------------------------------------------------------------------*/
/**
   This function measures the battery voltage using the APF9's LTC1598
   12-bit AD convertor.  Refer to SWJ14:82 for details and derivation of the
   resistor divider ratio.  This function returns the battery potential
   (volts). 
*/
float BatVolts(void)
{
   /* conversion for a 12-bit AD converter on a 3.3V system */
   float v = (3.3 * (float)Lt1598Ad12(BATTERY, 16) / 4095); 
   
   /* apply the resistor divider ratio (ref SWJ14:82) */
   return v*5.99;
}

/*------------------------------------------------------------------------*/
/* measure the 8-bit battery volt-counts using APF9's 12-bit AD convertor */
/*------------------------------------------------------------------------*/
/**
   This function measures the battery voltage and returns an 8-bit AD
   count.  Refer to SWJ14:82 for details and derivations.
*/
unsigned char BatVoltsAd8(void)
{
   return Lt1598Ad8(BATTERY);
}

/*------------------------------------------------------------------------*/
/* measure the electric current consumption by COM1 serial port           */
/*------------------------------------------------------------------------*/
/**
   This function measures the electric current being consumed by the device
   attached to the COM1 serial port.  The return value is in Amps.  Refer to
   SWJ14:78,p121 for derivations of the equations used in this function.
*/
float Com1Amps(void)
{
   /* conversion for a 12-bit AD converter on a 3.3V system */
   float amps = 3.3 * (float)Lt1598Ad12(COM1, 16) / 4095; 

   /* conversion for a 12-bit AD converter on a 3.3V system */
   return amps;
}

/*------------------------------------------------------------------------*/
/* measure 8-bit AD conversion of electric current to COM1 serial port    */
/*------------------------------------------------------------------------*/
/**
   This function measures electric current being consumed from the device
   attached to the COM1 serial port.  The return value is in 8-bit AD
   counts.  Refer to SWJ14:78,p121 for derivations of relevant equations.
   The relationship between electric current and 8-bit counts is:
   I=C*(dI/dC) where I is the electric current (mA), C is the 8-bit counts
   returned by this function, and dI/dC=12.94mA/count is the sensitivity.
   The total range is [0mA, 3300mA].
*/
unsigned char Com1AmpsAd8(void)
{
   return Lt1598Ad8(COM1);
}

/*------------------------------------------------------------------------*/
/* function to measure the electric current consumption from the battery  */
/*------------------------------------------------------------------------*/
/**
   This function measures the electric current being consumed by the device
   attached to the COM2 serial port.  The return value is in Amps.  Refer to
   SWJ14:78,p121 for derivations of the equations used in this function.
*/
float Com2Amps(void)
{
   /* conversion for a 12-bit AD converter on a 3.3V system */
   float amps = 3.3 * (float)Lt1598Ad12(COM2, 16) / 4095; 

   /* conversion for a 12-bit AD converter on a 3.3V system */
   return amps;
}

/*------------------------------------------------------------------------*/
/* measure 8-bit AD conversion of electric current to COM2 serial port    */
/*------------------------------------------------------------------------*/
/**
   This function measures electric current being consumed from the device
   attached to the COM1 serial port.  The return value is in 8-bit AD
   counts.  Refer to SWJ14:78,p121 for derivations of relevant equations.
   The relationship between electric current and 8-bit counts is:
   I=C*(dI/dC) where I is the electric current (mA), C is the 8-bit counts
   returned by this function, and dI/dC=12.94mA/count is the sensitivity.
   The total range is [0mA, 3300mA].
*/
unsigned char Com2AmpsAd8(void)
{
   return Lt1598Ad8(COM2);
}

/*------------------------------------------------------------------------*/
/* function to convert APF9 8-bit AD barometer-counts to in.Hg            */
/*------------------------------------------------------------------------*/
/**
   This function converts APF9 8-bit AD barometer-counts inches of mercury.
   Refer to SWJ14:79 for derivation and details.

       \begin{verbatim}
       input:
          inHgAd8 .... The 8-bit AD barometer-counts to be converted to
                       inches of Hg.

       output:
          This function returns the barometric pressure (in.Hg) that
          corresponds to the 8-bit AD count.
       \end{verbatim}
*/
float inHg(unsigned char inHgAd8)
{
   /* sensitivity coefficient (see SWJ14:79) */
   const float dPdC = 1/3.456;

   /* atmospheric pressure */
   const float Patm = 29.92; /* in.Hg */

   /* convert barometer-counts to inches of Hg (relative to atmospheric pressure) */
   return dPdC*inHgAd8 - Patm; 
}

/*------------------------------------------------------------------------*/
/* function to clock the AD conversion out of the LTC1598 AD convertor    */
/*------------------------------------------------------------------------*/
/*
   This function clocks the AD conversion out of the LTC1598 AD convertor.
   It was written by John Backes at SeaBird, Inc.  I have not taken the time
   to understand it completely.
*/
static unsigned int Ltc1598(void)
{
   int i;
   unsigned int v;

   unsigned char *q = (unsigned char *)&v;
   Psd835PortEClear(ADC_CLK);
   Psd835PortEClear(ADC_MUX_CS);				// set both select lines low
   Psd835PortDClear(ADC_CS);
   WaitTicks(5);

   for(i = 0;i < 3;i++)
	{
      Psd835PortESet(ADC_CLK);
      WaitTicks(5);
      Psd835PortEClear(ADC_CLK);
      WaitTicks(5);
	}
   bd1 = 0;								// bit addressable byte
   for(i = 0;i < 4;i++)
	{
      Psd835PortESet(ADC_CLK);
      bd1 = bd1 << 1;
      BD1_LSB = ADC_DOUT;
      WaitTicks(5);
      Psd835PortEClear(ADC_CLK);
      WaitTicks(5);
	}
   q[1] = bd1;
   bd1 = 0;								// bit addressable byte
   for(i = 0;i < 8;i++)
	{
      Psd835PortESet(ADC_CLK);
      bd1 = bd1 << 1;
      BD1_LSB = ADC_DOUT;
      WaitTicks(5);
      Psd835PortEClear(ADC_CLK);
      WaitTicks(5);
	}
   q[0] = bd1;

   return(v);
}

/*------------------------------------------------------------------------*/
/* driver function for LTC1598 12-bit AD convertor                        */
/*------------------------------------------------------------------------*/
/*
  This is a hardware driver for the LTC1598 12-bit AD convertor.  It was
  written by John Backes (SeaBird, Inc) and I have not analyzed exactly what
  it does.  Six of the eight channels are assigned to various AD
  measurements.  Channels 6 and 7 are not used and tied to ground.

     \begin{verbatim}
     input:
        channel ... This selects the channel for the conversion.
        n ......... The number of samples to average into the final result.

     output:
        This function will return an integer in the closed range [0, 4095]
        that maps to the voltage range [0V, 3.3V].
     \end{verbatim}
*/
static unsigned int Lt1598Ad12(unsigned char channel, unsigned int n)
{
   /* define the logging signature */
   static cc FuncName[] = "Lt1598Ad12()";
   
   int i;
   unsigned long int sum=0;
   unsigned int v=0;

   if (channel<NCHANNELS)
   {
      Psd835PortCSet(ANALOG_PWR_ON);
      Wait(5);
      Psd835PortESet(ADC_CLK);
      Psd835PortESet(ADC_MUX_CS);				// both select lines are high to start
      Psd835PortDSet(ADC_CS);
      channel = channel | 8;		// set enable bit
      channel = channel << 4;		// data is sent MSB first
      for(i=0; i<4; i++)			// send enable and A/D channel select
      {
         Psd835PortEClear(ADC_CLK);
         WaitTicks(5);
         if((channel & 0x80) == 0x80)
         {
            Psd835PortDSet(ADC_DIN);
         }
         else
         {
            Psd835PortDClear(ADC_DIN);
         }
         Psd835PortESet(ADC_CLK);
         WaitTicks(5);
         channel = channel << 1;
      }
      Psd835PortEClear(ADC_CLK);
      Psd835PortEClear(ADC_MUX_CS);		 // set both select lines low
      Psd835PortDClear(ADC_CS);
      Wait(5);						// wait for low pass filter to settle
      for(i=0; i<n; i++)
      {
         sum += (unsigned long)Ltc1598();
         Psd835PortDSet(ADC_CS);
         Psd835PortESet(ADC_CLK);
         WaitTicks(5);
         Psd835PortEClear(ADC_CLK);
         WaitTicks(5);
         Psd835PortESet(ADC_CLK);
         WaitTicks(5);
      }
      Psd835PortESet(ADC_MUX_CS);				// leave both select lines high
      WaitTicks(5);
      v = (unsigned int)(sum / n);
      Psd835PortDClear(ADC_DIN);
      Psd835PortDClear(ADC_CS);
      Psd835PortEClear(ADC_CLK);
      Psd835PortEClear(ADC_MUX_CS);
      Psd835PortCClear(ANALOG_PWR_ON);
   }
   else
   {
      /* create the message */
      static cc format[]="Invalid AD channel: %d.\n";
      
      /* log the message */
      LogEntry(FuncName,format,channel);
   }
   
   return(v);
}

/*------------------------------------------------------------------------*/
/* function to decimate the 12-bit AD cnoversion down to 8-bits           */
/*------------------------------------------------------------------------*/
/*
   This function decimates the 12-bit AD conversion to fit into 8-bits.  It
   calls Lt1598Ad12() and then scales the result to fit into 8-bits.  The
   channel for the electric current is scaled down to use only 31.3% of the
   available range.  This has the effect of (approximately) tripling the
   resolution.  Since this could also hypothetically overflow an 8-bit
   result, the final value is checked to make sure it is within 8-bit range.
   Overflows generate a full-scale 8-bit return value of 255.
*/
static unsigned char Lt1598Ad8(unsigned char channel)
{
   unsigned int rv;
   unsigned int counts;

   /* get the 12-bit AD conversion */
   rv = Lt1598Ad12(channel, 16);

   /* decimate the 12-bit conversion for the electric current into 8-bits */
   if(channel == CURRENT) {counts = rv/5; if((rv%5) >= 2) ++counts;}

   /* decimate the 12-bit AD counts to fit into 8-bits */
   else {counts = rv/16; if((rv%16) > 7) ++counts;}

   /* check for overflow */
   if (counts>255) counts=255;
      
   return (unsigned char)counts;
}

/*------------------------------------------------------------------------*/
/* function to measure the buoyancy engine's high pressure pump piston    */
/*------------------------------------------------------------------------*/
/**
   This function measures the buoyancy engine's high pressure pump piston.
   The piston position is returned as an 8-bit AD count in the closed range
   [0, 255].
*/
unsigned char PistonPosition(void)
{
   return Lt1598Ad8(PISTON_POSITION);
}

/*------------------------------------------------------------------------*/
/* function to convert APF9 8-bit AD volt-counts to volts                 */
/*------------------------------------------------------------------------*/
/**
   This function converts APF9 8-bit AD volt measurements to volts.
   Refer to SWJ14:81,p125 for derivations and details.

       \begin{verbatim}
       input:
          VoltsAd8 .... The 8-bit AD volt-counts to be converted to volts.

       output:
          This function returns the voltage that corresponds to the 8-bit AD
          count.
       \end{verbatim}
*/
float Volts(unsigned char VoltsAd8)
{
   return 19.767*VoltsAd8/255;
}
