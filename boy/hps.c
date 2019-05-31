// hps.c
#include <main.h>

/******************************************************************************\
** HPADMain.c
** Simple AD program for MPC board.  Not for CSACDAQ.
** MPC uses CF2 pin 19 to enable ADC and REF ICs, whereas CSACDAQ uses 28.
** MPC uses CF2 pin 26 to power ON. 1=ON, 0=OFF  
**
** Revised to set pin 28 (AD_REF_SHDN_PIN) on to enable AD on receipe card.(8/08/2011)
** Revised to control pin 35 to turn on/off pre-amp (rev 9b). (08/07/2011)
**	
**	Program to read A/D values of NOAADAQ2 board using ADS8344 chip.			
**	
**	Release:		2001/12/22
**  Version 2		2003/08/21
**					Updated mcp for Palm 8.0 Codewarrior and newer BIOS 3.00
**
*****************************************************************************
**	
**	COPYRIGHT (C) 1995-2001 PERSISTOR INSTRUMENTS INC., ALL RIGHTS RESERVED
**	
**	Developed by: John H. Godley for Persistor Instruments Inc.
**	254-J Shore Road, Bourne, MA 02532  USA
**	jhgodley@persistor.com - http://www.persistor.com
**	
**	Copyright (C) 1995-2001 Persistor Instruments Inc.
**	All rights reserved.
**	
*****************************************************************************
**	
**	Copyright and License Information
**	
**	Persistor Instruments Inc. (hereafter, PII) grants you (hereafter,
**	Licensee) a non-exclusive, non-transferable license to use the software
**	source code contained in this single source file with hardware products
**	sold by PII. Licensee may distribute binary derivative works using this
**	software and running on PII hardware products to third parties without
**	fee or other restrictions.
**	
**	PII MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
**	SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
**	IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
**	OR NON-INFRINGEMENT. PII SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY
**	LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING THE SOFTWARE OR
**	ITS DERIVATIVES.
**	
**	By using or copying this Software, Licensee agrees to abide by the
**	copyright law and all other applicable laws of the U.S. including, but
**	not limited to, export control laws, and the terms of this license. PII
**	shall have the right to terminate this license immediately by written
**	notice upon Licensee's breach of, or non-compliance with, any of its
**	terms. Licensee may be held legally responsible for any copyright
**	infringement or damages resulting from Licensee's failure to abide by
**	the terms of this license. 
**	
\******************************************************************************/

//#define DEBUG			// comment this out to turn off diagnostics
//#include <cfxbios.h>		// Persistor BIOS and I/O Definitions
//#include <cfxpico.h>		// Persistor PicoDOS Definitions
//#include <cfxad.h>		// Generic SPI A-D QPB Driver for CFx
//#include "ADExamples.h"	// Common definitions for the A-D Examples
//#include <ADS8344.h>
//#include <stdio.h>
//#include	<stdlib.h>
//#include	<ctype.h>
//#include <dosdrive.h>

#define PICODEV 4
#ifndef NOT_R216_RECIPECARD
#ifndef NO_AD_REF_SHDN_PIN
#define     AD_REF_SHDN_PIN     28  // default /SHDN on Pii boards
#endif /*  */
#endif /*  */
//#define ADTYPE ADisADS8344
//#ifdef DEBUG
  	//#define	DBG(X)	X	// template:	DBG( cprintf("\n"); )
	//#pragma message	("!!! "__FILE__ ": Don't ship with DEBUG compile flag set!")
//#else
  //#define	DBG(X)		// nothing
//#endif
#define ADREF_SHDN_PIN 19 //For MPC board 3/12/2018 HM
#define HPSENS_PWR_ON  26 //For HP sensor power ON/OFF 1=ON, 0=OFF 3/12/2018 HM
//#define ADREF_SHDN_PIN 28 //For CSACDAQ

/******************************************************************************\
**	main
\******************************************************************************/
//main()
	//{

///
// HP Sensor stats
int hpsRead (HpsStats *hps) {
  float ret;
  readStat( &ret, 0);
  readStat( &(hps->curr), 0);
  readStat( &(hps->volt), 1);
  readStat( &(hps->pres), 2);
  readStat( &(hps->humi), 3);
  // turn off
  readStat( &ret, 4);
  return 0;
} // hpsRead

///
// read one stat
int readStat (float *ret, int c) {
	///char		c, r;
	bool		uni = true;		// true for unipolar, false for bipolar
	bool		sgl = true;		// true for single-ended, false for differential
	short	   sample,  *samples;
	CFxAD		adbuf, *ad;
	float		vref = VREF;   //defined in ADExamples.h
  	ushort	entryclock = TMGGetSpeed();
  	float    output;

	samples = (ushort *) malloc(8* sizeof(ushort)); 
	
	//Identify the program and build
	///printf("\nProgram: %s: %s %s \n", __FILE__, __DATE__, __TIME__);
	///cprintf("ADREF PIN = %d, Sensor board PWR ON pin = %d\n",AD_REF_SHDN_PIN, HPSENS_PWR_ON);
	///cprintf("This program is for MPC.You must use a different prog for CSACDAQ. Proceed ? y/n ");
   ///r=cgetc();
   ///cprintf("%c\n",r);
   ///putflush();
   ///if(r=='y'||r=='Y')
   {
		//Power on
		PIOSet(ADREF_SHDN_PIN);  //ADC and Ref IC power ON
		PIOSet(HPSENS_PWR_ON);   //HP-Sensor board power on from DiFar Power port  (J14) 
		
		// Initialize QPB to accept our A-D with its QSPI connection.
		ad = CFxADInit(&adbuf, ADSLOT, ADInitFunction);

		///cprintf("\nKey in 0 to 3 for a single channel ADC. '.' to quite\n");
		///cprintf("0 = current, 1 = Bat voltage, 2 = Pressure, 3 = Humidity\n");
		///cprintf("also Unipolar, Bipolar, Single-ended, Differential, Powerdown\n");
	 	///cprintf("\n%c%c> ", uni ? 'U' : 'B', sgl ? 'S' : 'D');
	   ///fflush(stdout);  
		
		///while (true)
			{
			///c = cgetc();
			///switch (c = toupper(c))
                        switch (c)
				{
				///case '0' : 
				///case '1' : 
				///case '2' : 
				///case '3' : 
				case 0 : 
				case 1 : 
				case 2 : 
				case 3 : 
					///sample = CFxADSample(ad, c - '0', uni, sgl, false);
					sample = CFxADSample(ad, c, uni, sgl, false);
					output=CFxADRawToVolts(ad, sample, vref, uni);
					///if (c=='0') 
					if (c==0) 
						{
						output=output*1000.;  //Current in mA
						///printf("Current  = %7.1f mA\n", output);
						}
					///if (c=='1') 
					if (c==1) 
						{
						output= output*100.;  //Voltage in V
						///printf("Bat Volt = %7.1f V\n", output);
						}

					///if (c=='2') 
					if (c==2) 
						{
						//output=(output*0.4+0.040)*2439.0;//pressure
						output=(output*0.4+0.040)*2494.0;//pressure
						///printf("Pressure = %7.1f mBar\n", output);
						}
					///if (c=='3') 
					if (c==3) 
						{
						output=(output*2-0.831)/0.029;//humidity
						///printf("Humidity = %7.1f %\n", output);
						}
                    *ret = output;
					break;
	   		///putflush();				
				//case 'U' :	uni = true;		break;
				//case 'B' :	uni = false;	break;
				//case 'S' :	sgl = true;		break;
				//case 'D' :	sgl = false;	break;
				//case 'P' :	CFxADPowerDown(ad, true);	break;
				///case '.' :	PIOClear(ADREF_SHDN_PIN); PIOClear(HPSENS_PWR_ON); return 0;
				case 4 :	PIOClear(ADREF_SHDN_PIN); PIOClear(HPSENS_PWR_ON); return 0;
				}
			}
        }
	return 0;

	}	// readStat

	
