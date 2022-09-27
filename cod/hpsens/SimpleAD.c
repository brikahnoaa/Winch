/******************************************************************************\
**SimpleAD.c Simple   A-D Example for Persistor CFx  
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
#include <cfxbios.h>		// Persistor BIOS and I/O Definitions
#include <cfxpico.h>		// Persistor PicoDOS Definitions
#include <cfxad.h>		// Generic SPI A-D QPB Driver for CFx
// Place non-PicoDAQ A-D definitions here - before including <ADExamples.h>
#include "ADExamples.h"	// Common definitions for the A-D Examples
#include <ADS8344.h>
#include <stdio.h>
#include	<stdlib.h>
#include	<ctype.h>
#include <dosdrive.h>

#define PICODEV 4
//#define ADTYPE ADisADS8344
#ifdef DEBUG
  	#define	DBG(X)	X	// template:	DBG( cprintf("\n"); )
	#pragma message	("!!! "__FILE__ ": Don't ship with DEBUG compile flag set!")
#else
  #define	DBG(X)		// nothing
#endif//#define NO_AD_REF_SHDN_PIN

/******************************************************************************\
**	main
\******************************************************************************/
main()
	{
	char		c;
	char     r;
	bool		uni = true;		// true for unipolar, false for bipolar
	bool		sgl = true;		// true for single-ended, false for differential
	short		i; 
	short	   sample,  *samples;
	CFxAD		adbuf, *ad;
	float		vref = VREF;
  	ushort	entryclock = TMGGetSpeed();
  	float    output;

	samples = (ushort *) malloc(8* sizeof(ushort)); 
	
	//printf("%lu\n: ", KB_SIZE);
	//Identify the program and build
	printf("\nProgram: %s: %s %s \n", __FILE__, __DATE__, __TIME__);
	//printf("\nIf you are using Persistor R216AU recipe card, provide reference voltage by"); 
	//printf("\nsetting pin 28 to ON by 'PS 28' command before launching this program.\n");

	printf("ADREF PIN = %d",AD_REF_SHDN_PIN);
	PIOSet(AD_REF_SHDN_PIN);
	printf("\nADTYPE= %d",ADTYPE);
	printf("\nTexas Instrument 8-ch 16-bit AD ADS8345");
	printf("\nRef Voltage = %f",VREF);
	// Initialize QPB to accept our A-D with its QSPI connection.
	ad = CFxADInit(&adbuf, ADSLOT, ADInitFunction);

	cprintf("\nKey 0-7 for a single channel, 8 for all eight, '.' to quit\n");
	cprintf("also Unipolar, Bipolar, Single-ended, Differential, Powerdown\n");
   cprintf("Turning on the power of pre-amp by PIOSet(35)? (y/n) ");
   
   r=cgetc();
   cprintf("%c\n",r);
   if(r=='y'||r=='Y')PIOSet(35);
   PIOSet(AD_REF_SHDN_PIN);
   PIOClear(31);//set gain0=0
   PIOClear(32);//set gain1=0
   
	while (true)
		{
		printf("\n%c%c> ", uni ? 'U' : 'B', sgl ? 'S' : 'D');
		fflush(stdout);
		c = cgetc();
		switch (c = toupper(c))
			{
			case '0' : 
			case '1' : 
			case '2' : 
			case '3' : 
			case '4' : 
			case '5' : 
			case '6' : 
			case '7' : 
				DBG( PinClear(25); )
				sample = CFxADSample(ad, c - '0', uni, sgl, false);
				DBG( PinSet(25); )
				output=CFxADRawToVolts(ad, sample, vref, uni);
				printf("\nCh %d = %04X, %5d, %7.5fV",
					c - '0', sample, sample, CFxADRawToVolts(ad, sample, vref, uni));
				break;
			case 'R' : // repeat single channel
				CFxADLock(ad);
				DBG( PinClear(25); )
				sample = CFxADRepeat(ad);
				DBG( PinSet(25); )
				CFxADUnlock(ad);
				printf("\nCh ? = %04X, %5d, %6.4fV",
					sample, sample, CFxADRawToVolts(ad, sample, vref, uni));
				break;
			case '8' : 
				DBG( PinClear(25); )
				samples = CFxADSampleBlock(ad, 0, 8, 0, uni, sgl, false);
				DBG( PinSet(25); )
				for (i = 0; i < 8; i++)
				{
					printf("\n  Ch %d = %04X, %5u, %6.4fV",
						i, samples[i], samples[i], CFxADRawToVolts(ad, samples[i], vref, uni));
				}

				break;
			case 'U' :	uni = true;		break;
			case 'B' :	uni = false;	break;
			case 'S' :	sgl = true;		break;
			case 'D' :	sgl = false;	break;
			case 'P' :	CFxADPowerDown(ad, true);	break;
			case '.' :	return 0;
			}
		}

   if(r=='y'||r=='Y')PIOClear(35);

	return 0;

	}	//____ main() ____//

	
