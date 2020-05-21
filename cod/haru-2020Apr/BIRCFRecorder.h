/******************************************************************************\
**	CFxRecorder.h HM 09/15/2011
**	
** 2003-07-08 --jhg Changes for PicoDOS 3.x and FAT32
**		
**	#include	<NOAABIRecorder2.h>	// BigIDEA Recorder Definitions
**	
**	1.0 release:		Sunday, December 27, 1998
**	1.1 release:		Monday, May 10, 1999
**	1.2 release:		Sunday, July 18, 1999
**	1.3 release:		Friday, September 10, 1999
**	
*****************************************************************************
**	
**	Licensed by:	Persistor Instruments Inc. for the Peristor CF1
**	info@persistor.com - http://www.persistor.com
**	
*****************************************************************************
**	
**	Developed by:	John H. Godley dba Peripheral Issues
**	jhgodley@periph.com - http://www.periph.com
**	Copyright (C) 1996-1998 Peripheral Issues.	All rights reserved.
**	
*****************************************************************************
**	
**	Copyright and License Information
**	
**	Peripheral Issues (hereafter, PERIPH) grants you (hereafter, Licensee)
**	a non-exclusive, non-transferable license to use the software source
**	code contained in this single source file. Licensee may distribute
**	binary derivative works using this software to third parties without
**	fee or restrictions.
**	
**	PERIPH MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
**	BINARY SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
**	THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
**	PURPOSE, OR NON-INFRINGEMENT. PERIPH SHALL NOT BE LIABLE FOR ANY DAMAGES
**	SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING THE
**	BINARY SOFTWARE OR ITS DERIVATIVES.
**	
**	By using or copying this Software, Licensee agrees to abide by the
**	copyright law and all other applicable laws of the U.S. including, but
**	not limited to, export control laws, and the terms of this license. PERIPH
**	shall have the right to terminate this license immediately by written
**	notice upon Licensee's breach of, or non-compliance with, any of its
**	terms. Licensee may be held legally responsible for any copyright
**	infringement or damages resulting from Licensee's failure to abide by
**	the terms of this license. 
**	
\******************************************************************************/

#ifndef		__BIRCFRecorder_H
#define		__BIRCFRecorder_H

#include	<cfxbios.h>		// Persistor CF1 BIOS and I/O Definitions
#include	<cfxpico.h>		// Persistor CF1 PicoDOS Definitions  

#define		BIR_MAX_BIGIDEA	5	// Maximum number of BigIDEAs for this software

// DO NOT CHANGE THESE -- THEY MUST REMAIN CONSTANT AND TOTAL 256 
#define		BIR_SYS_HDR_SIZE	64	// Size in bytes of BIR portion of header
#define		BIR_USER_HDR_SIZE	192	// Size in bytes of USER portion of header


typedef	uchar 	BIRPage[256];

typedef struct
	{
//
// ----- First 32 bytes of BIR header is working variables ------
//
	char	   BIRHdrID[4];	// "BIR\0"
	ushort	BIRVersion;		// version.release * 10
	ushort	BIRUserHeaderSize;	// size in bytes for user header
	ushort	BIRUnused;		// currently unused (zero)
	ulong	   RTCsecs;			// current RTC seconds
	ushort	RTCticks;		// current RTC ticks
//
	ulong	BIRCapacityKB;		// total capacity of current drive in KB (1024)
	ulong	BIRStartFreeKB;	// free space on current drive at start in KB (1024)
	ulong	BIRReceivedKB;		// total number of KB (1024) received (likely to wrap)
	ulong	BIRWrittenKB;		// total number of KB (1024) written (likely to wrap)
//
// ----- Second 32 bytes of BIR header is copy of VEE settings ------
//
	long	CFPPBSZ;			   // size of CompactFlash buffer, typ. 40MB
	long	RAMPPBSZ;			// size of data RAM PP buffer, typ. 16 to 64KB
	long	RAMHDBFSZ;			// size of CF to HD copy buffer, typ. 16 to 64KB
	long	MINFREEKB;			// minimum free space until switch to next drive
//
	char	CFDOSDRV[4];		// DOS drive assigned to hard disk ("D:")
	short	NODRVTEST;			// testing without accessing drive
	short	UARTMONIT;			// sending diagnostics to RS-232 port
	short	FLOGFLAG;			// flag to log for major events (startup, spinups)
	short	BIADEVICE;			// device type attached to BigIDEA(s)
	short	CURBIA;				// index to current BigIDEA/drive
	short	UNUSED1;			   // currently unused
//
//	User header gets appended to the end of this
//
	}	BIRData;

enum
	{
	  birErrorStart = 4000
	, birSpinUpTimeout				// 1	timed out waiting for spin-up ready
	, birWriteTimeout				// 2	timed out completing the write
	, birCacheSetupFailed			// 3	couldn't set up PicoZOOM
	, birStartIdxOpenFailed			// 4	problem opening the index file
	, birStartIdxWriteFailed		// 5	problem writing the index file
	, birStartIdxCloseFailed		// 6	problem closing the index file
	, birDataLogOpenFailed			// 7	problem opening the data log file
	, birDataLogWriteFailed			// 8	problem writing the data log file
	, birDataLogCloseFailed			// 9	problem closing the data log file
	, birAllocRamPPBFailed			// 10	problem allocating memory for ram PPB
	, birOpenRamPPBFailed			// 11	problem opening the ram PPB
	, birReadRamPPBFailed			// 12	problem reading the ram PPB
	, birAllocCFBufFailed			// 13	problem allocating memory for CF buffer
	, birFindCFSSDFailed			// 14	problem finding any CF buffer SSD
	, birFindCFBigSSDFailed			// 15	problem finding big enough CF buffer SSD
	, birRegCFBigBSDFailed			// 16	problem registering CF buffer SSD to BSD
	, birOpenCFPPBFailed			// 17	problem opening the CF PPB
	, birWriteCFPPBFailed			// 18	problem writing the CF PPB
	, birWriteRAMPPBFailed			// 19	problem writing the RAM PPB
	, birNoFreeSpace				// 20	no free space left to record
	, birDriverOutOfDate			// 21	driver is not compatible
	, birUnknownIDEDevice			// 22	unknown IDE device
	, birInvalidHdrSize				// 23	sizeof(header) doesn't match constant
	};	// WHEN GROWING THIS, REMEMBER TO ALSO UPDATE THE BIRErrorString() FUNCTION



// Perform one-time BIR setup using BIR environment variables
//typedef void	*Hdrf(void);
typedef enum
	{ 
	turningOn, 			// just before turning on hard drive
	turningOff, 		// just before turning off hard drive
	startingWrite, 		// just before opening the hd write file
	endedWrite, 		// just after closing the hd write file
	gotSettings, 		// just after reading the BIR VEE settings
	reqUserHdr, 		// requesting user header info before file write
	reqTimeStr			// requesting time string for flogf and debug
	} BIRCollabReq;

typedef void	*Collabf(BIRCollabReq, const BIRData *, void *);
//short BIRSetup(Collabf *collabf);
short BIRSetup(Callocf *callocf, Freef *freef, Collabf *collabf);

// Returns the free space in bytes on the current disk drive 
long BIRCheckCapacityKB(void);

// Must be called periodically to keep the buffers flowing
//short BIRServiceBuffers(void);

// Accept data and move into chain of buffers
//short BIRRecordPage(BIRPage data);

// Perform all flush, close, and drive shutdown operations
//void BIRFinish(bool abort);

// Return C string corrosponding to errcode
char *BIRErrorString(short errcode);

// Update header information
//void BIRUpdateHeader(void);
static	void	BIRGetSettings(void);

//Set up the next volume and find out the next file name
short BIRSetupNextVolume(bool BIAchange, bool SlotChange);

//Find out the next file name and number
char	*BIRNextFileName(bool hunt, bool incIndex, ulong *filecount);

void  BIRShutdown(bool releaseCache);

void BIRCurrentSetup(short *BIACardNum, short *BIADevice, char *Drive);

#define	BIR_CFPPBSZ_NAME		"BIR.CFPPBSZ"
#define	BIR_CFPPBSZ_DEFAULT	"0" 
#define	BIR_CFPPBSZ_DESC		\
	"This variable is the size of the data files which is determined by ACQ.SRATEHZ\n" \
	"*2*ACQ.ACTIVESEC. The program calculates this parameter, and you do not have to\n" \
	"set the value.\n" \

#define	BIR_FLOGFLAG_NAME		"BIR.FLOGFLAG"
#define	BIR_FLOGFLAG_DEFAULT	"1"
#define	BIR_FLOGFLAG_DESC		\
	"This variable, when non-zero, tells the BIR (BigIDEA Recorder) portion\n" \
	"of the code to record major events, like startup and drive transfers, by\n" \
	"calling flogf. It is up to the main application to setup flogf to either\n" \
	"ignore these calls, or send the data to a CompactFlash file, the RS-232\n" \
	"port, or both.\n" \

#define	BIR_CFDOSDRV_NAME		"BIR.CFDOSDRV"
#define	BIR_CFDOSDRV_DEFAULT	"D:"
#define	BIR_CFDOSDRV_DESC		\
	"This variable is used internally by the BIR (BigIDEA Recorder) portions\n" \
	"of the code to determine the current working CF card drive. All\n" \
	"partitions on all hard disk drives become this DOS drive name during\n" \
	"acquisition. This will typically be drive D:.\n" \

#define	BIR_MINFREEKB_NAME		"BIR.MINFREEKB"
#define	BIR_MINFREEKB_DEFAULT	"0"
#define	BIR_MINFREEKB_DESC		\
	"This variable governs the minimum free space threshold in KB (1024 bytes) \n" \
	"for switching to the next hard drive volume. Unless you are prepared to do \n" \
	"extensive testing, this should be the same value as one-half of the current\n" \
	"CFPPBSZ / 1024, which is the value that will be filled in if this is zero or\n" \
	"left blank at startup. After each hard drive write, the remaining free\n" \
	"space is compared to this value and when it becomes less than\n" \
	"BIR.MINFREEKB, the current volume is retired and the next volume is\n" \
	"mounted.\n" \

#define	BIR_CURBIA_NAME			"BIR.CURBIA"
#define	BIR_CURBIA_DEFAULT		"0"
#define	BIR_CURBIA_DESC			\
	"This variable is used to select the next BigIDEA when the current drives\n" \
	"final partition fills. If is normally zero as the program starts writing\n" \
	"data from the first BigIDEA.\n" \

#define	BIR_BIADEVICE_NAME		"BIR.BIADEVICE"
#define	BIR_BIADEVICE_DEFAULT	"3"
#define	BIR_BIADEVICE_DESC		\
	"This numeric variable specifies the type of IDE devices the system will\n" \
	"be working with. If will normally 3 for the master slot of the CF cards mount.\n" \
	"It will go back and force between 3 and 4 (slave slot) during the logging.\n" \
	
#endif	//	__BIRecorder_H

