/******************************************************************************\
** QUEContMain3_3.c (8/25/06)
** AD data collected in the same way as 3_2 version. Only the last file to be
** transmitte is limited by QUE.BLKLNGTH*64, but not the rest.  The rest of
** of the stored files in c: are in the size of QUE.CFPPBSZ, which could be huge.
** 
** QUEContMain3_2.c				Persistor CF2 program
**
**  Collect AD data through pingpong buffer operation. File sizes are same.
**	Power condumption is 2mA, which is less power than QUEContMain3_2 (6mA).
**  The file size transmitted by IRIDIUM is limited by QUE.BLKLNGTHx64.
**
**  File size = CFPPBSZ+256.  256 is the header=190 and trailer=66 size.
**  CFPPBSZ +256 <= BLKLNGTH * 64 (i.e., BLKGLNGTH=1024, CFPPBSZ=65280.
**  To save power on PROV side, enable MAX3222 and clear pin 34. 
**		1. Clear pin 29 and set pin 30 (enable MAX3222)
**		2. Clear pin 34 to bring PROV RX logic to "zero."
**	If you don't do this, PROV uses about 50mA.
**	Controls PROVOR float and NAL IRIDIUM/GPS modem.  
**  Remove JP2 in order to use pin 35 and 37 DIO.
**
**	To turn power ON, remove the magnet outside the pressure housing
**	1. Turn ON Iridium/GPS->Check pressure->Check enough satellites->Set RTC clock
**		->Check depth->Call landstation->Send message
**	2. Dive->Check if target depth->Make sure  
**	Succsessfull? Y/N->(Y) Confirmation Ready to deploy
**	Remove PC serial connection->Seal connector with pig tail->Launch
**	Within 120 seconds start dive
**	
**	30 days later
**	Is is surface? ->(Y) Power ON GPS/IRIDIUM->GPS lock ON->GPS positioning
**	Check if next dive is <2000dB->Make Iridium call->Send data->GPS position
**	->Report drift->Receive instructions->Check again if <2000dB -> 
**	(Y) Go dive to bottom (N) Park at 1500dB for xx days.
**
**	xx days later
**	Come up to surface->GPS->Make Iridium call->Transmit data
**	->Receive instruction
**	
**	Rudic  phone QUE.LANDPHNUM=0088160000519
**  PMEL  phone  QUE.LANDPHNUM=0012065264495
*****************************************************************************
**	PIN FUNCTION				DESCRIPTION
**	41	IRQ2					USED FOR PROVOR COM INTERRUPUT
**	50	IRQ3/CTS				OPEN 
**	43	IRQ4/RXD				PC COM'S INTERRUPT	
**	39	IRQ5 					Any Program Interrupt (e.g., PBM SW)
**	40	IRQ7					Open
**
**	22	IRID or GPS				TPU1 ON=GPS, OFF=IRIDIUM
**	23	GAIN0 					TPU2 Pre-amp gain
**	24	GAIN1 					TPU3 Pre-amp gain 
**	25	IRIDIUM/GPS PWR SW 		TPU4
**	26	PROVOR COM SW 			TPU5
**	27	PROVOR RESET			TPU6
**	28	PROVOR INTERRUPT ENABLE	TPU7
**	29	AUX1&2 TX ENABLE		TPU8
**	30	AUX1&2 RX ENABLE		TPU9
**	31	IRID/GPS RXOUT			TPU10 AUX2
**	32	IRID/GPS TXIN			TPU11 AUX2
**	33	PROV RXOUT				TPU12 AUX1
**	34	PROV TXIN				TPU13 AUX1
**	35	OPEN					TPU14
**	37	OPEN					TPU15
**
**************************************************************************************
**	WARNING!!!
**  Connect Pin 2 of JP1 to VLIN (Pin 11 3.3V) to activate the LT1461 and ADS8344.
**	You need to activate A/D IC of the CF2 recipe card in order to digitize the signal.
**************************************************************************************
** PROVOR PHASES
** PHASE DESCRIPTION	   	   COM MODE
**  0	PRE-DEPLOYMENT TESTING	2
**	1	SURFACE					2			
**	2	SURFACE->DIVE			1
**	3	DIVE					1
**	4	PARKING					2
**  5	GROUNDING				2
**	6	DRIFT-ASCENT			1
**	7	DRIFT-DESCENT			1
**	8	ASCENT					1 
**	9	EMERGENCY ASCENT		1 (TO THE SURFACE ALL THE WAY)
*****************************************************************************
\******************************************************************************/

#define	QUE_VERSION_NUMBERS	1.0	// keep this up to date!!! 
//#define DEBUG
#ifdef DEBUG
  	#define	DBG(X)	X	// template:	DBG( cprintf("\n"); )
	#pragma message	("!!! "__FILE__ ": Don't ship with DEBUG compile flag set!")
#else
  #define	DBG(X)		// nothing
#endif

#include	<cfxbios.h>		// Persistor BIOS and I/O Definitions
#include	<cfxpico.h>		// Persistor PicoDOS Definitions
#include	<cfxad.h>		// Generic SPI A-D QPB Driver for CFx
// Place non-PicoDAQ A-D definitions here - before including <ADExamples.h>
#include	<ADExamples.h>	// Common definitions for the A-D Examples
#include	<CFxRecorder.h>

#include	<FloatSettings1.h>
#include	<assert.h>
#include	<ctype.h>
#include	<errno.h>
#include	<float.h>
#include	<limits.h>
#include	<locale.h>
#include	<math.h>
#include	<setjmp.h>
#include	<signal.h>
#include	<stdarg.h>
#include	<stddef.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<time.h>

// WDT Watch Dog Timer definition
short	CustomSYPCR = WDT419s | HaltMonEnable | BusMonEnable | BMT32;

#include	<dirent.h>		// PicoDOS POSIX-like Directory Access Defines
#include	<dosdrive.h>	// PicoDOS DOS Drive and Directory Definitions
#include	<fcntl.h>		// PicoDOS POSIX-like File Access Definitions
#include	<stat.h>		// PicoDOS POSIX-like File Status Definitions
#include	<termios.h>		// PicoDOS POSIX-like Terminal I/O Definitions
#include	<unistd.h>		// PicoDOS POSIX-like UNIX Function Definitions
#include	<ADS8345.h>		// ADS8344/8345 SPI A-D QPB Driver for CF1 

#define	SYSCLK		8000	// choose: 160 to 32000 (kHz)//NOAA debug 11/29/99
#define	WTMODE		nsStdSmallBusAdj	// choose: nsMotoSpecAdj or nsStdSmallBusAdj
//Iridrium data TX related
#define	MAX_RESENT          4		//Max resent try per connection
#define	MinSQ				3 		//minimum signal quality
//#define maxfile				5		//debug
		
//QUE struct related 
#define PLTID_LENMAX		4		//System ID length
#define	LAT_LENMAX			10		//Latitude length
#define	LONG_LENMAX			12		//Longitude max
#define	GMT_LENMAX			46		//GMT char length
#define EXPID_LENMAX		2		//EXP ID, usually year
#define	PROGNAME_LENMAX 	12		//Prog file name
#define PROJID_LENMAX		4
#define	BATHYF_NAME_MAX		14		//Bathy file name
#define LANDPHNUM_LENMAX 	14		//Land station phone number
#define LOGF_NAME_MAX		14		//Max filename length for log file
#define	ZOOM

typedef struct
	{
// ----- Total 190 bytes header
	char	PLTFRMID[PLTID_LENMAX];		//Platform ID (e.g., Q001)
	char	LATITUDE[LAT_LENMAX];		//Latitude in degrees   N45:02.356=N45deg 02.356min
	char	LONGITUDE[LONG_LENMAX];		//Longitude in degrees W128:34.872=W128 deg 34.872min
	ulong	RTCTIME;					//RTC time in sec since 1/1/1970 when synched to GPS
	short	RTCMSEC;					//RTC ms when synched to GPS
	ulong	GPSTIME;					//GPS time in sec when RTC was synched to GPS
	short	GPSMSEC;
	char	EXPID[EXPID_LENMAX];		//Exp ID   2
	char	PROGNAME[PROGNAME_LENMAX];	//Program name  12
	char	PROJID[PROJID_LENMAX];		//Project ID 4
	ushort	FILECOUNT;					//Data File count
	char 	LASTTRNS;					//1=last trans successful; 0=fail tx
	char	INGRID;						//Whether OUE phone is in grid 1=in grid, =0 out of range
	short	ESTMDEPTH;					//Estimated bottom depth from bathy 
	short	DRIFTLAT;					//Drift in seconds in latitude
	short	DRIFTLON;					//Drift in seconds in longitude
	char 	LASTGPS;					//Last GPS fix st`atus 1=OK,0=Fail
	uchar	STARTUPS;					//Number of program resets we've seen
	uchar	NUMOFSAT;					//Number of satellites
	uchar	NUMOFGPS;					//Number of GPS fetch tries
	uchar 	GOBOTTOM;					//1 = go to bottom (bottom is <max depth),= 0 stay surface
	uchar	GOPARK;						//1 = go parking depth; =0 go bottom or stay surface 
	short	MAXDRFTCL;					//Max number of drift calls

	short	PARKDEPTH;					//Park depth in meter.
	char	BATHYF[BATHYF_NAME_MAX];	//Bathy file name in C: dirve 14
	char	LANDPHNUM[LANDPHNUM_LENMAX];//Land station phone number 14
	short	GPSWARM;					//GPS warm up duration in seconds 80sec
	short	GOTODIVE;					//Whether ready to dive 1=OK to dive depth 0=NO, -1=Go to parking depth
	short	MAXCALLS;					//Max number float dial up per surface before give up
	short	NUMDRFTCL;					//Current number calls float made after the mission	
	short	SECDRFTCL;					//How many seconds apart float call after mission completed
	short	SURFSAMPL;					//Sampling period in minutes while at surface 
	short	PARKSAMPL;					//Sampling period in minutes during park
	char	PROVTERM;					//=1 Enables PROVOR comm
	char	IRIDTERM;					//=1 Enables IRID/GPS comm
	char	LOGONLY;					//Debug parameter. Allows to log without Provor or IRID/GPS enabled 
	char 	DUMMY;
	short	MAXDEPTH;					//Max dive depth in dBar (2000dBar max= 1917m in sea water)
	char	LOGFILE[LOGF_NAME_MAX];		//Filename for event logging 14 chars 
	uchar	MAXSTRTS;					//Maximum program resets allowed (typically up to 255)
	uchar	NUMOFDIVE;					//Number of past dives
	uchar	MAXNUMDIV;					//Maximum number of dives
	uchar	IRIDWARM;					//How many seconds allowed for IRIDIUM to warm up
	uchar	STATUS;						//Init=0,Surf=1,SF_dive=2,Dive=3,4=Park,Grnd=5,Ascnt=8;Emrgcy=9,	
	uchar	GAIN;						//additional pre-amp gain 0 to 3 with 6dB inc
	uchar	GOMONITOR;					//1 = monitor, 0 = not to monitor sound
	uchar	SAMPLES;					//sample size and type 0=8 bit, 2=12bit(2 byte), 3=16 bit 
	short	SRATEHZ;					//sample rate in hertz
	ushort	PREPMIN;					//Duration in min required to prepare before deployment
	long	CFPPBSZ;					//size of CompactFlash buffer, typ. 40MB
	long	BLKLNGTH;					//size of data RAM PP buffer, typ. 16 to 64KB
	long	MINFREEKB;					//minimum free space until switch to next drive
	short	FILENUM;
	short 	MAXFILE;
	long	RAMPPBSZ;
	char	JUNK[20];
	}	QUEData;

//static	BIRData	BIRS;	// runtime settings filled in at startup 
BIRData	BIRS;				// runtime settings filled in at startup 
QUEData	QUE;			    // runtime settings filled in at startup 
FILE	*fp;
long	RAMPPBSZ_DIV2;	
long	filelength;
void	*RAMppb;				// dynamically allocated RAM ping-pong buffer (big)
void	*ppbuf;
char	*RAMPingPongBuf;		// ping-pong buffer, never directly accessed

#define	LOGDRIVE	('C'-'A')
enum {NO_ERROR, MEM_ERROR_PINGPONG, OPEN_ERROR_PINGPONG};
char	drivestr[3] = { LOGDRIVE + 'A', ':', '\0' };

// GLOBALS
bool 	debug    = true;
bool	SatTerm  = false;	//debug dummy: true=Sat is ON
bool	ProvTerm = false;   //debug dummy: true=Prov COM ON
bool	LogOnly  = false;
ulong 	TimingRuptCount;  
bool 	TimeExpired	= false;//LowPowerDelay 
char	loc_chr[22];	  	//global GPS location 
ulong	DiveStart;

short	LOGTOCON = 1;
short	longwait = 400;		//drift_ascent & descent, dive
short	shortwait= 170;
ulong 	TotalvDur= 0L;
ulong	TotalpDur= 0L;
short	pMax     = 1940;	//pressure max in dBAR
char	LonChar[12];
float	pConv    = 1.033;	//conversion factor from meter to dBar in sea water
char	LatChar[10];


#define IRQ2		41		 //PROVOR COM IRQ. Keep HI. When low, TX char is from PROVOR controller
#define AUTOVECT	level2InterruptAutovector

//	RecipeCard or other Platform Board
#define	PRCPLG		1		 // Standard RecipeCard with Max146
#define	CUSTOM		2
#define	NOAADAQ		3		 //*JHG990922--ADD LINE---
#define NOAADAQ2    4        //Addition for NOAADAQ2 board May 13/2002 HM
#define	NOAADAQ3	5		 //Addition for QUE-phone DAQ board
#define	PLATFORM 	NOAADAQ3 //NOAADAQ3(ADS8344 16-bit A/D CF2 board with 2 AUX RS232)

//	DAQ board definition
#if(PLATFORM == NOAADAQ2 || PLATFORM == NOAADAQ3) //Added for ADS8344 A/D driver HM NOAA, 6/10/2002
#define	ADSLOT	NMPCS3	// non-multiplexed connection to NMPCS0 TEST only by HM, NOAA
#define	FCHAN	0		// first channel
#define	NCHAN	1	
#define	UNI		true	// choose: true or false for unipolar or bipolar
#define	SGL		true	// choose: true or false for single-ended or differential
#define	VREF	2.5		// Ref voltage HM, NOAA 6/20/2002 (-may not need)
#define	NUMCHAN	8		// You need to define this.  8ch/2byte data fills up 16-byte
  							// QSPI register (QRR)  HM, NOAA 6/20/2002
//#else // ????????????
#endif

//ADC related 
uchar	ADidx=0;
uchar	ADdata[256]; 				// Temp RAM buffer to store raw AD data
ulong	ADTimingRuptCount=0L;		// total convert interrupts (will wrap)
ulong	ADSamplingRuptCount=0L;		// total A-D interrupts (will wrap)

#define	GAIN0		23  // 1  = 0 dB  |0 = 6dB | 1 = 12dB | 0 = 18dB,
#define	GAIN1		24	// 1          |1       | 0        | 0
#define DFLIP		28  //This clears PROV COM RX D-Filp Flop and ready for interrupt
//Define Pin I/O functions here
PinIODefineTPU(GAIN0,2);//23
PinIODefineTPU(GAIN1,3);//24
PinIODefineTPU(DFLIP,7);//28

// This macro will convert CF1 and CF2 ticks into course milliseconds
// It replaces the CFX specific ticks>>6	JHG-2003-07-28
#define	TICKS_TO_MS(x)	((ushort) (x / (CFX_CLOCKS_PER_SEC / 1000)))

//PROVOR related parameters
short	result;		
short	batVolt, extPressure, reservLevel, intVacuum;
short	valvDuration, pumpDuration;
short 	pStatus,lvStatus, phase, pDur,vDur,pressure=0;
short	cpuSt,vacSt, tankSt,batSt,pSensorSt;
short	pDepth, pError, pSample;
short   MinBatVolt=12000;//mV
short	prevPhase;	     //Previous Phase status
short	rqPressure;	     //current pressure and requested pressure
short	check_int = 300; //surface check interval

//short 	dummy=0;

//Bathy coordinate related
short	bathyArray[46][46];
short 	dmDim;
float 	refLat, refLon;
float 	dmSpac;
char 	refNS, refEW;
//extern	NS,EW;
float	AveLat, AveLon;
short	NumGPSAve      = 3; //# of GPS data to be averaged
short	Max_No_SigQ_Chk=10; //# of signal quality checks before giving up 
float	FudgeFactor    =1.1;//fudge factor to estimate the depth based on drift 
float	FudgeFactor2   =1.2;//fudge factor to estimate the depth based on drift 

char	GMT_char[20];	
char	dive_info[100];		//dive # target depth in meter
char	Drift_Info[55];		//drift in lat and long in seconds
char	time_header[45];
ulong	SecondsSince1970;
char	longbuf[64];
char	shortbuf[4];

short	TX_Success;
bool	SatComOpen  = false;  //GPS IRIDIRUM COM Open=true or close=false
bool	ProvComOpen = false;  //Provor com open=true, close=false

bool	ProgramFinished = false;
bool	ADFinished = false;

//Prototype 
void   PreRun(void);
short  SetupHardware(void);
void   QUEGetSettings(void);
ulong  WhatTime(char *GMT_char, ushort *msec);

void   Delay(short centisecs);
void   IridiumGPSPowerON(bool sw1);	//Turn the power ON IRIDIUM/GPS

int    Calc_Crc(unsigned char *buf, int cnt);
void   Convert_BitMap_To_CharBuf(ulong val0, ulong val1, char *bin_str);
void   Send_Platform_ID(char *Platform_ID, char *Proj_ID);
int    Send_Blocks(char *bitmap, uchar NumOfBlks,ushort blklngth, ushort lastblklngth, char *fname);
bool   Check_If_ACK(void);
short  Check_If_Done_Or_Resent(ulong *val0, ulong *val1);
bool   Call_Land(char *);
short  SwitchAntenna(char r[4]);
bool   OpenComIridiumGPS(short iridwarm);
bool   OpenComGPSIridium(short warmup);
bool   EnableIridiumFlow(void);

bool   CloseComIridiumGPS(void);
time_t GetGPSTime(bool set, char*);
bool   GetGPSDateTimeSyncRTC(bool set, char *time_chr, short *num_sat, time_t *GPSsec, short *msec, time_t *RTCsec, short *rtcms);
void   LogFailure(short errcode);
void   LowPowerDelay(ulong Delay);  //Delay is 0.102 sec increment
IEV_C_PROTO(TimingRuptHandler);
IEV_C_PROTO(ExtFinishPulseRuptHandler);
short  AveGPSLocation(short repeat, char *loc_chr, char *LatChar, char *LonChar, float *AveLat, float *AveLon);
bool   EnableIridiumCPIN(void);
short  SetIRIDBaud(short speed);	
short  ReadIRIDBaud(void);	
short  HangUp(void); 
short  PhoneStatus(void);
short  SignalQuality(short *signal_quality);
short  TurnOffModem(void);
void   WriteLocEPROM(char ns, short intLat, float LatMin, char ew, short intLon, float LonMin);
void   Get_Last_Dive_Location(float *LastLatDeg, float *LastLonDeg);
bool   Connect(void);
bool   Acknowledge(void);
short  Send_File(char *fname);
bool   Write_Header_To_File(char *fname, char *SysHeader, ushort SysSize, \
		char *LocTimeHeader,  ushort TimeSize, char *Drift_Info,  ushort GPSSize);
bool   Write_Trailer_To_File(char *fname, char *LocTimeHeader, ushort TimeSize,\
        char *SysHeader, ushort SysSize, char *DrifChr, ushort DrifSize);
bool   Write_SysLocGMT_To_File(char *fname, char *LocTimeHeader, ushort TimeSize);
void   Get_Last_Drift(short *DriftLatOld, short *DriftLonOld);
short  Get_GPS_Position_Time(bool SetClock, float *AvLat, float *AvLon, char *time_header);
short  EstimateDepthFromDriftBathy(short *FudgeDepth,short GPSFixOK, float DriftLatOld,\
       float DriftLonOld, float LastLatDeg, float LastLonDeg, short *size_drift_info);
void   LockOnToGPSTime(void);
short  GetGPSLocation(float *Lat, float *Lon);
short  Connect_Transmit_DataFile(char *fname); 
void   fakeGPS(void);//debug

short  InqProvorStatus(short *pStatus, short *lvStatus, short *phase, short *pDur,\
	   short *vDur, short *pressure);				   
bool   OpenCommProv(void);
bool   CloseCommProv(void);
short  BatteryCheck(short *batVolt);
short  GetExtPressure(short *exPressure);
short  GetReservoirLevel(short *reservLevel);
short  GetIntVacuum(short *intVacuum);
short  ActivateValve(short valvDuration);
short  ActivatePump(short pumpDuration);
short  FloatDiagonostic(short *cpuSt, short *intVacuum, short *vacSt,\
       short *reservLevel, short *tankSt, short *batVolt, short *batSt,\
       short *extPressure, short *pSensorSt);				   
short  GoTo(short pDepth, short dError, short pSample);					   
void   Delay(short centisecs);
void   TestProv(void);
static void Irq2RxISR(void); 	//interrupt by TU AUX1 RX port (PROVOR)
bool   ListenProvorLowPower(short waitSec, short RepeatMax);
short  RecProvorStatus(short *pStatus, short *lvStatus, short *phase, short *pDur,\
		short *vDur, short *pressure);//Just receive status.  Use it after !G				   
short  LoadBathymetry(void);
short  FindDepth(float Lat, float Lon);
void   WritePhase(void);
short  FindPROVORPhase(void);
void   ResetProv(void);
short  EmptyReservoir(short duration);
short  BalanceReservoir(short duration);

//Acoustic loggind related funcation prototypes
IEV_C_PROTO(ADTimingRuptHandler);	// implied (IEVStack *ievstack:__a0) parameter
IEV_C_PROTO(ADWordRuptHandler);	// implied (IEVStack *ievstack:__a0) parameter
vfptr ADSamplingRuptHandler;
static void BIRGetSettings(void);
FILE *Write_Header(char *fname);
void BIRUpdateTrailer(void);
void QUEUpdateHeader(void);
short SetupAcquisition(void);
short Acquire(char *fname);
void WriteSurfacePrepResult();
bool MakeSureSurface(short check_int);
FILE *Write_Trailer(char *fname);
bool CheckResPumpIfNotEmpty(short duration);
bool CheckResOpenValve(short duration);

/******************************************************************************\
**	main
\******************************************************************************/
int main()
{
	static char	fnew[]="C:00000000.DAT";	//first detection file
	static char	fold[]="C:00000000.DAT";	//old  detection file
	short	result =  0;					//no errors so far
	short	EstimDepth;
	short	FudgeDepth;
	char	strbuf[7];
	short	IntSample = 30;
	char	*p;
	float	LastLatDeg, LastLonDeg;
	float	DriftLatOld, DriftLonOld;
	short	GPSFixOK;
	short	size_drift_info; 
	short 	size_dive_info;
	bool	File_Creation_Success = false;
	short	AntSW, i;
	ulong	bottom_time = 120L;//12 sec bottom time - only for testing 
	bool	SetClock=true;
	ushort 	msec;
	bool	TimeUp=false;
	short	RepInq;
	short	filenum;

	QUEGetSettings();		//Get logging and float control parameters
	BIRGetSettings();		//Get the logging parameter
	if(QUE.PROVTERM==1) ProvTerm = true;
	if(QUE.IRIDTERM==1) SatTerm  = true;
	if(QUE.LOGONLY ==1)	LogOnly  = true;
	else if(!SatTerm)
		{
		uprintf("WARNING! QUE.LOGONLY=0 but QUE.IRIDTERM=0. Exiting\n");
		exit(0);
		}
	if(QUE.LOGONLY == 1 && QUE.PROVTERM ==1 && QUE.IRIDTERM==1)
		{
		uprintf("WARNING LOGONLY, IRIDTERM PROVTERM ARE ALL 1\n");
		exit(0);
		}	
			
	PreRun();
		
	phase = SetupHardware();//Check the current phase of PROVOR   
	LoadBathymetry();	    //Read bathy data from C: (file name specified in VEE setting)

	//00000000.dat is the first file to send, and contains time and coordinates only. 
	//Monitoring data will be appended upon the end of logging and resurfacing.
	sprintf(&fnew[2],"%08lu.DAT",(long) ++QUE.FILECOUNT);//name a new file
	//sprintf(strbuf, "%u", QUE.FILECOUNT);
	//VEEStoreStr(QUE_FILECOUNT_NAME,strbuf);
	
	//
	//**Main loop**************************************************************************			
	//
	while(QUE.GOTODIVE == 1) //You have a permission to dive
		{
		if(phase <4)
			{
			RTCGetTime(&DiveStart, NULL);
			BIRS.DIVESTART=DiveStart;
			sprintf(longbuf, "%lu", BIRS.DIVESTART);
			VEEStoreStr(BIR_DIVESTART_NAME, longbuf);	//Keep wake-up time in VEE	
			
			p = VEEFetchData(QUE_LASTGPS_NAME).str; //Fetch the last GPS result from EPROM
			QUE.LASTGPS = atoi(p ? p : QUE_LASTGPS_DEFAULT);
			pDepth = pMax; // go for maximum depth
			if(QUE.NUMOFDIVE == 0 && QUE.STARTUPS == 0)//going to dive for the first time
				{
				//Make sure reservoir is empty.  If not,run the pump for a specified duration.
				CheckResPumpIfNotEmpty(600);
				}
			}
		if((LogOnly || SatTerm) && (phase ==0 || phase ==1))//Still at surface
	 		{
	 		//Before turning on IRID/GPS, make sure the float is at surface
			if(ProvTerm)
				{
				MakeSureSurface(check_int);
				if(ProvComOpen)ProvComOpen=CloseCommProv();
				}
			PinRead(IRQ2);//Disable IRQ2
								
	 		//**Turn on GPS/IRIDIUM unit and get GPS time and position
			//if(SatTerm)SatComOpen = OpenComIridiumGPS(QUE.GPSWARM);//Power ON Iridium&GPS, warm up  & open COM
			if(SatTerm)
				{
				AntSW   = SwitchAntenna("GPS");  //AntSW=0=GPS antenna, 1=Iridium antenna
				SatComOpen = OpenComGPSIridium(QUE.GPSWARM);//Power ON Iridium&GPS, warm up  & open COM
				}
			if(LogOnly)SatComOpen = true;
			//uprintf("ComOpen=%d AntSW=%d\n",ComOpen, AntSW);//debug

			if(SatComOpen)//Com open and antenna is GPS
				{
				TickleSWSR();
				Get_Last_Dive_Location(&LastLatDeg, &LastLonDeg);//from QUE.LAT and LAN
				if(SatTerm && AntSW==0) GPSFixOK = Get_GPS_Position_Time(SetClock, &AveLat, &AveLon, time_header);

				//debug 
				if(!SatTerm)
					{
					GPSFixOK=1;		//Debug. Use it when GPS is not connected. Remove it for real.	
					fakeGPS(); 	 	//Debug. Use it when GPS is not connected. Remove this!
					SetClock=false;	//debug
					}
				//debug end
				
				//**Estimate the new depth from GPS position and drift  
				Get_Last_Drift(&DriftLatOld, &DriftLonOld);	//get the last drift infor
				EstimDepth=EstimateDepthFromDriftBathy(&FudgeDepth,GPSFixOK, DriftLatOld,DriftLonOld,\
					LastLatDeg,LastLonDeg, &size_drift_info);
				flogf("ESTIMATED DEPTH = %d\n", EstimDepth);cdrain();coflush();
				//**Now check if the estimated bathy depth is safe to dive
				QUE.ESTMDEPTH = EstimDepth;
				if(EstimDepth >0)
					{
					if(EstimDepth <= QUE.MAXDEPTH)
						{//Depth is OK and within the grid
						sprintf(strbuf, "%d", EstimDepth);
						VEEStoreStr(QUE_ESTMDEPTH_NAME,strbuf);//Estimated bathy depth
							strncat(dive_info,"  FLOAT INSIDE BATHY GRID ",26);
						QUE.GOBOTTOM = 1;
						QUE.GOPARK   = 0;
						}
					else //Still in grid, but depth is too deep
						{
						strncat(dive_info,"  TOO DEEP. PARK AT ",20);
					 	sprintf(strbuf,"%.4d M",QUE.PARKDEPTH);
					 	strncat(dive_info,strbuf,6);
						pDepth = QUE.PARKDEPTH * pConv;          //Parking depth in dBar
						sprintf(strbuf, "%d", QUE.PARKDEPTH);
						VEEStoreStr(QUE_ESTMDEPTH_NAME,strbuf);//Bathy depth
						QUE.GOBOTTOM = 0;
						QUE.GOPARK   = 1;
					 	}
					}
				else
				 	{//==-1. Depth too deep or out of bathy grid, so go park 
					QUE.GOBOTTOM = 0;
					QUE.GOPARK   = 1;
					if(EstimDepth <0)
						{
						if(GPSFixOK == 1)
							{//either no GPS or out of grid
							strncat(dive_info,"  FLOAT OUT OF BATHY GRID ",26);
							}
						else 
							{
							strncat(dive_info,"  GPS FIX FAILED          ",26);
							}
						}	
					pDepth = QUE.PARKDEPTH * pConv;        //Parking depth in dBar
					sprintf(strbuf, "%d", QUE.PARKDEPTH);
					VEEStoreStr(QUE_ESTMDEPTH_NAME,strbuf);//Bathy depth
					}
				size_dive_info=strlen(dive_info);

				WriteSurfacePrepResult();
								
				//**Send a data file to land station
				//TX_Success=1; //debug dummy if 1 skip transmission, if 0, transmit
				if(SatTerm) AntSW = SwitchAntenna("IRD");			//Switch antenna
				TickleSWSR();
				if(QUE.NUMOFDIVE == 0)//going to dive for the first time
					{
					//First to send a QUE header only 
					//flogf("HEADER0\n");
					fp = Write_Header(fnew);
					if(SatTerm && AntSW == 1) TX_Success = Connect_Transmit_DataFile(fnew); 
					}
				else
					{					
					fp = Write_Trailer(fnew);
					if(SatTerm && SatComOpen && AntSW == 1) 
						{
						TX_Success = Connect_Transmit_DataFile(fnew); 
						}
					//Advance file counter
					//This is a trick not to advance the FILECOUN if TX is not success
					if((QUE.NUMOFDIVE <= QUE.MAXNUMDIV) && (TX_Success || LogOnly) )//successful TX. Create a new file. 
						{
						sprintf(&fnew[2],"%08lu.DAT",(long)++QUE.FILECOUNT);//name a new detection file
						sprintf(strbuf, "%u", QUE.FILECOUNT);
						VEEStoreStr(QUE_FILECOUNT_NAME,strbuf);
						//flogf("HEADER1\n");
					    fp = Write_Header(fnew);
						}
					}
				}
		  	//
			if(QUE.NUMOFDIVE >= QUE.MAXNUMDIV) 
				{
				QUE.GOTODIVE = 0;
				sprintf(strbuf, "%u", QUE.GOTODIVE);
				VEEStoreStr(QUE_GOTODIVE_NAME,strbuf);//Record if GoToDive result
				}

			}
		else if(phase > 1 && phase <8) //SatTerm=0, phase=2,3,4,5,6,7
			{//The float is already submerged state. Came here because CF2 reset
			 //Find out the target dive depth from the past record
			EstimDepth = QUE.ESTMDEPTH;   		//Bathy depth from the past
			pSample    = QUE.PARKSAMPL;
			}
		else//SatTerm=0 or phase 0 or 1. Or phase = 8 or 9 going up
			{
			pSample     = QUE.SURFSAMPL;
			}			
		
  		LowPowerDelay(50L);
  		
  //**Came here because IRIDIUM transmission was successful and ready to dive or already in dive mode
		if((ProvTerm || LogOnly) && QUE.GOTODIVE)
			{
			//Set up PROVOR comm
			if(!ProvComOpen)
				{
				ProvComOpen=OpenCommProv();
				RTCDelayMicroSeconds(5000000L);
				TickleSWSR();
				} 
			TickleSWSR(); 
			pSample=QUE.PARKSAMPL;
			pError=50;

			if(phase <2 && ProvTerm) 	//2-way com; phase=0 or =1 (still at surface or surface_dive)
				{
				//Leaving surface
				RTCGetTime(&BIRS.RTCLVSRFC, &msec);
				sprintf(longbuf, "%lu", BIRS.RTCLVSRFC);
				VEEStoreStr(BIR_RTCLVSRFC_NAME, longbuf);
				result=10; 				//initialize
				while(result>1)result=InqProvorStatus(&pStatus, &lvStatus, &phase, \
					&pDur,	&vDur, &pressure);		
				flogf("PROVAR STATUS %d,%d,%05d,%05d,%d,%04d\n",pStatus,lvStatus,pDur,\
					vDur, phase, pressure); cdrain(); coflush();
				TickleSWSR();
				CheckResPumpIfNotEmpty(60);	//Check reservoir level again, make sure it is ampty
		
				flogf("DIVE TO %d dBars W/ +/-%d dBars CHECKING EVERY %d MIN\n", pDepth, pError, pSample);
				cdrain();coflush();RTCDelayMicroSeconds(1000L);
				putflush();
				WritePhase();
				
				result=10;
				while(result>1) result = GoTo(pDepth, pError, pSample);//nothing happens for 40 sec or so.

				flogf("DIVE # %d\n",QUE.NUMOFDIVE);cdrain();coflush();RTCDelayMicroSeconds(3000000L);
				TotalvDur = 0L;
				TotalpDur = 0L;

				while(phase == 0 || phase == 1)//Check if still at surface or surface_dive mode
					{//still surface
					TimeUp=ListenProvorLowPower(shortwait, 16);
					}//Now in surface_dive 
				WritePhase();
				flogf("AT SURFACE ACCUMULATED VALVE ACTION     = %ld SEC\n", TotalvDur/10L);cdrain();coflush();
				}
			//SatTerm=false;
			//sprintf(strbuf, "%u", QUE.IRIDTERM=0);
			//VEEStoreStr(QUE_IRIDTERM_NAME,strbuf);//Record SatTerm result

			if((phase == 2) && ProvTerm) //Advanced to Surface_Dive mode
				{
				while(phase <3)
					{//still surface_dive mode
					TimeUp=ListenProvorLowPower(shortwait, 160);
					} 
				WritePhase();
				flogf("IN SURFACE_DIVE ACCUMULATED VALVE ACTION = %ld SEC\n", TotalvDur/10L);cdrain();coflush();
				}
	 		
			if((phase == 3) && ProvTerm) //Full dive mode
				{
				while(phase <4)
					{
					TimeUp=ListenProvorLowPower(longwait, 1256);
					}//Now grounded or parked!! PROVOR 2-way COMM
				WritePhase();
				flogf("IN_DIVE ACCUMULATED VALVE ACTION         = %ld SEC\n", TotalvDur/10L);cdrain();coflush();
				}

		//Parked.  The bottom is too deep, or depth not known.  Log data anyway.  
			if((phase == 4 || phase == 6 || phase == 7) && QUE.GOMONITOR !=1)
				{
				BIRS.PREVDEPTH = (short)((float)pressure/pConv);
				flogf("PARKING MODE AT          = %dM %ddBar. PHASE=%d\n", BIRS.PREVDEPTH, pressure,phase);cdrain();coflush();
				flogf("ACCUMULATED VALVE ACTION = %ld SEC\n", TotalvDur/10L);cdrain();coflush();
				QUE.GOMONITOR = 1;//Activate monitor while parked
				sprintf(strbuf, "%u", QUE.GOMONITOR);
		 		VEEStoreStr(QUE_GOMONITOR_NAME,strbuf);
				}
		
		//Grounded bottom! 
			if((phase == 4 || phase == 5) && QUE.GOMONITOR !=1)
				{
				RTCGetTime(&BIRS.RTCARVBTM, NULL);
				sprintf(longbuf, "%lu", BIRS.RTCARVBTM);
				VEEStoreStr(BIR_RTCARVBTM_NAME, longbuf);	//Keep wake-up time in VEE	
				}

			if(phase == 5 && QUE.GOMONITOR !=1 || LogOnly)//2-way com now.
				{
				BIRS.PREVDEPTH = (short)((float)pressure/pConv);	
				flogf("GROUNDED AT %dM %ddBars\n", BIRS.PREVDEPTH, pressure);cdrain();coflush();
				result=10;
				Delay(10);
				QUE.GOMONITOR = 1;
				sprintf(strbuf, "%u", QUE.GOMONITOR);
		 		VEEStoreStr(QUE_GOMONITOR_NAME,strbuf);
				}
			if(QUE.GOMONITOR != 1)
				{
				sprintf(strbuf, "%u", BIRS.PREVDEPTH);//Record the sensor depth in EEPROM
				VEEStoreStr(BIR_PREVDEPTH_NAME,strbuf);
				BIRS.STATUS = phase;				  //Activate monitor while parked
				sprintf(strbuf, "%u", BIRS.STATUS);
			 	VEEStoreStr(BIR_STATUS_NAME,strbuf);
				}

			//We are here because;
			//(1) grounding is confirmed or (status=5)
			//(2) arrived the intended parking depth, (status=4,6,7)
			if((phase > 3  && phase < 8) || LogOnly)
				{
				if(ProvTerm && ProvComOpen) ProvComOpen=CloseCommProv();//this may cause a serial buffer problem
				while(QUE.GOMONITOR ==1)
					{
					//Your acquistion program starts here
					//Write the last mode (park or ground) and depth
					TickleSWSR();
					WritePhase();
					BIRS.STATUS = phase;				  
					sprintf(strbuf, "%u", BIRS.STATUS);
			 		VEEStoreStr(BIR_STATUS_NAME,strbuf);
					//Open detection file and append the data
					//SetupAcquisition();
						//{
						//Log multiple files QUE.FILENUM=file index within the same dive
						for(filenum=QUE.FILENUM;filenum<=QUE.MAXFILE;++filenum)
							{
							//Advancing the file number within the same dive
							if(filenum>1)//After the first file in the dive, advance fileount
								{
								sprintf(&fnew[2],"%08lu.DAT",(long)++QUE.FILECOUNT);//name a new detection file
								sprintf(strbuf, "%u", QUE.FILECOUNT);
								VEEStoreStr(QUE_FILECOUNT_NAME,strbuf);
						        //flogf("HEADER2\n");
								fp = Write_Header(fnew);
								}//debug end
							//added to make the last file size=4096+256;	
							if(filenum==QUE.MAXFILE)filelength=4096;
								else filelength=QUE.CFPPBSZ;
							if(!SetupAcquisition())
								if(Acquire(fnew)==-1)filenum=QUE.MAXFILE;//you only need this line

							//You need this to write multiple files in one dive
							if(filenum<QUE.MAXFILE)
								{					
								fp = Write_Trailer(fnew);
								}
							sprintf(strbuf, "%u", filenum);
				 			VEEStoreStr(QUE_FILENUM_NAME,strbuf);
							}
						//}

					flogf("LOGGING ENDED\n");
					QUE.FILENUM=1;
					sprintf(strbuf, "%u", filenum);
				 	VEEStoreStr(QUE_FILENUM_NAME,strbuf);
					//Ends the acoustic monitoring

					QUE.GOMONITOR=0;//logging cycle ends, so change the monitoring state
					sprintf(strbuf, "%u", QUE.GOMONITOR);
		 			VEEStoreStr(QUE_GOMONITOR_NAME,strbuf);
					}
				
				//Going up to the surface
				
				pDepth    = 0;
				pError    = 50;
				TotalpDur = 0L;
				TotalvDur = 0L;
				pSample	  = QUE.SURFSAMPL;
				result    = 10;
				
				WhatTime(GMT_char, NULL);
				flogf("%.20s ",GMT_char);cdrain();coflush();
				flogf("OPENING PROVOR'S COMM TO START ASCENT\n");
				LowPowerDelay(20L);
				
				//Opening the PROVOR's COMM
				if(ProvTerm)
					{
					if(!ProvComOpen)ProvComOpen=OpenCommProv();
					TickleSWSR();

					//Find out what phase is now.  It may be 6 or 7.
					WhatTime(GMT_char, NULL);
					flogf("%.20s ",GMT_char);cdrain();coflush();
					flogf("FIND OUT THE PHASE ");cdrain();coflush();
					TimeUp=ListenProvorLowPower(longwait, 1);
					WritePhase();
					flogf("PHASE NOW = %d\n", phase);cdrain();coflush();
						
					if(phase == 6 || phase ==7)
						{
						RepInq=(QUE.PARKSAMPL *60)/longwait+2;
						while (phase == 6 || phase == 7)ListenProvorLowPower(longwait, RepInq);
						WritePhase();
						flogf("PHASE NOW = %d\n", phase);cdrain();coflush();
						}
	
					if(phase==4 || phase==5 )
						{
						//PROVOR is in 2-way comm, now safe to give a command				
			 			result=10;
						while(result>1) result = GoTo(pDepth,pError,pSample);	
						WhatTime(GMT_char, NULL);
						flogf("ASCENT COMMAND %.20s\n", GMT_char);
						putflush();
						for(i=0;i<4;i++)//kill time here
							{
							LowPowerDelay(600);
							TickleSWSR();
							}
						//Wait for mode to change to ascent
						TimeUp=false;
						TimeUp=ListenProvorLowPower(longwait,1);
						while(phase != 8) ListenProvorLowPower(longwait, 256);//10 minutes wait
						BIRS.RTCASCSTR = WhatTime(GMT_char, NULL);
						sprintf(longbuf, "%lu", BIRS.RTCASCSTR);
						VEEStoreStr(BIR_RTCASCSTR_NAME, longbuf);	//Keep wake-up time in VEE	
						WhatTime(GMT_char, NULL);
						flogf("GOING UP                 %.20s\n", GMT_char);
						putflush();
						}
					}
				}
			
				WritePhase();
				Delay(10);
			//**Normal ascent (=8) or emergency ascent mode (=9)	
			if(phase >7 && ProvTerm) 
				{
				if(phase == 9) 
					{
					QUE.GOMONITOR = 0;//Emergency ascent- do not monitor
					sprintf(strbuf, "%u", QUE.GOMONITOR);
		 			VEEStoreStr(QUE_GOMONITOR_NAME,strbuf);
					WhatTime(GMT_char, NULL);
					flogf("EMERGENCY ASCENT         %.20s\n",GMT_char);cdrain();coflush();
					}
				while(phase !=1)//phase=8
					{
					ListenProvorLowPower(longwait, 1256);
					//Resting at surface. Ascent successfull. Increment dive number
					++QUE.NUMOFDIVE;
					sprintf(strbuf, "%u", QUE.NUMOFDIVE);
		 			VEEStoreStr(QUE_NUMOFDIVE_NAME,strbuf);
					} //Now surface!!
				BIRS.RTCASCEND = WhatTime(GMT_char, NULL);
				sprintf(longbuf, "%lu", BIRS.RTCASCEND);
				VEEStoreStr(BIR_RTCASCEND_NAME, longbuf);	//Keep wake-up time in VEE	
				flogf("SURFACE!                     %.20s\n",GMT_char);cdrain(); coflush();	
				flogf("DIVE # %d IS DONE\n",QUE.NUMOFDIVE-1);cdrain();coflush();
				flogf("TOTAL PUMP ACTION          = %ld SEC\n", TotalpDur/10L);cdrain();coflush();

				//Check if it is really at surface
				result = 10;	//initialize
				while(result>1)result=InqProvorStatus(&pStatus, &lvStatus, &phase, &pDur,&vDur,&pressure);		
				flogf("%.20s ",GMT_char);cdrain();coflush();
				flogf("STATUS %d,%d,%05d,%05d,%d,%04d\n",pStatus,lvStatus,pDur,\
					vDur, phase, pressure); cdrain();coflush();
				putflush();
				WritePhase();
				}
			}

//debug fake remove these lines below
		if(!ProvTerm && QUE.GOTODIVE == 1)//for testing just without PROVOR unit- to advance the dive number
			{
			LowPowerDelay(100L);//10 sec delay
			++QUE.NUMOFDIVE;    //new dive number
			//uprintf("Pass 3 %d\n", QUE.NUMOFDIVE);//debug
			sprintf(strbuf, "%d", QUE.NUMOFDIVE);
			VEEStoreStr(QUE_NUMOFDIVE_NAME,strbuf);//Record dive count
			}
//debug fake end	
		if(!ProvTerm && (QUE.NUMOFDIVE > QUE.MAXNUMDIV)) 
			{
			QUE.GOTODIVE = 0;
			sprintf(strbuf, "%u", QUE.GOTODIVE);
			VEEStoreStr(QUE_GOTODIVE_NAME,strbuf);//Record if GoToDive result
			}

/*		if(QUE.NUMOFDIVE >= QUE.MAXNUMDIV) 
			{
			if((LogOnly || ProvTerm) && QUE.NUMOFDIVE==QUE.MAXNUMDIV)
				{
				QUE.GOTODIVE=0;
				VEEStoreStr(QUE_GOTODIVE_NAME,"0");//Record READY TO DIVE result
				}
			}
*/	
		}
  //**Both data logging and diving ended. Float will be drifting at the surface until picked up.	
  //**Continue sending the float location until "Interrupt" button is pushed or number of 
  //**IRIDUM calls exeeds maximum	
	//ResetProv();
  	flogf("***********ALL DIVES AND LOGGINGS ARE DONE.  DRIFTING NOW********\n");cdrain();coflush();
	LowPowerDelay(100L);
	ProgramFinished=false;
	//PITSet100usPeriod(PITOff);	// disable 100us timer
	//PITRemoveChore(0);			// get rid of all current chores
	while(PinRead(IRQ5) && !ProgramFinished && QUE.NUMDRFTCL < QUE.MAXDRFTCL && (SatTerm || LogOnly))//until interrupted
		{
  //**Turn on GPS/IRIDIUM unit and get GPS time and position
  		flogf("**************REPORTING GPS POS WHILE DRIFTING TX#=%d***********\n", QUE.NUMDRFTCL);cdrain();coflush();
		if(ProvTerm)
			{
			if(!ProvComOpen)ProvComOpen=OpenCommProv();
			flogf("CHECKING IF RESERVOIR IS EMPTY\n");RTCDelayMicroSeconds(10000L);
			CheckResPumpIfNotEmpty(600);
			ProvComOpen=CloseCommProv();
			TickleSWSR();				// another reprieve
			LowPowerDelay(50L);
			}
		if(SatTerm) 
			{
			AntSW = SwitchAntenna("GPS");			//Switch antenna
			LowPowerDelay(50L);
			SatComOpen = OpenComGPSIridium(QUE.GPSWARM);//Power ON Iridium&GPS, warm up  & open COM
			TickleSWSR();				// another reprieve
			GPSFixOK   = Get_GPS_Position_Time(SetClock, &AveLat, &AveLon, time_header);
			
			Get_Last_Drift(&DriftLatOld, &DriftLonOld);	//get the last drift infor
			EstimDepth = EstimateDepthFromDriftBathy(&FudgeDepth,GPSFixOK, DriftLatOld,DriftLonOld,\
				LastLatDeg,LastLonDeg, &size_drift_info);
			
			sprintf(&fnew[2],"%08lu.DAT",(long)++QUE.FILECOUNT);//name a new detection file
			sprintf(strbuf, "%u", QUE.FILECOUNT);
			VEEStoreStr(QUE_FILECOUNT_NAME, strbuf);
			++QUE.NUMDRFTCL;
			sprintf(strbuf, "%d", QUE.NUMDRFTCL);
			VEEStoreStr(QUE_NUMDRFTCL_NAME,strbuf);//Record dive count
			Write_Header(fnew);
			AntSW=SwitchAntenna("IRD");		//Switch antenna
			TickleSWSR();				// another reprieve
			if(SatTerm && SatComOpen && AntSW == 1) TX_Success = Connect_Transmit_DataFile(fnew);
			} 
		IEVInsertCFunct(&ExtFinishPulseRuptHandler, level5InterruptAutovector);
		while(PinTestIsItBus(IRQ5)==0){PinBus(IRQ5);}					// enable PBM (IRQ5) interrupt button
		//Enable ProgramFinish pulse at IRQ5
		LowPowerDelay(QUE.SECDRFTCL*10L);
		}
	flogf("DONE!\n");
	PinRead(IRQ5);	
	return result;
}	//____ main() ____//

//********************************************************************************************
// LogFailure
// Log the failue mode in "activity.log"
// Walter Hannah (July 2004, Revised Haru Matsumoto 10/11/2004
//********************************************************************************************
void LogFailure(short errcode)
	//Log various failures of the Device
	//this probably doesn't need to be added anywhere
{
	char 	err[8];
	RTCtm	*rtc_time;				//CFX's real time clock
	ulong	SecondsSince1970 =  NULL;
	ushort	Ticks;
	
	if(errcode == 1)
			sprintf(err,"cpufail");	//cpu failure
	else if(errcode == 2)
			sprintf(err,"vacfail");	//vacuum failure
	else if(errcode == 3)
			sprintf(err,"tnkfail");	//tank failure
	else if(errcode == 4)
			sprintf(err,"batfail");	//battery failure//Depth is OK and within the grid
	else if(errcode == 5)
			sprintf(err,"prsfail");	//pressure reading failure
	else if(errcode == 6)
			sprintf(err,"!Gtmout");	//no response in !G function
	else if(errcode == 7)
			sprintf(err,"?Stmout");	//no response in ?S function
	else if(errcode == 8)
			sprintf(err,"!Ctmout");	//no response in !C function
	else if(errcode == 9)
			sprintf(err,"veefail");	//failure reading or writing to VEEPROM
	else if(errcode == 10)
			sprintf(err,"GPScomE");	//if GPS com errors during set up
	else if(errcode == 11)
			sprintf(err,"GPSfail"); //GPS timeout or data is void
	else if(errcode == 12)
			sprintf(err,"ISUfail"); //Iridium timeout or data is void
	else if(errcode == 13)
			sprintf(err,"SQLfail"); //Iridium signal quality too low
	else if(errcode == 14)
			sprintf(err,"Bytfail"); //Iridium byte failure
	else if(errcode == 15)
			sprintf(err,"PhStbad"); //Iridium phone status bad
	else if(errcode == 16)
			sprintf(err,"ResentE"); //Iridium Resent error
	else if(errcode == 17)
			sprintf(err,"DoneRsE"); //Iridium done or resent signal garbled
	else if(errcode == 18)
			sprintf(err,"SystIDE"); //Iridium done or resent signal garbled

	RTCGetTime(&SecondsSince1970, &Ticks);
	rtc_time = RTClocaltime (&SecondsSince1970);
	
	flogf("ERROR! %.7s ;%.3d:%.3d:%.2d:%.2d;%d;%d\n",err,rtc_time->tm_year, 
				rtc_time->tm_yday,rtc_time->tm_hour,rtc_time->tm_min,
				phase, pressure);RTCDelayMicroSeconds(10000L);
}
/******************************************************************************\
**	PreRun		Exit opportunity before we start the acquisition program
**	
**	
\******************************************************************************/
void PreRun(void)
	{
	short		nd, ndelay;
	short		i;
	char		c;
	char 		*ProgramDescription = 
		{
		"\n"
		"The program will start in ten seconds unless an operator keys in a\n"
		"period '.' character. If a period is received, you have about\n"
		"### seconds to respond to access other software functions on this\n"
		"system before a watchdog reset occurs.\n"
		"\n"
		};
	time_t		nowsecs;
	char		strbuf[64];
	struct tm 	t;
	ushort		Ticks;
	uchar		mirrorpins[] = {35,37};

	PIOMirrorList(mirrorpins);
	//PROV related pins
	PinClear(29);
	PinSet(30);
	PinClear(26);//PROVOR COM off
	PinSet(DFLIP);//Preset D FF to ignore PROVOR serial interrupt
	PinRead(IRQ2);//Disable IRQ2
	//Iridium related pins	
	PinClear(25);
	Initflog(QUE.LOGFILE, LOGTOCON ? true : false);//Create or open the event logging file

	flogf("\nProgram: %s: %s %s \n", __FILE__, __DATE__, __TIME__);
	nowsecs = RTCGetTime(NULL,&Ticks);		// get RTC clock right now
	t = *localtime(&nowsecs);
	strftime(strbuf, sizeof(strbuf), "%m/%d/%Y  %H:%M:%S", &t);
	flogf("Program start time: %s.%.3d [ctime: %lu]\n", strbuf, Ticks/40,nowsecs);
	flogf(ProgramDescription);
	
	flogf("PAST # OF STARTUPS                  = %d\n",QUE.STARTUPS);
	flogf("PAST # OF DIVES                     = %d\n",QUE.NUMOFDIVE);
	flogf("PROVOR STATUS                       = %d\n",QUE.STATUS);
	flogf("MAXIMUM # OF PROVOR DIVES           = %d\n",QUE.MAXNUMDIV);
	flogf("MAXIMUM # OF DRIFT POSITION REPORTS = %d\n",QUE.MAXDRFTCL);
	flogf("BLOCK LENGTH FOR DATA TRANSMISSION  = %ld\n",QUE.BLKLNGTH);
	
	//**Exceeded maximun number of dives. Take care the reset situation after all the 
	//**dives are done so that the float does not repeat the dives
	//**Reset dive conditions
	QUE.GOBOTTOM = 0;
	QUE.GOPARK   = 0;
	if(QUE.NUMOFDIVE >= QUE.MAXNUMDIV) 
		{
		if(QUE.GOTODIVE != 0)
			{
			QUE.GOTODIVE = 0;
			sprintf(strbuf, "%u", QUE.GOTODIVE);
			VEEStoreStr(QUE_GOTODIVE_NAME,strbuf);//Record GoToDive result
			}
		}
	
	if(QUE.GOTODIVE!=1)
		flogf("WARNING! DIVING IS DISABLED. CHECK QUE.GOTODIVE AND QUE.NUMOFDIVE.\n");
	if(LogOnly)
		flogf("WARNING! LogOnly==1. Are you sure?\n");
	if(QUE.PROVTERM==1 && LogOnly && QUE.IRIDTERM==0)
		flogf("WARNING! WILL NOT TRANSMIT DATA.  INTERNALLY LOG ONLY.\n");
	if(QUE.MAXDRFTCL == 0 || QUE.NUMDRFTCL>=QUE.MAXDRFTCL)
		{
		flogf("WARNING! THIS FLOAT DOES NOT REPORT POSITION AFTER MISSION\n");
	 	flogf("CHECK QUE.MAXDRFTCL OR QUE.NUMDRFTCL\n");
		}
	
	//if(QUE.STARTUPS==0)
	//	{ 
		nd=QUE.PREPMIN;
		ndelay=nd*10;//Number min * 60 seconds
		flogf("You have %d seconds to launch\n", ndelay);
	
		for (i = ndelay; i > 0; i--)
			{
			flogf("%u..", i);
			c = SCIRxGetCharWithTimeout(1000);	// 1 second
			TickleSWSR();	// another reprieve
			if (c == '.')
				break;
			if (c == -1)
				continue;
			i = 0;		// any other key ends the timeout
			}
	//	}
	if (i <= 0)
		{
		flogf("\nStarting...\n");
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

	SCIRxFlush();
	SCITxFlush();
	//if(QUE.STARTUPS == 0)//Count down only at the first pass
	//	{
	//	nd=QUE.PREPMIN;
	//	ndelay=nd*60;//Number min * 60 seconds
	//	flogf("You have %d seconds to launch\n", ndelay);
	//	for (i=0;i<nd;i++)
	//		{
	//		LowPowerDelay(600L); 		//each loop waits for 60 seconds
	//		TickleSWSR();
	//		}
	//	}

	return;		// to start acquisition
	
	}	//____ PreRun() ____//

//***********************************************************************************************
// Set IO pins, set SYSCLK, if surface tries to open GPS receiver and set RTC time.  Set
// gain.  Return the current phase.
// 10/25/2004 H. Matsumoto
//***********************************************************************************************
short SetupHardware(void)
	{
	short		err = 0;
	short		result=0;
	short		waitsFlash, waitsRAM, waitsCF;
	ushort		nsRAM, nsFlash, nsCF;
	short		nsBusAdj;
	char		strbuf[20];
	uchar		mirrorpins[] = { 1, 15, 16, 17, 18, 19, 21, 35, 37, 0 };
	bool		TimeUp=false;
		
	DBG( cprintf("SetupHardware");  cdrain();)

//
//	Setup runtime clock speed and optimize wait states
//
	CSSetSysAccessSpeeds(nsFlashStd, nsRAMStd, nsCFStd, WTMODE);
	TMGSetSpeed(SYSCLK);

//	Turn off hardware we're not actively using
	#ifndef DEBUG
		CTMRun(false);				// turn off CTM6 module
		CFEnable(false);			// turn off the CompactFlash card
	#endif
//
//	FORCE FLOATING I/O PINS TO OUTPUTS TO REDUCE CURRENT
//	Sample each I/O pin, and set it back to an output at the same
//	level. This works for everything except driven signals that
//	may change and keeps floaters from wasting power.
	PIOMirrorList(mirrorpins);
	#if (PLATFORM == NOAADAQ3)
		PIOSet(35);	    //same
		PIOSet(37); 	//same
	#endif

	PIOSet(1);		// DS, force as diag
	Initflog(QUE.LOGFILE, LOGTOCON ? true : false);
	
	flogf("\n-------------------------------------------------------------------\n");
	flogf("Program: %s,  Version: %3.1f,  Build: %s %s\n", 
		__FILE__, QUE_VERSION_NUMBERS, __DATE__, __TIME__);
	CSGetSysAccessSpeeds(&nsFlash, &nsRAM, &nsCF, &nsBusAdj);
	flogf("System Parameters: CF2 SN %05ld, PicoDOS %d.%02d, BIOS %d.%02d\n",
		BIOSGVT.CF1SerNum, BIOSGVT.PICOVersion, BIOSGVT.PICORelease,
		BIOSGVT.BIOSVersion, BIOSGVT.BIOSRelease);
	CSGetSysWaits(&waitsFlash, &waitsRAM, &waitsCF);	// auto-adjusted
	flogf("%ukHz nsF:%d nsR:%d nsC:%d adj:%d WF:%-2d WR:%-2d WC:%-2d SYPCR:%02d\n",
		TMGGetSpeed(), nsFlash, nsRAM, nsCF, nsBusAdj,
		waitsFlash, waitsRAM, waitsCF, * (uchar *) 0xFFFFFA21); fflush(NULL); coflush(); ciflush();
	
	if(QUE.STARTUPS == 0) VEEStoreStr(QUE_LASTGPS_NAME, "0");

	//Increment the number of startups
	sprintf(strbuf, "%u", QUE.STARTUPS + 1);
	VEEStoreStr(QUE_STARTUPS_NAME, strbuf);

	//phase = 0;//debug dummy
	//if(ProvTerm && !LogOnly) OpenCommProv();
	if(ProvTerm && !ProvComOpen) ProvComOpen=OpenCommProv();
	TickleSWSR();

	//Check Provor
	//First check its current PROVOR phase
	//If CF2 was reset, PROVOR may be in 1-way comm status. If so, it does not respond to inquiry.
	//The following if statement assumes PROVOR is 2-way from the last PROVOR status
	phase=QUE.STATUS;
	//if(!LogOnly && ProvTerm && (QUE.STATUS==0 || QUE.STATUS==1 || QUE.STATUS==4 || QUE.STATUS==5))
	if(ProvTerm && (QUE.STATUS==0 || QUE.STATUS==1 || QUE.STATUS==4 || QUE.STATUS==5))
		{
		result=10;						//initialize
		TickleSWSR();
		Delay(10);
 		while(result>1)result=FloatDiagonostic(&cpuSt, &intVacuum, &vacSt, \
 		&reservLevel,&tankSt, &batVolt, &batSt, &extPressure, &pSensorSt);				   
		//cprintf("Result=%d\n",result);
		//putflush();
		flogf("PROVOR DIAGONOSTICS\n \
		CPU state =%01d, Int Vac= %04d, Vac State  =%01d \n \
		Rserev Lvl=%04d, Tank St= %01d, Bat Volt   =%05d\n \
		Bat State =%01d, Ext Pres=%05d, pSens State=%01d\n",cpuSt,\
		intVacuum,vacSt,reservLevel,tankSt, batVolt, batSt, extPressure,pSensorSt);
		fflush(NULL);
		if(batVolt < MinBatVolt) 
			{
			flogf("Voltage %dmV is too low. Do not deploy this float.\n", batVolt);
			putflush();
			QUE.GOTODIVE=0;
			VEEStoreStr(QUE_GOTODIVE_NAME,"0");	//Record READY TO DIVE result
			phase = -1;
			}
		//Check if the internal oil tank is empty
		//If not, pump out the oil to the external bladder.	
		//reservLevel = 100;Debug!	
		result=10;
		while(reservLevel > 0 && (QUE.STATUS==0 || QUE.STATUS==1))
			{
			reservLevel=EmptyReservoir(600);
			}

		result=10; 						//initialize
		while(result>1)result=InqProvorStatus(&pStatus, &lvStatus, &phase, \
		&pDur,	&vDur, &pressure);		
		flogf("PROVOR STATUS %d,%d,%05d,%05d,%d,%04d\n",pStatus,lvStatus,pDur,\
		vDur, phase, pressure);
		putflush();
		WritePhase();
		//TestProv();
		}
	else if(ProvTerm) 		//COM is one-way. Phase = 2, 3, 6, 7, 8, or 9.
		{
		//CF2 reset. If PROVOR is in one-way COM and won't respond to inruiry.
		//So let it finish what was doing and wait until the first status mssg 
		//from PROVOR is received.  It may take a few minutes until the next message appears. 
		result=10;
		flogf("SYSTEM RESET. COM ONE-WAY. STATUS =2,3,6,7,8, OR 9. WAIT UP TO %dSEC FOR RESPONSE.\n",longwait);
			RTCDelayMicroSeconds(10000L);
		while(result > 1)
			{
			TimeUp=ListenProvorLowPower(longwait, 1);//it listens only once to check the status
			if(TimeUp)
				{
				flogf("SEND AN INQUIRY\n");RTCDelayMicroSeconds(10000L);
				result=InqProvorStatus(&pStatus, &lvStatus, &phase, &pDur,	&vDur, &pressure);		
				if(result<=1)
					{
					flogf("PROVOR STATUS %d,%d,%05d,%05d,%d,%04d\n",pStatus,lvStatus,pDur,\
					vDur, phase, pressure);
					putflush();
					WritePhase();
					}
					else
					{
					flogf("NO RESPONSE FROM PROVOR. CLOSE & OPEN PROV COMM, & TRY AGAIN.\n");RTCDelayMicroSeconds(10000L);
					//Close and open PROVOR COMM
					if(ProvComOpen)ProvComOpen=CloseCommProv();
					LowPowerDelay(40);
					ProvComOpen=OpenCommProv();
					LowPowerDelay(50);
					}
				}
			else
				{	
				//Got response from PROVOR
				if(phase != QUE.STATUS)	WritePhase();
				result=0;	//added 7/18/06
				flogf("FOUND PHASE = %d.  CONINUE...\n", phase);
				}
			}
		//This is where you may need to reset PROVOR if no respose
		//.....
		}
		
	//Continue deployment	
	if(phase ==0) 
		{
		if(QUE.STARTUPS==0)
			{
			flogf("**************  READY TO DEPLOY ****************\n");
			cdrain(); coflush();
			}
		}
		
	//Set up pre-amp gain 
	// 0= gain (both pins high), 3= both pins low
	PIOSet(GAIN0);
	PIOSet(GAIN1);
	if(QUE.GAIN == 1){ 		//+6dB
		PIOClear(GAIN0); 	//Secure way (61/6/2004 HM)	
		}
	if(QUE.GAIN == 2){  	//+12dB
		PIOClear(GAIN1); 	//Secure way (6/16/2004 HM)
		}
	if(QUE.GAIN == 3){  	//+18dB
		PIOClear(GAIN0);
		PIOClear(GAIN1); 	//Secure way (6/16/2004. HM)
		}		

	//Display the control variables we'll be using
	//DisplayParameters();
  
  //**Enable IRQ5 to interrupt
	//PITSet100usPeriod(PITOff);		// disable timer to drop power
	//PITRemoveChore(0);				// get rid of all current chores
	//IEVInsertCFunct(&ExtFinishPulseRuptHandler, level5InterruptAutovector);
	//PinBus(IRQ5);					// enable PBM (IRQ5) interrupt button

#ifdef DEBUG
	flogf("************** QUE DEBUG ENABLED ***************\n");
#endif
	return phase;
	}	//____ SetupHardware() ____//

/******************************************************************************\
**	QUEGetSettings		Read in settings from VEE or use defaults if not found
**
**  1-st release 9/12/99
**  2nd 9/18/02 HM
\******************************************************************************/
void QUEGetSettings(void)
	{
	char  *p;
	short intLat, intLon;
	float LatMin, LonMin, LastLatDeg, LastLonDeg;
	char  ns, ew, b1;
	
	CLEAR_OBJECT(QUE); 		//zero entire structure (see <cfxbios.h>
	if(sizeof(QUE) != QUE_USER_HDR_SIZE)
		{
		uprintf("QUE parameter size does not match %d. Supposed to be %d\n",\
		(short) sizeof(QUE), QUE_USER_HDR_SIZE);
		exit(1);
		}
	p = VEEFetchData(QUE_PLTFRMID_NAME).str;
	strncpy(QUE.PLTFRMID, p ? p : QUE_PLTFRMID_DEFAULT, sizeof(QUE.PLTFRMID));
	DBG( uprintf("QUE.PLTFRMID=%s (%s)\n", QUE.PLTFRMID, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_STARTUPS_NAME).str;
	QUE.STARTUPS = atoi(p ? p : QUE_STARTUPS_DEFAULT);
	DBG( uprintf("QUE.STARTUPS=%d (%s)\n", QUE.STARTUPS, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_NUMOFDIVE_NAME).str;
	QUE.NUMOFDIVE= atoi(p ? p : QUE_NUMOFDIVE_DEFAULT);
	DBG( uprintf("QUE.NUMOFDIVE=%d (%s)\n", QUE.NUMOFDIVE, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_IRIDTERM_NAME).str;
	QUE.IRIDTERM= atoi(p ? p : QUE_IRIDTERM_DEFAULT);
	DBG( uprintf("QUE.IRIDTERM=%d (%s)\n", QUE.IRIDTERM, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_LOGONLY_NAME).str;
	QUE.LOGONLY= atoi(p ? p : QUE_LOGONLY_DEFAULT);
	DBG( uprintf("QUE.LOGONLY=%d (%s)\n", QUE.LOGONLY, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_PROVTERM_NAME).str;
	QUE.PROVTERM= atoi(p ? p : QUE_PROVTERM_DEFAULT);
	DBG( uprintf("QUE.PROVTERM=%d (%s)\n", QUE.PROVTERM, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_MAXSTRTS_NAME).str;
	QUE.MAXSTRTS = atoi(p ? p : QUE_MAXSTRTS_DEFAULT);
	DBG( uprintf("QUE.MAXSTRTS=%d (%s)\n", QUE.MAXSTRTS, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_GOMONITOR_NAME).str;
	QUE.GOMONITOR = atoi(p ? p : QUE_GOMONITOR_DEFAULT);
	DBG( uprintf("QUE.GOMONITOR=%u (%s)\n", QUE.GOMONITOR, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_SRATEHZ_NAME).str;
	QUE.SRATEHZ = atoi(p ? p : QUE_SRATEHZ_DEFAULT);
	DBG( uprintf("QUE.SRATEHZ=%u (%s)\n", QUE.SRATEHZ, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_PREPMIN_NAME).str;
	QUE.PREPMIN = atoi(p ? p : QUE_PREPMIN_DEFAULT);
	DBG( uprintf("QUE.PREPMIN=%u (%s)\n", QUE.PREPMIN, p ? "vee" : "def");  cdrain();)
 
	p = VEEFetchData(QUE_SAMPLES_NAME).str;
	QUE.SAMPLES = atoi(p ? p : QUE_SAMPLES_DEFAULT);
	DBG( uprintf("QUE.SAMPLES=%u (%s)\n", QUE.SAMPLES, p ? "vee" : "def");  cdrain();)
	if(QUE.SAMPLES != 0 && QUE.SAMPLES != 3)
		{
		uprintf("\nQUE.SAMPLES is wrong.  Check QUE.SAMPLES.\n");cdrain();coflush();
		abort();
		}
	 
	p = VEEFetchData(QUE_LOGFILE_NAME).str;
	strncpy(QUE.LOGFILE, p ? p : QUE_LOGFILE_DEFAULT, sizeof(QUE.LOGFILE));
	DBG( uprintf("QUE.LOGFILE=%s (%s)\n", QUE.LOGFILE, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_LATITUDE_NAME).str;
	strncpy(QUE.LATITUDE, p ? p : QUE_LATITUDE_DEFAULT, sizeof(QUE.LATITUDE));
	DBG( uprintf("QUE.LATITUDE=%s (%s)\n", QUE.LATITUDE, p ? "vee" : "def");  cdrain();)
	sscanf(QUE.LATITUDE,"%c%2d%c%f", &ns, &intLat,&b1,&LatMin);
	//printf("%c%.2d:%5.2f\n",ns,intLat,LatMin);
	LastLatDeg=intLat+LatMin/60.;
	if(ns=='s' || ns=='S')LastLatDeg=-LastLatDeg;
	//printf("%f\n", LatDeg);

	p = VEEFetchData(QUE_LONGITUDE_NAME).str;
	strncpy(QUE.LONGITUDE, p ? p : QUE_LONGITUDE_DEFAULT, sizeof(QUE.LONGITUDE));
	DBG( uprintf("QUE.LONGITUDE=%s (%s)\n", QUE.LONGITUDE, p ? "vee" : "def");  cdrain();)
	sscanf(QUE.LONGITUDE,"%c%3d%c%f", &ew,&intLon,&b1, &LonMin );
	//printf("%c%.3d:%5.2f\n",ew,intLon, LonMin);
	LastLonDeg=intLon+LonMin/60.;
	if(ew=='e' || ew=='W')LastLonDeg=-LastLonDeg;
	//printf("%f\n",LonDeg);

	p = VEEFetchData(QUE_DRIFTLAT_NAME).str;
	QUE.DRIFTLAT = atoi(p ? p : QUE_DRIFTLAT_DEFAULT);
	DBG( uprintf("QUE.DRIFTLAT=%u (%s)\n", QUE.DRIFTLAT, p ? "vee" : "def");  cdrain();)
	if(QUE.DRIFTLAT !=0 && QUE.STARTUPS == 0)
		{
		uprintf("\QUE.DRIFTLAT should be zero, but %d\n",QUE.DRIFTLAT);cdrain();coflush();
		abort();
		}

	p = VEEFetchData(QUE_DRIFTLON_NAME).str;
	QUE.DRIFTLON = atoi(p ? p : QUE_DRIFTLON_DEFAULT);
	DBG( uprintf("QUE.DRIFTLON=%d (%s)\n", QUE.DRIFTLON, p ? "vee" : "def");  cdrain();)
	if(QUE.DRIFTLON !=0 && QUE.STARTUPS == 0)
		{
		uprintf("\QUE.DRIFTLON should be zero, but %d\n",QUE.DRIFTLON);cdrain();coflush();
		abort();
		}
		
	p = VEEFetchData(QUE_GAIN_NAME).str;
	QUE.GAIN = atoi(p ? p : QUE_GAIN_DEFAULT);
	DBG( uprintf("QUE.GAIN=%u (%s)\n", QUE.GAIN, p ? "vee" : "def");  cdrain();)
	if(QUE.GAIN<0 || QUE.GAIN>3)
		{
		uprintf("\nGain is wrong.  Check QUE.GAIN.\n");cdrain();coflush();
		abort();
		}

	p = VEEFetchData(QUE_EXPID_NAME).str;
	strncpy(QUE.EXPID, p ? p : QUE_EXPID_DEFAULT, sizeof(QUE.EXPID));
	DBG( uprintf("QUE.EXPID=%s (%s)\n", QUE.EXPID, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_PROJID_NAME).str;
	strncpy(QUE.PROJID, p ? p : QUE_PROJID_DEFAULT, sizeof(QUE.PROJID));
	DBG( cprintf("QUE.PROJID=%s (%s)\n", QUE.PROJID, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_PROG_NAME).str;
	strncpy(QUE.PROGNAME, p ? p : QUE_PROG_DEFAULT, sizeof(QUE.PROGNAME));
	DBG( uprintf("QUE.PROGNAME=%s (%s)\n", QUE.PROGNAME, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_BATHYF_NAME).str;
	strncpy(QUE.BATHYF, p ? p : QUE_BATHYF_DEFAULT, sizeof(QUE.BATHYF));
	DBG( uprintf("QUE.BATHYF=%s (%s)\n", QUE.BATHYF, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_MAXDEPTH_NAME).str;
	QUE.MAXDEPTH= atoi(p ? p : QUE_MAXDEPTH_DEFAULT);
	DBG( uprintf("QUE.MAXDEPTH=%d (%s)\n", QUE.MAXDEPTH, p ? "vee" : "def");  cdrain();)
	
	p = VEEFetchData(QUE_FILECOUNT_NAME).str;
	QUE.FILECOUNT= atoi(p ? p : QUE_FILECOUNT_DEFAULT);
	DBG( uprintf("QUE.FILECOUNT=%d (%s)\n", QUE.FILECOUNT, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_FILENUM_NAME).str;
	QUE.FILENUM= atoi(p ? p : QUE_FILENUM_DEFAULT);
	DBG( uprintf("QUE.FILENUM=%d (%s)\n", QUE.FILENUM, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_MAXFILE_NAME).str;
	QUE.MAXFILE= atoi(p ? p : QUE_MAXFILE_DEFAULT);
	DBG( uprintf("QUE.MAXFILE=%d (%s)\n", QUE.MAXFILE, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_PARKDEPTH_NAME).str;
	QUE.PARKDEPTH= atoi(p ? p : QUE_PARKDEPTH_DEFAULT);
	DBG( uprintf("QUE.PARKDEPTH=%d (%s)\n", QUE.PARKDEPTH, p ? "vee" : "def");  cdrain();)

	if(QUE.PARKDEPTH<0 || QUE.PARKDEPTH>1918)
		{
		uprintf("\nParking depth is wrong. Should be >0 and <1917\n");
		abort();
		}

	p = VEEFetchData(QUE_LASTGPS_NAME).str;
	QUE.LASTGPS = atoi(p ? p : QUE_LASTGPS_DEFAULT);
	DBG( uprintf("QUE.LASTGPS = %d (%s)\n", QUE.LASTGPS, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_NUMOFSAT_NAME).str;
	QUE.NUMOFSAT = atoi(p ? p : QUE_NUMOFSAT_DEFAULT);
	DBG( uprintf("QUE.NUMOFSAT = %d (%s)\n", QUE.NUMOFSAT, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_NUMOFGPS_NAME).str;
	QUE.NUMOFGPS = atoi(p ? p : QUE_NUMOFGPS_DEFAULT);
	DBG( uprintf("QUE.NUMOFGPS = %d (%s)\n", QUE.NUMOFGPS, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_GOTODIVE_NAME).str;
	QUE.GOTODIVE = atoi(p ? p : QUE_GOTODIVE_DEFAULT);
	DBG( uprintf("QUE.GOTODIVE = %d (%s)\n", QUE.GOTODIVE, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_GPSWARM_NAME).str;
	QUE.GPSWARM = atoi(p ? p : QUE_GPSWARM_DEFAULT);
	DBG( uprintf("QUE.GPSWARM = %d (%s)\n", QUE.GPSWARM, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_LANDPHNUM_NAME).str;
	strncpy(QUE.LANDPHNUM, p ? p : QUE_LANDPHNUM_DEFAULT, sizeof(QUE.LANDPHNUM));
	DBG( uprintf("QUE.LANDPHNUM=%s (%s)\n", QUE.LANDPHNUM, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_MAXCALLS_NAME).str;
	QUE.MAXCALLS= atoi(p ? p : QUE_MAXCALLS_DEFAULT);
	DBG( uprintf("QUE.MAXCALLS=%d (%s)\n", QUE.MAXCALLS, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_NUMDRFTCL_NAME).str;
	QUE.NUMDRFTCL= atoi(p ? p : QUE_NUMDRFTCL_DEFAULT);
	DBG( cprintf("QUE.NUMDRFTCL=%d (%s)\n", QUE.NUMDRFTCL, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_MAXDRFTCL_NAME).str;
	QUE.MAXDRFTCL= atoi(p ? p : QUE_MAXDRFTCL_DEFAULT);
	DBG( cprintf("QUE.MAXDRFTCL=%d (%s)\n", QUE.MAXDRFTCL, p ? "vee" : "def");  cdrain();)
	
	p = VEEFetchData(QUE_LASTTRNS_NAME).str;
	QUE.LASTTRNS= atoi(p ? p : QUE_LASTTRNS_DEFAULT);
	DBG( uprintf("QUE.LASTTRNS=%d (%s)\n", QUE.LASTTRNS, p ? "vee" : "def");  cdrain();)
	if(QUE.LASTTRNS != 1	&& QUE.STARTUPS == 0)
		{
		uprintf("\nThe result of past transmission is not 1.  Set QUE.LASTTRNS=1\n");
		//abort();
		}

	p = VEEFetchData(QUE_STATUS_NAME).str;
	QUE.STATUS= atoi(p ? p : QUE_STATUS_DEFAULT);
	DBG( uprintf("QUE.STATUS=%d (%s)\n", QUE.STATUS, p ? "vee" : "def");  cdrain();)
	if(QUE.STATUS != 0 && QUE.STARTUPS == 0)
		{
		uprintf("\nPROVOR phase is not 0.  Set QUE.STATUS=0.\n");
		//abort();
		}
		
	p = VEEFetchData(QUE_SECDRFTCL_NAME).str;
	QUE.SECDRFTCL= atoi(p ? p : QUE_SECDRFTCL_DEFAULT);
	DBG(uprintf("QUE.SECDRFCL=%d (%s)\n", QUE.SECDRFTCL, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_SURFSAMPL_NAME).str;
	QUE.SURFSAMPL = atoi(p ? p : QUE_SURFSAMPL_DEFAULT);
	DBG(uprintf("QUE.SURFSAMPL=%d (%s)\n", QUE.SURFSAMPL, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_PARKSAMPL_NAME).str;
	QUE.PARKSAMPL = atoi(p ? p : QUE_PARKSAMPL_DEFAULT);
	DBG(uprintf("QUE.PARKSAMPL=%d (%s)\n", QUE.PARKSAMPL, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_MAXNUMDIV_NAME).str;
	QUE.MAXNUMDIV= atoi(p ? p : QUE_MAXNUMDIV_DEFAULT);
	DBG(uprintf("QUE.MAXNUMDIV=%d (%s)\n", QUE.MAXNUMDIV, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_IRIDWARM_NAME).str;
	QUE.IRIDWARM= atoi(p ? p : QUE_IRIDWARM_DEFAULT);
	DBG(uprintf("QUE.IRIDWARM=%d (%s)\n", QUE.IRIDWARM, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_ESTMDEPTH_NAME).str;
	QUE.ESTMDEPTH = atoi(p ? p : QUE_ESTMDEPTH_DEFAULT);
	DBG(uprintf("QUE.ESTMDEPTH=%d (%s)\n", QUE.ESTMDEPTH, p ? "vee" : "def");  cdrain();)
	if((QUE.ESTMDEPTH <0 || QUE.ESTMDEPTH >=2013) && QUE.STARTUPS == 0)
		{
		flogf("\nDive depth is out of range.  Check QUE.ESTMDEPTH.\n");
		//abort();
		}

	p = VEEFetchData(QUE_GOPARK_NAME).str;
	QUE.GOPARK= atoi(p ? p : QUE_GOPARK_DEFAULT);
	DBG(uprintf("QUE.GOPARK=%d (%s)\n", QUE.GOPARK, p ? "vee" : "def");  cdrain();)

	p = VEEFetchData(QUE_GOBOTTOM_NAME).str;
	QUE.GOBOTTOM= atoi(p ? p : QUE_GOBOTTOM_DEFAULT);
	DBG(uprintf("QUE.GOBOTTOM=%d (%s)\n", QUE.GOBOTTOM, p ? "vee" : "def");  cdrain();)

//	CFPPBSZ		CompactFlash output file in bytes.
	QUE.CFPPBSZ = VEEFetchLong(QUE_CFPPBSZ_NAME, atol(QUE_CFPPBSZ_DEFAULT));
		DBG( cprintf("\nQUE.CFPPBSZ=%lu\n", QUE.CFPPBSZ);  cdrain();)

//	Block length for transmission in bytes.
	QUE.BLKLNGTH = VEEFetchLong(QUE_BLKLNGTH_NAME, atol(QUE_BLKLNGTH_DEFAULT));
		DBG( cprintf("QUE.BLKLNGTH=%lu\n", QUE.BLKLNGTH);  cdrain();)

	//if((QUE.CFPPBSZ+256L) > ((long) QUE.BLKLNGTH * 64L))
	//	{
	//	flogf("QUE.CFPPBSZ exceeded the max file size to transmit. CFPPBSZ<=64*BLKLNGTH+256\n");
	//	abort();
	//	//return -1;
	//	}			
//	RAMPPBSZ	RAM ping-pong buffer size in bytes.
	QUE.RAMPPBSZ = VEEFetchLong(QUE_RAMPPBSZ_NAME, atol(QUE_RAMPPBSZ_DEFAULT));
		DBG( cprintf("\nQUE.RAMPPBSZ=%lu\n", QUE.RAMPPBSZ);  cdrain();)

	if(QUE.RAMPPBSZ>QUE.CFPPBSZ)
		{
		flogf("QUE.RAMPPBSZ is larger than QUE.CFPPBSZ\n");
		abort();
		}
//	MINFREEKB	Minimum free KB to continue use of current hard disk volume
	QUE.MINFREEKB = VEEFetchLong(QUE_MINFREEKB_NAME, atol(QUE_MINFREEKB_DEFAULT));
	if (QUE.MINFREEKB == 0)	// Fill in default if not explicitly defined 
		QUE.MINFREEKB = QUE.CFPPBSZ / KB_SIZE;
		DBG( cprintf("QUE.MINFREEKB=%lu\n", QUE.MINFREEKB);  cdrain();)

	}	//____ QUEGetSettings() ____//

/*************************************************************************************
**	WhatTime
**
**	Returns 20-char long current timein RTC time in GMT format. 
**
**	|RTC__--------------|
**	104 245:13:34:45:100
**  123456789012345678901
**           1         2         3         4
**	NOAA, Nerport, OR
**	Haru Matsumoto 1-st version 10/26/04	hm
*************************************************************************************/
ulong WhatTime(char *GMT_char, ushort *RTCms)
	{
	RTCtm	*rtc_time;				//CFX's real time clock
	ushort	Ticks;

	SecondsSince1970=RTCGetTime(&SecondsSince1970, &Ticks);
	rtc_time = RTClocaltime (&SecondsSince1970);
	*RTCms= TICKS_TO_MS(Ticks);
	sprintf(GMT_char,"%.3d %.3d:%.2d:%.2d:%.2d:%.3u",
	rtc_time->tm_year, rtc_time->tm_yday + 1,\
	rtc_time->tm_hour,rtc_time->tm_min,rtc_time->tm_sec, *RTCms);
	return SecondsSince1970;
	}	//WhatTime
//***************************************************************************
// TestProv
// Test PROVOR before submerging
// H. Matsumoto
//***************************************************************************
void TestProv(void)
{
	ProvComOpen=OpenCommProv();
	
	Delay(1);						//Wait COM1 to be ready
	result=10; 						//initialize
	while(result>1)result=GetExtPressure(&extPressure);
	//cprintf("Result=%d\n",result);
	//putflush();
	uprintf("External pressure %ddB\n",extPressure);
	putflush();

	result=10;						//initialize
	while(result>1)result=GetIntVacuum(&intVacuum);
	//uprintf("Result=%d\n",result);
	//putflush();
	uprintf("Internal vacuum pressure %dmBars\n",intVacuum);
	putflush();

	result=10;						//initialize
	valvDuration=15;
	while(result>1)result=ActivateValve(valvDuration);
	//uprintf("Result=%d\n",result);
	//putflush();
	uprintf("Valve active period %ddS\n",valvDuration);
	putflush();

	result=10;						//initialize
	pumpDuration=10;
 	//while(result>1)result=ActivatePump(pumpDuration);
	//uprintf("Result=%d\n",result);
	//putflush();
	uprintf("Pump active period %ddS\n",pumpDuration);
	putflush();

	result=10;						//initialize
	RTCDelayMicroSeconds(1500000L);
 	while(result>1)result=FloatDiagonostic(&cpuSt, &intVacuum, &vacSt, &reservLevel,\
 	     &tankSt, &batVolt, &batSt, &extPressure, &pSensorSt);				   
	//uprintf("Result=%d\n",result);
	//putflush();
	uprintf("Diagonostic %01d %04d %01d %04d %01d %05d %01d %05d %01d\n",cpuSt,intVacuum,\
	vacSt,reservLevel,tankSt, batVolt, batSt, extPressure,pSensorSt);
	putflush();

	result=10;						//initialize
	while(result>1)result= InqProvorStatus(&pStatus, &lvStatus, &phase, &pDur,\
		&vDur, &pressure);				   
	//uprintf("Result=%d\n",result);
	//putflush();
	uprintf("Status %d,%d,%05d,%05d,%d,%04d\n",pStatus,lvStatus,pDur,vDur,phase,pressure);
	putflush();

	result=10;
	while(result>1)result=BatteryCheck(&batVolt);
	//uprintf("Result=%d\n",result);
	//putflush();
	uprintf("Battery voltage =%dmV\n",batVolt);
	putflush();
	
	result=10;
	while(result>1)result=GetReservoirLevel(&reservLevel);
	//uprintf("Result=%d\n",result);
	//putflush();
	uprintf("Reservoir Level %d cm3\n", reservLevel);
	putflush();
	ProvComOpen=CloseCommProv();
}//TestProv
/******************************************************************************\
**	Irq2RxISR			Interrupt handler for IRQ2 (tied to CMOS RxD)
**	
**	This single interrupt service routine handles both the IRQ2 interrupt
**	and the very likely spurious interrupts that may be generated by the
**	repeated asynchronous and non-acknowledged pulses of RS-232 input.
**	
**	The handler simply reverts IRQ2 back to input (to prevent further level
**	sensative interrupts) and returns. It's assumed the routine that set this
**	up is just looking for the side effect of breaking the CPU out of STOP
**	or LPSTOP mode.
**	
**	Note that this very simple handler is defined as a normal C function
**	rather than an IEV_C_PROTO/IEV_C_FUNCT. We can do this because we know
**	(and can verify by checking the disassembly) that is generates only
**	direct addressing instructions and will not modify any registers.
\******************************************************************************/
static void Irq2RxISR(void)
	{

	PinIO(IRQ2);		// 41 // /IRQ2 (tied to Rx from PROVOR to CF2)
	RTE();

	}	//____ Irq2RxISR() ____//
//*********************************************************************************
// FindPROVORPhase(void)
// Get the status of PROVOR - Uses more power than the ListenProvorLowPower
//*********************************************************************************
short FindPROVORPhase(void)
{
short	result;	
	result=10;				// Initialize, must be >1
	while(result>1)result= RecProvorStatus(&pStatus, &lvStatus, &phase, &pDur,\
	&vDur, &pressure);				   
	//flogf("Status check error =%d\n",result);
	//putflush();
	flogf("STATUS %d,%d,%05d,%05d,%d,%04d\n",pStatus,lvStatus,pDur,vDur,phase,pressure);
	return phase;
}	
//******************************************************************************	
// ListenProvorLowPower
// Keeps CF2 low power interrupt mode while listening to Provor using IRQ2.
// It awaits until specified "waitSec" is up or receives messages from Provor up
// to RepeatMax times.  If nothing happens during "waitSec", it sends an inquirey 
// to PROVOR.  "waitSec" is approximately 300 (set 330) sec during 3, 6 and 7. 
// 							= 120 (set 140) sec during 2, 8 and 9.
// H. Matsumoto, 2/24/2005 NOAA
//******************************************************************************
bool ListenProvorLowPower(short waitSec, short RepeatMax)
	{
	short	i;
	short 	result;
	short	Repeat;
	short	InqMax=2;
	bool	TimeUp;
	//ushort	msec;
	//ulong	Count=0L;
	
	TickleSWSR();
	PinSet(DFLIP);				//Reset D-F/F by TPU 7 (pin 28) /Q is high
	RTCDelayMicroSeconds(10000L);
	//PinClear(DFLIP);//Ready to take interrupt

	//shutting down the power
	//SCITxWaitCompletion();		// let any pending UART data finish

	PITSet100usPeriod(PITOff);	// disable 100us timer
	PITRemoveChore(0);			// get rid of all current chores
	IEVInsertAsmFunct(Irq2RxISR, level2InterruptAutovector);
	IEVInsertAsmFunct(Irq2RxISR, spuriousInterrupt);
	
	//The follwings made no difference to the power usage
	//CTMRun(false);			// turn off CTM6 module
	//QSMStop();				// shut down the QSM
	//BIAShutDown();			// shut down the hard drive

	pressure = 0;
	Repeat   = 0;
	//for (i=0;i<2;i++)			// debug repeat 3 times
	while(phase == QUE.STATUS && Repeat < RepeatMax)
		{
		//PIOClear(ProvRX);		//Set PROVOR RX input zero to reduce current

		PinClear(DFLIP);		//Ready to take an interrupt
		RTCDelayMicroSeconds(5L);
		while(PinRead(DFLIP)){PinClear(DFLIP);RTCDelayMicroSeconds(5);}	

		EIAForceOff(true);		// turn off the RS232 driver
		CFEnable(false);		// turn off the CompactFlash card-APP code hangs 
		PinBus(IRQ2);		    // IRQ2 bus pin enabled
		//while(PinTestIsItBus(IRQ2)==0){PinBus(IRQ2);}		
		
		TimingRuptCount=0L;
		//PITRemoveChore(0);			// get rid of all current chores
		PITSet51msPeriod(PITOff);	// disable timer (drops power)
		TimeUp = false;
		//IEVInsertCFunct(&TimingRuptHandler, pitVector);//This is for a spurious interrupt
		PITAddChore(&TimingRuptHandler, 1);//This is for PIT handler
		PITSet51msPeriod(20);	// 1020msec interval timing pulse ~1 sec
		TickleSWSR();			// Ping to keep active & safe from watch-dog reset 
		//while(PinTestIsItBus(IRQ2)==0){PinBus(IRQ2);}
		while(PinRead(IRQ2) && !TimeUp)    // we saw TU serial activity at PROVOR
			{
			while(PinTestIsItBus(IRQ2)==0){PinBus(IRQ2);}//You really need this "while.."		
			LPStopCSE(CPUStop); // Only CPUStop works. We are here until interrupted
			if(((short)TimingRuptCount) > waitSec)TimeUp = true;//waiting is over 
			}
		//PinIO(34); 			// Set PROV RX as IO 
		PinSet(DFLIP);			// Reset D-F/F again
		PinRead(IRQ2);
		PITSet51msPeriod(PITOff);	
		if(!TimeUp) 			// Received response in time by IRQ interrupt 
			{
			result = RecProvorStatus(&pStatus, &lvStatus, &phase, &pDur,\
			&vDur, &pressure);
			TotalvDur += vDur;
			TotalpDur += pDur;
			}
		else	//Time expired. PROVOR may be 2-way. Send an inquirey.
			{	//if no response, CF2 resets.
			result=10; 	//initialize
			i=0;		//Revised 11/04/05
			while(result>1 && i<=InqMax)
				{
				result = InqProvorStatus(&pStatus, &lvStatus, &phase, \
				&pDur, &vDur, &pressure);
				i++;
				}
				//ResetProv();//Last resort 11/04/05		
			}				   
		PITRemoveChore(&TimingRuptHandler);		// get rid of the chore
		TickleSWSR();				// Ping to keep active & safe from watch-dog reset 
		//PinRead(IRQ2);  			// disable interrupt by IRQ2
		EIAForceOff(false);			// turn ON the RS232 driver
		CFEnable(true);				// turn ON the CompactFlash card
		WhatTime(GMT_char, NULL);
		flogf("%.20s ",GMT_char);cdrain();coflush();
		if(TimeUp)flogf("TIME EXPIRED. INQUIRED PROVOR STATUS\n"); cdrain(); coflush();
		RTCDelayMicroSeconds(10000L);
		
		if(result == 0)
			{
			flogf("STATUS %d,%d,%05d,%05d,%d,%04d\n",pStatus,lvStatus,pDur,vDur,phase,pressure);
			}
			else if(phase==3 || phase==6 || phase==7)
			{
			flogf("NO RESPONSE OR UNEXPECTED RESULT FROM PROVOR- ERR COUNT=%d STATUS=%d\n", result, phase);
			putflush();
			result=10; 	//initialize
			flogf("INQUIRE PROVOR STATUS\n");cdrain();coflush();
			result=InqProvorStatus(&pStatus, &lvStatus, &phase, \
					&pDur,	&vDur, &pressure);		
			flogf("PROVAR STATUS %d,%d,%05d,%05d,%d,%04d\n",pStatus,lvStatus,pDur,\
				vDur, phase, pressure); cdrain();coflush();
			}
		PinSet(DFLIP);			// Reset D-F/F again
		while(!PinRead(DFLIP)){PinSet(DFLIP);RTCDelayMicroSeconds(10);}
		RTCDelayMicroSeconds(1000L);
		Repeat++;
		}
	RTCDelayMicroSeconds(100L);
	while(PinRead(DFLIP)){PinClear(DFLIP);RTCDelayMicroSeconds(10);}// IC pin 1, Ready IRQ2 D Flip Flop
	TickleSWSR();				// another reprieve
	CFEnable(true);				// turn on the CompactFlash card
	EIAForceOff(false);			// turn on the RS232 driver
	PinRead(IRQ2);  			// Disable interrupt by IRQ2
	PITRemoveChore(0);
	return TimeUp;
	//CTMRun(true);				// No difference
	//QSMRun();					// Need QSM
}//ListenProvorLowPower
//*************************************************************************************
//LowPowerDelay
//The following uses 51msec periodic interrupt to delay time (low power)
//Low power 1 to 2mA. 1L delays 102msec. 10L delays 1.020sec, etc.
// Haru Matsumoto (NOAA) 10/13/2004
//*************************************************************************************
void LowPowerDelay(ulong nDelay)
{
	ulong 		TempCount=0L;
	//ulong 		passtime;
	//RTCTimer 	rt;

	SCITxWaitCompletion();		// let any pending UART data finish
//	TMGSetSpeed(8000);

	TimingRuptCount= 0L;
	TimeExpired    =false;
	ProgramFinished=false;
	PITSet51msPeriod(PITOff);	// disable timer
	PITRemoveChore(0);			// get rid of all current chores
	IEVInsertCFunct(&TimingRuptHandler, pitVector);	// replacement fast routine

	//Shut down other unnecessary operations
	CFEnable(false);
	EIAForceOff(true);
	CTMRun(false);		
	//QSMStop();
	
	//RTCElapsedTimerSetup(&rt);
	PITSet51msPeriod(2); //A pulse every 102msec
	while(!TimeExpired) 
	{
		if(ProgramFinished)TimeExpired = true;
		if(TimingRuptCount == TempCount)
			{
			LPStopCSE(FastStop);
			}
			else 
				{
				TempCount=TimingRuptCount;
				if(TimingRuptCount >= nDelay || ProgramFinished) //0.515 sec delay
					{
					TimeExpired = true;
					}
				}
	}

	//passtime=RTCElapsedTime(&rt);
	TimeExpired=false;

	EIAForceOff(false);
	CFEnable(true);
	QSMRun();
	PITSet51msPeriod(PITOff);
	PITRemoveChore(0);
}//LowPowerDelay	
//***************************************************************************************
//Interrupt routine for PIT51msPeriod (low power)
//***************************************************************************************
IEV_C_FUNCT(TimingRuptHandler)
	{
	#pragma unused(ievstack)	// implied (IEVStack *ievstack:__a0) parameter

	if (!TimeExpired)
		{
		TickleSWSR();				// another reprieve//added May 1, 06
		TimingRuptCount++;
		}
	}	//____ TimingRuptHandler() ____//
//*************************************************************************
//	LoadBathymetry()
//	Load the bathymetry grid (dmDim x dimDim) into memory from a file; 
//  estimate the depth at a given coordinates while on the surface
//
//	dmDim dmSpac refLat refLon
//	(0,dmDim-1)----------------(dmDim-1,dmDim-1)
//	|							|
//	|							|
//	|							|
//	|							|	]=dmSpac
//	(0,0)--------------------(dmDim-1,0)
//	dim inc refLon refLat		
//
//	dmDim  = Num of elements in the bathymetry array
//  dmSpac = spacing in 'minutes'
//  refLat = Lat in lower left corner
//	refLon = Lon in lower left corner
//
//  Originally by Walter Hannah
//  revised by HM, NOAA (Mar 14, 2005)
//  revised by HM, NOAA (Feb 3, 2006)
//*************************************************************************
short LoadBathymetry(void)
{
	short	i,j;
	FILE *bathyFile;

	bathyFile=fopen(QUE.BATHYF,"r");
	
	if(bathyFile == NULL)
	{
		flogf("Bathy file could not be opened");cdrain();coflush();
		return -1;
	}
	fflush(NULL);

	fscanf(bathyFile, "%d %f %f %f",&dmDim, &dmSpac, &refLat, &refLon);
	for(j=dmDim-1;j>=0;j--)
		{
		for(i=0;i<dmDim;i++)
		{				
			fscanf(bathyFile,"%d",&bathyArray[i][j]);
			//cprintf("%d ",bathyArray[i][j]);
			//fflush(NULL);
		}
		}
	fclose(bathyFile);
	flogf("BATHYMETRY FILE %s LOADED\n", QUE.BATHYF);cdrain();coflush();
	return 0;
	}
//----------LoadBathymetry-----------------------------------------------------------
//**************************************************************************************
// FindDepth
// Find the depth from bathymetry data from the lat and lon of GPS.  Return a depth 
// (short) in meter.  The depth at a given coordinates is weight-averaged within the 
// nearest four bathy points inside the grid.  Return -1 if the coordinate is outside
// the specified grid.
// Originally written by Walter Hannah, Revised by HM 3/15/2005
//************************************************************************************** 
short FindDepth(float Lat, float Lon)
	//use the bathymetry grid to estimate the depth
	//estimation is a weighted average of four data points that surround
	//the point of the current position
{
	short i,j,i1,j1,im,jm;
	float a,b,c,d,e;		//four corners of grid square going clockwise from top left
	short avDepth;			//final depth estimation
	float Ar,Br,Cr,Dr,Er;	//distance to each corner of grid square from the current position
	float delta;			//decimal degree of grid spacings
	
	//printf("%f %f \n",Lat, Lon);
	//printf("%f %f \n",refLat, refLon);
	//printf("%f\n",dmSpac);
	//putflush();
	
	// (i,j) are our current coodinates on the grid
	delta=dmSpac/60.;//changed dmSpac in degrees
	j= (Lat-refLat)/delta+0.5;
	i= (Lon-refLon)/delta+0.5;
	printf("Coordinate %d %d\n", i,j);

	if(i>=dmDim || j>=dmDim || i<0 || j<0 )	
		{
		//Out of the bathy grid space and we do not know the depth-- what we do?? 
		//Pop up or stay at certain depth until time is up
		QUE.INGRID=0;
		VEEStoreStr(QUE_INGRID_NAME,"0");
		return -1;			
		}
	else
		{	 
		QUE.INGRID=1;
		VEEStoreStr(QUE_INGRID_NAME,"1");
		}
	
	//find the distances to each corner
	i1=i+1;
	if(i1>=dmDim)i1=i;
	j1=j+1;
	if(j1>=dmDim)j1=j;
	im=i-1;
	if(im<0)im=0;
	jm=j-1;
	if(jm<0)jm=0;
	Ar=sqrt(pow(Lat-((float)(j)*delta+refLat),2)+pow(Lon-((float)(i)*delta+refLon),2));
	Br=sqrt(pow(Lat-((float)(j)*delta+refLat),2)+pow(Lon-((float)(i+1)*delta+refLon),2));
	Cr=sqrt(pow(Lat-((float)(jm)*delta+refLat),2)+pow(Lon-((float)(i)*delta+refLon),2));
	Dr=sqrt(pow(Lat-((float)(j+1)*delta+refLat),2)+pow(Lon-((float)(i)*delta+refLon),2));
	Er=sqrt(pow(Lat-((float)(j)*delta+refLat),2)+pow(Lon-((float)(im)*delta+refLon),2));

	//find the depth at each point
	a = bathyArray[i  ][j  ];
	b = bathyArray[i1 ][j  ];
	c = bathyArray[i1 ][j1 ];
	d = bathyArray[i  ][j1 ];
	e = bathyArray[im ][jm ];
	//take the weighted average
	if(Ar == 0.0)avDepth=a;
		else if(Br == 0.0)avDepth=b;
			else if(Cr == 0.0)avDepth=c;
				else if(Dr == 0.0)avDepth=d;
					else if(Er == 0.0)avDepth=e;
					else
					{
					avDepth=(a/Ar+b/Br+c/Cr+d/Dr+e/Er)/(1./Ar+1./Br+1./Cr+1./Dr+1./Er);
					}
	//printf("\ndepth = %d \n",avDepth);
	//printf("\n");
	//printf("a = %f\n",a);
	//printf("b = %f\n",b);
	//printf("c = %f\n",c);
	//printf("d = %f\n",d);
	//printf("\n");
	//printf("Ar = %f\n",Ar);
	//printf("Br = %f\n",Br);
	//printf("Cr = %f\n",Cr);
	//printf("Dr = %f\n",Dr);
	LowPowerDelay(10);
	return avDepth;		
}
//***********************************************************************
void fakeGPS()
{
RTCtm	*rtc_time;				//CFX's real time clock
ushort 	Ticks;
ulong	SecondsSince1970 =  NULL;
ushort	msec;

	//char	temp_chr[21];
//fake data for Axial volcano bathy
AveLat=45.9597;
AveLon=-130.0097;
sprintf(loc_chr,"N45:57.585W130:00.582");
sprintf(LatChar,"N45:57.585");
sprintf(LonChar,"W130:00.582");
//WriteLocEPROM('N', 45, 57.586, 'W', 130, 0.582);//Write new location to VEEPROM

//fake data for HMSC 
/*AveLat=44.78;
AveLon=-123.83;
sprintf(loc_chr,"N44:46.800W123:49.800");
sprintf(LatChar,"N44:46.800");
sprintf(LonChar,"W123:49.800");
*/
//AveLat=44.6243;
//AveLon=-124.0354;
//sprintf(loc_chr,"N44:37.458W124:02.124");
//sprintf(LatChar,"N44:37.458");
//sprintf(LonChar,"W124:02.124");
QUE.RTCTIME    = WhatTime(GMT_char, &QUE.RTCMSEC);
QUE.GPSTIME    = QUE.RTCTIME;
QUE.GPSMSEC    = QUE.RTCMSEC;
BIRS.RTCGPSFIX = QUE.RTCTIME;

RTCGetTime(&SecondsSince1970, &Ticks);
rtc_time = RTClocaltime (&SecondsSince1970);
msec=TICKS_TO_MS(Ticks);

sprintf(time_header," %.3d %.3d:%.2d:%.2d:%.2d:%.3u ",rtc_time->tm_year, rtc_time->tm_yday + 1,\
		rtc_time->tm_hour,rtc_time->tm_min,rtc_time->tm_sec, msec);
sprintf(time_header+22,"%.3d %.3d:%.2d:%.2d:%.2d:%.3u",rtc_time->tm_year, rtc_time->tm_yday + 1,\
		rtc_time->tm_hour,rtc_time->tm_min,rtc_time->tm_sec, msec);
//WriteLocEPROM('N', 44, 46.800, 'W', 123, 49.800);//Write new location to VEEPROM
}
 
void WritePhase()
{
char strbuf[4];	
	WhatTime(GMT_char, NULL);
	flogf("%.20s ",GMT_char);cdrain();coflush();
	if(phase==0)flogf("INIT(0)\n");
	if(phase==1)flogf("SURFACE(1)\n");
	if(phase==2)flogf("SURFACE_DIVE(2)\n");
	if(phase==3)flogf("DIVE(3)\n");
	if(phase==4)flogf("PARKING(4)\n");
	if(phase==5)flogf("GROUNDING(5)\n");
	if(phase==6)flogf("DRIFT_ASCENT(6)\n");
	if(phase==7)flogf("DIRFT_DESCENT(7)\n");
	if(phase==8)flogf("ASCENT(8)\n");
	if(phase==9)flogf("EMERGENCY(9)\n");	
	QUE.STATUS = phase;
	sprintf(strbuf, "%u", phase);
	VEEStoreStr(QUE_STATUS_NAME,strbuf);
}

void WriteSurfacePrepResult()
{
	char strbuf[4];	
	sprintf(strbuf, "%d", QUE.LASTGPS);
	VEEStoreStr(QUE_LASTGPS_NAME,strbuf);			//Record GPSfixOK result
	RTCDelayMicroSeconds(100000L);
	sprintf(strbuf, "%u", QUE.GOPARK);
	VEEStoreStr(QUE_GOPARK_NAME,strbuf);
	RTCDelayMicroSeconds(100000L);
	sprintf(strbuf, "%u", QUE.GOBOTTOM);
	VEEStoreStr(QUE_GOBOTTOM_NAME,strbuf);
}

void WriteLocEPROM(char ns, short intLat, float LatMin, char ew, short intLon, float LonMin)
{
	char	lat[10], lon[12];	
	sprintf(lat,"%c%.2d:%6.3f",ns,abs(intLat),LatMin);
	RTCDelayMicroSeconds(30000L);
	sprintf(lon,"%c%.3d:%6.3f",ew,abs(intLon),LonMin);
	RTCDelayMicroSeconds(30000L);
	VEEStoreStr(QUE_LATITUDE_NAME,lat);//Record lat
	VEEStoreStr(QUE_LONGITUDE_NAME,lon);//Record lat
	VEEStoreStr(BIR_LATITUDE_NAME,lat);//Record lat
	VEEStoreStr(BIR_LONGITUDE_NAME,lon);//Record lat
}
void Get_Last_Dive_Location(float *LastLatDeg, float *LastLonDeg)
{
char  *p, ns, ew, b1;
short intLat, intLon;
float LatMin, LonMin; 
	p = VEEFetchData(QUE_LATITUDE_NAME).str;
	strncpy(QUE.LATITUDE, p ? p : QUE_LATITUDE_DEFAULT, sizeof(QUE.LATITUDE));
	sscanf(QUE.LATITUDE,"%c%2d%c%f", &ns, &intLat,&b1,&LatMin);
	DBG(printf("%c%.2d:%5.2f ",ns,intLat,LatMin));
	*LastLatDeg=intLat+LatMin/60.;
	if(ns=='s' || ns=='S')*LastLatDeg=-*LastLatDeg;
	//DBG(printf("%f\n", *LastLatDeg));

	p = VEEFetchData(QUE_LONGITUDE_NAME).str;
	strncpy(QUE.LONGITUDE, p ? p : QUE_LONGITUDE_DEFAULT, sizeof(QUE.LONGITUDE));
	sscanf(QUE.LONGITUDE,"%c%3d%c%f", &ew,&intLon,&b1, &LonMin );
	DBG(printf("%c%.3d:%5.2f\n",ew,intLon, LonMin));
	*LastLonDeg=intLon+LonMin/60.;
	if(ew=='e' || ew=='W')*LastLonDeg=-*LastLonDeg;
	//DBG(printf("%f\n",*LastLonDeg));
}
//************************************************************************************
// Send_File
// Return 1 if successful, 0 resending, -1 failed. 
//************************************************************************************
short Send_File(char *fname)
{
short	i;
short	Num_Resent;
char 	bitmap[64];//63-byte array corresponds to 64-bit map, val0 and val1
				   //for resending the data block
ulong 	val0, val1;//64-bit bit map for resending the data block
struct	stat info;
ushort 	lastblklngth;
uchar	NumOfBlks;			//Num of data blocks to send <=63
long	filesize;
//short	count;
short   done;

	//Konnect=true;
	done=-1;

		stat(fname,&info);//Get file size info.
		//info.st_size=120;//debug
		flogf("FILE SIZE %ld\n",info.st_size);	
		RTCDelayMicroSeconds(10000L);

		NumOfBlks=info.st_size/QUE.BLKLNGTH;
		filesize = QUE.BLKLNGTH*NumOfBlks; //true file size may not be a multiple of BLKLNGTH*NumOfBlks
		if( filesize < info.st_size)
			{
			lastblklngth=info.st_size - filesize;
			NumOfBlks +=1;
			}
		else if(filesize == info.st_size)lastblklngth=QUE.BLKLNGTH;//true file size same 
		
		if(NumOfBlks >63) 
			{
			NumOfBlks=63;//Limit to 64 blocks
			lastblklngth=QUE.BLKLNGTH;
			}
		flogf("# OF BLKS = %d\n",NumOfBlks);RTCDelayMicroSeconds(10000L);	

		//Set the resend bit field all 1s to send all the blocks.
		for(i=0;i<NumOfBlks;i++) bitmap[63-i]='1';//Set bitmap all to '1s'
		TickleSWSR();
		Send_Blocks(bitmap, NumOfBlks, QUE.BLKLNGTH, lastblklngth, fname);

		//Check if the send-data went OK
		Num_Resent= 0;	//Number of resent trials
		while(done !=1 && Num_Resent<MAX_RESENT)
			{
			TickleSWSR();
			done = Check_If_Done_Or_Resent(&val0, &val1);	//0 == @@@, 1==done, ==-1 something else
			WhatTime(GMT_char, NULL);
			if(done !=1)
				{
				flogf("TX INCOMPLETE %.20s\n", GMT_char);RTCDelayMicroSeconds(10000L);
				if(done == 0) 				//@@@ string received, still need to re-send
					{
					Convert_BitMap_To_CharBuf(val0,val1,bitmap);
					Send_Blocks(bitmap, NumOfBlks, QUE.BLKLNGTH, lastblklngth, fname);
					//Debug
					DBG(for(count=0;count<64;count++)uprintf("%c",bitmap[count]));
					}
				uprintf("\n");
				if(done == -1) Num_Resent=MAX_RESENT;	
				Num_Resent++;
				}
			else flogf("TX COMPLETED %.20s\n",GMT_char);RTCDelayMicroSeconds(10000L);
			}

		if(Num_Resent>=MAX_RESENT)//Resent effort failed more than XX times
			{
			DBG(flogf("RESENT FAILED. %d. \n", Num_Resent));
			}
return done;
}//Send_File
//**********************************************************************************
// Connect
// Set baud rate, check phone status, check the signal quality, and call land
//**********************************************************************************
bool Connect()
	{
	short 	i;
	short	signal_quality;
	short	no_sigQ_chk; 
	short   BaudStatus;
	short	baud;
	short	phone_st;
	bool	Konnect;
	short	returnvalue;

	returnvalue   =0;
	signal_quality=0;
	no_sigQ_chk   =0;
	Konnect       =false;
 	//You need the next to enable Iridium if password is not deactivated
 	//while(EnableIridiumCPIN()==false) {RTCDelayMicroSeconds(1000000L);LowPowerDelay(100L);}
	i=0;
	BaudStatus=-1;
	LowPowerDelay(10L);
	TickleSWSR();
	while(i<5 && BaudStatus==-1)
		{
		if(QUE.LANDPHNUM[2]=='8') 
			{
			BaudStatus = SetIRIDBaud(71);	//For Rudics Set CF2 to Iridium baud rate to 9600 v.110, 71,0,1
			flogf("SETTING FOR RUDICS\n");cdrain();coflush();
			}
		if(QUE.LANDPHNUM[2]=='1') 
			{
			BaudStatus = SetIRIDBaud(7);	//PMEL Irid Set CF2 to Iridium baud rate to 9600 v.32, 7,0,1
			flogf("SETTING PMEL IRIDIUM\n");cdrain();coflush();
			}
		i++;
		}
	LowPowerDelay(10L);
	baud=ReadIRIDBaud();
	//flogf("BD = %d\n", baud);//9600=71
	LowPowerDelay(5L);
	phone_st=PhoneStatus();
	TickleSWSR();
	
	//check IRIDIUM signal quality
	while(signal_quality <MinSQ && no_sigQ_chk < Max_No_SigQ_Chk)
		{
		returnvalue = SignalQuality(&signal_quality);
		LowPowerDelay(100L);//Interval of 10 sec
		TickleSWSR();
		++no_sigQ_chk;
		}
	flogf("SQ = %d\n", signal_quality);
	if(returnvalue==-1)return false;
	RTCDelayMicroSeconds(10000L);
	
	if(signal_quality>=MinSQ && phone_st == 0)
		{
		WhatTime(GMT_char, NULL);
		flogf("IRIDIUM READY %.20s\n", GMT_char);//==0 Ready
		Konnect=Call_Land(QUE.LANDPHNUM);
		}
return Konnect;
}//**Connect**********************************************************************
//*********************************************************************************
// Acknowledge
// Send platform ID and check if Land_Ready signal is received.
// If successful, return true.
// HM, NOAA, CIMRS
//*********************************************************************************
bool Acknowledge()
{
bool 	Ack;
short 	Num_ACK;
short	AckMax = 5;
	//"Connect" mesg is received from IRIDIUM AUX, Proceed to send ID
	Ack = false;
	Num_ACK=0;
	while(Ack == false && Num_ACK < AckMax)		//Repeat 
		{
		TickleSWSR();
		Send_Platform_ID(QUE.PROJID, QUE.PLTFRMID);
		Ack = Check_If_ACK();		//check if Land_Ready signal is aknowledged
		if(Ack==true)flogf("ACKNOWLEDGED\n");
		RTCDelayMicroSeconds(1000000L);//it was 10000L	
		//LowPowerDelay(7L);
		Num_ACK++;
		}
	if(Num_ACK>=AckMax)
		{
		flogf("PLATFORM ID TX FAILED.\n");
		RTCDelayMicroSeconds(20000L);
		LowPowerDelay(2L);	
		TickleSWSR();
		}
	RTCDelayMicroSeconds(10000L);
	return Ack;
}
void Get_Last_Drift(short *DriftLatOld, short *DriftLonOld)
{ 
	char *p;
	p=VEEFetchData(QUE_DRIFTLAT_NAME).str;//in seconds
	QUE.DRIFTLAT=atoi(p ? p : QUE_DRIFTLAT_DEFAULT);
	*DriftLatOld=(float)QUE.DRIFTLAT/3600.;//in degree
	p=VEEFetchData(QUE_DRIFTLON_NAME).str;//in seconds
	QUE.DRIFTLON=atoi(p ? p : QUE_DRIFTLON_DEFAULT);
	*DriftLonOld=(float)QUE.DRIFTLON/3600.;//in degree
}
short Get_GPS_Position_Time(bool SetClock, float *AvLat, float *AvLon, char *time_chr)
{
	float 	AveLat, AveLon;
	bool 	SatFetch;
	short	Num_Calls;
	short	GPSFixOK;
	short	num_sat=0;
	char 	strbuf[4];	

	Num_Calls  = 0;
	GPSFixOK   = 0;
	SatFetch   = false;
	while(GPSFixOK == 0 && Num_Calls < QUE.MAXCALLS)//float is at surface
		{
		SatFetch = GetGPSDateTimeSyncRTC(SetClock, time_chr, &num_sat, &QUE.GPSTIME, &QUE.GPSMSEC, &QUE.RTCTIME, &QUE.RTCMSEC);	//synchronize RTC time to GPS
		if(SatFetch)GPSFixOK = AveGPSLocation(NumGPSAve, loc_chr, LatChar, LonChar, &AveLat, &AveLon);
		++Num_Calls;
		}
	BIRS.RTCGPSFIX=QUE.RTCTIME;	
	QUE.NUMOFGPS  = Num_Calls;
	QUE.NUMOFSAT  = num_sat;	
	BIRS.NUMOFGPS = Num_Calls;
	BIRS.NUMOFSAT = num_sat;	
	sprintf(strbuf, "%u", Num_Calls);
	VEEStoreStr(QUE_NUMOFGPS_NAME,strbuf);
	sprintf(strbuf, "%u", num_sat);
	VEEStoreStr(QUE_NUMOFSAT_NAME,strbuf);
	*AvLat=AveLat;
	*AvLon=AveLon;	
	return GPSFixOK;			
} //Get_GPS_Position_Time
//**********************************************************************************
//EstimateDepthFromDriftBathy
//**********************************************************************************		
short EstimateDepthFromDriftBathy(short *FudgeDepth,short GPSFixOK, float DriftLatOld,\
      float DriftLonOld,float LastLatDeg, float LastLonDeg, short *size_drift_info)
{
char	tmp_chr[6], tmpbuf[2];
float	DriftLat, DriftLon;
float   BathyDepth;
RTCtm	*rtc_time;				//CFX's real time clock

	if(GPSFixOK==1)  //If GPS fix successful. Check the depth
		{                  //12345678901234567890
		strncpy(Drift_Info,"\nGPS FIX SUCCESS ",18);
		rtc_time = RTClocaltime (&BIRS.RTCGPSFIX);
		flogf("GPS FIX SUCCESS %.3d %.3d:%.2d:%.2d:%.2d\n",
			rtc_time->tm_year, rtc_time->tm_yday + 1,\
			rtc_time->tm_hour,rtc_time->tm_min,rtc_time->tm_sec); CIOdrain();

		if(QUE.LASTGPS==1)//GPS prior last dive was OK
			{
			DriftLat = AveLat-LastLatDeg;//check the difference
			DriftLon = AveLon-LastLonDeg;
			if(QUE.NUMOFDIVE==0)	   //first dive
				{
				DriftLatOld=DriftLat;  //No historical drift data. Use the new ones
				DriftLonOld=DriftLon;
				}
			//Update new drift values (moving average)	
			DriftLat=(DriftLat+DriftLatOld)/2.;
			DriftLon=(DriftLon+DriftLonOld)/2.;
			}
	//we are here either last GPS was bad (so drift estimate not reliable) or 
	//the first dive calibrating at surface (no drift record)
		else  
			{
			DriftLat=DriftLatOld; //Use the old drift values
			DriftLon=DriftLonOld;
			}             //1234567890123456789012 
		strncat(Drift_Info,"DRIFT (IN SECOND) WAS ",22);	
		flogf("DRIFT (IN SECOND) %8.2f %8.2f\n", DriftLat*3600., DriftLon*3600.);
		flogf("DRIFT (IN METER ) %8.1f %8.1f\n", DriftLat*110952., DriftLon*110952.*cos(AveLat/57.296));
		//Target position is a half way	
		BathyDepth=FindDepth(AveLat+DriftLat/2.,AveLon+DriftLon/2.);
		*FudgeDepth=BathyDepth*FudgeFactor;//Add 10% error to the estimated depth
		sprintf(tmp_chr, "%.5d", (short)(DriftLat*3600.));
		strncat(Drift_Info,tmp_chr,5);
		VEEStoreStr(QUE_DRIFTLAT_NAME,tmp_chr );
		sprintf(tmp_chr, "%.5d", (short)(DriftLon*3600.));
		strncat(Drift_Info," ",1);
		strncat(Drift_Info,tmp_chr,5);
		strncat(Drift_Info,"\n",2);
		VEEStoreStr(QUE_DRIFTLON_NAME,tmp_chr);
		QUE.LASTGPS = 1;//last GPS fix result
		}
	else //GPS fix was no good this time.  Estimate depth from the past drift.
		{
		QUE.LASTGPS = 0;//last GPS fix result
		                   //1234567890123456789012 
		strncpy(Drift_Info,"\nGPS FIX FAILED.  ",18);
		WhatTime(GMT_char, NULL);
		flogf("GPS FIX FAILED %s\n", GMT_char);
		//Estimate the depth from the past drift. Decide the target depth
		BathyDepth=FindDepth(LastLatDeg+DriftLatOld/2.,LastLonDeg+DriftLonOld/2.);
		*FudgeDepth=BathyDepth*FudgeFactor2; //increase the depth			
		}
	*size_drift_info=sizeof(Drift_Info);
	sprintf(tmpbuf, "%u", QUE.LASTGPS);
	VEEStoreStr(QUE_LASTGPS_NAME,tmpbuf);
		
	return BathyDepth;		
}//Estimate_DepthFromDrift
//************************************************************************************
// Connect_Transmit_Data
// Switch the anteena to IRIDIUM, connect, send ID, hang up 
//************************************************************************************
short  Connect_Transmit_DataFile(char *fname) 
	{
	short icall, iFlow;
	bool  Konnect;
	char  strbuf[7];
	bool  Ack, DisKonnect;
	short IRIDWarm;
	bool  IridFlow;
	
 	TX_Success = 0;
	icall      = 0;
	IRIDWarm   = QUE.IRIDWARM;
	while(TX_Success != 1 && icall < QUE.MAXCALLS)
		{
		if(!SatComOpen) //IridGPS com was not open
			{
			IRIDWarm   = IRIDWarm + 30;//if first TX try fails, increase the warm up
			SatComOpen=OpenComGPSIridium(IRIDWarm);
			}
		iFlow	   = 0;
		IridFlow   = false;
		while(iFlow<3 && !IridFlow)
			{
			iFlow++;
			IridFlow = EnableIridiumFlow();
			}
		LowPowerDelay(20);	
		if(IridFlow)
			{	
			Konnect = Connect();
			if(Konnect)
				{
				TickleSWSR();
				//If 1-st dive - just report ID, position, depth, time and next action
				RTCDelayMicroSeconds(1000000L);
				Ack = Acknowledge();
				if(Ack) 
					{
					TX_Success = Send_File(fname);
					}
					else flogf("CALL DID NOT GO THROUGH\n");RTCDelayMicroSeconds(10000L);
				
				//QUE.GOTODIVE = RcvInst(); //Receive instruction
				//sprintf(strbuf, "%u", QUE.GOTODIVE);
				//VEEStoreStr(QUE_GOTODIVE_NAME,strbuf);//Record GoToDive result
				}
			icall++;
			DisKonnect=false;
			while(!DisKonnect){
				DisKonnect = HangUp();
				LowPowerDelay(20L);
				}
			}
		
			SatComOpen = CloseComIridiumGPS();
			//Connection failed. Try again with delay.
			if(!Ack || !Konnect)LowPowerDelay(100L);
			TickleSWSR();
		}
	BIRS.NUMOFIRID=icall;	
	sprintf(strbuf, "%u", icall);
	VEEStoreStr(BIR_NUMOFIRID_NAME,strbuf);
	sprintf(strbuf, "%u", TX_Success);
	VEEStoreStr(QUE_LASTTRNS_NAME,strbuf); //TX result recorded
	RTCDelayMicroSeconds(100000L);		
	return TX_Success;
}//Connect_Transmit_Data
/******************************************************************************\
**	ExtFinishPulseRuptHandler		IRQ5 logging stop request interrupt
**	
\******************************************************************************/
IEV_C_FUNCT(ExtFinishPulseRuptHandler)
	{
	#pragma unused(ievstack)	// implied (IEVStack *ievstack:__a0) parameter

	PinIO(IRQ5);
	ProgramFinished = true;
	}	//____ ExtFinishPulseRuptHandler() ____//

//The following routines were added to include the acoustic logging function
/******************************************************************************\
**	ADTimingRuptHandler		Initiate conversion
**	
**	This interrupt has a priority level of six, which makes it the highest 
**	priority task in the system.
**	
**	This starts the QSPI which generates a follow up interrupt in about 10uS
**	per channel sampled at 16MHz.
\******************************************************************************/
IEV_C_FUNCT(ADTimingRuptHandler)	// implied (IEVStack *ievstack:__a0) parameter
	{
	#pragma unused(ievstack)

	//DBG( PinClear(25); )
	QSMRun();
	QPBRepeatAsync();	// starts the QSPI running with previous parameters
	//DBG( PinSet(25); )

	}	//____ ADTimingRuptHandler() ____//
//******************************************************************************
// ADWordRuptHander
// An interrupt routine to handle AD8344 A/D converter. 2 byter resolution, up to
// 8 channel stored in QRR register by QSPI routine, and stored in RAM.
//******************************************************************************
IEV_C_FUNCT(ADWordRuptHandler)
	{
	#pragma unused(ievstack)	// implied (IEVStack *ievstack:__a0) parameter
	short			err = 0;
	short			i;
	long			avail;

	QPBClearInterrupt();	// ack and clear the interrrupt that got us here
	if (! ADFinished)
		{
		for (i = FCHAN; i < NCHAN; i++)
			{
		
		//Next line changed for 2-byte data transfer by ADS8344 by HM 06/24/02
			* (ushort *) &ADdata[ADidx] = (QRR[2*i]<<9 | QRR[1+2*i]>>7)& 0xFFFF;
		
			ADidx += sizeof(ushort);
			if (ADidx == 0)					// did we fill ???
				{
				PPBWrite(RAMppb, &ADdata, sizeof(ADdata));
				//err = BIRRecordPage(ADdata);
				avail = DIRFreeSpace(drivestr);
				if (avail < QUE.RAMPPBSZ)
					{
					ADFinished = true;
					goto fexit;
					}
				}
			}
		}
	fexit:
	ADSamplingRuptCount++;

	}	//____ ADWordRuptHandler() ____//


/******************************************************************************\
**	BIRGetSettings		Read in settings from VEE or use defaults if not found
**
**	Look in <CFxRecorder.h> for detailed descriptions of the variables.
\******************************************************************************/
static void BIRGetSettings(void)
	{
	char  *p;
	
//	FLOGFLAG	Log major operating events (when non-zero). Normally enabled.
	BIRS.FLOGFLAG = VEEFetchLong(BIR_FLOGFLAG_NAME, atol(BIR_FLOGFLAG_DEFAULT));
		DBG( cprintf("BIRS.FLOGFLAG=%u\n", BIRS.FLOGFLAG);  cdrain();)

	p = VEEFetchData(BIR_PREVDEPTH_NAME).str;
	BIRS.PREVDEPTH = atoi(p ? p : BIR_PREVDEPTH_DEFAULT);
	DBG( uprintf("BIR.PREVEDEPTH=%d (%s)\n", BIRS.PREVDEPTH, p ? "vee" : "def"); cdrain();)

	}	//____ BIRGetSettings() ____//

//******************************************************************************
// Write_Header
//******************************************************************************
FILE *Write_Header(char *fname)
	{
//	char *Debug=
//	{
//	"123456789a123456789b123456789c123456789d123456789e"
//	"123456789f123456789g123456789h123456789i123456789j"
//	"123456789k123456789l123456789m123456789o123456789p"
//	"123456789q123456789r123456789s123456789t"
//	};

	QUEUpdateHeader();
	if((fp = fopen(fname,"w+")) == NULL)
		{
	 	flogf("Cannot open file\n");//critical error
	 	//Stop logging and come up to the surface
		return false;
		}
	//sprintf(QUE.JUNK,"123456789012345678");//debug	
	flogf("WRITING HEADER TO  %s\n",fname);
	//fwrite(Debug,QUE_USER_HDR_SIZE, 1, fp);
	fwrite(&QUE, QUE_USER_HDR_SIZE, 1, fp);//header
	fclose(fp);
	return fp;
	}
//******************************************************************************
// Write_Trailer
//******************************************************************************
FILE *Write_Trailer(char *fname)
	{
	BIRUpdateTrailer();
	if((fp = fopen(fname,"a+")) == NULL)
		{
	 	flogf("Cannot open file\n");//critical error
	 	//Stop logging and come up to the surface
		return false;
		}
	flogf("WRITING TRAILER TO %s\n", fname);
	fwrite(&BIRS, BIR_USER_HDR_SIZE,1,fp);//trailer
	fclose(fp);
	return fp;
	}
/******************************************************************************\
**	BIRUpdateTrailer		Update trailer information
\******************************************************************************/
void BIRUpdateTrailer(void)
	{
	strncpy(BIRS.LATITUDE, &LatChar[0],sizeof(BIRS.LATITUDE));
	VEEStoreStr(BIR_LATITUDE_NAME,&LatChar[0]);//Record lat
	strncpy(BIRS.LONGITUDE, &LonChar[0],sizeof(BIRS.LONGITUDE));
	VEEStoreStr(BIR_LONGITUDE_NAME,&LonChar[0]);//Record longitude
	//RTCGetTime(&BIRS.RTCLOGSTR, &BIRS.RTCSTRTIC);	// update date and time
	}	//____ BIRUpdateTrailer() ____//

/******************************************************************************\
**	QUEUpdateHeader	
**	
\******************************************************************************/
void QUEUpdateHeader(void)
	{
	strncpy(QUE.LATITUDE, &LatChar[0],sizeof(QUE.LATITUDE));
	VEEStoreStr(QUE_LATITUDE_NAME,&LatChar[0]);//Record latitude
	strncpy(QUE.LONGITUDE, &LonChar[0],sizeof(QUE.LONGITUDE));
	VEEStoreStr(QUE_LONGITUDE_NAME,&LonChar[0]);//Record longitude
	sprintf(longbuf, "%lu", QUE.GPSTIME);
	VEEStoreStr(QUE_GPSTIME_NAME, longbuf);	//Keep wake-up time in VEE	
	sprintf(shortbuf, "%d", QUE.GPSMSEC);
	VEEStoreStr(QUE_GPSMSEC_NAME, shortbuf);
	sprintf(longbuf, "%lu", QUE.RTCTIME);
	VEEStoreStr(QUE_RTCTIME_NAME, longbuf);	//Keep wake-up time in VEE	
	sprintf(shortbuf, "%d", QUE.RTCMSEC);
	VEEStoreStr(QUE_RTCMSEC_NAME, shortbuf);
	}	//____ QUEUpdateHeader() ____//
//*****************************************************************************
// SetupAcquistion
// Set up bufferes for A/D and check if the disk has enough storage.
//*****************************************************************************
short SetupAcquisition(void)
	{
	long	freeSpace;
	long 	totalSpace;
	
	freeSpace = DIRFreeSpace("C:\\");
	totalSpace= DIRTotalSpace("C:\\");
	if(freeSpace < QUE.CFPPBSZ+256L)
			return 1;
	
	RAMPPBSZ_DIV2=QUE.RAMPPBSZ/2;

		//Allocate memory for it.
	if ((RAMPingPongBuf = calloc(QUE.RAMPPBSZ, 1)) == 0)
	{
		flogf("\nCan't Allocate PingPong Buffer\n");
		result = MEM_ERROR_PINGPONG;
		return result;
	}

	//Open it.
	RAMppb = PPBOpen(QUE.RAMPPBSZ, RAMPingPongBuf, 0, 0, 0);
	if (RAMppb == 0)
	{
		flogf("\nCan't Open PingPong Buffer\n");
		result = OPEN_ERROR_PINGPONG;
		return result;
	}


	Ads8345Init(ADSLOT);		// Init QPB with ADS8344/8345 connection
	if (! Ads8345Lock(ADSLOT))	// lock it down, we want to own the QSPI
		{
		flogf("\nCouldn't lock and own A-D with QSPI\n");
		return -1;
		}
	PITSet100usPeriod(PITOff);	// disable timer
	PITRemoveChore(0);			// get rid of all current chores
	IEVInsertCFunct(&ADTimingRuptHandler, pitVector);	// replacement fast routine
	IEVInsertCFunct(&ExtFinishPulseRuptHandler, level5InterruptAutovector);

	ADSamplingRuptHandler = &ADWordRuptHandler;
	result=NO_ERROR;
	return result;
	}	//____ SetupAcquisition() ____//
	

//*********************************************************************************************
// Acquire
// Set up A/D, record the start time, write data in CompactFlash, and record the end time 
// Time is in seconds since Jan 1, 1970.
//*********************************************************************************************

short Acquire(char *fname)
	{
	bool	uni = true;		// true for unipolar, false for bipolar
	bool	sgl = true;		// true for single-ended, false for differential
	char 	longbuf[10],strbuf[4];
	char   	Start_Chr[20], End_Chr[20];
	ulong   StartSec, EndSec;
	ushort	msec;
	long	ppavail;
	long	avail;
	long	accum;
		
	//#define ZOOM
	#ifdef ZOOM
		PZCacheSetup(LOGDRIVE, calloc, free);
	#endif
	
	//Open a file.  Create one, if not there. Append, if already exists. 
	//flogf("OPENING A FILE FOR A/D LOGGING %s\n", fname); 
	if((fp = fopen(fname,"a+")) == NULL)
		{
	 	flogf("Can't open file\n");
	 	return false;
		}

	Ads8345SampleBlock(ADSLOT, FCHAN, NUMCHAN, 0, UNI, SGL, false);
	RTCDelayMicroSeconds(100000);	// let it settle for 100ms
	Ads8345SampleBlock(ADSLOT, FCHAN, NUMCHAN, ADSamplingRuptHandler, UNI, SGL, true);
	Delay100us();					// let everything conclude
	ADTimingRuptCount = ADSamplingRuptCount = 0L;	// clear from false start

	ADidx = 0;						// reset index to data
	//ACQUpdateHeader();			// Initialize/update the headers
	StartSec       = WhatTime(Start_Chr, &msec);
	BIRS.RTCLOGSTR = StartSec;
	BIRS.RTCSTRTIC = TICKS_TO_MS(msec);

	flogf("FILE START: %s\n", Start_Chr);
	SCITxWaitCompletion();			// let any pending UART data finish
	EIAForceOff(true);				// turn off the RS232 driver
	PITSet100usPeriod(10000/QUE.SRATEHZ);// start the PIT
	accum=0L;
	ADFinished= false;
	while (!ADFinished )
		{
		TickleSWSR();	// another reprieve
		//Check to see how many bytes are available
		//NOTE: (will always be zero until ping pong)
		ppavail = PPBCheckRdAvail(RAMppb);
		accum+=ppavail;
		if(ppavail>=RAMPPBSZ_DIV2)
			{
			//flogf("\nWriting: %ld\n", ppavail);cdrain();coflush();
			ppbuf = PPBGetMemBuf(RAMppb, &ppavail, true);	// and flush it
			if(fp)
				{
				fwrite(ppbuf,ppavail,1,fp);
				}
			}
		if(accum>= filelength) ADFinished=true;
			

		if (QPBTestRunning())	// conversions in process ?
			{
			*(ushort *)0xffffe00c=0xF000; //force CF card into Card-1 active mode
			LPStopCSE(CPUStop);	// VCO on (immed. wake) CPU on, SIMCLK is VCO clock
			}
		else
			{
			*(ushort *)0xffffe00c=0xF000; //force CF card into Card-1 active mode
			LPStopCSE(FastStop);// VCO on (immed. wake) CPU off, SIMCLK is VCO clock
			}
		}
	PITSet100usPeriod(PITOff);	// stop AD
	WhatTime(End_Chr, &msec);
		EIAForceOff(false);			// turn on the RS232 driver
	flogf("FILE END  : %s\n", End_Chr);

	//**************************************
	//Flush anything that's left in PingPong
	//**************************************
	avail = DIRFreeSpace(drivestr);
	if (avail < QUE.RAMPPBSZ)
	{
	flogf("Waring! Disk space run out.\n");
	ADFinished=true;
	return -1;
	}
	else
	{
		PPBFlush(RAMppb);
		ppavail = PPBCheckRdAvail(RAMppb);
		if(ppavail>0)
		{
			flogf("\nWriting the remainder1: %ld\n", ppavail);cdrain();coflush();
			ppbuf = PPBGetMemBuf(RAMppb, &ppavail, true);	// and flush it
			// Leaving the file open will be better for fast writes. Change this
			//code if needed (move the open up before the while loop and move the
			//close to the bottom.
			if(fp)
				{
				fwrite(ppbuf,ppavail,1,fp);
				}
		}
		PPBClose(RAMppb);
	}
	fclose(fp);
	#ifdef ZOOM
		PZCacheRelease(LOGDRIVE); // safe call even if not activated
	#endif

	EndSec=WhatTime(End_Chr, &msec);
	flogf(" %.20s\n", Start_Chr);putflush();
	sprintf(longbuf, "%lu", BIRS.RTCLOGSTR);
	VEEStoreStr(BIR_RTCLOGSTR_NAME, longbuf);	//Keep wake-up time in VEE	
	sprintf(strbuf, "%d", BIRS.RTCSTRTIC);
	VEEStoreStr(BIR_RTCSTRTIC_NAME, strbuf);

	BIRS.RTCLOGEND = EndSec;
	BIRS.RTCENDTIC = msec;
	sprintf(longbuf, "%lu", BIRS.RTCLOGEND);
	VEEStoreStr(BIR_RTCLOGEND_NAME, longbuf);	//Keep wake-up time in VEE	
	sprintf(strbuf, "%d", BIRS.RTCENDTIC);
	VEEStoreStr(BIR_RTCENDTIC_NAME, strbuf);

	//flogf("Sampled %ld\n",i);cdrain();coflush();//debug

	return 0;
	}
//************************************************************************************
//Make sure the float's depth reading not changing more than margin and at least the 
//depth is minpres or shallower.
//************************************************************************************ 
bool MakeSureSurface(short interval)
	{
	short	oldpressure;
	short	margin = 1;//dBar
	bool	result = false;
	short	minpres= 8;//dBar
	
	flogf("MAKING SURE FLOAT IS AT SURFACE\n");cdrain();coflush();
	oldpressure = 2000;
	pressure    = 10;
	while((pressure>=oldpressure+margin)||(pressure<=oldpressure-margin)||pressure>minpres)
		{//Exit out the loop if current pressure is within +/-1 from the previous measurement
		result=10; 	//initialize
		TickleSWSR();
		oldpressure = pressure;
		while(result>1)result = InqProvorStatus(&pStatus, &lvStatus, &phase, \
			&pDur,	&vDur, &pressure);		
		flogf("PROVAR STATUS %d,%d,%05d,%05d,%d,%04d\n",pStatus,lvStatus,pDur,\
			vDur, phase, pressure); cdrain();coflush();
		TickleSWSR();
		LowPowerDelay((ulong) interval);//check at 60 sec interval
		TickleSWSR();
		}
	return true;
	}
bool CheckResPumpIfNotEmpty(short duration)
	{
	if(ProvTerm && (phase ==0 || phase ==1))
		{
		//flogf("MAKING SURE RESERVOIR IS EMPTY\n");cdrain();coflush();
		result=10;
		while(result>1) result = GetReservoirLevel(&reservLevel);
		//flogf("RESERVOIR LEVEL = %dcc\n", reservLevel);cdrain();coflush();RTCDelayMicroSeconds(10000L);
		while(reservLevel !=0)
			{
			reservLevel=EmptyReservoir(duration);
			TickleSWSR();
			}
		flogf("RESERVOIR IS EMPTY\n");cdrain();coflush();	
		//LowPowerDelay(50L);
		//ActivatePump(30);	
		}
	return true;
	}

bool CheckResOpenValve(short duration)
	{
	if(ProvTerm && (phase ==0 || phase ==1))
		{
		//flogf("MAKING SURE RESERVOIR IS EMPTY\n");cdrain();coflush();
		result=10;
		while(result>1) result = GetReservoirLevel(&reservLevel);
		flogf("RESERVOR LEVEL = %dcc\n", reservLevel);cdrain();coflush();RTCDelayMicroSeconds(10000L);
		while(reservLevel == 0)
			{
			reservLevel=BalanceReservoir(duration);
			TickleSWSR();
			}
		flogf("RESERVOIR IS BALANCED\n");cdrain();coflush();	
			}
	return true;
	}
