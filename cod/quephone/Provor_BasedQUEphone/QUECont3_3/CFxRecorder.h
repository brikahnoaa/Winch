/******************************************************************************\
** CFxRecorder.h
** 2006-02-13  by HM. Modified for a Compact Flash recorder.
** 2003-07-08 --jhg Changes for PicoDOS 3.x and FAT32
**		
**	#include	<NOAABIRecorder2.h>	// BigIDEA Recorder Definitions
\******************************************************************************/

#ifndef		__BIRecorder_H
#define		__BIRecorder_H

#include	<cfxbios.h>		// Persistor CF1 BIOS and I/O Definitions
#include	<cfxpico.h>		// Persistor CF1 PicoDOS Definitions  

// DO NOT CHANGE THESE -- THEY MUST REMAIN CONSTANT AND TOTAL 256 
#define		BIR_USER_HDR_SIZE	66	// Size in bytes of BIR portion of header
#define		QUE_USER_HDR_SIZE	190	// Size in bytes of USER portion of header

typedef	uchar 	BIRPage[256];

typedef struct
	{
//
	ulong	DIVESTART;			// dive start time
 	short	PREVDEPTH;			// Previous real diving depth (parked or grounded depth)  
	char	LATITUDE[10];		// Current lat
	char	LONGITUDE[12];
	char	NUMOFSAT;			// number of satellites
	char	NUMOFGPS;			// number of GPS fetch tries
	ulong	RTCGPSFIX;			// RTC time when GPS Fix was made
	char	NUMOFIRID;			// number of IRIDIUM TX tries
	char 	STATUS;				// Status during monitoring Park or Bottom 
	ulong	RTCLVSRFC;			// RTC sec of leaving surface
	ulong	RTCARVBTM;			// RTC sec of arriving bottom
	ulong	RTCLOGSTR;			// RTC time when logging started
	ushort	RTCSTRTIC;			// RTC log start ticks

	short	FLOGFLAG;			// flag to log for major events (startup, spinups)
	ulong	RTCLOGEND;			// RTC time when logging ended
	ushort 	RTCENDTIC;			// RTC log end ticks
	ulong	RTCASCSTR;			// RTC ascent starts 
	ulong	RTCASCEND;			// RTC ascent ended
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


//Perform one-time BIR setup using BIR environment variables
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

// Returns the free space in bytes on the current disk drive 
//long BIRCheckCapacityKB(void);

// Must be called periodically to keep the buffers flowing
//short BIRServiceBuffers(void);

// Accept data and move into chain of buffers
//short BIRRecordPage(BIRPage data);

// Perform all flush, close, and drive shutdown operations
//void BIRFinish(bool abort);

// Return C string corrosponding to errcode
//char *BIRErrorString(short errcode);

// Update header information
void BIRUpdateHeader(void);


#define	BIR_FLOGFLAG_NAME		"BIR.FLOGFLAG"
#define	BIR_FLOGFLAG_DEFAULT	"1"
#define	BIR_FLOGFLAG_DESC		\
	"This variable, when non-zero, tells the logger to record major events\n" \
	"by calling flogf. It is up to the main application to setup flogf to either\n" \
	"ignore these calls, or send the data to a CompactFlash file, the RS-232\n" \
	"port, or both.\n" \

#define	BIR_UARTMONIT_NAME		"BIR.UARTMONIT"
#define	BIR_UARTMONIT_DEFAULT	"0"
#define	BIR_UARTMONIT_DESC		\
	"Not used in this application\n" \

#define	BIR_NUMOFGPS_NAME		"BIR.NUMOFGPS"
#define	BIR_NUMOFGPS_DEFAULT		"0"
#define	BIR_NUMOFGPS_DESC			\
	"Number of trie to fetch GPS satellites\n" \

#define	BIR_NUMOFSAT_NAME		"BIR.NUMOFSAT"
#define	BIR_NUMOFSAT_DEFAULT	"0"
#define	BIR_NUMOFSAT_DESC		\
	"Number of satellites fetched during GPS location\n" \

#define	BIR_PREVDEPTH_NAME		"BIR.PREVDEPTH"
#define	BIR_PREVDEPTH_DEFAULT		"0"
#define	BIR_PREVDEPTH_DESC		\
	"Real depth that float reached in the previous dive \n"\

#define	BIR_LATITUDE_NAME		"BIR.LATITUDE"
#define	BIR_LATITUDE_DEFAULT	"N30:04.503"
#define	BIR_LATITUDE_DESC		\
	"Latitude in 10-char, starting 3 char long degree with N or S, followed by min\n" \
	"4 char long with 3 decimal points.\n" \

#define	BIR_LONGITUDE_NAME		"BIR.LONGITUDE"
#define	BIR_LONGITUDE_DEFAULT	"W130:13.862"
#define	BIR_LONGITUDE_DESC		\
	"Longitude in 11-char long, starting by W or W with 3 digit degree, followed by \n" \
	"4 char long minute with 3 decimal points\n"\

#define	BIR_NUMOFIRID_NAME		"BIR.NUMOFIRID"
#define	BIR_NUMOFIRID_DEFAULT		"0"
#define	BIR_NUMOFIRID_DESC		\
	"Number of the past attempts to call IRIDIUM to transmit the data\n"\

#define	BIR_STATUS_NAME			"BIR.STATUS"
#define	BIR_STATUS_DEFAULT		"0"
#define	BIR_STATUS_DESC		\
	"Provor status during the monitoring. Park(4,6,7) or Bottom (5) mode\n"\

#define	BIR_DIVESTART_NAME		"BIR.DIVESTART"
#define	BIR_DIVESTART_DEFAULT		"0"
#define	BIR_DIVESTART_DESC		\
	"Dive start time in RTC seconds since 1970.\n"\

#define	BIR_RTCLOGSTR_NAME		"BIR.RTCLOGSTR"
#define	BIR_RTCLOGSTR_DEFAULT		"0"
#define	BIR_RTCLOGSTR_DESC		\
	"Logging start time in RTC seconds since 1970.\n"\

#define	BIR_RTCSTRTIC_NAME		"BIR.RTCSTRTIC"
#define	BIR_RTCSTRTIC_DEFAULT		"0"
#define	BIR_RTCSTRTIC_DESC		\
	"Logging start time in ticks\n"\

#define	BIR_RTCLOGEND_NAME		"BIR.RTCLOGEND"
#define	BIR_RTCLOGEND_DEFAULT		"0L"
#define	BIR_RTCLOGEND_DESC		\
	"Logging end time in RTC seconds since 1970.\n"\

#define	BIR_RTCENDTIC_NAME		"BIR.RTCENDTIC"
#define	BIR_RTCENDTIC_DEFAULT		"0"
#define	BIR_RTCENDTIC_DESC		\
	"Logging end time in ticks.\n"\

#define	BIR_RTCASCSTR_NAME		"BIR.RTCASCSTR"
#define	BIR_RTCASCSTR_DEFAULT		"0"
#define	BIR_RTCASCSTR_DESC		\
	"Start ascending from the bottom in RTC seconds since 1970.\n"\

#define	BIR_RTCASCEND_NAME		"BIR.RTCASCEND"
#define	BIR_RTCASCEND_DEFAULT		"0"
#define	BIR_RTCASCEND_DESC		\
	"Ascent end time in RTC seconds since 1970.\n"\

#define	BIR_RTCGPSFIX_NAME		"BIR.RTCGPSFIX"
#define	BIR_RTCGPSFIX_DEFAULT		"0"
#define	BIR_RTCGPSFIX_DESC		\
	"GPS Fix time in RTC seconds since 1970.\n"\

#define	BIR_RTCARVBTM_NAME		"BIR.RTCARVBTM"
#define	BIR_RTCARVBTM_DEFAULT		"0"
#define	BIR_RTCARVBTM_DESC		\
	"Arrived bottom in RTC seconds since 1970.\n"\

#define	BIR_RTCLVSRFC_NAME		"BIR.RTCLVSRFC"
#define	BIR_RTCLVSRFC_DEFAULT		"0"
#define	BIR_RTCLVSRFC_DESC		\
	"Leave surface in RTC seconds since 1970.\n"\
	
	
#endif	//	__BIRecorder_H

