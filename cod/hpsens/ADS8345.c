/******************************************************************************\
**	ADS8345.c	(also for ADS8344)	QPB Driver for ADS8345 SPI A-D		
**	For testing we are using NMPCS0 instead of NMPCS3
**	
**	First release:			April 29, 2002
*****************************************************************************
**	
**	Licensed by:	NOAA, CIMRS
**	http://www.pmel.noaa.gov/vents/acoustics.html
**	
*****************************************************************************
**	
**	Developed by:	Haru Matsumoto, NOAA
**	matsumoto@pmel.noaa.gov
**	Copyright (C) 2002, NOAA	All rights reserved.
**	
*****************************************************************************
**	
**	Copyright and License Information
**	
**	
\******************************************************************************/

#include	<cfxbios.h>		// Persistor CF1 BIOS and I/O Definitions
#include	<cfxpico.h>		// Persistor CF1 PicoDOS Definitions
#include	<ADS8345.h>		// ADS8345/147 SPI A-D QPB Driver for CF1 

QPBDev ADS8345DevTemplate = 
	{
	NMPCS3,				// qslot			TEST NMPSC3 for ADS8345 our qspi slot HM 
	"ADS8345",			// devName			C string with device name (15 max) HM
	2400000,			// maxBaud			maximum baud rate in Hz for device
	16,					// bits				Bits Per Transfer
	iaLowSCK,			// clockPolar		SPI Clock Polarity
	captLead,			// clockPhase		SPI Clock Phase
	true,				// contCSMulti		Continue CS assert between mult xfrs
	true,				// autoTiming		Auto adjust timing to clock flag
	0,					// psDelaySCK		Min. Delay Before SCK (picoSecs)
	0,					// psDelayTXFR		Min. Delay After Transfer
	0,					// *rcvData			pointer to received data buffer
	0					// xfrCount			words transferred
	};

QPB		*Ads8345Table[QPBSLOTS];	// we can work with multiple devices

const uchar Ads8345SglChSel[8] = 	// convert chan to single-ended selector
		{ 0x00, 0x40, 0x10, 0x50, 0x20, 0x60, 0x30, 0x70 };
const uchar Ads8345DifChSel[8] = 	// convert chan to differential selector
		{ 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70 };

/******************************************************************************\
**	Ads8345Init		Initialize a Ads8345 QPB slot
\******************************************************************************/
bool Ads8345Init(ushort qslot)
	{

	if (qslot >= QPBSLOTS)		// sanity check
		return false;

	ADS8345DevTemplate.qslot = qslot;	// the one template field we must fill
	Ads8345Table[qslot] = QPBInitSlot(&ADS8345DevTemplate);

	return true;

	}	//____ Ads8345Init() ____//


/******************************************************************************\
**	Ads8345Lock		Lock the QPB for exclusive A-D use
\******************************************************************************/
bool Ads8345Lock(ushort qslot)
	{

	return QPBLockSlot(Ads8345Table[qslot]);
	
	}	//____ Ads8345Lock() ____//


/******************************************************************************\
**	Ads8345Unlock	Unlock the QPB for exclusive A-D use
\******************************************************************************/
bool Ads8345Unlock(ushort qslot)
	{

	return QPBUnlockSlot(Ads8345Table[qslot]);

	}	//____ Ads8345Unlock() ____//


/******************************************************************************\
**	Ads8345Sample		Sample the specified channel and return the raw results
\******************************************************************************/
ushort Ads8345SampleOrigin(ushort qslot, ushort chan, bool uni, bool sgl, bool pd)
	{
	ushort		adcmd[3];
	short		count = 2;

//	Setup the command word we send to the Ads8345 which is a combination of
//	selector bits that pick the channel, mode, and clocking.
	adcmd[0] = adcmd[1] =
			 		(sgl	? (Ads8345SglChSel[chan] | Ads8345SGL)
							: (Ads8345DifChSel[chan] | Ads8345DIF))
				|	(uni	? Ads8345UNI : Ads8345BIP)
				|	Ads8345CMD;

	adcmd[0] |= Ads8345EXT;
	if (pd)
		{
		adcmd[1] |= Ads8345PDFast;
		adcmd[count++] = 0;
		}
	else
		adcmd[1] |= Ads8345EXT;
		
//	Perform a QSPI transaction with two transfers, the first sets up the next
//	conversion, and the second value clocks out the previous channel results.
//	We don't set the QPBASync bit in the command count so the function will
//	wait for the conversion to complete before returning.

	QPBTransact(Ads8345Table[qslot], 0, count, adcmd);
//	The rcvData array contains the real result in its second word since word
//	one is the result of some previous conversion.

	return Ads8345Table[qslot]->dev->rcvData[1];

	}	//____ Ads8345SampleOrigin() ____//

/******************************************************************************\
**	ADS8345Sample		Sample the specified channel and return the raw results
**	
**	The ADS8345 really wants 32-bit transfers to perform the conversion and
**	return a 16-bit result, which, when complete, is stored in bits 22..7 of
**	the corrosponding 32-bit receive queue entry.
\******************************************************************************/
ushort Ads8345Sample(ushort qslot, ushort chan, bool uni, bool sgl, bool pd)
	{
	uchar		adcmd[4] = { 0, 0, 0, 0 };
	short		count = 2;
	ulong		result;

//	Setup the command word we send to the ADS8345 which is a combination of
//	selector bits that pick the channel, mode, and clocking.
	adcmd[0] =
			 		(sgl	? (Ads8345SglChSel[chan] | Ads8345SGL)
							: (Ads8345DifChSel[chan] | Ads8345DIF))
				|	(uni	? Ads8345UNI : Ads8345BIP)
				|	(pd		? Ads8345PDFull : Ads8345EXT)
				|	Ads8345CMD;

//	Perform a QSPI transaction with two transfers, the first sets up the next
//	conversion, and the second value clocks out the previous channel results.
//	We don't set the QPBASync bit in the command count so the function will
//	wait for the conversion to complete before returning.

	QPBTransact(Ads8345Table[qslot], 0, count, (ushort *) adcmd);
//	The rcvData array contains the real result in its second word since word
//	one is the result of some previous conversion.

	result = * (ulong *) Ads8345Table[qslot]->dev->rcvData;

	return (result >> 7L) & 0xFFFF;	// shift it into a proper 16-bit value

	}	//____ ADS8344Sample() ____//


/******************************************************************************\
**	Ads8345Repeat		Sample quickly repeating last Ads8345Sample parameters
\******************************************************************************/
ushort Ads8345Repeat(ushort qslot)
	{
		
	if (QPBTransact(Ads8345Table[qslot], 0, 0, 0) < 0)
		return -1;		// didn't work

	return Ads8345Table[qslot]->dev->rcvData[1];

	}	//____ Ads8345Sample() ____//


/******************************************************************************\
**	Ads8345SampleBlock	Sample multiple channels and return a pointer to array
\******************************************************************************/
ushort *Ads8345SampleBlock(ushort qslot, ushort first, ushort count, 
	 vfptr asyncf, bool uni, bool sgl, bool pd)
	{
	short			i, j;
	uchar			adcmd[32];
	
//	Setup the command words we send to the Ads8345 which is a combination of
//	selector bits that pick the channel, mode, and clocking.
//	for (i = first; i < count; i++)		// 98/09/13 bug --jhg
	CLEAR_OBJECT(adcmd);
	for (i = 0, j = first; i < count; i++, j++)
//		adcmd[i] =	(sgl	? (Ads8345SglChSel[i] | Ads8345SGL)
//							: (Ads8345DifChSel[i] | Ads8345DIF))
		{
		adcmd[i*4] =	(sgl	? (Ads8345SglChSel[j] | Ads8345SGL)
							: (Ads8345DifChSel[j] | Ads8345DIF))
						|	(uni	? Ads8345UNI : Ads8345BIP)
						|	Ads8345CMD
						|	Ads8345EXT;
//	If one of the power down modes is requested, we'll need to add one
//	final cycle of zero data to clock out the final conversion and let
//	the converter drop into low power mode.
		}
	if (pd)
		adcmd[(i-1)*4] &= ~Ads8345EXT;	
	
//	Perform a QSPI transaction with count+1 transfers, the first sets up the 
//	next conversions, and the subsequent value clocks out the previous channels
//	results. By specifying a structure (not a pointer to a structure), the
//	compiler will efficiently push the appropriate parameters for the call.
	QPBTransact(Ads8345Table[qslot], asyncf, count*2, (ushort *)adcmd);

//	If the caller specified a completion function then the results will have
//	to wait until the series of conversions completes and thier function gets
//	called to collect the data asynchronously.
	if (asyncf)
		return 0;

//	Return a pointer to the second entry in the results array. Remember the
//	first value is garbage from some previous conversion.
//	return &Ads8345Table[qslot]->dev->rcvData[1];
//	Return a pointer to the queue results array. 
	return ADS8345QueueToArray(Ads8345Table[qslot]->dev->rcvData, count);

	}	//____ Ads8345SampleBlock() ____//


/******************************************************************************\
**	Ads8345PowerDown		Put the Ads8345 into power down mode
**	
**	Use Ads8345PDFull for full powerdown, but up to 20ms wake recovery
**	Use Ads8345PDFast for full powerdown, but up to 20ms wake recovery
\******************************************************************************/
void Ads8345PowerDown(ushort qslot, bool pdfull)
	{
	ushort			adcmd[2];		// requires two commands ...

	// first, tell it which power down mode
	adcmd[0] = Ads8345CMD | (pdfull ? Ads8345PDFull : Ads8345PDFast);
	adcmd[1] = 0;	// next, send zero data and clocks to complete
	// it will power down at the end of the conversion

	QPBTransact(Ads8345Table[qslot], 0, 2, adcmd);
	
	}	//____ Ads8345PowerDown() ____//


/******************************************************************************\
**	ADS8345QueueToArray	Convert QSPI queue data to array
\******************************************************************************/
ushort *ADS8345QueueToArray(void *qp, ushort count)
	{
	short			i;
	static ushort	adat[8];

	ulong			*qpl = (ulong *) qp;
	for (i = 0; i < count; i++)
		adat[i] = qpl[i] >> 7L;
	return adat;

	}	//____ ADS8345QueueToArray() ____//
	
/******************************************************************************\
**	ADS8345RawToVolts	Convert raw sample to voltage
\******************************************************************************/
float Ads8345RawToVolts(ushort raw, float vref, bool )
	{

	return (float) raw * vref / 65536.0;

	}	//____ ADS8345RawToVolts() ____//
	
