/******************************************************************************\
** CFxLogSP3i3_6.c (6/13/2016) H. Matsumoto
** Changed BIRCFReorder3.c to BIRCFRecorder4.c. It sends TickleSWSR() every 13 sec
** to prevent from rebooting during the long free space search of CF card. No change 
** in CFxLogSP3i3_6.c. Without this fix, CF2 reboots during a long idling period when
** a new drive is mounted if the CF card is larger than 128 GB.  
**
** CFxLogSP3i3_5.c (7/08/2015) H. Matsumoto
**
**		1. You can use large CF cards (>=512GB) with multiple BigIDEAs. Each CF card is 
**		named differently depending on which CURBIA it is mounted
**
**    CURBIA	0  1	2	3	4  
**		Master	D	F	H	K	L
**		Slave		E	G	I	J	M
**
**		You can use SYS.F32TRUST=* PicoDOS option, which makes the free space search
**		quicker.  But you have to mount each drive with drive letter and do 'DIR' before
**		launching the program. For example
**			mount F: bfcm-1 and dir
**			mount G: bfcs-1 and dir
**			mount H: bfcm-2 and dir etc
**  
**    2. Added TickleSWSR in BIRSetupNextVolume() so that between BigIDEs and 
**			master and slave "Watchdog" (419s) would not reset CF2 during the free 
**			space search and A/D is in idling period. (7/10/2015)
**		3. For a single BigIDEA and CF card > 257 GB, set SYS.F32TRUST=*
**		4. For multiple drives, set SYS.F32TRUST=  No CF card > 257GB.
**			If =*, 3_4 version logs only the first BigIDEA and skipped 2nd and 3rd.
**    5. BIRCurrentSetup is added to obtain the correct CFDOSRV().
** 	6. When the logging restarts from E: drive, it did not mount D: and draws 20 mA
**       more current.  mount D: command is enerted.  July 23/2015
** 
** CFxLogSP3i3_4.c (4/12/2014) H. Matsumoto
**
**		1.Implemented a watch dog with 419 sec timer.
**      short	CustomSYPCR = WDT419s | HaltMonEnable | BusMonEnable | BMT32;
**      You have to change the prefix in C/C++ Language in project setting from mxcfxstd.h 
**      mxcfxwdt.h 
**		2.Implemented low-power SleepUntilWoken after disks are full.  
**  	  It waits for your keyboard stroke to end the program.
**    3. Added a safety net to check if BIR.CURBIA and BIR.CFDOSDRV are consistent.
** 
** CFxLogSP3i3_3.c (1/24/2014) H. Matsumoto
**    Fix the execessive current problem when writing to the slave CF card.  It 
**    mounts both master and slave drives at same time and they are accessed as 
**    d: and e: drive respectively.  It draws 1 mA more than previous version 
**    (CFxLogSP3i_3_2.c).  Optimum SPDQ_ACQ_CLOCK varies depending on ACQ.SRATEHZ
**    and ACQ.NCHAN.  But if SPDQ_ACQ_CLOCK increases, so does the current. Refer
**    to the Power Consumption section in this program description below.
**
** CFxLogSP3i3_2.c (4/19/2013)
** 	For H3DAQ2 (DAQ2D), SMDAQ or CSACDAQ (>=Rev 3)
**		For CSACDAQ, it is only for CF2 with a 3-pin clock jumper soldered. Refer
**    to CSACDAQ manual for this.  This 3-pin jumper allows to select an internal
**    40 kHz Xtal clock or external CSAC 40 kHz clock.  To use the CSAC clock, set 
**    the jumper near to the white LED.  Also CSACDAQ's P5 jumper has to be on.  
**    If CSAC clock is not used and to use the CF2's internal clock, set the jumper
**    pin near to the 50-pin connectyor and take P5 jumper off.
**    
** 	For BigIDEA multiple CF cards logging, set J6 jumper to D: (both SMDAQ and 
**		CSACDAQ).
**
**    It also logs multiple channel data.  You have to play the clock speed to find 
**    what optimum SPDQ_ACQ_CLOCK is for power cosumption and speed. Multi-channel 
**    requires a fast clock speed and more current than 1-chan data logging.
**		This version requires 22 mA more current to long on the slave drive.  
**
** CF2LogSP3i3_1.c (10/18/2011) H. Matsumoto
**		For H3-DAQ2 with CF2/CF1 and multiple BigIDEA CF Cards data logging system 
** 	Logs the data to multiple BIA CFs as D:.  Uses 1mA more power than C: logging.
**    The program requires min 512kB of RAM. If CF1, it runs on CF1I5 and CF11M only.
**    Not on earlier ones, e.g., CF11C.  
**
** Cautions
** 1. Connect pin 41 (IRQ2 -used to be 1pps interrupt) and pin 15 (PCS2) and change 
** 	spdqr.irq_pin to 41 in Acquire(). 
** 2. Uses BigIDEAs and CF cards to store data abd uses IRQ7 (pin 40)for data traffic. 
** 3. PCS2 (pin 15 A/D clock) is driven by IRQ2 (Pin 41) interrupt.
** 4. BIR.CFDOSDRV=D:
**
** BigIdea 44-pin IDE cable is connected to CF cards adaptor for master and slave.
** Addonics (model AD44MIDE2CF) is tested and works.  Still draws 22 mA when 
** mounted.
**
** All user-accessible IRQs are used up including IRQ2 (AD), IRQ5 (program finish) 
** and IRQ7 (BigIDEA).  Q-Tech 1-PPS/100PPS pulse count scheme is no longer possible.  
** If accurate timing is needed, use CF2 instead of CF1 and drive the system clock 
** by an external 40kHz by the Q-Tech or other accurate clock source.
**
** Set BIR.CURBIA=0 (to start logging from the 0th disk)
** Set BIR.BIADEVICE=3 (to start from the master=3 then to slave=4)
** Set BIR.CFDOSDRV=D: (to set BIA-mounted CF card as D: drive)
** Set BIR.FLOGFLAG=1 (to report the BIA device activity)
**
** Use the following commands to mount a specific CF Card 
** mount bfcm-i (i-th CF disk master), where i is from 0 to 10
** mount bfcs-i (i-th CF disk slave)
**
** The maximum BigIDEA number is defined by BIR_MAX_BIGIDEA in BIRCFRecorder.h (default 2)
** 
** Revised from CF2LogSP3i2_1.c (8/07/2011)
**
** For continuous data logging set:
** ACQ.DUTCYCYLE=ACQ.ACTIVESEC, ACQ.SLEEP=0 and ACQ.WARMUP=0.
**
** Improvements from CFxLogger2_1.
** Data logging program based on CFxLogger2_1JHG02.c, which uses POSIT file system. 
** File closing is much faster than CFxLogger2_1 and power consumption is 1/3 to 1/5. 
** The program is suited for continuous logging without pre-amp off/on.  Still uses an 
** external interrupt (IRQ5). The program provides pre-amp power switch ON/OFF.
**
** ACQ.MAXNUMFIL inconsitency fixed. A bug in max number of files (ACQ.MAXNUMFIL)
** made the program records a half the size of the CF card and stopped prematually.
**
** The program uses a short and effient SPI instruction set.  IT acheives a low
** power by lowering the clock speed to minimum required to maintain the A/D.
** It also uses POSIX-like file instruction which is more reliable and faster.
**  
** Hardware changes
** 1. pin 15 (PCS) and 41 (IRQ2)to be connected (for H3DAQ only) 
** 2. Pin 35 can be connected to pin 2 of pre-amp's J2 (2.5V ext at H3DAQ) to turn 
** on/off the pre-amp or external device.
**
** Cautions:
**    *Not to turn on the pre-amp before DAQ board.  SCR latch may occur at the A/D
**    channel 0 input by voltate >2.8V or < -0.3V.
**    *Turn on pre-amp well in advance of A/D logging. Warm-up period 10 sec
**    is recommended to allow pre-amp output to settle down.      
**
** Max clock speed =19200 (according to J.H.Godly)  - option has to be added in spdq.c
** This option allows sample at 96kHz but it is noisey.  64kHz is the recommended Max.
**
************** CSCADAQ power consumption estimate (@10V) ******************************
**						mA
** CSAC (low)		3		90%
** CSAC (high)		14		10%
** 
** To change the clock speed (SPDQ_ACQ_CLOK), the program has to be recompiled.
** SPDQ_ACQ_CLOK = 640 (640kHz)to 16000 (16 MHz)
**					VBat=15V	VBat=15V
**								DC-DC5V
**   SRATEHZ  Linear_mA DC-DC_mA   SPDQ_ACQ_CLOCK
**   96000       55     	26			19200
**   80000       40     	21			16000
**   64000       32     	19			16000
**   50000       --        19			16000
**   32000       32     	16			16000
**   32000       23    		12			8000
**   20000       20    	 	10			8000
**   10000       11	    	 7			3200
**    5000        8.1     	 5			1600 or 3200
**    2000        6.6     	 4			1600 
**    1000        5.3     	 3.8		640
**    500         4.8     	 3.5		640
**    250         4.5     	 3.1		640
**   During sleep & hibernation  
**    SYCLK       mA
**    ----         1
**
**	Log the data to D: compact flush RAM (CF card) in ACQ phone data format. 
**	No Ping-pong buffer is used. As a result there is a data gap (~500msec).
** Duty cycle is implemented.  To stop logging, push PBM (IRQ5) interrupt button.
**  
** ACQ.DUTYCYCLE = The interval (sec) that it repeats the data logging cycle.   
** ACQ.ACTIVESEC = Duration that logging is active in seconds.  
** File size     = ACQ.ACTIVESEC * ACQ.SRATEHZ *2.  No longer BIR.CFPPBSZ is used.
** ACQ.MAXNUMFIL   Not used to monitor the disk usage anymore. Infor only. 
**
**       
** Matsumoto, NOAA/CIMRS
\******************************************************************************/

/******************************************************************************\
**	Changes JHG01_Jun2011: Implemented faster & low-power QSPI A/D logging routine
**	1.	Added the AcceptChangesJHG01_Jun2011 immediately below to mark and
**		bracket all JHG01 changes. Search on AcceptChangesJHG01_Jun2011....
**	2.	Added VEE overides in main to force some settings and to warn if
**		others weren't as expected.
**	3.	Modified if((long)ACQ.MAXNUMFIL<1L) in SetupAcquisition() to stop
**		aborting because my smaller log files overflowed the short variable.
**	4.	Reversed calling order of SleepUntilBaptisedAD and SetupAcquisition
**		in main because the former relies on setup performed by the later.
**	5.	Moved up CFxADInit in SleepUntilBaptisedAD to deal with crashes
**		when attempting zero delay. More comments in code body.
**	6.	Added pin-27 monitoring of acquisition cycles to accumulate current
**		drain waveforms and statistics for analysis.
\******************************************************************************/

#define 	DAQ 3 					//H3DAQ2 = 1,
//#define DAQ 2					//SMDAQ  = 2 
//#define DAQ 3					//CSACDAQ= 3 
//#define DEBUG			      // comment this out to turn off diagnostics
//#define TESTACQ             //Test logging prior to the real logging when ACQ.STARTUPS=0
                              //and ACQ.TESTSEC >0.  Log all 8-channel 
//#define TEST                //Use the TES SPDQ_ACQ_CLOCK speed

#ifdef TEST
   #define  SPDQ_ACQ_CLOCK	19200		//  (16000 for up to 80kHz, 19200 for 96kHz - Crazy speed)  
	#pragma message	("!!! "__FILE__ ": Don't ship with TEST compile flag set!")
#else 
	#define SPDQ_ACQ_CLOCK	640		// for up to 1kHz 
   //#define SPDQ_ACQ_CLOCK	800		// for up to ?kHz
   //#define SPDQ_ACQ_CLOCK	1600		// for up to 5kHz
   //#define SPDQ_ACQ_CLOCK	3200		// for up to 10kHz
   //#define SPDQ_ACQ_CLOCK	4000		// for up to 20kHz
   //#define SPDQ_ACQ_CLOCK	8000		// for up to 40kHz 
   //#define SPDQ_ACQ_CLOCK	16000		// for up to 64kHz -Highest recomended by Persistor
   //#define  SPDQ_ACQ_CLOCK	19200		// for up to 96kHz -Higest tested by NOAA. Noisey but works. 

#endif

#ifdef DEBUG
  	#define	DBG(X)	X	   // template:	DBG( cprintf("\n"); )
	#pragma message	("!!! "__FILE__ ": Don't ship with DEBUG compile flag set!")
#else
  #define	DBG(X)		   // nothing
#endif  

/******************************************************************************\
**	Changes JHG02_Jun2011:
**
**	OVERVIEW
**		There are 23 numbered blocks of code bracketing the JHG02 changes and 
**		most exist to support the complete replacement of the LPFiledAD based
**		Aquire function with a much more power efficient SPDQ based function 
**		with similar purpose. 
**
**		All of the original code remains intact, and commenting out both
**		AcceptChangesJHG01_Jun2011 and AcceptChangesJHG02_Jun2011 will
**		produce the same binary as the originally received code base.
**
**		Each of the bracketed change segments has an upper-case comment each
**		change's primary purpose. These are listed below:
**
**	JHG ADDITIONS TO SUPPORT SPDQ ACQUISITION
**		This is key ingredient for achieving the over-arching goal of
**		reducing power and these are the changes that do that.
**
**	JHG DELETIONS TO DISENGAGE LPFILEDAD LEGACY CODE
**		These are code bits being replaced in several spots to keep them
**		from interfering with the SPDQ code.
**
**	JHG CHANGES TO REDUCE SLEEP MODE POWER
**		The LPFiledAD deletions left some functional holes and I filled
**		those where possibly with lower power alternatives.
**
**	JHG CHANGES TO OPTIMIZE FILE WRITE PERFORMANCE
**		The file operations are the most expensive power consumers and these
**		optimizations keep those costs to a minimum.
**
**	JHG TEMPORARY CHANGES TO SUPPORT CURRENT MONITORING
**	JHG TEMPORARY CHANGES TO SPEED UP DEVELOPMENT
**		Both of these are necessary to properly measure and evaluate the 
**		efficacy of the code changes. These probably want to be commented
**		out of the production code.
**
**	JHG CHANGES TO CORRECT POSSIBLY FAULTY CODE
**		These should be reviewed by the chief code maintainer because I
**		may have misunderstood the original intent
**
**	CHANGE LIST
**	1.	Include headers and globals needed for SPDQ.
**	2.	Eliminate RawData code no longer needed.
**	3.	Added code to have pin-27 track acq/sleep modes.
**	4.	Added ACQ.xxxx overrides and deviation warnings for consistent
**		test runs
**	5.	Rearranged main() calling sequence to prevent possible crash
**		condition
**	6.	Eliminate original ADTimingRuptHandler() and
**		ADSamplingRuptHandler() functions
**	7.	Recoded Open_File() to use unbuffered POSIX instead of standard C
**		buffered file functions for much greater speed and efficiency
**		(about a 2mA drop in early tests)
**	8.	Eliminate RawData code no longer needed in SetupAcquisition().
**	9.	Kludged a workaround  for a looming overflow condition in
**		SetupAcquisition() with the huge card and the number of files
**		exceeding the type short ACQ.MAXNUMFIL that could cause the
**		program to abort.
**	10.	Eliminate RawData code no longer needed in SetupAcquisition().
**	11.	THE BIG SPDQ code change in Acquire().
**	12.	The discarded LPFiledAD code from Acquire().
**	13.	Rearranged code to deal with reversed dependencies the AD driver
**		calls in SleepUntilBaptisedAD().
**	14.	Eliminate the original errant code.
**	15.	Eliminate the original errant code.
**	16.	Replace LPMODE with FullStop for lower power in
**		SleepUntilBaptisedAD() LPStopCSE() call.
**	17.	Eliminate the original LPMODE  code.
**	18.	Eliminate more legacy code in SleepUntilBaptisedAD().
**	19. to 23. All changes for lower power in LowPowerHibern()
**
**	PIN USAGE
**		Pin	CF1		CF2		RPU		NOAA				JHG02 adds:
**		 1	   DS		   DS			DIAG				
**		15	   PCS2	   PCS2				SPDQ PCS
**		16	   SCK	   SCK									
**		17	   PCS3	   PCS3				AD CS			
**		18	   MOSI	   MOSI				AD DI				
**		19	   PCS1	   PCS1				free				
**		20	   MISO	   MISO	   1M		AD DOUT					
**		21	   PCS0	   PCS0				free				
**		22	   CTD10	   TPU1			   CSAC-PC  COM ON	
**		23	   CTD9	   TPU2			   CSAC-CF2 COM ON		
**		24	   CTD7	   TPU3			   CSAC 1-PPS IN		
**		25	   CTD8	   TPU4			   CSAC 1-PPS ENABLE			
**		26	   CTD6	   TPU5			   CSAC !BITE OUT		
**		27	   CTD5	   TPU6			   <free>
**		28	   CTS14B	TPU7			   AD_REF_SHDN_PIN				
**		29	   CTD4	   TPU8			   EVT OUT (for time sync after deployment0					
**		30	   CTS14A	TPU9			   <free>				
**		31	   CTS18A	TPU10			   GAIN0 (SMDAQ and CSACDAQ)				
**		32	   CTS18B	TPU11			   GAIN1	(H3DAQ, SMDAQ, CSACDAQ)			
**		33	   CTD29	   TPU12			   GAIN0	(H3DAQ)  CSAC COM RX			
**		34	   CTD28	   TPU13			   TXOUT	to CSAC			
**		35	   CTD27	   TPU14			   PRAMPON				
**		36	   CTM31L	T2CLK	   1M		<open>					
**		37	   CTD26	   TPU15				free				
**		39	   IRQ5	   IRQ5	   10K	Interrupt				
**		40	   IRQ7	   IRQ7	   10K	SPDQ BigIDEA D: & E:
**		41	   IRQ2	   IRQ2	   10K	SPDQ C: Drive						
**		42	   MODCLK	MODCLK	10K	<open> 						
**		45	   IRQ4RXD	IRQ4RXD			<open>					
**		46	   TXD		TXD				<open>					
**		48	   TXX		TXX		1M		CSAC COM TX					
**		50	   IRQ3RXX	IRQ3RXX			CSAC COM RX					
\******************************************************************************/

//#define	Acquire(x)		Acquire_JHG02(x)
#define	WRITE_SIZE		65536UL		// power of 2 and max cluster size

#define	ACQ_VERSION_NUMBERS	4.8	// keep this up to date!!! - always V.R

#include	<cfxpico.h>		   // Persistor PicoDOS Definitions
#include	<cfxbios.h>		   // Persistor BIOS and I/O Definitions
#include	<BIRCFRecorder.h>	// NOAA's own BigIDEA CF Recorder Definitions
#include	<BIRCFSettings.h> // NOAA's own common for CFRAMDataLogger. Changed by hm 8/27/99
#include	<cfxad.h>		   // Generic SPI A-D QPB Driver for CFx
#include	<assert.h>
#include	<limits.h>
#include	<locale.h>
#include	<math.h>
#include	<setjmp.h>
#include	<signal.h>
#include	<stdarg.h>
#include	<stddef.h>

// Place non-PicoDAQ A-D definitions here - before including <ADExamples.h>
#include	<ADExamples.h>	   // Common definitions for the A-D Examples
#include	<stdio.h>
#include	<stdlib.h>
#include	<ctype.h>
#include	<string.h>
#include	<dosdrive.h>
#include	<time.h>
#include <ADS8345.h>
#include <dirent.h>

#include	<errno.h>
#include	<unistd.h>		// PicoDOS POSIX-like UNIX Function Definitions
#include	<fcntl.h>		// PicoDOS POSIX-like File Access Definitions
#include	<spdq.h>		   // SPI Intrinsic Interval Data Acquisition

long		AcqFileHandle;	// for POSIX acquisition file writes

#define	FCHAN		0					// first channel
//#define	NCHAN		1					// number of channels
#define	SYSCLK	14000				// choose: 160 to 32000 (kHz)
#define	WTMODE	nsStdSmallBusAdj	// choose: nsMotoSpecAdj or nsStdSmallBusAdj
#define	LPMODE	FastStop			// choose: FullStop or FastStop or CPUStop

//CFxAD	   adbuf, *ad;

extern   bool AllDrivesFull;
bool	   ProgramFinished = false;
#define	PITRATE	20	         //2/21/11 HM Equivalent to ~1Hz 51ms periodic interrupt rate
#define	NUMCHAN	8		      // You need to define this.  8ch/2byte data fills up 16-byte

long     NSamplePerFile;
long     NTestSamplePerFile;
long     TestDuration;

#if DAQ <2	//H3DAQ
	#define	GAIN0			33    // 1  = 0 dB  |0 = 6dB | 1 = 12dB | 0 = 18dB,
	#define	GAIN1			32	   // 1          |1       | 0        | 0
#endif

#define  CSACCAL		26		// input to read CSAC calib status
#define  PPSENAB		25    // output to enable 1-pps to come in tpu2 (pin 24)
#define  PPSIN			24		// input. 1-pps comes in if pin 25 is on //not used yet 5/7/2013
#define	CF2CSAC		23		// 1  = ON Enalbes CF2-CSAC com
#define  PCCSAC		22    // 1  = ON Enables PC-CSAC com

#define  PRAMPON     35    // 1  = ON, 0 = OFF Pre-Amp power switch

#if DAQ >= 2 	//SMDAQ & CSACDAQ
	#define	GAIN1				 32	 // 0          |0       | 1        | 1
	#define	GAIN0				 31    // 0  = 0 dB  |1 = 6dB | 0 = 12dB | 1 = 18dB,
	#define 	AD_REF_SHDN_PIN 28
#endif

#if CFX==1
	#if DAQ <2  //H3DAQ
	PinIODefineCTM(GAIN1,   0xF495, 0x11, 0x1);	//32
	PinIODefineCTM(GAIN0,   0xF4E9, 0x14, 0x04); //33
	PinIODefineCTM(PRAMPON, 0xFD9,  0x14, 0x04); //35
	#else			//SMDAQ and CSACDAQ
	PinIODefineCTM(GAIN1,   0xF495, 0x11, 0x1);	//32
	PinIODefineCTM(GAIN0,   0xF491, 0x11, 0x4); 	//31
	PinIODefineCTM(PRAMPON, 0xFD9,  0x14, 0x04); //35
	PinIODefineCTM(AD_REF_SHDN_PIN, 0x475,  0x11, 0x1); //28
	#endif
 
	/*JHG990922vvvBEGIN ADDvvv*/	//REPLACEMENT DEBUG I/O SIGNALS
	#ifdef DEBUG
	//PinIODefineCTM(WATCH_ACQUIRE,	0xF451, 0x14, 0x04);	// 22, CTD10
	//PinIODefineCTM(WATCH_ADTIMING,0xF449, 0x14, 0x04);	// 23, CTD9
	//PinIODefineCTM(WATCH_ADRUPT,0xF439, 0x14, 0x04);	// 24, CTD7
	//PinIODefineCTM(WATCH_IRQ, 	0xF441, 0x14, 0x04);	// 25, CTD8
	#endif
#endif	

#if CFX==2
	#if DAQ < 2  //H3DAQ=1
	PinIODefineTPU(GAIN1,		11);        //32
	PinIODefineTPU(GAIN0,		12);        //33
	PinIODefineTPU(PRAMPON,    14);        //35
   #else			//SMDAQ and CSACDAQ
	PinIODefineTPU(GAIN0,		10);        //31
	PinIODefineTPU(GAIN1,		11);        //32
	PinIODefineTPU(PRAMPON,    14);        //35
	PinIODefineTPU(AD_REF_SHDN_PIN, 7);   	//28
	PinIODefineTPU(CF2CSAC,     2);        //23
	PinIODefineTPU(PCCSAC,      1);        //22
	#endif
	   
   #ifdef DEBUG
   	//#define WATCH_ACQUIRE		   22	   // TPU1
      //#define WATCH_ADTIMING		23	   // TPU2
   	//#define WATCH_ADRUPT		   24	   // TPU3
   	//#define WATCH_IRQ			   25	   // TPU4
   	//PinIODefineTPU(WATCH_ACQUIRE,1);    // 22, TPU1
   	//PinIODefineTPU(WATCH_ADTIMING,2);   // 23, TPU2
   #endif
#endif

// DO NOT CHANGE THESE -- THEY MUST REMAIN CONSTANT AND TOTAL 256 
#define  ID_LENMAX			 4		//System ID length
#define	LAT_LENMAX			10		//Latitude length
#define	LONG_LENMAX			12		//Longitude max 
#define	GMT_LENMAX			46		//GMT char length
#define  LOGF_NAME_MAX		14		//JHG-2003-07-28 max filename length for log file
#define  EXPID_LENMAX		16		//EXP ID, usually year
#define	PROGNAME_LENMAX 	12		//Added by hm NOAA 09/30/2002

typedef struct //user header space 192 bytes
	{
	char	   PLTFRMID[ID_LENMAX];		//System ID (e.g., G001)
	char	   LATITUDE[LAT_LENMAX];	//Latitude in degrees   N45:02.356=N45deg 02.356min
	char	   LONGITUDE[LONG_LENMAX];	//Longitude in degrees W128:34.872=W128 deg 34.872min
	char	   TIME_GMT[GMT_LENMAX];	//System ID, Lat, Lang and GMT time
	char	   EXPID[EXPID_LENMAX];		//Exp ID   10
	char	   PROGNAME[PROGNAME_LENMAX];	//Program name  12
	ushort	ACQVersion;			      // version.release * 10
	ushort   WARMUP;
	char	   PROJID[ID_LENMAX];      //Project ID 4
	char	   LOGFILE[LOGF_NAME_MAX];	//Filename for event logging 14 chars 
	short	   STARTUPS;					//number of program resets we've seen
	short	   MAXSTRTS;					//Maximum allowable program resets up to 255
	long	   MAXNUMFIL;				   //Maximum number of files
	short	   GAIN;						   //additional pre-amp gain 0 to 3 with 6dB inc
	long	   SRATEHZ;					   //sample rate in hertz  NEW SRATEHZ is long == NEW
	short	   SAMPLES;					   //sample size and type 0=8 bit, 2=12bit(2 byte), 3=16 bit 
	short    PWFILT;                 //Pre-whitening filter setting
	short    LOPASS;                 //Low pass filter cut off 
	ushort   SLEEP;                  //sleep in hours before program launched
	ulong    ACTIVESEC;              //Active logging period in second
	ulong    DUTYCYCLE;              //Duty cycle in seconds
	short    HYDROSENS;              //Hydrophone sensitivity
	char     PRAMPNAME[10];          //PreAmp revision up to 10-char long
	ulong	   WAKEUP;				      // time to start logging in sec since 01/01/1970 11/02/99 NOAA hm	
	char     DAQNAME[10];            //DAQ board name
	char     HYDROSRN[6];				//Hydrophone serial number
	uchar 	NCHAN;
	uchar 	anything;
	short    TESTSEC;
	char     dummy[4];
	}	ACQData;

ACQData	ACQ;						   // runtime settings filled in at startup 
//static BIRData BIRS;				   // runtime settings filled in at startup 
static BIRData BIRS;				   // runtime settings filled in at startup July 27, 2015 

//static   BIRData	BIRS;				// runtime settings filled in at startup 
FILE	   *fp;
char	   time_chr[22];
ushort	nfile;                   //file counter

// This definition is the key to using the WDT  4/3/2014 HM
short	CustomSYPCR = WDT419s | HaltMonEnable | BusMonEnable | BMT32;
//short	CustomSYPCR = WDT26s | HaltMonEnable | BusMonEnable | BMT32;

#define CUSTOM_SYPCR 

static ulong 	OnePPSCounter = 0;
ulong	         WakeUpTime;		// computed end of delay in calendar time
extern bool    DriveIsOn;

#define	TICKS_TO_MS(x)	((ushort) (x / (CFX_CLOCKS_PER_SEC / 1000)))

IEV_C_PROTO(ExtFinishPulseRuptHandler);

void ACQGetSettings(void);
FILE *Open_File(ulong filecount);
static void BIRGetSettings(void);
void CFxBIRUpdateHeader();
//FILE *Write_Header(FILE *fp);
void PreRun(void);
void *ACQUpdateHeader(void);
ulong WhatTime(char *time_chr);
short SetupAcquisition(void);
short Acquire(bool volume_checked);
void SleepUntilWoken(void);
static void Irq4RxISR(void);        //NOAA	for interrupt by keystroke or IRQ4
void DisplayParameters(void);			//Display parameters stored in VEE (NOAA)
void SetupHardware(void);
void SleepUntilBaptisedAD(void);    //No Q-Tech version of pre-launch sleep
char *GMTString(time_t tp);		   //Convert sec since 1970 to GMT ASC time format
short LowPowerHibern(void);         //Low power hybonation mode 
void UpdateGain(void);              //Set the pre-amp gain
void TestGain(short gain);
short DiskCheck();
void LowPowerDelay(ulong delay);
void Turn_Off_Preamp(void);
void *ACQCollabBIR(BIRCollabReq req, const BIRData *bp, void *more);
void CheckBIRStartups(const BIRData *bp);
FILE *Open_Test_File(void);
short Test_Acquire(void);
short Test_Acquisition();
extern void  BIRShutdown(bool releaseCache);
extern short BIRSetupNextVolume(bool BIAChange, bool SlotChange);
short LowPowerHibern_old_05252011(void);
short waketime_logf(ulong SecondsSince1970);

/******************************************************************************\
**	main
\******************************************************************************/
main()
	{
	short HeaderSize;
   bool volume_checked=false;  //volume is checked and the 1-st file name identified-no need to increment
   short err;
   bool  BIAChange, SlotChange;
  	char	*failmsg = "None";
	char	strbuf[64];

   
   SetupHardware();	
	#ifdef HIGH_DURING_ACK_PIN		// 3. JHG TEMPORARY CHANGES TO SUPPORT CURRENT MONITORING
		#pragma message	("!!! "__FILE__ ": Don't ship with JHG TEMPORARY CHANGES !")
		PIOClear(HIGH_DURING_ACK_PIN);		// not recording
	#endif
	
	//Header size check in case
	HeaderSize=sizeof(BIRS)+sizeof(ACQ);
	if(HeaderSize != 256){
		flogf("Header size is %d, not 256.  Exiting the program\n\n",HeaderSize);
		ProgramFinished=true;
		goto exitProg;
		}
	//flogf("Header size is %d.\n\n",HeaderSize);

	//ACQ.STARTUPS = 1;	// no startup queries or delays
	//cprintf("\nACQ.STARTUPS=%d\n",ACQ.STARTUPS);

	PreRun();
	
  
	if(!SetupAcquisition())			// SleepUntilBaptisedAD relies on setup performed by SetupAcquisition, 
		{
		//The followings were added to test A/D routine
		#ifdef TESTACQ
   		if(ACQ.STARTUPS==0 && ACQ.TESTSEC>0){    // Test the A/D logging for TESTSEC
            if(Test_Acquisition()==-1)goto exitProg;
   		   }
	    #endif	   
								   // reverse order means sampling will be writing wildly into memory!
		SleepUntilBaptisedAD();
	   
	   PIOSet(PRAMPON);     //Pre-amp power ON
	   LowPowerDelay(1);    //minimum delay HM 12/15/2015
	   LowPowerDelay(ACQ.WARMUP);   //Give some more time to warm up
      UpdateGain();        //Set up the gain of pre-amp

      //Set the ACQ.WAKEUP time as the starting time and a reference
      if(ACQ.STARTUPS==0){
         OnePPSCounter = RTCGetTime(NULL,NULL); 
         WakeUpTime=OnePPSCounter; 
         sprintf(strbuf, "%lu", WakeUpTime);
         ACQ.WAKEUP=WakeUpTime;
   		VEEStoreStr(ACQ_WAKEUP_NAME, strbuf);	//Keep initial wake-up time in VEE only once	
         waketime_logf(WakeUpTime);             //record time to wake up
      }

 		if(ACQ.DUTYCYCLE!=ACQ.ACTIVESEC) LowPowerHibern();
 		
		while(!ProgramFinished)//repeat multi files
			{
			if(DiskCheck()==-1){// this takes a couple of sec
			   //Bring CFx up to the normal clock speed
	         CSSetSysAccessSpeeds(nsFlashStd, nsRAMStd, nsCFStd, WTMODE);
	         TMGSetSpeed(SYSCLK);
	         //flogf("Shut down the current BigIDEA CF card drive and turn on the next. \n");
	         //BIRShutdown(true);
	         if(BIRS.BIADEVICE==3){
	            BIAChange=false; //Slot change from master to slave. Stay on the same BigIDEA card
	            SlotChange=true;
	            //BIRS.CFDOSDRV[0]='E';
	            }
	         else{ //BIRS.BIADEVICE=4, next BIRS.CURBIA
	         flogf("Shut down the current BigIDEA CF card drive and turn on the next. \n");
	         BIRShutdown(true);
	            BIAChange=true;                //Move up to the next BigIDEA card. Change from slave to master slot. 
	            SlotChange=true;
	            //BIRS.CFDOSDRV[0]='D';
	            }
	         //flogf("Drive is on/off %d\n", DriveIsOn);
				TickleSWSR();  //added July 23, 2015
	         err = BIRSetupNextVolume(BIAChange, SlotChange);   
       
		      if(err)volume_checked=false;
		      //returncode = -1;
		      //failmsg = "Disk is full";
		      if(err==birNoFreeSpace || AllDrivesFull == true ){ //err =20 disk is full
		         ProgramFinished=true;
		         failmsg = "All disks are full.";
		         goto exitProg;  //jump to the end 
		         }
		      }	

	      #ifdef HIGH_DURING_ACK_PIN
		      PIOSet(HIGH_DURING_ACK_PIN);		// start recording
		      RTCDelayMicroSeconds(1e6);			// give it a sec
	      #endif
	      
	      if(CFX==2){
	      	LEDSetState(LEDleft, LEDgreen);//turn on begining of the file
	      	RTCDelayMicroSeconds(5e5);
	      	LEDSetState(LEDleft, LEDoff);
	      	}else //CF1 with H3DAQ
	      	{
	      	PIOSet(PPSIN);
	      	RTCDelayMicroSeconds(1e6);//turn on Yel LED for 1 sec
	      	PIOClear(PPSIN);
				}
		   if(Acquire(volume_checked)==-1)break;
		   volume_checked=true;

	      #ifdef HIGH_DURING_ACK_PIN
		      PIOClear(HIGH_DURING_ACK_PIN);		// done recording
	      #endif
	      
			//Make sure to clear communication line with CSAC
			if(DAQ==3){
				PIOClear(PCCSAC);
				PIOClear (CF2CSAC);//CSACDAQ Rev 3 board and after
	      }//nfile++;
	      //nfile or ACQ.FILECOUNT are not used anymore
	      nfile++;
	      //ACQ.FILECOUNT=nfile;//commented out 4/19/2013 HM
	      
	      //ACQ.FILECOUNT is updated at each file write. It uses VEE flash memory
	      //and eventuary fills up the page and stopps updating.  To avoid this, you must reboot CFx
	      //to clean start the VEE page at approximately every 400 ACQ.FILECOUNT updates.
	      //if(nfile%200==0){
	      //   flogf("REACHED THE END OF VEE PAGE. REBOOTING TO CLEAN UP VEE MEMORY\n");
         //   Turn_Off_Preamp();
	      //   BIOSReset(); //Reboot CFx to clean the VEE flash memory
			//   }
			if(ACQ.ACTIVESEC!=ACQ.DUTYCYCLE && LowPowerHibern()==-1)break;
			if(ProgramFinished)break;
			  
			//if(DiskCheck()==-1)break;
			}
		}
   #ifdef HIGH_DURING_ACK_PIN
	   PIOClear(HIGH_DURING_ACK_PIN);		// done recording
   #endif
  
   exitProg:
   if(DAQ>=2)PIOClear(AD_REF_SHDN_PIN);//shut down AD  
   BIRShutdown(true); //make sure BIA is down
   Turn_Off_Preamp(); //Pre-amp power down
	//Make sure to clear communication line with CSAC
	if(DAQ==3){
		PIOClear(PCCSAC);
		PIOClear (CF2CSAC);//CSACDAQ Rev 3 board and after
	}
	SleepUntilWoken();
	WhatTime(time_chr);
	flogf("Program ended         %s failmsg = %s\n", time_chr, failmsg);putflush();
	//BIOSResetToPicoDOS();
	return 0;
	
	}	//____ main() ____//

/******************************************************************************\
**	PreRun		Exit opportunity before we start the acquisition program
**	
**	
\******************************************************************************/
void PreRun(void)
	{
	short				i;
	char				c;
	long 				max_AD;
   char           what='n';
	char 				*ProgramDescription = 
		{
		"\n"
		"The program will start in ten seconds unless an operator keys in a\n"
		"period '.' character. If a period is received, you have about 10 \n"
		"seconds to respond to access other software functions on this system\n"
		"before a watchdog reset occurs. To start the program immediately, \n"
		"hit any other key.\n"
		"\n"
		};
	//Added July 2015		
	if(SPDQ_ACQ_CLOCK==640) max_AD=1;
	if(SPDQ_ACQ_CLOCK==1600)max_AD=5;
	if(SPDQ_ACQ_CLOCK==3200)max_AD=10;
	if(SPDQ_ACQ_CLOCK==8000)max_AD=40;
	if(SPDQ_ACQ_CLOCK==16000)max_AD=64;
	if(SPDQ_ACQ_CLOCK==19200)max_AD=96;
	
	flogf("\n########Clock speed = %d kHz, Max Sample Rate=%ld kHz########",  SPDQ_ACQ_CLOCK, max_AD);  //July 2015		
	//LOGTOCON  =ACQ.GOMONITOR;
	//Initflog(ACQ.LOGFILE, LOGTOCON ? true : false);//Create or open the event logging file
	   
   if(ACQ.STARTUPS==0) {
	   while(what != 'y')
		   {
		   what = 'y';
		   flogf("Did you set up CFx' clock to GMT? (Y/N): ");
		   ciflush();
		   what = getch();
		   flogf("%c\n",what);
		   if(what == 'n' || what == 'N') { 
			   flogf("\nExit out of the program and set CFx's RTC clock!");
            BIOSResetToPicoDOS();      
			   }
		   TickleSWSR();	// another reprieve
		   }
		// Display the control variables we'll be using
	DisplayParameters();
	fflush(NULL);
	ciflush();
	}


	cprintf(ProgramDescription);

	for (i = 10; i > 0; i--)
		{
		cprintf("%u..", i);
		TickleSWSR();	// another reprieve

		c = SCIRxGetCharWithTimeout(1000);	// 1 second
		if (c == '.')
			break;
		if (c == -1)
			continue;
		i = 0;		// any other key ends the timeout
		TickleSWSR();	// another reprieve
		}
	if (i <= 0)
		{
		cprintf("\nStarting...\n");
		return;		// to start acquisition
		}

	TickleSWSR();	// another reprieve

	QRchar("\nWhat next?(P=PicoDOS, S=Settings)",
		"%c", false, &c, "PS", true);
	if (c == 'S')
		{
		settings();
		BIOSReset();	// force clean restart
		}
	else if (c == 'P')
		BIOSResetToPicoDOS();
   
	return;		// to start acquisition
	
	}	//____ PreRun() ____//
/******************************************************************************\
**	ACQGetSettings		Read in settings from VEE or use defaults if not found
**
**  1-st release 9/12/99
**  2nd 9/18/02 HM
\******************************************************************************/
void ACQGetSettings(void)
	{
	char  *p;
	short intLat, intLon;
	float LatMin, LonMin, LastLatDeg, LastLonDeg;
	char  ns, ew, b1;

	CLEAR_OBJECT(ACQ);
	
	p = VEEFetchData(ACQ_PLTFRMID_NAME).str;
	strncpy(ACQ.PLTFRMID, p ? p : ACQ_PLTFRMID_DEFAULT, sizeof(ACQ.PLTFRMID));
	DBG( uprintf("ACQ.PLTFRMID=%s (%s)\n", ACQ.PLTFRMID, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(ACQ_STARTUPS_NAME).str;
	ACQ.STARTUPS = atoi(p ? p : ACQ_STARTUPS_DEFAULT);
	DBG( uprintf("ACQ.STARTUPS=%d (%s)\n", ACQ.STARTUPS, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(ACQ_MAXSTRTS_NAME).str;
	ACQ.MAXSTRTS = atoi(p ? p : ACQ_MAXSTRTS_DEFAULT);
	DBG( uprintf("ACQ.MAXSTRTS=%d (%s)\n", ACQ.MAXSTRTS, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(ACQ_NCHAN_NAME).str;
	ACQ.NCHAN = atoi(p ? p : ACQ_NCHAN_DEFAULT);
	DBG( cprintf("ACQ.NCHAN=%u (%s)\n", ACQ.NCHAN, p ? "vee" : "def");  cdrain();)
	//if(ACQ.NCHAN!=1)  Temp change HM 7/12/02
	if(ACQ.NCHAN !=8 && ACQ.NCHAN !=1 && ACQ.NCHAN !=2 && ACQ.NCHAN !=4)
		{
		uprintf("\nNumber of A/D chan must be 1, 2, 4 or 8.  Check ACQ.NCHAN.\n");
		abort();
		}
	
	p = VEEFetchData(ACQ_SRATEHZ_NAME).str;
	ACQ.SRATEHZ = atol(p ? p : ACQ_SRATEHZ_DEFAULT);
	DBG( uprintf("ACQ.SRATEHZ=%ld (%s)\n", ACQ.SRATEHZ, p ? "vee" : "def");  cdrain();)
 
	p = VEEFetchData(ACQ_SAMPLES_NAME).str;
	ACQ.SAMPLES = atoi(p ? p : ACQ_SAMPLES_DEFAULT);
	DBG( uprintf("ACQ.SAMPLES=%u (%s)\n", ACQ.SAMPLES, p ? "vee" : "def");  cdrain();)
	if(ACQ.SAMPLES != 0 && ACQ.SAMPLES != 3)
		{
		uprintf("\nACQ.SAMPLES is wrong.  Check ACQ.SAMPLES.\n");
		abort();
		}
	 
	p = VEEFetchData(ACQ_LOGFILE_NAME).str;
	strncpy(ACQ.LOGFILE, p ? p : ACQ_LOGFILE_DEFAULT, sizeof(ACQ.LOGFILE));
	DBG( uprintf("ACQ.LOGFILE=%s (%s)\n", ACQ.LOGFILE, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(ACQ_LATITUDE_NAME).str;
	strncpy(ACQ.LATITUDE, p ? p : ACQ_LATITUDE_DEFAULT, sizeof(ACQ.LATITUDE));
	DBG( uprintf("ACQ.LATITUDE=%s (%s)\n", ACQ.LATITUDE, p ? "vee" : "def");  cdrain();)
	sscanf(ACQ.LATITUDE,"%c%2d%c%f", &ns, &intLat,&b1,&LatMin);
	//printf("%c%.2d:%5.2f\n",ns,intLat,LatMin);
	LastLatDeg=intLat+LatMin/60.;
	if(ns=='s' || ns=='S')LastLatDeg=-LastLatDeg;
	//printf("%f\n", LatDeg);

	p = VEEFetchData(ACQ_LONGITUDE_NAME).str;
	strncpy(ACQ.LONGITUDE, p ? p : ACQ_LONGITUDE_DEFAULT, sizeof(ACQ.LONGITUDE));
	DBG( uprintf("ACQ.LONGITUDE=%s (%s)\n", ACQ.LONGITUDE, p ? "vee" : "def");  cdrain();)
	sscanf(ACQ.LONGITUDE,"%c%3d%c%f", &ew,&intLon,&b1, &LonMin );
	//printf("%c%.3d:%5.2f\n",ew,intLon, LonMin);
	LastLonDeg=intLon+LonMin/60.;
	if(ew=='e' || ew=='W')LastLonDeg=-LastLonDeg;
	//printf("%f\n",LonDeg);

	p = VEEFetchData(ACQ_GAIN_NAME).str;
	ACQ.GAIN = atoi(p ? p : ACQ_GAIN_DEFAULT);
	DBG( uprintf("ACQ.GAIN=%u (%s)\n", ACQ.GAIN, p ? "vee" : "def");  cdrain();)
	if(ACQ.GAIN<0 || ACQ.GAIN>3)
		{
		printf("\nGain is wrong.  Check ACQ.GAIN.\n");
		abort();
		}

	p = VEEFetchData(ACQ_EXPID_NAME).str;
	strncpy(ACQ.EXPID, p ? p : ACQ_EXPID_DEFAULT, sizeof(ACQ.EXPID));
	DBG( uprintf("ACQ.EXPID=%s (%s)\n", ACQ.EXPID, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(ACQ_PROJID_NAME).str;
	strncpy(ACQ.PROJID, p ? p : ACQ_PROJID_DEFAULT, sizeof(ACQ.PROJID));
	DBG( cprintf("ACQ.PROJID=%s (%s)\n", ACQ.PROJID, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(ACQ_PROG_NAME).str;
	strncpy(ACQ.PROGNAME, p ? p : ACQ_PROG_DEFAULT, sizeof(ACQ.PROGNAME));
	DBG( uprintf("ACQ.PROGNAME=%s (%s)\n", ACQ.PROGNAME, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(ACQ_WARMUP_NAME).str;
	ACQ.WARMUP= atoi(p ? p : ACQ_WARMUP_DEFAULT);
	DBG( uprintf("ACQ.WARMUP=%d (%s)\n", ACQ.WARMUP, p ? "vee" : "def");  cdrain();)
	if(ACQ.WARMUP<0 || ACQ.WARMUP >99)
	   {
	   printf("Pre-amp ACQ.WARMUP sec must be <99 and >=0.\n");
	   }

	//p = VEEFetchData(ACQ_FILECOUNT_NAME).str;
	//ACQ.FILECOUNT= atoi(p ? p : ACQ_FILECOUNT_DEFAULT);
	//DBG( uprintf("ACQ.FILECOUNT=%d (%s)\n", ACQ.FILECOUNT, p ? "vee" : "def");  cdrain();)

	//p = VEEFetchData(ACQ_MAXNUMFIL_NAME).str;
	//ACQ.MAXNUMFIL= atoi(p ? p : ACQ_MAXNUMFIL_DEFAULT);
	//DBG(uprintf("ACQ.MAXNUMFIL=%d (%s)\n", ACQ.MAXNUMFIL, p ? "vee" : "def");  cdrain();)
	
	p = VEEFetchData(ACQ_SLEEP_NAME).str;
	ACQ.SLEEP = atoi(p ? p : ACQ_SLEEP_DEFAULT);
	DBG( cprintf("ACQ.SLEEP=%u (%s)\n", ACQ.SLEEP, p ? "vee" : "def");  cdrain();)
	
	//TestDuration=ACQ.TESTSEC;
	p = VEEFetchData(ACQ_TESTSEC_NAME).str;
	ACQ.TESTSEC = atoi(p ? p : ACQ_TESTSEC_DEFAULT);
	DBG( cprintf("ACQ.TESTSEC=%u (%s)\n", ACQ.TESTSEC, p ? "vee" : "def");  cdrain();)
	TestDuration=(long)ACQ.TESTSEC; //8-channel test logging duration
  	NTestSamplePerFile = TestDuration * ACQ.SRATEHZ;//Test logging samples

	p = VEEFetchData(ACQ_ACTIVESEC_NAME).str;
	ACQ.ACTIVESEC = atol(p ? p : ACQ_ACTIVESEC_DEFAULT);
	DBG( cprintf("ACQ.ACTIVESEC=%lu (%s)\n", ACQ.ACTIVESEC, p ? "vee" : "def");  cdrain();)
   
   //Determine the number of samples per file and CFPPBSZ buffer size here
   NSamplePerFile= ACQ.ACTIVESEC * ACQ.SRATEHZ*ACQ.NCHAN; //4/19/2013 HM
   DBG(cprintf("ACQ.SRATEHZ =%ld\nNSamplePerFile=%ld\n", ACQ.SRATEHZ, NSamplePerFile));
   
   BIRS.CFPPBSZ  = NSamplePerFile * 2L + 256L;  //True byte size of the file
   DBG(cprintf("CFPPBSZ=%lu\n", BIRS.CFPPBSZ));
   if(BIRS.CFPPBSZ<262144){//262114 is spdqcb.ring_buf_size in Acquire()
      cprintf("ACQ.ACTIVESEC must be at least %d sec\n", (short)((float)(262114-256)/(float)(ACQ.SRATEHZ*2)+1));
      abort();
      }

	p = VEEFetchData(ACQ_DUTYCYCLE_NAME).str;
	ACQ.DUTYCYCLE = atol(p ? p : ACQ_DUTYCYCLE_DEFAULT);
		DBG( cprintf("ACQ.DUTYCYCLE=%lu\n", ACQ.DUTYCYCLE);  cdrain();)
   if(ACQ.DUTYCYCLE < ACQ.ACTIVESEC)
      {
      printf("ACQ.DUTYCYCLE must be larger or equal to ACQ.ACTIVESEC\n");
      abort();
      }

	p = VEEFetchData(ACQ_HYDROSENS_NAME).str;
	ACQ.HYDROSENS = atoi(p ? p : ACQ_HYDROSENS_DEFAULT);
	DBG( cprintf("ACQ.HYDROSENS = %u (%s)\n", ACQ.HYDROSENS, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(ACQ_PRAMPNAME_NAME).str;
	strncpy(ACQ.PRAMPNAME, p ? p : ACQ_PRAMPNAME_DEFAULT, sizeof(ACQ.PRAMPNAME));
	DBG( cprintf("ACQ.PRAMPNAME=%s (%s)\n", ACQ.PRAMPNAME, p ? "vee" : "def");  cdrain();)
	
	p = VEEFetchData(ACQ_DAQNAME_NAME).str;
	strncpy(ACQ.DAQNAME, p ? p : ACQ_DAQNAME_DEFAULT, sizeof(ACQ.DAQNAME));
	DBG( cprintf("ACQ.DAQNAME=%s (%s)\n", ACQ.DAQNAME, p ? "vee" : "def");  cdrain();)
	//if(!strncmp(ACQ.DAQNAME,"H3DAQ",5))
	//	{
	//	if(DAQ!=1)
	//		{
	//		printf("1ACQ.DAQNAME does not match. Wrong version. Aborting!\n");
	//		abort();
	//		}	
	//	}
	//if(!strncmp(ACQ.DAQNAME,"SMDAQ",5))
	//	{
	//	if(DAQ!=2)
	//		{
	//		printf("2ACQ.DAQNAME does not match. Wrong version. Aborting!\n");
	//		abort();
	//		}
	//	}
	//if(!strncmp(ACQ.DAQNAME,"CSACD",5))
	//	{
	//	if(DAQ!=3)
	//		{
	//		printf("3ACQ.DAQNAME does not match. Wrong version. Aborting!\n");
	//		abort();
	//		}
	//	}					
	p = VEEFetchData(ACQ_HYDROSRN_NAME).str;
	strncpy(ACQ.HYDROSRN, p ? p : ACQ_HYDROSRN_DEFAULT, sizeof(ACQ.HYDROSRN));
	DBG( cprintf("ACQ.HYDROSRN=%s (%s)\n", ACQ.HYDROSRN, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(ACQ_WAKEUP_NAME).str;
	ACQ.WAKEUP = atol(p ? p : ACQ_WAKEUP_DEFAULT);
		DBG( cprintf("ACQ.WAKEUP=%lu\n", ACQ.WAKEUP);  cdrain();)

	p = VEEFetchData(ACQ_PWFILT_NAME).str;
	ACQ.PWFILT = atoi(p ? p : ACQ_PWFILT_DEFAULT);
	DBG( cprintf("ACQ.PWFILT=%u (%s)\n", ACQ.PWFILT, p ? "vee" : "def"); cdrain();)
	if(ACQ.PWFILT <=0 || ACQ.PWFILT >=5 )
		{
		printf("\nPre-Whitening filter switch setting is wrong.  Check ACQ.PWFILT.\n");
		abort();
		}
	p = VEEFetchData(ACQ_LOPASS_NAME).str;
	ACQ.LOPASS = atoi(p ? p : ACQ_LOPASS_DEFAULT);
	DBG( cprintf("ACQ.LOPASS=%u (%s)\n", ACQ.LOPASS, p ? "vee" : "def"); cdrain();)
	//if(ACQ.LOPASS <=44 || ACQ.LOPASS >2000 )
	//	{
	//	printf("\nLow-pass filer cut-off is wrong.  Check ACQ.LOPASS.\n");
	//	abort();
	//	}
	
	}	//____ ACQGetSettings() ____//

/*************************************************************************************
**	WhatTime
**
**	Returns 20-char long current time in RTC time in GMT format. 
**
**	|RTC__--------------|
**	104 245:13:34:45:100
**  123456789012345678901
**           1         2         3         4
**	NOAA, Nerport, OR
**	Haru Matsumoto 1-st version 10/26/04	hm
*************************************************************************************/
ulong WhatTime(char *time_chr)
	{
	//RTCtm	*QT_time;			//Q-Tech clock time
	RTCtm	*rtc_time;				//CFX's real time clock
	ulong	SecondsSince1970 =  NULL;
	ushort	ticks;

	RTCGetTime(&SecondsSince1970, &ticks);
	rtc_time = RTClocaltime (&SecondsSince1970);
	sprintf(time_chr,"%.3d %.3d:%.2d:%.2d:%.2d:%.3u",
	rtc_time->tm_year, rtc_time->tm_yday + 1,\
	rtc_time->tm_hour,rtc_time->tm_min,rtc_time->tm_sec, TICKS_TO_MS(ticks));
	return SecondsSince1970;
	}	//WhatTime



/******************************************************************************\
**	BIRGetSettings		Read in settings from VEE or use defaults if not found
**
** April 12, 2014
** Added a safety to check if BIR.CURBIA and BIR.CFDOSDRV match.
**	Look in <BIRecorder2.h> for detailed descriptions of the variables.
\******************************************************************************/

static void BIRGetSettings(void)
	{
	
//	FLOGFLAG	Log major operating events (when non-zero). NOT USED
	BIRS.FLOGFLAG = VEEFetchLong(BIR_FLOGFLAG_NAME, atol(BIR_FLOGFLAG_DEFAULT));
		DBG( cprintf("BIRS.FLOGFLAG=%u\n", BIRS.FLOGFLAG);  cdrain();)

//	CFDOSDRV	DOS drive letter for hard disk  NOT USED
	strncpy(BIRS.CFDOSDRV, VEEFetchStr(BIR_CFDOSDRV_NAME, BIR_CFDOSDRV_DEFAULT), sizeof(BIRS.CFDOSDRV));
		DBG( cprintf("BIRS.CFDOSDRV=%s\n", BIRS.CFDOSDRV);  cdrain();)

//	MINFREEKB	Minimum free KB to continue use of current hard disk volume
	BIRS.MINFREEKB = VEEFetchLong(BIR_MINFREEKB_NAME, atol(BIR_MINFREEKB_DEFAULT));
	if (BIRS.MINFREEKB == 0)	// Fill in default if not explicitly defined 
		BIRS.MINFREEKB = (BIRS.CFPPBSZ) / KB_SIZE + 100L;//fudge factor
		DBG(cprintf("BIRS.MINFREEKB=%lu\n", BIRS.MINFREEKB);  cdrain());

//	CURBIA	Index for current BigIDEdrive 
	BIRS.CURBIA = VEEFetchLong(BIR_CURBIA_NAME, atol(BIR_CURBIA_DEFAULT));
		DBG( cprintf("BIRS.CURBIA=%u\n", BIRS.CURBIA);  cdrain();)
		
//	BIADEVICE		BigIDEA device 3= mater, 4= slave
	BIRS.BIADEVICE = (BIADEV) VEEFetchLong(BIR_BIADEVICE_NAME, atol(BIR_BIADEVICE_DEFAULT));
		DBG( cprintf("BIRS.BIADEVICE=%u\n", BIRS.BIADEVICE);  cdrain();)
	//BIRS.CFDOSDRV[0]=(BIADEV) VEEFetchStr(BIR_CFDOSDRV_NAME, BIR_CFDOSDRV_DEFAULT);
	strncpy(BIRS.CFDOSDRV, VEEFetchStr(BIR_CFDOSDRV_NAME, BIR_CFDOSDRV_DEFAULT), sizeof(BIRS.CFDOSDRV));
	 DBG(cprintf("BIRS.CFDOSDRV=%s\n", BIRS.CFDOSDRV);  cdrain();)

	//uprintf("pass 0 %s %u\n", BIRS.CFDOSDRV, BIRS.BIADEVICE);
	if(!strncmp(BIRS.CFDOSDRV,"D:",2) || !strncmp(BIRS.CFDOSDRV,"d:",2))
		{
		//uprintf("pass1\n");
	
		if(BIRS.BIADEVICE != 3)
		{
		uprintf("BIRS.BIADEVICE=%d\n", BIRS.BIADEVICE);
		uprintf("\n#BIR.BIADEVICE and BIR.CFDOSDRV are inconsistent.\n");
		abort();
		}
	}
	if(!strncmp(BIRS.CFDOSDRV,"E:",2) || !strncmp(BIRS.CFDOSDRV,"e:",2))
	   {
	   //uprintf("pass 2\n");
		if(BIRS.BIADEVICE != 4)
		{
		uprintf("\n##BIR.BIADEVICE and BIR.CFDOSDRV are inconsistent.\n");
		abort();
		}
	}
		

	}	//____ BIRGetSettings() ____//
	

FILE *Open_File(ulong filecount)
{	
static char	fnew[]="x:00000000.DAT";			//first detection file

fnew[0]= BIRS.CFDOSDRV[0]; 						//Add the drive letter E: or D:
//char	strbuf[7];
	sprintf(&fnew[2], "%08lu.DAT", filecount);//name a new detection file
	//flogf("2 New file name %s\n",fnew);
	//sprintf(strbuf, "%u", ++filecount);
	//VEEStoreStr(ACQ_FILECOUNT_NAME, strbuf);
	//execstr("mount bfcm");
	fp = (FILE *) &AcqFileHandle;
	if ((AcqFileHandle = open(fnew, O_RDWR | O_CREAT | O_TRUNC)) <= 0)
		{
		fp = 0;
		flogf("Couldn't open data file!\n");putflush();
		return fp;
		}
	flogf( "%s is created\n",fnew);putflush();
	return fp;
}
//******************************************************************************
// Write_Header  (256 bytes)
//******************************************************************************
//FILE *Write_Header(FILE *fp)
//	{
//	CFxBIRUpdateHeader();
//	fwrite(&BIRS, sizeof(BIRS),1,fp);
//	fwrite(&ACQ, BIRS.BIRUserHeaderSize,1,fp);
//	return fp;
//	}
/******************************************************************************\
**	CFxBIRUpdateHeader		Update header information
\******************************************************************************/
void CFxBIRUpdateHeader()
	{

	RTCGetTime(&BIRS.RTCsecs, &BIRS.RTCticks);	// update date and time
	
	}	//____ CFxBIRUpdateHeader() ____//
/******************************************************************************\
**	ACQUpdateHeader	Return pointer to file header information
**	
**	THIS SHOULD BE CUSTOMIZED TO YOUR APPLICATIONS REQUIREMENTS
**	
\******************************************************************************/
void *ACQUpdateHeader(void)
	{
	WhatTime(time_chr);
	strcpy(ACQ.TIME_GMT, time_chr);

	return &ACQ;

	}	//____ ACQUpdateHeader() ____//

//********************************************************************************
// Set up file system and calculate file and disk sizes here.  Make sure there is 
// enough disk space.
// HM 9/28/2011
//********************************************************************************

short SetupAcquisition(void)
	{
	long	freeSpacekB;
	long 	totalSpacekB;
	long  freeSectors;
	long  totalSectors;
	long  freeByteskB;
	long  filesizeB;
	short err;   
	bool  BIAChange, SlotChange;
	char  *failmsg="";
	
	//Clear communication line with CSAC for CSACDAQ
	if(DAQ==3){ //CSACDAQ only
		PIOClear(PCCSAC);
		PIOClear (CF2CSAC);//CSACDAQ Rev 3 board and after
		//PIOSet(CF2CSAC);		//Attention Only for CSACDAQ Rev 2.  
	}
	
	BIRS.BIRUserHeaderSize = BIR_USER_HDR_SIZE;//needs this to update the headers

	flogf("ACQ.STARTUPS         = %d\n", ACQ.STARTUPS);
	flogf("Setting up file system including the BIA.\n");

	//cprintf("pass 1 %d %s\n", BIRS.CURBIA, BIRS.CFDOSDRV);
	//Search the first BIADEVICE available 
  	TickleSWSR();  //Added July 2015
  	if ((err = BIRSetup(calloc, free, ACQCollabBIR)) != 0)
  	
  	//BIRGetSettings();
  	cprintf("\nCan't setup the BIR components !\n [%d = %s]!\n", err, BIRErrorString(err));
	//cprintf("BIRVersion = %d\n",BIRS.BIRVersion);

	//cprintf("pass 2 %d %s\n", BIRS.CURBIA, BIRS.CFDOSDRV);

	while(!AllDrivesFull  && BIRS.CURBIA <= BIR_MAX_BIGIDEA){

		BIRCurrentSetup(&BIRS.CURBIA, &BIRS.BIADEVICE, &BIRS.CFDOSDRV);//added July 2015
      freeSectors  = DSDFreeSectors( BIRS.CFDOSDRV[0]-'A');
   	totalSectors = DSDDataSectors( BIRS.CFDOSDRV[0]-'A');
   	freeSpacekB  = freeSectors/2;
   	totalSpacekB = totalSectors/2;            //??
   	freeByteskB  = freeSpacekB;
   	filesizeB    = BIRS.CFPPBSZ; 
		TickleSWSR();	// another reprieve
      
      flogf("BigIDEA card number   = %d\n", BIRS.CURBIA);
      flogf("Master(3)/slave(4)    = %d\n", BIRS.BIADEVICE); 
      flogf("Drive                   %c:\n", BIRS.CFDOSDRV[0]); 
   	flogf("Disk size             = %ld kB\n", totalSpacekB);
   	flogf("Total Free Disk Space = %ld kB\n", freeSpacekB);
   	//flogf("Total disk space left= %ld \n", freeByteskB);
   	//flogf("File size (bytes)    = %ld \n", BIRS.CFPPBSZ);
   	//flogf("File size            = %f kB\n", filesizekB);
      BIRS.BIRStartFreeKB=freeSpacekB;
      BIRS.BIRCapacityKB=totalSpacekB;	
   	if(freeSpacekB <= (BIRS.CFPPBSZ/KB_SIZE+10L) || freeSpacekB <= BIRS.MINFREEKB){
   	   flogf("Disk %d slot %d space is full.\n", BIRS.CURBIA, BIRS.BIADEVICE);
         BIRShutdown(true);
         if(BIRS.CURBIA==4) {
            BIAChange=true;	//Now slave=E: drive change BIA
            SlotChange=true;	//Change slot to D: 
            }
         else {
            BIAChange=false;	//Now master=D: 
            SlotChange=true;	//Change slot only to E:
            }

       DBG( flogf("\n[SETUP%d]=SetupAcquisition()\n", __LINE__);  cdrain();)
			TickleSWSR();  //July 2015
         err = BIRSetupNextVolume(BIAChange, SlotChange);//Don't change the slot or disk
	      //update the BIA settings
	      BIRS.CURBIA = VEEFetchLong(BIR_CURBIA_NAME, atol(BIR_CURBIA_DEFAULT));	
	      BIRS.BIADEVICE = (BIADEV) VEEFetchLong(BIR_BIADEVICE_NAME, atol(BIR_BIADEVICE_DEFAULT));
			strncpy(BIRS.CFDOSDRV, VEEFetchStr(BIR_CFDOSDRV_NAME, BIR_CFDOSDRV_DEFAULT), sizeof(BIRS.CFDOSDRV));
   	}	else break;
   //execstr("SET SYS.F32TRUST=*");//Force F32 optimiization
   }
   
   if(err==birNoFreeSpace || AllDrivesFull == true || BIRS.CURBIA >=BIR_MAX_BIGIDEA){ //err =20 disk is full
      ProgramFinished=true;
      flogf("All disks and CF card slots are full.\n");
      return -1;
      }

	//numfiles=freeBytes/filesizekB;
	//flogf("%f, %f, %f\n", freeBytes, filesizekB, numfiles);
	//ACQ.MAXNUMFIL=numfiles*1000.-1;
   //printf("ACQ.MAXNUMFIL=%f\n",freeByteskB/filesizekB);

	ACQ.MAXNUMFIL=(long)(freeByteskB/(BIRS.CFPPBSZ/KB_SIZE+10L));
	flogf("Max number of files for this disk = %ld\n", ACQ.MAXNUMFIL);
	if(ACQ.MAXNUMFIL<1L){
	   flogf("BIR.CFPPBSZ is probably too small or disk has no space left\n");
	   ProgramFinished=true;
	   //flogf("Free space in C:= %ld\n", DIRFreeSpace("c:\\"));
	   return -1;
	   }
	
	//execstr("dir D:");

	CSSetSysAccessSpeeds(nsFlashStd, nsRAMStd, nsCFStd, WTMODE);
	TMGSetSpeed(SYSCLK);

	return 0;
	}
//SetupAcquistion	
	
/******************************************************************************\
**	Acquire	// Minimalist SPDQ Recorder
\******************************************************************************/

short Acquire(bool volume_checked)
	{
	bool		uni = true;		// true for unipolar, false for bipolar
	bool		sgl = true;		// true for single-ended, false for differential
	//char 	*p;
	long		i;             // A/D sample counter index
	long		freeSpacekB;

	short	   returncode = 0;
	char	   *failmsg = "";
	bool	   pz = false;			// becomes true when PicoZOOM is setup
	ushort	entryclock = TMGGetSpeed();
	char 	   combobuf[sizeof(BIRS) + BIR_USER_HDR_SIZE];
	long     NSamplePerFileHeader;
   char     *fname;
   ulong    filecount;
   char     time_chr[22];
 
	static spdqcb	spdqr = 
		{
		  SPDQ_ACQ_CLOCK	// sys_clk
		, 0					// agg_sps;
		, 1					// chans;			
		, 1					// inp_end sgl;			
		, AD_REF_SHDN_PIN	// adc_shdn;		
		, 3					// adc_pcs;		
		, 2					// irq_pcs;		
		, 41				   // irq_pin IRQ2;	//New
		, 0				   // pwm_pin;			
		, 0					// con_dbg;		
		, 0					// p1_diag_lwr;
		, 0					// p42_diag_up;   //This makes the logging faster
		, 262144			   // ring_buf_size;	
		, WRITE_SIZE		// req_blk_size;	
		, WRITE_SIZE		// max_req_size;	
		, 0					// pre_trig_size
		};				

	fp = 0;		// clear at start so we don't attempt fclose
	errno = 0;
	
	//PIOSet(PRAMPON);     //Pre-amp power ON

	spdqr.agg_sps = (ulong)ACQ.SRATEHZ*ACQ.NCHAN; //4/19/2013 HM
	spdqr.chans   = (uchar)ACQ.NCHAN;					//3/20/2013 HM
	//flogf("spdqr.agg_sps=%lu\n", spdqr.agg_sps);
	//flogf("spdqr.chans  =%c\n", spdqr.chans);
	
	//Getting the next file name
	//p = VEEFetchData(ACQ_FILECOUNT_NAME).str;
	//ACQ.FILECOUNT= atoi(p ? p : ACQ_FILECOUNT_DEFAULT);
	
	//Do not update file name if it's been incremented by SetupAcquisition()
	fname = BIRNextFileName(true, volume_checked, &filecount);
	//BIRS.CURBIA = VEEFetchLong(BIR_CURBIA_NAME, atol(BIR_CURBIA_DEFAULT));	
	//BIRS.BIADEVICE = (BIADEV) VEEFetchLong(BIR_BIADEVICE_NAME, atol(BIR_BIADEVICE_DEFAULT));
	//cprintf("File name %s\n", fname);

	//ACQ.FILECOUNT=filecount; //4/19/2013 HM
	 
	//For now just one master CF card 9/20/2011 HM
   TickleSWSR();	// another reprieve

	//BIRS.CFDOSDRV[0]=fname[0]; //2/03/2014 HM

	BIRCurrentSetup(&BIRS.CURBIA, &BIRS.BIADEVICE, &BIRS.CFDOSDRV);//added July 2015
	freeSpacekB  = DSDFreeSectors( BIRS.CFDOSDRV[0]-'A')/2; //Pick the right drive
	if(freeSpacekB < (BIRS.CFPPBSZ/KB_SIZE + 100L) || freeSpacekB < BIRS.MINFREEKB){
	      //flogf("Acquire(): Disk %d slot % d is full. \n", BIRS.CURBIA, BIRS.BIADEVICE );
	      flogf("Acquire(): %c: Disk is full.\n", BIRS.CFDOSDRV[0]);
			returncode = -1;
			failmsg = "Disk is full";
			goto exitAcquire;
			}	

	pz = PZCacheSetup( BIRS.CFDOSDRV[0]-'A', calloc, free); //write to d: or e: drive
	if (! pz)
		{
		returncode = -2;
		failmsg = "Can't open PicoZOOM";
		goto exitAcquire;
		}

	WhatTime(time_chr);
	flogf("File %c:%08lu started ", BIRS.CFDOSDRV[0],filecount);cdrain();coflush();
	flogf("%s ", time_chr);putflush();

	errno = 0;
	fp = Open_File (filecount);
	if (fp == 0)
		{
		returncode = errno;
		failmsg = "Can't open data file";
		goto exitAcquire;
		}

	ACQUpdateHeader();
   UpdateGain();        //Set up the gain of pre-amp
	TickleSWSR();	// another reprieve

	if (spdq_open(&spdqr) != 0)
		{
		returncode = spdqr.errcode;
		failmsg = "Can't open SPDQ";
		goto exitAcquire;
		}

	// Alternative to Write_Header function
	CFxBIRUpdateHeader();
	memcpy(combobuf, &BIRS, sizeof(BIRS));
	//memcpy(&combobuf[sizeof(BIRS)], ((char *) &ACQ) + sizeof(BIRS), BIR_USER_HDR_SIZE);
	memcpy(&combobuf[sizeof(BIRS)], &ACQ, BIR_USER_HDR_SIZE);
	
	if (! spdq_write_header(&spdqr, combobuf, sizeof(combobuf)))
		{
		returncode = errno;
		failmsg = "errno writing header";
		goto exitAcquire;
		}

	//debug test gain=0,1,2,3.  Each file created has a 6dB higher gain than the previous  
	//DBG(TestGain(testgain%4));
	//DBG(printf("Test gain = %d\n",  testgain%4));
	//DBG(testgain++);

	//RTCDelayMicroSeconds(100000L);//wait to settle		// JHG02 ??????? why
	//RTCDelayMicroSeconds(100000L);//wait to settle		// JHG02 ??????? why

	CTMRun(false);				   // turn off CTM6 module
	SCITxWaitCompletion();		// let any pending UART data finish
	EIAForceOff(true);			// turn off the RS232 driver

	i=0L;

	//Enable the IRQ5 (PBS) interrupt to stop acquisition
	IEVInsertCFunct(&ExtFinishPulseRuptHandler, level5InterruptAutovector);
	PinBus(IRQ5);					 // enable PBM (IRQ5) interrupt button

	// START
	spdq_activate();
   
   NSamplePerFileHeader=NSamplePerFile+128L;
	// ACQUISITION LOOP
	while (! ProgramFinished && i < NSamplePerFile && spdq_is_active())
		{
		short	idle_t;
		ulong	towrite;
		if (spdq_request_rb_contig_block(&spdqr))
			{
		   TickleSWSR();	// another reprieve

			errno = 0;
			towrite = 2 * (NSamplePerFileHeader - i);
			if (towrite >= spdqr.contig_bytes)
				 towrite = spdqr.contig_bytes;

			if (write(fp->handle, spdqr.rbcp, towrite) != towrite)
				{
				returncode = errno;
				failmsg = "write failed during acquisition";
				goto exitAcquire;
				}
			i += towrite / 2;
			spdq_release_rb_contig_block(&spdqr);
			continue;	// 01Jul2011-JHG	skip the next sleep cycle 
			}
		
		if (spdqr.errcode != 0)
			{
			returncode = spdqr.errcode;
			failmsg = "SPDQ failed during acquisition";
			goto exitAcquire;
			}

		idle_t = spdqr.agg_sps / 10;	// low power loop for 100ms before checking for data or break
		while (idle_t-- && spdq_is_active())
			LPStopCSE(CPUStop);	// VCO on (immed. wake) CPU on, SIMCLK is VCO clock
			
		}

	if (ProgramFinished)
		{
		returncode = -1;
		failmsg = "ProgramFinished";
		goto exitAcquire;
		}
	else if (! spdq_is_active())
		{
		returncode = spdqRBOverflow;
		failmsg = "spdqRBOverflow";
		}

	spdq_idle();
	//while (spdq_flush_ring_buffer(&spdqr))
	//	{
	//	write(fp->handle, spdqr.rbcp, spdqr.contig_bytes);
	//	spdq_release_rb_contig_block(&spdqr);
	//	}


exitAcquire:

	TMGSetSpeed(entryclock);

	// CONCLUDE
	PinIO(IRQ5);

	// CLEANUP
	spdq_close(&spdqr);

	WhatTime(time_chr);
	EIAForceOff(false);			// turn the RS232 driver back on

	if (fp)
		{
		close(fp->handle);
		fp->handle = 0;
		fp = 0;
	   //execstr("mount bfcm /d");
		}
	//PZCacheRelease('C'-'A');
	PZCacheRelease( BIRS.CFDOSDRV[0]-'A');

	//flogf("File %08u ended:  %s sampled %ld\n",ACQ.FILECOUNT, time_chr,i);cdrain();coflush();
	flogf("File %c:%08lu ended:  ", BIRS.CFDOSDRV[0],filecount);cdrain();coflush();
	flogf("%s", time_chr);cdrain();coflush();
	flogf(" Sampled %ld\n",i);putflush();cdrain();coflush();
	
	//Don't power off the pre-amp.  Next logging cycle begins immediately.
	//PIOClear(GAIN0);           //Bring the gain pins all zeros to be safe
	//PIOClear(GAIN1);
	//PIOClear(PRAMPON);         //Safe to turn off pre-amp power and save battery

	if (returncode)
		{
		if(returncode == -1)return 0;
		else{
   		cprintf("\n###Acquire failed: returncode = %d, failmsg = %s\n", returncode, failmsg);
		   if(ACQ.STARTUPS<=ACQ.MAXSTRTS)BIOSReset();//Keep on rebooting 8/12/2011 HM
   		return -1;
		   }
		}

	return 0;

	}	//____ Acquire ____//

/******************************************************************************\
**	ExtFinishPulseRuptHandler		IRQ5 logging stop request interrupt
**	
\******************************************************************************/
IEV_C_FUNCT(ExtFinishPulseRuptHandler)
	{
	#pragma unused(ievstack)	// implied (IEVStack *ievstack:__a0) parameter

	PinIO(IRQ5);
	ProgramFinished = true;
	WhatTime(time_chr);
	flogf("Program terminated by IRQ5 interrupt. %s \n", time_chr);putflush();
	}	//____ ExtFinishPulseRuptHandler() ____//

/******************************************************************************\
**	SleepUntilWoken		Sleep until IRQ4 is interrupted
**	
** 1-st release 9/14/99
** 2nd release 6/24/2002 by HM -Changed to use ADS8344/45
\******************************************************************************/
void SleepUntilWoken(void)
	{

	char	time_chr[42];
	
	ciflush();					// flush any junk
	flogf("Number of past resets %d\n",ACQ.STARTUPS);
	//if (ACQ.ONFINSUSP)
	//	{
	//	flogf("Suspendng (ground /WAKE-Pin38 to revive) ...\n");
		
	//	PWRSuspendTicks(0, false, WakePinOrCF);
	//	//PWRSuspendTicks(0, true, WakePinOrCF);
	//	}
	//else
	//	{
	//IRQ4 is connected to the RS232 RX
	//Less than 1mA low power mode
	flogf("\nLow-power sleep mode until keyboard input is received...\n");
	
	// Install the interrupt handlers that will break us out by "break signal" from RS232 COM input.
	IEVInsertAsmFunct(Irq4RxISR, level4InterruptAutovector);
	IEVInsertAsmFunct(Irq4RxISR, spuriousInterrupt);

	PITSet51msPeriod(PITOff);	// disable timer (drops power)
	CTMRun(false);				   // turn off CTM6 module
	SCITxWaitCompletion();		// let any pending UART data finish
	EIAForceOff(true);			// turn off the RS232 driver
	QSMStop();					   // shut down the QSM
	CFEnable(false);			   // turn off the CompactFlash card

	PinBus(IRQ4RXD);			   // make it an interrupt pin
		
	TickleSWSR();					// another reprieve
	TMGSetSpeed(1600);
	//TMGSetSpeed(800);			//sometimes it does not respond
	//while (PinRead(IRQ4RXD)) 
	while (PinTestIsItBus(IRQ4RXD)) 
		{
      //*HM050613 added to reduce current when Silicon System CF card is used	
		//*(ushort *)0xffffe00c=0xF000; //force CF card into Card-1 active mode

		LPStopCSE(FullStop);		// we will be here until interrupted
		TickleSWSR();				// by break
		}

	CSSetSysAccessSpeeds(nsFlashStd, nsRAMStd, nsCFStd, WTMODE);
	TMGSetSpeed(SYSCLK);
	
	// CONCLUDE
	PinIO(IRQ4RXD);

	EIAForceOff(false);			// turn on the RS232 driver
	QSMRun();					   // bring back the QSM
	CFEnable(true);				// turn on the CompactFlash card
	PIORead(IRQ4RXD);  			// disable interrupt by IRQ4
	ciflush();					   // discard any garbage characters
	WhatTime(time_chr);
	flogf("Aquisition ended!  Time now %s\n", time_chr);putflush();// tell 'em we're back
//			BIOSResetToPicoDOS();
	 //}
	}	//____ SleepUntilWoken() ____//

/******************************************************************************\
**	Irq4RxISR			Interrupt handler for IRQ4 (tied to CMOS RxD)
**	
**	This single interrupt service routine handles both the IRQ4 interrupt
**	and the very likely spurious interrupts that may be generated by the
**	repeated asynchronous and non-acknowledged pulses of RS-232 input.
**	
**	The handler simply reverts IRQ4 back to input (to prevent further level
**	sensative interrupts) and returns. It's assumed the routine that set this
**	up is just looking for the side effect of breaking the CPU out of STOP
**	or LPSTOP mode.
**	
**	Note that this very simple handler is defined as a normal C function
**	rather than an IEV_C_PROTO/IEV_C_FUNCT. We can do this because we know
**	(and can verify by checking the disassembly) that is generates only
**	direct addressing instructions and will not modify any registers.
\******************************************************************************/
static void Irq4RxISR(void)
	{

	PinIO(IRQ4RXD);		// 31 // /IRQ4 (tied to Rx)
	RTE();

	}	//____ Irq4RxISR() ____//

/**********************************************************************************************
**	DisplayParameters
**
**  NOAA, HM
**  1-st rev 9/02/99
**  2nd rev 9/18/02 
***********************************************************************************************/
void DisplayParameters()
{
short	i;
short duty;
   duty=(short)((float)(ACQ.ACTIVESEC*100L)/(float)ACQ.DUTYCYCLE);
	// Display the control variables we'll be using
	flogf(" ACQ CLOCK SPEED             = %d\n",  SPDQ_ACQ_CLOCK);
	flogf(" LOG FILE NAME               = %.14s\n", ACQ.LOGFILE);
	//flogf(" SYSTEM NAME               = %.3s\n", ACQ.SYSTEM);
	//flogf(" LOCATION                  = %.7s\n", ACQ.LOCATION);
	flogf(" HOURS OF PRE-LAUNCH SLEEP   = %u\n", ACQ.SLEEP);
	flogf(" ACTIVE LOGGING IN SEC/HOUR  = %ld\n",ACQ.ACTIVESEC);	
	flogf(" LOGGING INTERVAL IN SECONDS = %ld\n",ACQ.DUTYCYCLE);
	flogf(" DUTY CYCLE                  = %d percents\n", duty);
	flogf(" PRE-AMP GAIN SET [0,1,2,3]  = %d\n", ACQ.GAIN);
	//flogf(" NUM OF CHAN               = %d\n", FCHAN);
	flogf(" PRE_AMP WARMUP IN SECONDS   = %d\n", ACQ.WARMUP);
	flogf(" NUMBER OF CHAN   [1,2,4,8]  = %d\n", ACQ.NCHAN);
	//flogf(" SAMPLE RATE [HZ][250/1000/2000,5000]= %u\n", ACQ.SRATEHZ);
	flogf(" LOW PASS FILT CUT-OFF [HZ]  = %d\n", ACQ.LOPASS);
	flogf(" PRE-AMP PREWHITEN FILT SET  = %d\n", ACQ.PWFILT);
	flogf(" NO. OF PAST START-UPS[0]    = %d\n", ACQ.STARTUPS);
	flogf(" NO. OF MAX START-UPS [99]   = %d\n", ACQ.MAXSTRTS);
	flogf(" 1BYTE/2BYTE/16BIT[0,2,3]    = %d\n", ACQ.SAMPLES);

	flogf(" SAMPLING RATE [HZ]          = %ld\n", ACQ.SRATEHZ);
   //flogf(" LOG TO CON ?     [1]      = %d\n", ACQ.LOGTOCON);
	flogf(" EXPEDITION ID               = %.16s\n", ACQ.EXPID);
	flogf(" PROGRAM NAME                = %.16s\n", ACQ.PROGNAME);
	flogf(" FILE SIZE                   = %ld\n", BIRS.CFPPBSZ);
	flogf(" MAX # OF FILES ON C: DRIVE  = %ld\n", ACQ.MAXNUMFIL);
	flogf(" PR-AMP BOARD NAME & REV#    = %.10s\n", ACQ.PRAMPNAME);
	//flogf(" PR-AMP FREQ RESP TABLE    = %.4s\n", ACQ.TABLE);
	flogf(" DAQ BOARD NAME & BOARD#    	= %.10s\n", ACQ.DAQNAME);
	//flogf(" CLOCK BOARD NAME & REV#   = %.10s\n", ACQ.CLK);
	flogf(" HYDROPHONE SENSITIVY        = %d\n", ACQ.HYDROSENS);
	flogf(" HYDROPHONE SERIAL NUMBER    = %6s\n", ACQ.HYDROSRN);
		
	for (i = 0; ACQ.EXPID[i] && isprint(ACQ.EXPID[i]); i++)
	;	// scan through looking for any unprintables
	if (i && ACQ.EXPID[i] == '\0')	// got all the way through, so should be safe
	flogf("\n");

}
/******************************************************************************\
**	SetupHardware		Setup all of the hardware
**	
**	The CF1 does not need a lot of additional hardware setup when a program
**	starts since most of the initialization has been taken care of in the
**	BIOS and PicoDOS. Here we just tweak the hardware to minimize power.
**
**	1. 	Set the clock rate for an optimum balance between computation speed
**		and power drain. The 8MHz selected is just about right.
**
**	2.	Shut down the CF card in case one is installed. This saves about 350uA
**		when there's a card, though for minimum power drain remove the card and
**		save an addition 60uA. See item 5.
**	
**	3.	Convert all of the floating digital I/O lines to outputs and drive
**		them at the level last detected by reading them as inputs. Floating
**		inputs draw unpredictable amounts of current and make managing  
**		battery budgets impossible.
**	
**	4.	Shut down the RS-232 driver chip. This saves about 3mA. See item 5.
**	
**	5.	In the case of a fatal program exception, the pabort function takes
**		care of re-enabling the hardware necessary to display and log errors.
**
**	6.	Set the gain of pre-amp.	Not with H4 pre-amp (hard set) NOAA version by hm
**
**	7.  Synchronize 1pps/100pps interrupt counter to GPS reciver pulse, or if
**		the system has already started logging and this is the run after reboot,
**		1-pps/100pps counter clocks are synched to RTC instead.  NOAA version by hm
**
**	9/16/99  hm
\******************************************************************************/
void SetupHardware(void)
	{
	short		err = 0;
	//short    gain; //Not used for H4 pre-amp
	short		waitsFlash, waitsRAM, waitsCF;
	ushort	nsRAM, nsFlash, nsCF;
	short		nsBusAdj;
	char		strbuf[64];
	char		time_chr[42];
	struct tm 	t;
	time_t	nowsecs;
	short    LOGTOCON;
	
   uchar		mirrorpins[] = { 1, 16, 19, 21, 27, 30, 37, 0 };
	
	DBG( cprintf("SetupHardware\n");  cdrain();)
	
	TickleSWSR();	// another reprieve

//	Read in our control variables from VEE	
	CLEAR_OBJECT(ACQ);	// zero entire structure (see <cf1bios.h>
	ACQGetSettings();
	ACQ.ACQVersion = ACQ_VERSION_NUMBERS * 10;	// version.release * 10

   BIRGetSettings();

//
//	Setup runtime clock speed and optimize wait states
//
	CSSetSysAccessSpeeds(nsFlashStd, nsRAMStd, nsCFStd, WTMODE);
	TMGSetSpeed(SYSCLK);

//	Turn off hardware we're not actively using
//#ifndef DEBUG
//	CTMRun(false);				// turn off CTM6 module
//	CFEnable(false);			// turn off the CompactFlash card
//#endif
//
//	FORCE FLOATING I/O PINS TO OUTPUTS TO REDUCE CURRENT
//		Sample each I/O pin, and set it back to an output at the same
//		level. This works for everything except driven signals that
//		may change and keeps floaters from wasting power.
	PIOMirrorList(mirrorpins);
	//PIOSet(37); 	      // same
	//PIOSet(1);		      // DS, force as diag
	
// Take care the pre-amp	
	PIOClear(GAIN0);     //Make sure no voltage to the gain pins of the pre-amp
	PIOClear(GAIN1);
	PIOClear(PRAMPON);   //Make sure pre-amp power is off.  Save battery.
	
	// Identify the program and build
	LOGTOCON=1;
	Initflog(ACQ.LOGFILE, LOGTOCON ? true : false);

	nowsecs = time(NULL);		// get the time right now
	t = *localtime(&nowsecs);
	strftime(strbuf, sizeof(strbuf), "%m/%d/%Y  %H:%M:%S", &t);
	flogf("Start Time: %s [ctime: %lu, 0x%lX]\n", strbuf, nowsecs, nowsecs);

	flogf("-------------------------------------------------------------------\n");
	flogf("Program: %s,  Version: %3.1f,  Build: %s %s\n", 
		__FILE__, ACQ_VERSION_NUMBERS, __DATE__, __TIME__);
	CSGetSysAccessSpeeds(&nsFlash, &nsRAM, &nsCF, &nsBusAdj);
	flogf("System Parameters: CFx SN %05ld, PicoDOS %d.%02d, BIOS %d.%02d\n",
		BIOSGVT.CF1SerNum, BIOSGVT.PICOVersion, BIOSGVT.PICORelease,
		BIOSGVT.BIOSVersion, BIOSGVT.BIOSRelease);
	CSGetSysWaits(&waitsFlash, &waitsRAM, &waitsCF);	// auto-adjusted
	flogf("%ukHz nsF:%d nsR:%d nsC:%d adj:%d WF:%-2d WR:%-2d WC:%-2d SYPCR:%02d\n",
		TMGGetSpeed(), nsFlash, nsRAM, nsCF, nsBusAdj,
		waitsFlash, waitsRAM, waitsCF, * (uchar *) 0xFFFFFA21);

#ifdef DEBUG
	flogf("***** WARNING ! ACQUISITION DEBUG ENABLED *****\n");
	flogf("!!!!! DO NOT USE THIS FOR REAL DEPLOYMENT !!!!!\n");
#endif
	sprintf(strbuf, "%u", ACQ.STARTUPS + 1);
	VEEStoreStr(ACQ_STARTUPS_NAME, strbuf);
	
	// Bump and check startup count to keep from flailing the drive
	if (ACQ.STARTUPS >= ACQ.MAXSTRTS || ProgramFinished == true)//Rev 11/27/01 HM
	//if (ACQ.STARTUPS >= ACQ.MAXSTRTS )
				{
				WhatTime(time_chr);
				flogf("System reboot # %d occured at %s", ACQ.STARTUPS, time_chr);
				//SleepUntilWoken();
				exit;
				}
	
	if(ACQ.STARTUPS > 0) 
		{
		WhatTime(time_chr);
		flogf("System reboot # %d occured at %s", ACQ.STARTUPS, time_chr);putflush();
		}

	}	//____ SetupHardware() ____//

/***********************************************************************************
**	SleepUntilBaptisedAD()	
**
**  No Q-tech clock version.  Use ~1-Hz A/D interrupt to check the RTC time 
**  at every ~1-sec interval.  Not accurate but enough for marine mammal studies.
**
**	Do nothing until the specified wait period expires before starting the real logging 
**	Just check time time now and then.
**
**	05/24/2011 Modified
** 02/21/2011 Modified
** 11/02/99 Modified by hm
**	9/30/99	Modified by jg
**	9/05/99	hm NOAA  1-st version
**  2/21/2011 HM NOAA No Q-Tech clock version
***********************************************************************************/
void SleepUntilBaptisedAD(void)
	{
	char 	c, *p;
	ulong	WakeUpTime;		// computed end of delay in calendar time
	ulong	TimeNow = 0L;
	ulong OneHour = 3600L;  //change this to 3600L###
	short	i;
	char	strbuf[64];
	char	time_chr[42];
	short err=0;

	if(ACQ.STARTUPS==0)
		{
   	p = VEEFetchData(ACQ_SLEEP_NAME).str;
   	ACQ.SLEEP = atoi(p ? p : ACQ_SLEEP_DEFAULT);
   	DBG( cprintf("ACQ.SLEEP=%u (%s)\n", ACQ.SLEEP, p ? "vee" : "def");  cdrain();)
		flogf("\nThis system goes to sleep for %d hours, then starts logging\n", ACQ.SLEEP);//10/29/99 hm
	
		OnePPSCounter = RTCGetTime(NULL,NULL); //This line is here temp. Should be removed. For debug only NOAA HM
		WakeUpTime=OnePPSCounter  + (ulong)ACQ.SLEEP * OneHour; //DBug should be 3600L HM 
		//1-st entry, no reset occurred yet. Keep wake-up time to VEE 11/02/99 HM NOAA
		sprintf(strbuf, "%lu", WakeUpTime);
		VEEStoreStr(ACQ_WAKEUP_NAME, strbuf);	//Keep wake-up time in VEE	only once 
		//flogf("check 1\n");
		}
	if(ACQ.STARTUPS > 0) {
		WakeUpTime=ACQ.WAKEUP;	//Read startup time 11/02/99 HM
		}
	else{	
		//The following 5 lines for shutting down unnecessary funtions and enabling interrupt by RS232 NOAA hm 09/29/99
		// Install the interrupt handlers that will break us out by "break signal" from RS232.
		IEVInsertAsmFunct(Irq4RxISR, level4InterruptAutovector);
		IEVInsertAsmFunct(Irq4RxISR, spuriousInterrupt);
		
		OnePPSCounter = RTCGetTime(NULL,NULL); //This line is here temp. Should be removed. For debug only NOAA HM
		while(WakeUpTime > OnePPSCounter)	// outer loop
			{
			OnePPSCounter=WhatTime(time_chr);
			flogf("\nTime Now %s", time_chr);
			flogf("\nPre-acquisition sleep cycle. Send RS-232 BREAK to interrupt.");	
			flogf("\nLogging will start at %s (in %lu min and %lu sec)\n",
				GMTString(WakeUpTime), (WakeUpTime - OnePPSCounter) / 60L,
				(WakeUpTime-OnePPSCounter) % 60L);putflush();

			PITInit(SIM_PITR_DEF_IPL);

			SCITxWaitCompletion();		// let any pending UART data finish
			EIAForceOff(true);			// turn off the RS232 driver
			CFEnable(false);			   // turn off the CompactFlash card
			BIRShutdown(true);         // turn off BIA added 2/21/2012

			//PITSet100usPeriod(PITOff);	// disable timer
			PITSet51msPeriod(PITOff);	// disable timer
			PITRemoveChore(0);			// get rid of all current chores

			CTMRun(false);				// turn off CTM6 module

			OnePPSCounter = RTCGetTime(NULL,NULL);
			PITSet51msPeriod(PITRATE);		// start 1-Hz AD engine ...
			
			while(WakeUpTime > OnePPSCounter)	// inner loop
				{
				PinBus(IRQ4RXD);				// allow RS232 interrupts

				LPStopCSE(FullStop);	// VCO on (immed. wake) CPU off, SIMCLK is VCO clock
				OnePPSCounter = RTCGetTime(NULL, NULL);
				TickleSWSR();	// another reprieve
				
				// The following block is only executed if an operator sends a break, so
				// the "normal" inner loop is actually quite small
				if (! PinTestIsItBus(IRQ4RXD))	// we saw some RS-232 activity
					{
					EIAForceOff(false);			// turn on the RS232 driver
					CFEnable(true);				// turn on the CompactFlash card
					cprintf("\n<<<BREAK>>>\n");	// acknowledge so they let go
					cdrain();					// flush output
					for (i = 0; i < 50; i++, Delay1ms())// wait for break to stop and stay quit
						if (PinRead(IRQ4RXD) == 0)	// still active ?
							i = 0;					// yes, reset timeout
					ciflush();					    // flush any junk

					flogf("BREAK interrupted pre-acquisition sleep at %s\n", GMTString(OnePPSCounter));	// Record this!
					fflush(NULL);	
					QRchar("\nWhat next?(P=PicoDOS,S=Settings,A=Acquire now, ?=Status)",
						"%c", false, &c, "PSA?", true);
					//flogf("\nPassword accepted '%c' action requested\n", c);	// Record this too!
					if (c == 'S')
						{
						settings();
						BIOSReset();	// force clean restart
						}
					else if (c == 'P')
						BIOSResetToPicoDOS();
					else if (c == 'A')
						WakeUpTime = RTCGetTime(NULL,NULL);	// force termination
					//else if (c == '?')
					else if (c != -1)	//anything else
						DisplayParameters();
					break;	// to outer loop to reprompt, and resume low power
					}
				}
			}
			//flogf("check 2\n");
			
			PITSet51msPeriod(PITOff);	// stop 1-Hz sampling
			EIAForceOff(false);			// turn on the RS232 driver
			CFEnable(true);				// turn on the CompactFlash card
			TickleSWSR();	// another reprieve
			
			//
         //	RE-INITIALIZE THE IDE ADAPTER added 2/21/2012 HM
         //
		   TickleSWSR();	// another reprieve
         
     	   BIAPowerUp(true);
     	   //Mount D:
     	   if ((err ==DSDValidate('D' - 'A')) == 1){	// mounted ?

     	      err = DSDMountATADevice(BIAGetDriverName((BIADEV) BIRS.BIADEVICE),
		      BIAGetDriver((BIADEV) BIRS.BIADEVICE), 0, 'D' - 'A');
         }
         //Mount E:
		   TickleSWSR();	// another reprieve

     	   if ((err ==DSDValidate('E' - 'A')) == 1){	// mounted ?

     	      err = DSDMountATADevice(BIAGetDriverName((BIADEV) BIRS.BIADEVICE),
		      BIAGetDriver((BIADEV) BIRS.BIADEVICE), 0, 'E' - 'A');
         }

	      //BIRSetupNextVolume(false, false);//no slot change
		
			//WhatTime(time_chr);
			//flogf("\nResetting to start logging at %s", time_chr);

			RTCDelayMicroSeconds(100000);	// let it settle for 1000ms
			//BIOSReset();
		}	//flogf("\nStarting...\n");
		#ifndef DEBUG
			EIAForceOff(true);		// turn off the RS232 driver Moved here by hm NOAA
		#endif
		return;		// to start acquisition	

	} //SleepUntilBaptisedAD
/***********************************************************************************
**	LowPowerHibern()	
** Revised 10/13/2011
** 05/25/2011
** Store the next wake-up time in the file instead of VEE. (revision 10/13/2011)  
** Low power hybonation using the 1-Hz interrupt with RTC time check every sec.
** Hibernates for "ACQ.DUTYCYCLE-ACQ.ACTIVESEC".
** Haru Matsumoto
***********************************************************************************/
short LowPowerHibern(void)
	{
	static ulong	WakeUpTime;		   // computed end of delay in calendar time
	ulong	TimeNow = 0L;
	char	strbuf[64];
	char	time_chr[42];
   //ulong hib_sec;
   static short status=0;
   static nhiber;
   bool  lagged=false;
   //char *p;
   
   if(status==0){//here because first time after a normal startup or rebooted 
      //hib_sec=ACQ.DUTYCYCLE - ACQ.ACTIVESEC;
      //printf("hib_sec=%lu\n", hib_sec);
	   OnePPSCounter = RTCGetTime(NULL,NULL); //This line is here temp. Should be removed. For debug only NOAA HM
	   //WakeUpTime=OnePPSCounter  + hib_sec;   //set up a regular wake up time
	   if(nfile==0)nfile=1;
      nhiber = nfile-1;
	   WakeUpTime=ACQ.DUTYCYCLE*nhiber+ACQ.WAKEUP;//nfile is defined by GetNextFileName
      //flogf("LowPowerHyb...Next file=%lu\n", nfile);
	   //flogf("Status =0 nhiber=%u\n", nhiber);
	   if(ACQ.STARTUPS >= 1) //rebooted.  File may be small and short.
	      {
         //synchronize the file starting time
	      while(WakeUpTime<OnePPSCounter)
	      //Assume reboot happened during hybernation, and it passed the wakeup time
	         {
	         WakeUpTime +=ACQ.DUTYCYCLE;
	         nhiber++;
	         if(!lagged){
	            flogf("Logging cycle missed ");
	            lagged=true;
	            }
	         }
	         flogf("%d times\n", nhiber);
	      }
	   
	   status=1;  
	   }
	   else //no reboot and regular file increment with ACQ.FILECOUNT>=1
	   {
	   nhiber++;//increment once
	   WakeUpTime =ACQ.DUTYCYCLE*nhiber+ACQ.WAKEUP;
	   //flogf("Status =1 nhiber=%u\n", nhiber);
	   }
	
	sprintf(strbuf, "%lu", WakeUpTime);
	waketime_logf(WakeUpTime);                //Keep file number and wake-up time in file
	//VEEStoreStr(ACQ_WAKEUP_NAME, strbuf);	//Keep wake-up time in VEE	

	//The following 5 lines for shutting down unnecessary funtions and enabling interrupt by RS232 NOAA hm 09/29/99
	//Install the interrupt handlers that will break us out by "break signal" from RS232.
	//IEVInsertAsmFunct(Irq4RxISR, level4InterruptAutovector);
	//IEVInsertAsmFunct(Irq4RxISR, spuriousInterrupt);
	
	OnePPSCounter = RTCGetTime(NULL,NULL); //This line is here temp. Should be removed. For debug only NOAA HM
	while(WakeUpTime > OnePPSCounter)	// outer loop
		{
		OnePPSCounter=WhatTime(time_chr);
		flogf("\nTime Now %s", time_chr);
		flogf("\nLow-power hibernation before the next logging cycle begins.");	
		flogf("\nLogging will start at %s (in %lu min and %lu sec)\n",
				GMTString(WakeUpTime+ACQ.WARMUP), (WakeUpTime+ACQ.WARMUP - OnePPSCounter) / 60L,
				(WakeUpTime+ACQ.WARMUP-OnePPSCounter) % 60L);      
      cdrain();coflush();
      
		PITInit(SIM_PITR_DEF_IPL);
		SCITxWaitCompletion();		// let any pending UART data finish
		EIAForceOff(true);			// turn off the RS232 driver
		CFEnable(false);			   // turn off the CompactFlash card

		CTMRun(false);				   // turn off CTM6 module

		//OnePPSCounter = RTCGetTime(NULL,NULL);
		//Enable the IRQ5 (PBS) interrupt to stop acquisition
      IEVInsertCFunct(&ExtFinishPulseRuptHandler, level5InterruptAutovector);
      PinBus(IRQ5);					// enable PBM (IRQ5) interrupt button

		PITSet51msPeriod(PITRATE);	// start 1-Hz AD engine ...
		
		while(!ProgramFinished && WakeUpTime > OnePPSCounter)	// inner loop
			{
			LPStopCSE(FullStop);	// VCO on (immed. wake) CPU off, SIMCLK is VCO clock
			OnePPSCounter = RTCGetTime(NULL, NULL);
			TickleSWSR();	         // another reprieve

			
			// The following block is only executed if an operator sends a break, so
			// the "normal" inner loop is actually quite small
			if (ProgramFinished)	// we saw someone pushed PBM button
				{
				EIAForceOff(false);			// turn on the RS232 driver
				CFEnable(true);				// turn on the CompactFlash card
				cprintf("\n<<<BREAK AWAY FROM HIBERNATION MODE>>>\n");// acknowledge so they let go
				cdrain();					   // flush output
				ciflush();					   // flush any junk

				WakeUpTime = RTCGetTime(NULL,NULL);	// force termination
				break;	// to outer loop to reprompt, and resume low power
				}
			}
			if(ProgramFinished)break;
		}
		
		PITSet51msPeriod(PITOff);	// stop 1-Hz sampling
		EIAForceOff(false);			// turn on the RS232 driver
		CFEnable(true);				// turn on the CompactFlash card
		TickleSWSR();	// another reprieve

		//WhatTime(time_chr);
		//flogf("\nResetting to start logging at %s", time_chr);
		RTCDelayMicroSeconds(100000);	// let it settle for 100ms
		//BIOSReset();
		//flogf("\nStarting...\n");
	#ifndef DEBUG
		EIAForceOff(true);		// turn off the RS232 driver Moved here by hm NOAA
	#endif
	if(ProgramFinished)return -1;//stop acquisition
	else 	return 0;		// to start acquisition	

	} //LowPowerHibern()

/***********************************************************************************
**	LowPowerHibern_old_05252011()	
** 05/25/2011
** Store the next wake-up time in VEE.  
** Low power hybonation using the 1-Hz AD interrupt with RTC time check every sec.
** Hibernates for "ACQ.DUTYCYCLE-ACQ.ACTIVESEC".
** Haru Matsumoto
***********************************************************************************/
short LowPowerHibern_old_05252011(void)
	{
	static ulong	WakeUpTime;		   // computed end of delay in calendar time
	ulong	TimeNow = 0L;
	char	strbuf[64];
	char	time_chr[42];
   ulong hib_sec;
   static short status=0;
   char *p;
   
   if(status==0){//either first time or rebooted
      hib_sec=ACQ.DUTYCYCLE - ACQ.ACTIVESEC;
      //printf("hib_sec=%lu\n", hib_sec);
	   OnePPSCounter = RTCGetTime(NULL,NULL); //This line is here temp. Should be removed. For debug only NOAA HM
	   WakeUpTime=OnePPSCounter  + hib_sec;   //set up a regular wake up time
	   
	   if(ACQ.STARTUPS > 1) //rebooted
	      {
  	      p = VEEFetchData(ACQ_WAKEUP_NAME).str;
	      WakeUpTime = ACQ.DUTYCYCLE+atol(p ? p : ACQ_WAKEUP_DEFAULT);
	      }
	   
	   status=1;  
	   }
	   else
	   {
  	   p = VEEFetchData(ACQ_WAKEUP_NAME).str;
	   WakeUpTime =ACQ.DUTYCYCLE+atol(p ? p : ACQ_WAKEUP_DEFAULT);
	   }
	
	sprintf(strbuf, "%lu", WakeUpTime);
	VEEStoreStr(ACQ_WAKEUP_NAME, strbuf);	//Keep wake-up time in VEE	

	//The following 5 lines for shutting down unnecessary funtions and enabling interrupt by RS232 NOAA hm 09/29/99
	//Install the interrupt handlers that will break us out by "break signal" from RS232.
	IEVInsertAsmFunct(Irq4RxISR, level4InterruptAutovector);
	IEVInsertAsmFunct(Irq4RxISR, spuriousInterrupt);
	
	OnePPSCounter = RTCGetTime(NULL,NULL); //This line is here temp. Should be removed. For debug only NOAA HM
	while(WakeUpTime > OnePPSCounter)	// outer loop
		{
		OnePPSCounter=WhatTime(time_chr);
		flogf("\nTime Now %s", time_chr);
		flogf("\nLow-power hibernation before the next logging cycle begins.");	
		flogf("\nLogging will start at %s (in %lu min and %lu sec)\n",
			GMTString(WakeUpTime), (WakeUpTime - OnePPSCounter) / 60L,
						(WakeUpTime-OnePPSCounter) % 60L);putflush();

		PITInit(SIM_PITR_DEF_IPL);

		SCITxWaitCompletion();		// let any pending UART data finish
		EIAForceOff(true);			// turn off the RS232 driver
		CFEnable(false);			   // turn off the CompactFlash card

		CTMRun(false);				   // turn off CTM6 module

		//OnePPSCounter = RTCGetTime(NULL,NULL);
		//Enable the IRQ5 (PBS) interrupt to stop acquisition
      IEVInsertCFunct(&ExtFinishPulseRuptHandler, level5InterruptAutovector);
      PinBus(IRQ5);					// enable PBM (IRQ5) interrupt button

		PITSet51msPeriod(PITRATE);	// start 1-Hz AD engine ...
		
		while(!ProgramFinished && WakeUpTime > OnePPSCounter)	// inner loop
			{
			LPStopCSE(FullStop);	// VCO on (immed. wake) CPU off, SIMCLK is VCO clock
			OnePPSCounter = RTCGetTime(NULL, NULL);
			TickleSWSR();	         // another reprieve

			
			// The following block is only executed if an operator sends a break, so
			// the "normal" inner loop is actually quite small
			if (ProgramFinished)	// we saw someone pushed PBM button
				{
				EIAForceOff(false);			// turn on the RS232 driver
				CFEnable(true);				// turn on the CompactFlash card
				cprintf("\n<<<BREAK AWAY FROM HIBERNATION MODE>>>\n");// acknowledge so they let go
				cdrain();					   // flush output
				ciflush();					   // flush any junk

				WakeUpTime = RTCGetTime(NULL,NULL);	// force termination
				break;	// to outer loop to reprompt, and resume low power
				}
			}
		}
		
		PITSet51msPeriod(PITOff);	// stop 1-Hz sampling
		EIAForceOff(false);			// turn on the RS232 driver
		CFEnable(true);				// turn on the CompactFlash card
		TickleSWSR();	// another reprieve

		//WhatTime(time_chr);
		//flogf("\nResetting to start logging at %s", time_chr);

		RTCDelayMicroSeconds(100000);	// let it settle for 100ms
		//BIOSReset();
		//flogf("\nStarting...\n");
	#ifndef DEBUG
		EIAForceOff(true);		// turn off the RS232 driver Moved here by hm NOAA
	#endif
	if(ProgramFinished)return -1;//stop acquisition
	else 	return 0;		// to start acquisition	

	} //LowPowerHibern()
	
/*************************************************************************************
**	GMTString		Convert localtime to GMT string in the form YY DDD:HH:MM:SS
**
**  NOAA, HM
**  9/02/99
**************************************************************************************/
char *GMTString(time_t tp)
	{
	static char	s[16];	// buffer for "YY DDD:HH:MM:SS" + '\0'
	strftime(s, sizeof(s), "%y %j:%H:%M:%S", localtime(&tp));
	return s;
	}	//____ GMTString() ____//

/******************************************************************************\
**	UpdateGain
**  
**   rev 5/26/2011 (HM)
**   Put a safe-guard 12/26/01 HM
**	  						H3DAQ  SMDAQ/CSACDAQ
**   GAIN1    			32			32
**   GAIN0    			33			31
**	  H3DAQ	  			=0 sets the Gain bit on
**	  CSADDAQ/SMDAQ 	=1 sets the Gain bit on					
\******************************************************************************/
void UpdateGain(void)
	{
		if(DAQ < 2){//Old H3DAQ type
			PIOSet(GAIN0);       //gain = 0dB
			PIOSet(GAIN1);
			if(ACQ.GAIN == 1){   //+6dB
			PIOClear(GAIN0);	
			}
			if(ACQ.GAIN == 2){   //+12dB
			PIOClear(GAIN1);
			}
			if(ACQ.GAIN == 3){   //+18dB
			PIOClear(GAIN0);
			PIOClear(GAIN1);
			}
		}else if(DAQ >=2){//SMDAQ and CSACDAQ type	
			
			PIOClear(GAIN0);       //gain = 0dB
			PIOClear(GAIN1);
			if(ACQ.GAIN == 1){   //+6dB
			PIOSet(GAIN0);	
			}
			if(ACQ.GAIN == 2){   //+12dB
			PIOSet(GAIN1);
			}
			if(ACQ.GAIN == 3){   //+18dB
			PIOSet(GAIN0);
			PIOSet(GAIN1);
			}			
		}	
			//flogf("GPIN0 %d, GPIN1 %d\n",PIOTestAssertSet(GPIN0),PIOTestAssertSet(GPIN1));

	}	//____ UpdateGain() ____//

/******************************************************************************\
**	TestGain(gain)
**   rev 5/27/2011 (HM)
**   Put a safe-guard 12/26/01 HM
**   GAIN1           TPU11    32
**   GAIN0           TPU12    33
\******************************************************************************/
void TestGain(short gain)
	{
		if(DAQ < 2){//Old H3DAQ type
			PIOSet(GAIN0);       //gain = 0dB
			PIOSet(GAIN1);
			if(gain == 1){   //+6dB
			PIOClear(GAIN0);	
			}
			if(gain == 2){   //+12dB
			PIOClear(GAIN1);
			}
			if(gain == 3){   //+18dB
			PIOClear(GAIN0);
			PIOClear(GAIN1);
			}
		}else if(DAQ >=2){//SMDAQ and CSACDAQ type	
			
			PIOClear(GAIN0);       //gain = 0dB
			PIOClear(GAIN1);
			if(gain == 1){   //+6dB
			PIOSet(GAIN0);	
			}
			if(gain == 2){   //+12dB
			PIOSet(GAIN1);
			}
			if(gain == 3){   //+18dB
			PIOSet(GAIN0);
			PIOSet(GAIN1);
			}			
		}	

	}	//____ TestGain() ____//
/************************************************************************
** DiskCheck
** Check the disk if there is enough free space to write
\************************************************************************/

short DiskCheck()
   {
   long  freeSpacekB;   
	float fnumfiles;
	long  numfiles;
 
	freeSpacekB  = DSDFreeSectors( BIRS.CFDOSDRV[0]-'A')/2;  //check D: drive
	if(freeSpacekB < BIRS.CFPPBSZ/KB_SIZE+100L || freeSpacekB <BIRS.MINFREEKB){
	      flogf("This disk is full. Hunt for the next one.\n");
			return -1;
			}
			
	fnumfiles = (float)(freeSpacekB-BIRS.MINFREEKB)/((float)BIRS.CFPPBSZ/1024. +1.);
	//ACQ.MAXNUMFIL=fnumfiles*1000L - 1; // don't update ACQ.MAXNUMFIL
	numfiles=(long)fnumfiles;

	flogf("Free disk space after this file   = %lu kB\n",freeSpacekB-BIRS.MINFREEKB);
	flogf("Remaining file capacity           = %ld files\n", numfiles);cdrain();coflush();
   BIRS.BIRStartFreeKB=freeSpacekB;
	
	if(ACQ.MAXNUMFIL<1L){
	   flogf("Probably too many small files or disk has no space left\n");	
      return -1;
      }
      
	return 0;
	}//DiskCheck()
	
//*********************************************************************************
// LowPowerDelay
// Use LPStopCSE(FullStop) to delay for ## seconds.
//**********************************************************************************
void LowPowerDelay(ulong delay)
{
   ulong WakeUpTime;
   
   OnePPSCounter=RTCGetTime(NULL,NULL);

	WakeUpTime=OnePPSCounter+delay;
	PITSet51msPeriod(PITRATE);		// start 1-Hz AD engine ...
	
	while(WakeUpTime > OnePPSCounter)	// inner loop
		{
		//PinBus(IRQ4RXD);				// allow RS232 interrupts
		LPStopCSE(FullStop);	   // VCO on (immed. wake) CPU off, SIMCLK is VCO clock

		OnePPSCounter = RTCGetTime(NULL, NULL);
		TickleSWSR();	// another reprieve
		
		// The following block is only executed if an operator sends a break, so
		// the "normal" inner loop is actually quite small
		}
	
	PITSet51msPeriod(PITOff);	// stop 1-Hz sampling
}

//*********************************************************************************
// Turn_Off_Preamp()
//*********************************************************************************
void Turn_Off_Preamp(void)
{	
	PIOClear(GAIN0);           //Bring the gain pins all zeros to be safe
	PIOClear(GAIN1);
	PIOClear(PRAMPON);         //Safe to turn off pre-amp power and save battery
}

/******************************************************************************\
**	CheckBIRStartups		//  Put a safe-guard 12/26/01 HM
\******************************************************************************/
void CheckBIRStartups(const BIRData *bp)
	{
	short	no_of_startups;
	char what;
	char *p;
	
	p = VEEFetchData(ACQ_STARTUPS_NAME).str;
	no_of_startups = atoi(p ? p : ACQ_STARTUPS_DEFAULT);

		if(bp->CURBIA != 0 && no_of_startups <=1)
		{
			flogf("\n1-st BigIDEA (BIR.CURBIA) supposed to be zero. It is %u. Are you sure?(y/n) ",bp->CURBIA);
			ciflush();
			what = getch();
			flogf("%c\n",what);
			if(what == 'n' || what == 'N') { 
			flogf("\nAborting the program!\n");
			abort();
			}
		}

	}	//____ CheckBIRStartups() ____//

//From here BIR-CF related routines********************************************

/******************************************************************************\
**	ACQCollabBIR	Called by BIR at key status change points
**	
**	THIS SHOULD BE CUSTOMIZED TO YOUR APPLICATIONS REQUIREMENTS
**	
\******************************************************************************/
void *ACQCollabBIR(BIRCollabReq req, const BIRData *bp, void *more)
	{
	#pragma unused(more)
	switch (req)
		{
		case	turningOn : 		// just before turning on hard drive
			//DiskLEDControl(bp->CURBIA);	
			//if (ACQ.TRMCTLPIN)		// are we to enable data line termination (active low)
			//	PIOClear(ACQ.TRMCTLPIN);	// but must come after detecting drive ready
			break;

		case	turningOff : 		// just before turning off hard drive
			//ClearAllLED();
			//if (ACQ.TRMCTLPIN)
				//PIOSet(ACQ.TRMCTLPIN);		// disable data line termination (active low)
			break;

		case	startingWrite : 	// just before opening the hd write file
			//UpdateGains();
			break;

		case	endedWrite : 		// just after closing the hd write file
			break;

		case	gotSettings : 		// just after reading the BIR VEE settings
			CheckBIRStartups(bp);
			break;

		case	reqUserHdr : 		// requesting user header info before file write
			//return ACQGetHeader();
         break;
         
		case	reqTimeStr :		// requesting time string for flogf and debug
			//return ACQGetTimeStr();
         break;
		}
	return 0;

	}	//____ ACQCollabBIR() ____//

//******************************************************************************
// Open a small test.dat file to test logging before the real logging begins.
// Test AD and file system.
//******************************************************************************
FILE *Open_Test_File(void)
{	
static char	*fnew="C:TEST.DAT";				   //test file
//char	strbuf[7];
//	sprintf(&fnew[2], "%08lu.DAT", (long) filecount);//name a new detection file
//	sprintf(strbuf, "%u", ++filecount);
//	VEEStoreStr(ACQ_FILECOUNT_NAME, strbuf);
//   fnew=GetNextFileName(true, true);
	fp = (FILE *) &AcqFileHandle;
	if ((AcqFileHandle = open(fnew, O_RDWR | O_CREAT | O_TRUNC)) <= 0)
		{
		fp = 0;
		flogf("Couldn't open data file!\n");putflush();
		return fp;
		}
	flogf("%s is created\n",fnew);putflush();
	return fp;
}//Open_Test_File
/******************************************************************************\
**	Test_Acquire		// Minimalist SPDQ Recorder
\******************************************************************************/
short Test_Acquire(void)
	{
	bool	   uni = true;		// true for unipolar, false for bipolar
	bool	   sgl = true;		// true for single-ended, false for differential
	//char 	*p;
	long	   i;             // A/D sample counter index
	//long	freeSpacekB;
	static   short testgain=0;
	short	   returncode = 0;
	char	   *failmsg = "None";
	bool	   pz = false;			// becomes true when PicoZOOM is setup
	ushort	entryclock = TMGGetSpeed();
	char 	   combobuf[sizeof(BIRS) + BIR_USER_HDR_SIZE];
	long     NSamplePerFileHeader;
   char     time_chr[22];

	static spdqcb	spdqr = 
		{
		  SPDQ_ACQ_CLOCK	// sys_clk
		, 0					// agg_sps;
		, 8					// chans;			
		, 1					// inp_end sgl;			
		, AD_REF_SHDN_PIN	// adc_shdn;		
		, 3					// adc_pcs;		
		, 2					// irq_pcs;		
		, 41				   // irq_pin IRQ7;		
		, 25				   // pwm_pin;			
		, 0					// con_dbg;		
		, 1					// p1_diag_lwr;		
		, 1					// p42_diag_up;		
		, 262144			   // ring_buf_size;	
		, WRITE_SIZE		// req_blk_size;	
		, WRITE_SIZE		// max_req_size;	
		, 0					// pre_trig_size
		};				

	fp    = 0;		// clear at start so we don't attempt fclose
	errno = 0;

	spdqr.agg_sps = ACQ.SRATEHZ*ACQ.NCHAN; //4/20/2013 HM
	spdqr.chans   = ACQ.NCHAN;					//4/20/2013 HM
	
   //New file name
	//p = VEEFetchData(ACQ_FILECOUNT_NAME).str;
	//ACQ.FILECOUNT= atoi(p ? p : ACQ_FILECOUNT_DEFAULT);
	//Replaced by GetNextFileName.  FILECOUNT is no longer stored in VEE.
	
	if(DiskCheck()==-1){// this takes a couple of sec
	      //flogf("Disk is full.  Aborting the program.\n");
			returncode = -1;
			failmsg = "Disk is full";
			goto exitTestAcquire;
			}	

	pz = PZCacheSetup('C'-'A', calloc, free);
	if (! pz)
		{
		returncode = -2;
		failmsg = "Can't open PicoZOOM";
		goto exitTestAcquire;
		}

	errno = 0;
	fp = Open_Test_File ();
	if (fp == 0)
		{
		returncode = errno;
		failmsg = "Can't open data file";
		goto exitTestAcquire;
		}

	WhatTime(time_chr);
	flogf("Test logging started %s\n", time_chr);cdrain();coflush();
	strcpy(ACQ.TIME_GMT, time_chr);

	if (spdq_open(&spdqr) != 0)
		{
		returncode = spdqr.errcode;
		failmsg = "Can't open SPDQ. Possibly not a right DAQ board (Pins 15-40 shunt).";
		goto exitTestAcquire;
		}

	// Alternative to Write_Header function
	CFxBIRUpdateHeader();
	memcpy(combobuf, &BIRS, BIR_SYS_HDR_SIZE);
	//memcpy(&combobuf[sizeof(BIRS)], ((char *) &ACQ) + sizeof(BIRS), BIR_USER_HDR_SIZE);
	memcpy(&combobuf[sizeof(BIRS)], &ACQ, BIR_USER_HDR_SIZE);
	//memcpy(&combobuf[BIR_SYS_HDR_SIZE], &ACQ, BIR_USER_HDR_SIZE);
	//flogf("BIRS=%ld\n", sizeof(BIRS));
	if (! spdq_write_header(&spdqr, combobuf, sizeof(combobuf)))
		{
		returncode = errno;
		failmsg = "errno writing header";
		goto exitTestAcquire;
		}

	CTMRun(false);				   // turn off CTM6 module
	SCITxWaitCompletion();		// let any pending UART data finish
	EIAForceOff(true);			// turn off the RS232 driver

	i=0L;

	//Enable the IRQ5 (PBS) interrupt to stop acquisition
	IEVInsertCFunct(&ExtFinishPulseRuptHandler, level5InterruptAutovector);
	PinBus(IRQ5);					// enable PBM (IRQ5) interrupt button

	// START
	spdq_activate();

	// TEST_ACQUISITION LOOP
	NSamplePerFileHeader=NTestSamplePerFile+128L;
	while (i < NSamplePerFileHeader && spdq_is_active())
		{
		short	idle_t;
		ulong	towrite;
		if (spdq_request_rb_contig_block(&spdqr))
			{
			errno = 0;
			towrite = 2 * (NSamplePerFileHeader - i);
			if (towrite >= spdqr.contig_bytes)
				 towrite = spdqr.contig_bytes;
				 
         //Test routine to check if ADC outputs are zeros
		   if(i>128L && * (ulong *) spdqr.rbcp == 0x00000000) // look for mystery
		      {
		      spdq_idle();
            failmsg = "0x00000000 data detected";
            EIAForceOff(false);         // turn the RS232 driver back on
            cprintf("\n###%s: spdqr.contig_bytes=%lu\n", failmsg,spdqr.contig_bytes);
            cstructdump(spdqr);
            cstructdump(QSM);
            cstructdump(BIRS);
            cstructdump(ACQ);
            cprintf("\nHit '.' to exit.  You must power off/on CFx.\n");
            if (cgetc() == '.')
               {
               returncode = 'F0';
               goto exitTestAcquire;
               }
            spdq_activate(); // resume for oscilloscope testing
            }
            // END: Diagnose 0000 files 15Jul2011--JHG

			if (write(fp->handle, spdqr.rbcp, towrite) != towrite)
				{
				returncode = errno;
				failmsg = "write failed during acquisition";
				goto exitTestAcquire;
				}
			i += towrite / 2;
			spdq_release_rb_contig_block(&spdqr);
			continue;	// 01Jul2011-JHG	skip the next sleep cycle 
			}
		
		if (spdqr.errcode != 0)
			{
			returncode = spdqr.errcode;
			failmsg = "SPDQ failed during acquisition";
			goto exitTestAcquire;
			}

		idle_t = spdqr.agg_sps / 10;	// low power loop for 100ms before checking for data or break
		while (idle_t-- && spdq_is_active())
			LPStopCSE(CPUStop);	// VCO on (immed. wake) CPU on, SIMCLK is VCO clock
			
		}

	if (! spdq_is_active())
		{
		returncode = spdqRBOverflow;
		failmsg = "spdqRBOverflow";
		}

	spdq_idle();
//	01Jul2011-JHG	skip flush, we've already recorded what we came for
//	while (spdq_flush_ring_buffer(&spdqr))
//		{
//		write(fp->handle, spdqr.rbcp, spdqr.contig_bytes);
//		spdq_release_rb_contig_block(&spdqr);
//		}


exitTestAcquire:

	TMGSetSpeed(entryclock);

	// CONCLUDE
	PinIO(IRQ5);

	// CLEANUP
	spdq_close(&spdqr);

	if(DAQ==1){
		PIOSet(GAIN0);           //Bring the gain pins all zeros to be safe
		PIOSet(GAIN1);
		}else if(DAQ>=2){
		PIOClear(GAIN0);
		PIOClear(GAIN1);
		}
	PIOClear(PRAMPON);       //Safe to turn off pre-amp power and save battery

	if (fp)
		{
		close(fp->handle);
		fp->handle = 0;
		fp = 0;
		}
	PZCacheRelease('C'-'A');

	WhatTime(time_chr);
	EIAForceOff(false);			// turn the RS232 driver back on

	flogf("Test File ended:         %s \n",time_chr);cdrain();coflush();
   //cprintf("Test logging samples %ld\n", NTestSamplePerFile);
	

	if (returncode)
		{
		if(returncode!=-1)
		flogf("\n###Test_Acquire failed: returncode = %d, failmsg = %s\n", returncode, failmsg);
		if(returncode == 14 && ACQ.STARTUPS<=ACQ.MAXSTRTS)BIOSReset();
		return -1;
		}

	return 0;

	}	//____ Test_Acquire ____//
	//************************************************************************************
// TestAcquisition
// Test logging for a short period (TestDuration=10sec) to make sure A/D is working.
// Occasionally A/D freezes and the outputs become all zeros.  Only way to correct
// this is to power off and off the CFx.
// H. Matsumoto 8/01/2011
//************************************************************************************
short Test_Acquisition()
{
   flogf("Testing data acquisition for the next %ld seconds...\n", TestDuration);
   PIOSet(PRAMPON);
   LowPowerDelay(2L);
   UpdateGain();
   LowPowerDelay(1L);
   if(Test_Acquire()==-1){
      flogf("Test acquisition failed\n");
      ProgramFinished=true;
      return -1;
      }
   else{
      flogf("Test logging success! Proceeding ...\n");
   	execstr("dir");
      }
   cdrain();coflush();
   return 0;
}
/******************************************************************************\
** waketime_logf
**
** Appends wakeup time in sec since Jan 1, 1970.  H. Matsumoto (7/06/2011)
**	Based on spiiidaq_logf		// Insert experiment status in log file
**	
**	Appends text to c:\spiiidaq.log using standard printf formatting
**
**	If rtp is non-null, it's assumed to contain valid time and begins a new
**	line containing the formatted time and global spdq settings string
**	
**	If both rtp and format are null, the log file is closed.
\******************************************************************************/
short waketime_logf(ulong SecondsSince1970)
	{
	short			   result = 0, error = 0;
	static FILE		*logfp = 0;
	
	RTCtm	         *rtc_time;				//CFX's real time clock

	rtc_time = RTClocaltime (&SecondsSince1970);
	sprintf(time_chr,"%.3d %.3d:%.2d:%.2d:%.2d",
	rtc_time->tm_year, rtc_time->tm_yday + 1,\
	rtc_time->tm_hour,rtc_time->tm_min,rtc_time->tm_sec);

	if (logfp == 0)
		//if ((logfp = fopen("c:\\spiiidaq\\spiiidaq.log", "a")) == 0)
		if ((logfp = fopen("c:\\waketime.log", "a")) == 0)
			return -1;
   if(SecondsSince1970)
      {
	   result = fprintf(logfp, "#T# %u %lu %s\r\n", nfile, SecondsSince1970, time_chr);
	   }
	else if (logfp == 0)	// NULL is close request
		{
		fclose(logfp);
		logfp = 0;
		return 0;
		}
	
	fcommit(logfp);

   fflush(stdout);	// comment this out to disable console display
	return result;
 	
	}	//____ waketime_logf() ____//






