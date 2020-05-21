/******************************************************************************\
**	#include	<ADExamples.h>	// Common definitions for the A-D Examples
**	
**	Release:		2001/12/22
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

#ifndef		__ADEXAMPLES_H
#define		__ADEXAMPLES_H

#include	<cfxad.h>		// Generic SPI A-D QPB Driver for PicoDOS

//#define DEBUG			// comment this out to turn off diagnostics
//#ifdef DEBUG
// #define	DBG(X)	X	// template:  DBG( PinSet(1); ) or DBG( cprintf("!"); )
//#else
//  #define	DBG(X)		// nothing
//#endif

// EITHER DEFINE THESE HERE OR IN YOUR C FILE BEFORE INCLUDING THIS FILE
// For example, the following values would be appropriate for a PicoDAQ
// RecipeCard modified to hold a Burr-Brown ADS8344 16-bit 8-channel A-D
// in place of the Max146 with an external 3.3V applied to VREF.
/**/
#define	ADSLOT	NMPCS3			// The QPB slot number (0..14)
//#define	ADTYPE	ADisMAX146		// The A-D selector from <cf1AD.h>
#define ADTYPE ADisADS8344
#define	VREF	2.5				// A-D reference voltage
#define	PRCPLG
/**/

// DEFAULT TO PICODAQ RECIPECARD IF ADSLOT AND ADTYPE ARE UNDEFINED
#ifndef	ADSLOT
  #ifndef	ADTYPE
    #define	PRCPLG		// define this for the Max146 RecipeCard
	#define	ADSLOT		NMPCS3
	#define	ADTYPE		ADisMAX146
	#ifndef	VREF	// could be external reference
	  #define	VREF	2.5
	#endif
  #endif
#endif

#if ADTYPE == ADisMAX146
  #include	<Max146.h>		// MAX146/147 SPI A-D QPB Driver for CF1 
  #define	ADInitFunction	Max146InitAD
#elif  (ADTYPE == ADisADS8344)
  #include	<ADS8344.h>		// Burr-Brown ADS8344 SPI A-D QPB Driver  
  #define	ADInitFunction	ADS8344Init
//#elif (ADTYPE == ?????)
// ?????
#endif


#endif	//	__ADEXAMPLES_H

