/******************************************************************************\
**	TPWM.c			Persistor CF2 PWM
**	
**	Release:		2002/06/27
*****************************************************************************
**	
**	COPYRIGHT (C) 2002 PERSISTOR INSTRUMENTS INC., ALL RIGHTS RESERVED
**	
**	Developed by: John H. Godley for Persistor Instruments Inc.
**	254-J Shore Road, Bourne, MA 02532  USA
**	jhgodley@persistor.com - http://www.persistor.com
**	
**	Copyright (C) 2002 Persistor Instruments Inc.
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

#if (CFX == 2)

#include	<cfxbios.h>		// Persistor BIOS and I/O Definitions
#include	<cfxpico.h>		// Persistor PicoDOS Definitions
#include	<mc68332.h>		// Persistor 68332 Module Map

enum 	// PWM Parameter RAM Fields
	{
	pwmChanControl,		// standard channel control fields
	pwmOldRis,			// time of the previous low-to-high transition 
	pwmPWMHi,			// current pulse high time
	pwmPWMPer,			// current PWM period
	pwmPWMRis			// current calculated rise time at beginning of pulse
	};


enum	// PWM Host Service Requests
	{
	pwmNoHostService,
	pwmImmediateUpdate,
	pwmInitialization,
	pwmUndefined
	};


void TPWMInit(short tch, short tcr, short period, short hightime, TPUPriority priority);
void TPWMUpdate(short tch, short period, short hightime);


/******************************************************************************\
**	TPWMInit	Initialize TPU channel as Pulse Width Modulator
**		see PWM_PulseWidthModulation_tpupn17.pdf in
**			C:\Program Files\Persistor\MotoCross Support\CFX\Docs\pdf\TPU
\******************************************************************************/
void TPWMInit(
	short tch,				// TPU channel 1-15
	short tcr,				// TCRn timebase 1 or 2
	short period,			// PWM period in TCRn cycles
	short hightime,			// PWM high time in TCRn cycles
	TPUPriority priority	// TPU channel priority, tpuDisabled to turn off
	)
	{
	
	TPUChannelPrioritySelect(tch, tpuDisabled);
	if (priority == tpuDisabled)			// just shutting it down
		return;
	TPUChannelFunctionSelect(tch, PWM);
	TPRAM[tch][pwmChanControl] = tbsOutput | 
		(tcr == 1 ? tbsCap1Match1 : tbsCap2Match2) | pacDoNotChange | 
		(hightime ? pscForcePinHigh : pscForcePinLow);
	TPRAM[tch][pwmPWMHi] =  hightime;
	TPRAM[tch][pwmPWMPer] = period;
	TPUHostServiceSession(tch, pwmInitialization, 0, priority, true);

	}	//____ TPWMInit() ____//


/******************************************************************************\
**	TPWMUpdate		Update period and high time for TPU PWM channel
**		see PWM_PulseWidthModulation_tpupn17.pdf in
**			C:\Program Files\Persistor\MotoCross Support\CFX\Docs\pdf\TPU
\******************************************************************************/
void TPWMUpdate(
	short tch,				// TPU channel 1-15
	short period,			// PWM period in TCRn cycles
	short hightime			// PWM high time in TCRn cycles
	)
	{

	* (ulong *) &TPRAM[tch][pwmPWMHi] = ((ulong) hightime << 16L) | (ulong) period;
	TPUHostServiceSession(tch, pwmImmediateUpdate, 0, tpuDontChangePriority, true);

	}	//____ TPWMUpdate() ____//


#endif	// (CFX == 2)
