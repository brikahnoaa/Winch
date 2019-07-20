//Iridum/GPS related routines
#include	<cfxbios.h>		// Persistor BIOS and I/O Definitions
#include	<cfxpico.h>		// Persistor PicoDOS Definitions
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
#include	<stat.h>		// PicoDOS POSIX-like File Status Definitions

//#define DEBUG
#ifdef DEBUG
 	#define	DBG(X)	X	// template:	DBG( cprintf("\n"); )
	#pragma message	("!!! "__FILE__ ": Don't ship with DEBUG compile flag set!")
#else
	#define	DBG(X)		// nothing
#endif

FILE 	*det_file;		//Detection file to be sent 
TUPort 	*tuport2; 		//IRIDIUM COM
short	rxch2,txch2;	//IRIDIUM COM chan	
long 	baud2 = 19200;	//Iridium COM baud rate
TUChParams IridParams;
//char	NS, EW; 		//probably not needed
float	LatMin, LonMin;
short	intLat,intLon;
char	ns, ew;

RTCTimer rt;
ulong	elp;
short 	flag=1;
short	GPSmax 			= 10;	//Number of attempt to capture the enough number of satellites
short	GPSInterval 	= 20;	//GPS positioning interval in sec

#define	IRDGPSPWR		25  	//Iridium Power pin (1=ON, 0=OFF)
#define	RxTIMEOUT		50  	//milliseconds
#define	IRIDRxTIMEOUT	500	    //milliseconds was 50
#define IRDANT			22  	//TPU pin # if ON=IRID ON, OFF GPS ON
#define GPSWAITTIME		500000L	//microseconds
#define	IRIDWAITTIME  	180000L //microSeconds
#define IRDTIMEOUT		20000	//milliseconds
#define HANDSHAKE		15      //seconds

// This macro will convert CF1 and CF2 ticks into course milliseconds
// It replaces the CFX specific ticks>>6	JHG-2003-07-28
#define	TICKS_TO_MS(x)	((ushort) (x / (CFX_CLOCKS_PER_SEC / 1000)))
#define MS_TO_TICKS(x)	((ulong)  (x * (CFX_CLOCKS_PER_SEC/1000)))

PinIODefineTPU(IRDANT, 			1);	//22 TPU 1

void   LogFailure(short errcode);
void   LowPowerDelay(ulong nDelay);
//IEV_C_FUNCT(TimingRuptHandler);

//Routines used for Iridium/GPS control
bool   CloseComIridiumGPS(void);
bool   OpenComIridiumGPS(short warmup);
bool   OpenComGPSIridium(short warmup);
bool   EnableIridiumFlow(void);

short  AveGPSLocation(short repeat, char *loc_chr, char *lat_chr, char *lon_chr, float *AveLat, float *AveLon);
time_t GetGPSTime(bool set, char*);
bool   GetGPSDateTimeSyncRTC(bool set, char *time_chr, short *num_sat, time_t *GPSsec, short *msec, time_t *RTCsec, short *rtcms);
void   LockOnToGPSTime(void);
short  GetGPSLocation(float *Lat, float *Lon);
bool   EnableIridiumCPIN(void);
short  SetIRIDBaud(short speed);	
short  ReadIRIDBaud(void);	
bool   HangUp(void); 
short  PhoneStatus(void);
short  SignalQuality(short *signal_quality);
short  TurnOffModem(void);
void   IridiumGPSPowerON(bool sw);
//void   Delay(short centisecs);
//void   WriteLocEPROM(char ns, short intLat, float LatMin, char ew, short inLon, float LonMin);
short  SwitchAntenna(char r[4]);
int    Calc_Crc(unsigned char *buf, int cnt);
void   itoa(ushort a, uchar b[2]);
void   Convert_BitMap_To_CharBuf(ulong val0, ulong val1, char *bin_str);
void   Send_Platform_ID(char *Platform_ID, char *Proj_ID);
int    Send_Blocks(char *bitmap, uchar NumOfBlks, ushort blklngth, ushort lastblklngth, char *fname);
bool   Check_If_ACK(void);
short  Check_If_Done_Or_Resent(ulong *val0, ulong *val1);

bool   Call_Land(char *);
bool   Write_Header_To_File(char *fname, char *LocTimeHeader, ushort TimeSize, \
		char *DiveHeader, ushort DiveSize, char *DrifChr, ushort DrifSize);
bool   Write_Trailer_To_File(char *fname, char *LocTimeHeader, ushort TimeSize,\
  		char *DiveHeader, ushort DiveSize, char *DrifChr, ushort DrifSize);
bool   Write_SysLocGMT_To_File(char *fname, char *LocTimeHeader, ushort TimeSize);
ulong  WhatTime(char *time_chr, ushort *rtcms);

//***************************************************************************
//Open COM2 for IRIDIUM 
//Open COM2 to communicate with Iridium
// by Walter Hannah
// Revised by H. Matsumoto 10/06/2004
//***************************************************************************
bool OpenComIridiumGPS(short warm)
{
	bool    FlowControl=true;
	bool	status;
	char    r[12];
	short   i;
	short   imax;//Number of charters to receive
	short	loc;

	imax=11;
	//RTCDelayMicroSeconds(1000000L);
	DBG(flogf("OPENING GPS IRIDIUM COM\n");RTCDelayMicroSeconds(20000L););

	TUInit(calloc, free);
	rxch2 = TPUChanFromPin(31); // AUX2
	txch2 = TPUChanFromPin(32); // AUX2
	PIOSet(29);					// enable MAX322 transmitter (/OFF)
	PIOClear(30);				// enable MAX322 receivers (/EN)
	LowPowerDelay(5L);
	IridiumGPSPowerON(true);	//Power ON Iridium GPS
	TickleSWSR();
	LowPowerDelay(20L);

	tuport2= TUOpen(rxch2,txch2,baud2, 0);
	for(i=0;i<4;i++)
		{
	 	if ((tuport2 == 0) && i != 2)
	   		{
			flogf("\n!!! Unexpected error opening satellite COM\n");
			tuport2= TUOpen(rxch2,txch2,baud2, 0);
	   		}
	 	if((tuport2 == 0) && i == 2)
	   		{
	   		flogf("\nSatellite com chan could not be opened\n");
	   		BIOSReset();
	   		}  
		}
	SCIRxSetBuffered(true);	// switch SCI to buffered receive mode
	SCITxSetBuffered(true);	// switch SCI to buffered transmit mode
	TickleSWSR();
	LowPowerDelay(20L);//Need this so that GPS rec can process
	
	TUTxFlush(tuport2);
	TURxFlush(tuport2);	

	flogf("WARMING UP IRID/GPS UNIT FOR %d SEC\n", warm);
	TickleSWSR();
	LowPowerDelay(warm*10);
		
	//Followings are for IRIDIUM
	TUTxPrintf(tuport2,"AT&D0\n"); //Talking in 3-wire serial RS232
	TUTxWaitCompletion(tuport2);
	//RTCDelayMicroSeconds(20000L);//Need this so that GPS rec can process

	r[0]=TURxGetByteWithTimeout(tuport2, 6000);
	for(i=1;i<imax;i++)
		{
		if((r[i] = TURxGetByteWithTimeout(tuport2, RxTIMEOUT*10)) == -1)
			{
				LogFailure(12);
				status = false;
			}
		}
	loc=strcspn(r,"O"); 
	if(r[loc+1] == 'K')//found OK
		{
		//flogf("SAT DTR ENABLED\n");
		status=true;
		}
	else 
		{
		flogf("SAT DTR NOT ENABLED\n");
		status= false;
		}
	TickleSWSR();
	
	RTCDelayMicroSeconds(20000L);
	LowPowerDelay(20L);
	TURxFlush(tuport2);
	TUTxPrintf(tuport2,"AT&K0\n"); //Talking in 3-wire serial RS232
	TUTxWaitCompletion(tuport2);
	//RTCDelayMicroSeconds(20000L);//Need this so that GPS rec can process
	
	r[0]=TURxGetByteWithTimeout(tuport2, 2000);
	for(i=1;i<imax;i++)
		{
		if((r[i] = TURxGetByteWithTimeout(tuport2, RxTIMEOUT*10)) == -1)
			{
				LogFailure(12);
				status = false;
			}
		}
	loc=strcspn(r,"O"); 
	if(r[loc+1] == 'K')//found OK
		{
		//flogf("SAT FLOW CNTRL ENABLED\n");RTCDelayMicroSeconds(10000L);
		status = true;
		}
	else 
		{
		flogf("SAT FLOW CNTRL NOT ENABLED\n");RTCDelayMicroSeconds(10000L);
		status = false;
		}

	return status;	
}
// OpenComIridiumGPS
//***************************************************************************
//Open COM2 for GPS
//Open COM2 to communicate with Iridium later
// by Walter Hannah
// Revised by H. Matsumoto 03/30/2006
//***************************************************************************
bool OpenComGPSIridium(short warm)
{
	short   i;
	bool	status;

	DBG(flogf("OPENING GPS IRIDIUM COM\n");RTCDelayMicroSeconds(20000L););

	TUInit(calloc, free);
	rxch2 = TPUChanFromPin(31); // AUX2
	txch2 = TPUChanFromPin(32); // AUX2
	PIOSet(29);					// enable MAX322 transmitter (/OFF)
	PIOClear(30);				// enable MAX322 receivers (/EN)
	LowPowerDelay(5L);
	IridiumGPSPowerON(true);	//Power ON Iridium GPS
	TickleSWSR();
	LowPowerDelay(20L);
	//memcpy(&IridParams, TUGetDefaultParams(),sizeof(TUChParams));
	//IridParams.baud=baud2;
	//tuport2= TUOpen(rxch2,txch2,baud2, &IridParams);
	tuport2= TUOpen(rxch2,txch2,baud2, 0);
	for(i=0;i<4;i++)
		{
	 	if ((tuport2 == 0) && i != 2)
	   		{
			flogf("\n!!! Unexpected error opening satellite COM\n");
			tuport2= TUOpen(rxch2,txch2,baud2, 0);
	   		}
	 	if((tuport2 == 0) && i == 2)
	   		{
	   		flogf("\nSatellite com chan could not be opened\n");
	   		BIOSReset();
	   		}  
		}
	SCIRxSetBuffered(true);	// switch SCI to buffered receive mode
	SCITxSetBuffered(true);	// switch SCI to buffered transmit mode
	TickleSWSR();
	LowPowerDelay(20L);//Need this so that GPS rec can process
	TUTxFlush(tuport2);
	TURxFlush(tuport2);	

	flogf("WARMING UP GPS/IRID UNIT FOR %d SEC\n", warm);
	cdrain();coflush();
	TickleSWSR();
	LowPowerDelay(warm*10);
	status=true;
	return status;
}
// OpenComGPSIridium
//***********************************************************************
//	EnableIridiumFlow
//
//***********************************************************************
bool EnableIridiumFlow(void)
{
	char    r[12];
	short	loc, i;
	bool 	status;
	
	TUTxFlush(tuport2);
	//TURxFlush(tuport2);	
	//Followings are for IRIDIUM
	TUTxPrintf(tuport2,"AT&D0\n"); //Talking in 3-wire serial RS232
	TUTxWaitCompletion(tuport2);
	//RTCDelayMicroSeconds(20000L);//Need this so that GPS rec can process

	r[0]=TURxGetByteWithTimeout(tuport2, 2000);
	for(i=1;i<11;i++)
		{
		if((r[i] = TURxGetByteWithTimeout(tuport2, RxTIMEOUT*10)) == -1)
			{
			LogFailure(12);
			status = false;
			}
		}
	TURxFlush(tuport2);
	loc=strcspn(r,"O"); 
	if(r[loc+1] == 'K')//found OK
		{
		//flogf("SAT DTR ENABLED\n");
		status = true;
		}
	else 
		{
		flogf("SAT DTR NOT ENABLED\n");
		status = false;
		}
	TickleSWSR();
	
	RTCDelayMicroSeconds(20000L);
	LowPowerDelay(20L);
	TURxFlush(tuport2);
	TUTxFlush(tuport2);
	TUTxPrintf(tuport2,"AT&K0\n"); //Talking in 3-wire serial RS232
	TUTxWaitCompletion(tuport2);
	//RTCDelayMicroSeconds(20000L);//Need this so that GPS rec can process
	
	r[0]=TURxGetByteWithTimeout(tuport2, 2000);
	for(i=1;i<11;i++)
		{
		if((r[i] = TURxGetByteWithTimeout(tuport2, RxTIMEOUT*10)) == -1)
			{
			LogFailure(12);
			status = false;
			}
		}
	TURxFlush(tuport2);	
	loc=strcspn(r,"O"); 
	if(r[loc+1] == 'K')//found OK
		{
		//flogf("SAT FLOW CNTRL ENABLED\n");RTCDelayMicroSeconds(10000L);
		status = true;
		}
	else 
		{
		flogf("SAT FLOW CNTRL NOT ENABLED\n");RTCDelayMicroSeconds(10000L);
		status = false;
		}
	return status;
}		
//**************************************************************************************
// Close COM2 for Iridium and GPS unit
//**************************************************************************************
bool CloseComIridiumGPS(void)
{
	bool status=false;
	flogf("POWER OFF IRIDIUM/GPS UNIT\n"); RTCDelayMicroSeconds(20000L);
	IridiumGPSPowerON(false);//Power OFF Iridium GPS
	RTCDelayMicroSeconds(10000L);
	TurnOffModem();	
	RTCDelayMicroSeconds(10000L);
	if (tuport2)
	{
		TURxFlush(tuport2);
		TUTxFlush(tuport2);
		TUClose(tuport2);
		TURelease();
	}

	//To save power disable MAX3222
	PinClear(29);			// disable MAX322 transmitter (/OFF)
	PinSet(30);				// disable MAX322 receivers (/EN)
	RTCDelayMicroSeconds(1000000L);
return status;	
}
//***************************************************************************************
// AveGPSLocation
// Sets the CF2's Real time clock to the GPS time. More 
// accurate method than GetGPStime(true).  It also collects GPS locations over 
// NumGPSLoc times at 1 second interval and average the values.
// Originally written by Walter Hannah (July 2004) 
// Revised by Haru Matsumoto (10/08/2004) NOAA
//***************************************************************************************
short AveGPSLocation(short NumGPSLoc, char *loc_chr, char *lat_chr, char *lon_chr,float *AveLat, float *AveLon)
{
	short	i,cnt;
	float	totalLat=0.,totalLon=0.;
	short	NumGoodGPS;
	float	Lat, Lon;

	cnt=0;
	NumGoodGPS=0;
	for(i=0;i<NumGPSLoc;i++)
	{
	//DBG(cprintf("Getting location #%d...\n",i+1));
		if( -1 == GetGPSLocation(&Lat, &Lon))
			{
			cnt++;
			if(cnt == NumGPSLoc)
				{
				LogFailure(11);
				flogf("GPS FETCHING FAILED %d TIMES\n", NumGPSLoc);
				putflush();
				CIOdrain();
				return 0;
				}
			//try again
			i=0;
			flogf("CAN'T GET GPS LOCATION %d\n", cnt);
			putflush();
			cdrain();coflush();
			}else //GPS data fetch OK
			{
			NumGoodGPS++;
			totalLat +=Lat;
			totalLon +=Lon;
			}
		LowPowerDelay(10L);
		//RTCDelayMicroSeconds(1000000L); //1 sec delay 
		TickleSWSR();
	}
			
	*AveLat = totalLat/(float)NumGoodGPS;
	*AveLon = totalLon/(float)NumGoodGPS;
	fflush(NULL);
	cdrain();coflush();
	LowPowerDelay(1L);
	intLat=(int)*AveLat;
	intLon=(int)*AveLon;
	LatMin=(*AveLat - intLat)*60.;
	if(LatMin<0.)
		{
		LatMin=-LatMin;
		ns ='S';
		}
		else ns ='N';
		
	LonMin=(*AveLon - intLon)*60.;	
	if(LonMin<0.)
		{
		LonMin=-LonMin;
		ew ='W';
		}
		else ew ='E'; 	
	//flogf("AVE GPS COORD %.5f %.5f\n",*AveLat, *AveLon);
	flogf("AVE GPS COORD %c%d:%6.3f%c%d:%6.3f\n",ns,abs(intLat),LatMin,ew,abs(intLon),LonMin);
	cdrain();coflush();RTCDelayMicroSeconds(100000L);
	sprintf(loc_chr,"%c%.2d:%6.3f%c%.3d:%6.3f",ns,abs(intLat),LatMin,ew,abs(intLon),LonMin);
	sprintf(lat_chr,"%c%.2d:%6.3f",ns,abs(intLat),LatMin);
	sprintf(lon_chr,"%c%.3d:%6.3f",ew,abs(intLon),LonMin);
	//WriteLocEPROM(ns, intLat, LatMin, ew, intLon, LonMin); //Write new location to VEEPROM
	return 1;
}
//--AveGPSLocation-----------------------------------------------------------
//*****************************************************************************************
// GetGPSDateTimeSyncRTC()
// Get GPS date and time and return as char and UTCSecs.  First search for satellites, then
// only if more than one satellite is available, fetch the time.
// If set == true, lock RTC time to the GPS time.  If set==false, just returns the UTCsec.
// Char array time_chr contains time.
// H. Matsumoto 10-26-2004
//*****************************************************************************************
bool GetGPSDateTimeSyncRTC(bool set, char *time_chr, short *num_sat_d, time_t *GPSsec, short *msec, time_t *RTCTime, short *rtcms)
//get GPS time, set the real time clock if set is true
{
	short	i;
	char	dbuf[3], mbuf[3], sbuf[3], ybuf[4];
	char	minbuf[3], hbuf[3], secbuf[3], milbuf[4];
	short	hour, min, sec, msecdely;
	short	year, month, day;
	ulong	offset;
	RTCTimer timer;
	time_t	RTCsec;
	//time_t	GPSsec;
	time_t	GPSTime;
	struct tm gps;
	short	imax = 61;
	char	r[62]={NULL};
	short	num_sat_t;
	short	loc, jmax;
	ldiv_t	miloffset;
	RTCtm	*rtc_time;			//CFX's real time clock
	ushort	ticks;
	short	j; 
	long	dSec, dmsec;

	*num_sat_d=0;
	num_sat_t=0;
	j=0;
	while(num_sat_t<4 && j<GPSmax)
	{
		TUTxFlush(tuport2);
		TURxFlush(tuport2);      			//Need this for the next TU serial command
		TickleSWSR();
		RTCDelayMicroSeconds(50000L);	
		
		//Get UTC date, month and year. Year is since AD 0, month starts 0.
		//Need more than 1 satellites up
		TUTxPrintf(tuport2,"AT+PD\r"); 		//Get time in ZDA format
		RTCDelayMicroSeconds(GPSWAITTIME);	//Need this so that GPS rec can process
		TUTxWaitCompletion(tuport2);
		// get the first response
		//r[0]=TURxGetByte(tuport2, true);
		for(i=0;i<imax;i++)
		{
			if((r[i] = TURxGetByteWithTimeout(tuport2,RxTIMEOUT+50000)) == -1)
			{
				jmax=i;
				exit;
				LogFailure(11);
				return false;
			}
		}
		j++;//increment
		TURxFlush(tuport2);      //Need this for the next TU serial command
	
		//debug1
		//cprintf("%s\n",r);cdrain();coflush();
		
		loc=strcspn(r,"U");
		strncpy(mbuf,r+loc+9,2);
		month = atoi(mbuf);//Jan=1 (not 0)
		strncpy(dbuf,r+loc+12,2);
		day = atoi(dbuf);
		strncpy(ybuf,r+loc+15,4);
		year  = atoi(ybuf);//since AD 0
		//cprintf("year=%d month=%d day=%d\n",year, month, day);RTCDelayMicroSeconds(100000L);
		loc=strcspn(r,"S");
		//cprintf("%d\n",loc);
		//strncpy(cbuf,r+loc,10);
		//cprintf("%s\n",cbuf);
		strncpy(sbuf,r+16+loc,2);
		*num_sat_d = atoi(sbuf);
		TickleSWSR();
		//debug1
		//uprintf("%d\n",*num_sat_d);RTCDelayMicroSeconds(10000L);
		
		flogf("NUM OF SAT %d\n", *num_sat_d); cdrain();coflush();
		TUTxFlush(tuport2);
		TURxFlush(tuport2);
		LowPowerDelay(20L);
		//Enough satellite is up
		if(*num_sat_d>0)
		{
			//Get UTC time from GPS  
			TickleSWSR();
			RTCDelayMicroSeconds(500000L);
			//RTCDelayMicroSeconds(5000000L);//5 sec delay
			RTCGetTime(&RTCsec, &ticks);
			RTCElapsedTimerSetup(&timer);
			TUTxPrintf(tuport2,"AT+PT\r"); 		//Get ZDA format
			RTCDelayMicroSeconds(GPSWAITTIME);	//Need this so that GPS rec can process
			TUTxWaitCompletion(tuport2);
	
			//r[0]=TURxGetByte(tuport2, true);		
			for(i=0;i<imax;i++)
			{//Skip Echo
				if((r[i] = TURxGetByteWithTimeout(tuport2,RxTIMEOUT+100)) == -1)
				{
					LogFailure(11);
					return false;
				}
			}
			//debug1
			//uprintf("%s", r);RTCDelayMicroSeconds(100000L);
	
			strncpy(hbuf,r+23,2);
			hour = atoi(hbuf);
			strncpy(minbuf,r+26,2);
			min  = atoi(minbuf);
			strncpy(secbuf,r+29,2);
			sec  = atoi(secbuf);
			strncpy(milbuf,r+32,3);
			*msec=atoi(milbuf);
			loc=strcspn(r,"S"); 
			//cprintf("Hour=%d,Min=%d,Sec=%d\n",hour,min,sec);	
			//cprintf("%d\n",loc);
			//strncpy(cbuf,r+loc+8,10);
			//cprintf("%s\n",cbuf);
			strncpy(sbuf,r+16+loc,2);
			num_sat_t=atoi(sbuf);
			if(hour ==23 && min==59 && sec>56) num_sat_t=0;//too close to the end of day
		}else LowPowerDelay(GPSInterval*10);			  //GPS interval
	}
	if(j >= GPSmax) return false; //GPS sync failed
	//Convert UTC time to struct *tm format
	gps.tm_year = year - 1900;//RTCSetTime is since 1900
	gps.tm_mon  = month - 1;  //RTCSetTime Jan is 0
	gps.tm_mday = day;
	gps.tm_hour = hour;
	gps.tm_min  = min;
	gps.tm_sec  = sec;
	
	*GPSsec = mktime(&gps);
	GPSTime= *GPSsec;
	if(set)
		{
		offset = RTCElapsedTime(&timer);//520msec
		miloffset= ldiv(offset,1000L);
		//cprintf("%ld %d\n",miloffset.quot, msec);
		msecdely=miloffset.quot+*msec;
		if(msecdely >1000) 
			{
			msecdely -=1000;
			*GPSsec +=1L;
			}
		//printf(ctime(&RTCsec));
		//RTCDelayMicroSeconds(999900L-(ulong) msecdely*1000L);
		RTCSetTime(*GPSsec, MS_TO_TICKS(msecdely));
		}
	
	rtc_time = RTClocaltime (&RTCsec);
	*rtcms   = TICKS_TO_MS(ticks);
	*RTCTime = RTCsec;
	flogf("CF2 TIME %.3d %.3d:%.2d:%.2d:%.2d:%.3u\n",rtc_time->tm_year, rtc_time->tm_yday + 1,\
		rtc_time->tm_hour,rtc_time->tm_min,rtc_time->tm_sec, *rtcms);cdrain();coflush();
	sprintf(time_chr," %.3d %.3d:%.2d:%.2d:%.2d:%.3u ",rtc_time->tm_year, rtc_time->tm_yday + 1,\
		rtc_time->tm_hour,rtc_time->tm_min,rtc_time->tm_sec, *rtcms);
	flogf  ("GPS TIME %.3d %.3d:%.2d:%.2d:%.2d:%.3u %.2d\n",gps.tm_year,gps.tm_yday+1,hour,min,sec,*msec\
		,num_sat_t);cdrain();coflush();
	sprintf (time_chr+22,"%.3d %.3d:%.2d:%.2d:%.2d:%.3u %.2d",gps.tm_year,gps.tm_yday+1,hour,min,sec,*msec\
		,num_sat_t);
	cdrain();
	coflush();
	
	if(set)
		{
		flogf("CFX TIME SET\n");cdrain();coflush();
		dSec  = GPSTime - RTCsec;
		dmsec = *msec - *rtcms;
		//flogf("CLOCK ERROR = GPS - RTC = %ldsec %ldmsec\n",dSec,dmsec);
		dmsec = dSec*1000L + dmsec;
		dSec  = dmsec/1000L;
		dmsec = dmsec - dSec *1000L;
		flogf("CLOCK ERROR = GPS - RTC = %ldsec %ldmsec\n",dSec,dmsec);
		//flogf("OFFSET %ld\n",offset); //RTC Timing error from GPS 
		//flogf("%d MSEC\n",msecdely);
		cdrain();coflush();
		}
		
	//DBG(cprintf("%d/%d/%d %.2d:%.2d:%.2d.%.3d %.2d\n",year,month,day,hour,min,sec,msec,num_sat_t);)
	//TUTxFlush(tuport2);
	TURxFlush(tuport2);      //Need this for the next TU serial command

	//rtc_time = RTClocaltime (&RTCsec);
	//sprintf(time_chr,"%.3d %.3d:%.2d:%.2d:%.2d:%.3u",rtc_time->tm_year, rtc_time->tm_yday + 1,\
	//rtc_time->tm_hour,rtc_time->tm_min,rtc_time->tm_sec, TICKS_TO_MS(ticks));
	TickleSWSR();
	cprintf("");//for timing purpose
	return true;
}
//**GetGPSDateTimeSyncRTC()************************************************************

//***************************************************************************
//GetGPSLocation
//Get current GPS position and store in global variables in decimal degrees
// + Northern hemispher, - Southern hemispher
// + East,				 - West
// Original written by Walt Hannah
// Revised by H. Matsumoto 10/07/2004 NOAA
//***************************************************************************
short GetGPSLocation(float *Lat, float *Lon)
{
	short	i;
	char	buf[66]={NULL};	
	short	latdeg,londeg;
	short	latmindec,lonmindec;
	short	latmin,lonmin;
	//char	ns[1],ew[1];
	char	buflat[3];
	char	buflon[4];
	char 	bufmin[3];
	char	buflonmin[3];
	char	bufdec[5];
	short	imax;

	imax=65;
	TUTxFlush(tuport2);
	TURxFlush(tuport2);      //Need this for the next TU serial command
	
	// AT command to get GLL GPS NMEA data 
	TUTxPrintf(tuport2,"AT+PA=2\r");
	TUTxWaitCompletion(tuport2);
	RTCDelayMicroSeconds(GPSWAITTIME+120000L);

	// get response
	buf[0]=TURxGetByteWithTimeout(tuport2,5000);
	for(i=1;i<imax;i++)
	{
		buf[i] = TURxGetByteWithTimeout(tuport2,RxTIMEOUT);
		//RTCDelayMicroSeconds(1000L);
		if(buf[i] == -1) i = imax;
	}
	// Convert ASC char to numbers
	strncpy(buflat,buf+23,2);//latitude
	latdeg = atoi(buflat);
	strncpy(bufmin,buf+25,2);	  
	latmin=atoi(bufmin);
	strncpy(bufdec,buf+28,4);
	latmindec=atoi(bufdec);
	ns    = buf[33];

	strncpy(buflon,buf+35,3);//longitude
	londeg = atoi(buflon);
	strncpy(buflonmin,buf+38,2);
	lonmin= atoi(buflonmin);
	strncpy(bufdec,buf+41,4);
	lonmindec=atoi(bufdec);
	ew     = buf[46];
	
	*Lat = (float)latdeg + ((float)latmin+(float)latmindec/10000.)* 0.016667;
	//NS  = ns;
	if(ns =='S') *Lat=-*Lat;
	*Lon = (float)londeg + ((float)lonmin+(float)lonmindec/10000.)* 0.016667; 
	//EW  = ew;
	if(ew =='W') *Lon =-*Lon;

	//DBG(RTCDelayMicroSeconds(10000L));
	cprintf("%.2d%:%.2d.%.4d%c ",latdeg,latmin,latmindec,ns);
	RTCDelayMicroSeconds(10000L);
	cprintf("%.3d:%.2d.%.4d%c\n",londeg,lonmin,lonmindec,ew);
    RTCDelayMicroSeconds(100000L);
	
	//DBG(cprintf("%f%c %f%c\n",Lat,NS,Lon,EW));

	if(buf[59] != 'A')//Check Status character 
	{
		LogFailure(11);//GPS data invalid
		//if(!VEEStoreStr("QUE.LSTGPSOK","F")) LogFailure(9);
		return -1;
	}
	//else 
	//	if(!VEEStoreStr("QUE.LSTGPSOK","T")) LogFailure(9);
	TURxFlush(tuport2);
	TUTxFlush(tuport2);

	return 0;
}//GetGPSLocation
//***********************************************************************************
//EnableIridiumCPIN
//
//Haru Matsumoto, 11/01/2004, NOOA
//***********************************************************************************
bool EnableIridiumCPIN(void)
	{	
	short	i;
	char	r[30], b[8];
	//short	loc; 
	short	loc2;
	
	TUTxFlush(tuport2);
	TURxFlush(tuport2);      //Need this for the next TU serial command
	//TUTxPrintf(tuport2,"AT+CPIN?\n");
	//TUTxWaitCompletion(tuport2);
	//r[0]=TURxGetByte(tuport2, true);
	//for(i=1;i<28;i++)	//Echo
	//	{
	//	r[i] = TURxGetByteWithTimeout(tuport2,RxTIMEOUT);
	//	RTCDelayMicroSeconds(1000L);
	//	}
	//cprintf("%s\n",r);
	//loc=strcspn(r,":"); 
	//RTCDelayMicroSeconds(200000L);
	//TURxFlush(tuport2);

	//if(r[loc+5] != 'P')
	//{
		//flogf("Trying to enable IRIDIUM\n");
		TUTxPrintf(tuport2,"AT+CPIN=%c1111%c\n",'"','"');
		TUTxWaitCompletion(tuport2);
		//RTCDelayMicroSeconds(IRIDWAITTIME);
		RTCDelayMicroSeconds(100L);
		r[0]=TURxGetByteWithTimeout(tuport2, 3000);
		for(i=1;i<16;i++)	//Echo
			{
			r[i] = TURxGetByteWithTimeout(tuport2,RxTIMEOUT);
			RTCDelayMicroSeconds(500L);
			}
		//cprintf("%s\n",r);
		TURxFlush(tuport2);
		RTCDelayMicroSeconds(500L);
		b[0]=TURxGetByteWithTimeout(tuport2,3000);
		for(i=1;i<7;i++) b[i]= TURxGetByteWithTimeout(tuport2,1000);
		//loc=strcspn(r,"2");
		//printf("2 at %d\n",loc);
		//cprintf("%s\n",b);
		TUTxFlush(tuport2);
		TURxFlush(tuport2);      //Need this for the next TU serial command
		loc2=strcspn(b,"O"); 
		if(b[loc2+1] == 'K')//found OK
			{
			flogf("IRIDIUM ENABLED\n");
			return true;
			}
		else 
			{
			flogf("IRIDIUM NOT ENABLED\n");
			return false;
			}
	//}else
	//{
	//flogf("IRIDIUM NOT ENABLED\n");
	//return false;
	//}      
	
}//--EnableIridiumCPIN----------------------------------------------------------
//***********************************************************************************
//Call_Land
//
//Haru Matsumoto, 4/15/2005, NOOA
//***********************************************************************************
bool Call_Land(char PhoneNum[14])
	{	
	char 	r[20], b[16], temp[6];
	short	i, j=0,k;
	bool 	CallOK=false;
	short	result;
	ushort  rtcms;
	
	//TUTxFlush(tuport2);      //Need this for the next TU serial command
	TURxFlush(tuport2);      //Need this for the next TU serial command
	flogf("CALLING LAND %s\n",PhoneNum);RTCDelayMicroSeconds(20000L);
	TUTxPrintf(tuport2,"ATDT %s\n",PhoneNum);
	//TUTxWaitCompletion(tuport2);
	r[0]=TURxGetByte(tuport2, true);//Wait til get the 1-st char
	for(i=1;i<20;i++)	//Display ATDT ##########
		{
		r[i]=TURxGetByteWithTimeout(tuport2,RxTIMEOUT);
		if(r[i] == '-1') return false;
		}
	//DBG(cprintf("%s\n",r);RTCDelayMicroSeconds(20000L));
	TURxFlush(tuport2); //Need this for the next TU serial command
	TickleSWSR();
	RTCDelayMicroSeconds(5000000L); //5 sec delay
	TickleSWSR();
	
	b[0]=TURxGetByte(tuport2, true);
	//b[0]=TURxGetByteWithTimeout(tuport2, 5000);
	for(i=1;i<15;i++)
		{//Diplays "CONNECT 19200".  
		b[i]=TURxGetByteWithTimeout(tuport2,RxTIMEOUT);
		if(b[i] == '-1') return false;
		}//Connected to sat, but land modem is needs hand shake
	//DBG(cprintf("%s\n",b);RTCDelayMicroSeconds(20000L));
	k=strcspn(b,"C");
	strncpy(temp,b+k,6);
	result=strncmp(temp,"CONNEC",6);
	WhatTime(r, &rtcms);
	if(result!=0) 
		{
		flogf("CONNECT FAILED %.20s\n", r);
		RTCDelayMicroSeconds(20000L);
		return false;
		}
	else flogf("CONNECTED %.20s\n", r);
	RTCDelayMicroSeconds(20000L);
	TUTxFlush(tuport2);
	TURxFlush(tuport2); //Need this for the next TU serial command
	TickleSWSR();
	RTCDelayMicroSeconds(HANDSHAKE*1000000L);//Needs >20 sec for hand shake
	TickleSWSR();
	ciflush();
	coflush();
	fflush(NULL);
	DBG(flogf("SENDING ID\n");RTCDelayMicroSeconds(20000L));
	return true; 		//successful
}//--Call_Land----------------------------------------------------------

//*******************************************************************************
// SetIRIDBaud
// Set Iridium baud rate to 9600
//*******************************************************************************
short SetIRIDBaud(short speed)	
	//use this to select baud rate of satellite transmissions
	//See ISU AT Command Reference for a list of speeds
	{
	short	i;
	char	baud[16], rec[8];
	
	TURxFlush(tuport2);
	TUTxFlush(tuport2);
	RTCDelayMicroSeconds(1000000L);
	TUTxPrintf(tuport2,"AT+CBST=%d,0,1\n",speed);	
	TUTxWaitCompletion(tuport2);
	baud[0]=TURxGetByte(tuport2,true);
	for(i=1;i<15;i++)	//Echo
		{
		baud[i]=TURxGetByteWithTimeout(tuport2,RxTIMEOUT);
		if(baud[i] == '-1')return -1;
		}
	//DBG(cprintf("%s\n",baud));
	rec[0]=TURxGetByte(tuport2,true);
	for(i=1;i<7;i++)	//Response OK or ERROR
		{
		rec[i]=TURxGetByteWithTimeout(tuport2,RxTIMEOUT);
		if(rec[i] == '-1')return -1;
		}
	//DBG(cprintf("%s\n",rec));
	
	return 0;
	}//-SetIRIDBaud--------------------------------------------------------------
//*******************************************************************************
//ReadIRIDBaud
//check the baud rate of satellite transmissions
//See ISU AT Command Reference for a list of speeds
// 	1 = 300bps
//	2 = 1200
//	4 = 2400
//	6 = 4800
//	7 = 9600  V.32
//  71 = 9600 V.110
//*******************************************************************************
short ReadIRIDBaud(void)	
{
	short	i,speed;
	char	r[38];
	short	count=0;
	char	loc;
	char    blk[2];

	TURxFlush(tuport2);
	TUTxFlush(tuport2);
	TUTxPrintf(tuport2,"AT+CBST?\n");	
	TUTxWaitCompletion(tuport2);
	RTCDelayMicroSeconds(IRIDWAITTIME);		
	for(i=0;i<37;i++)
		{//Echo
		r[i]=TURxGetByteWithTimeout(tuport2,RxTIMEOUT);
		}
	//count=TURxQueuedCount(tuport2);
	//cprintf("count= %d\n",count);
	//cprintf("%s\n", r);
	loc=strcspn(r,":");
	//cprintf("%d\n",loc);					
	if(loc >19 || loc <16 || r[0] == '-1')//Detected ERROR
		{
		speed=-1;
		LogFailure(12);
		}
		else
		{
		blk[0]=r[loc+2];
		blk[1]=r[loc+3];
		//speed = atoi(r+loc+3);//OK
		speed=atoi(blk);
		}
	
	TURxFlush(tuport2);
	TUTxFlush(tuport2);
	return speed;
}

//-----------------------------------------------------------------------
bool HangUp(void) 
{
	short	i;
	char	b[6];
	
	TUTxFlush(tuport2);
	TURxFlush(tuport2);
	TickleSWSR();
	TUTxPrintf(tuport2,"+++\n");
	TUTxWaitCompletion(tuport2);
	//RTCElapsedTimerSetup(&rt);
	//b[0]=TURxGetByte(tuport2,true);
	b[0]=TURxGetByteWithTimeout(tuport2,9000);//typical 5 sec
	//elp=RTCElapsedTime(&rt);
	for(i=1;i<5;i++)	//Echo
		{
		b[i]=TURxGetByteWithTimeout(tuport2,RxTIMEOUT);
		if(b[i]=='-1')return false;
		}
	//DBG(flogf("%c%c%c\n",b[0],b[1],b[2])); RTCDelayMicroSeconds(10000L);
	//uprintf("%ld\n",elp);RTCDelayMicroSeconds(10000L);
	
	TURxFlush(tuport2);
	TUTxPrintf(tuport2,"ATH\n");	
	TUTxWaitCompletion(tuport2);

	//RTCElapsedTimerSetup(&rt);
	//b[0]=TURxGetByte(tuport2,true);
	TickleSWSR();
	b[0]=TURxGetByteWithTimeout(tuport2,18000);//typical 9 sec
	//elp=RTCElapsedTime(&rt);
	for(i=1;i<5;i++)
		{	//Echo
		b[i]=TURxGetByteWithTimeout(tuport2,RxTIMEOUT);
		if(b[i]=='-1')return false;
		}
	TURxFlush(tuport2);
	TickleSWSR();
	//uprintf("%ld\n",elp);RTCDelayMicroSeconds(10000L);
	
	return true;
}
//*****************************************************************************
// PhoneStatus
// Check whether a call is in progress or if commands are accepted.
// Returns 0 if "Ready", "Data Call Ringing", " Data Call In Progress".
// Returns -1 if "Unavailable" or "Unknown."
// Walter Hannah
//*****************************************************************************
short PhoneStatus(void)
{
	short	i, returnvalue=0;
	char	r[22];
	char	loc, locE;
	locE=22;
	TUTxFlush(tuport2);
	TURxFlush(tuport2);
	TUTxPrintf(tuport2,"AT+CPAS\n");	
	TUTxWaitCompletion(tuport2);
	//RTCDelayMicroSeconds(100000L);		
	r[0]=TURxGetByte(tuport2,true);
	for(i=1;i<20;i++)	//Echo
		{
		r[i]=TURxGetByteWithTimeout(tuport2,RxTIMEOUT);
		if(r[i] == '-1'){
						returnvalue=-1;
						LogFailure(15);
						}
		}
	loc=strcspn(r,":");
	locE=strcspn(r,"E");	//E for Error
	//cprintf("%d\n",loc);	
	if(atoi(&r[loc+3]) == 1 || atoi(&r[loc+3])==2) returnvalue=-1;
		//cprintf("%s\n",r);
	if(locE<19) {
				returnvalue=-1;
				LogFailure(14);
				}	
	TUTxFlush(tuport2);
	TURxFlush(tuport2);
	
	return returnvalue;
}
//*****************************************************************************
// SignalQuality
// Check Iridium signal quality
// H. Matsumoto
//*****************************************************************************
short SignalQuality(short *signal_quality)
{
	short	i, returnvalue=0;
	char	r[22];
	char	loc, locE;
//	short	timeout;
//	timeout=5000;
	TickleSWSR();
	TUTxFlush(tuport2);
	TURxFlush(tuport2);
	TUTxPrintf(tuport2,"AT+CSQ?\n");	
	TUTxWaitCompletion(tuport2);
	//RTCDelayMicroSeconds(10000L);		
	r[0]=TURxGetByte(tuport2, true);
	TickleSWSR();
	for(i=1;i<20;i++)	//Echo
		{
		r[i]=TURxGetByteWithTimeout(tuport2,3000);
		if(r[i] == '-1'){
						returnvalue=-1;
						LogFailure(13);
						}
		}
	loc=strcspn(r,":");
	locE=strcspn(r,"E");	
	//cprintf("%d\n",loc);
	*signal_quality=atoi(&r[loc+1]);	
	if(*signal_quality <1 ) returnvalue=-1;
		//cprintf("%s\n",r);
	if(locE<19) {//ERROR detected
				returnvalue=-1;
				LogFailure(14);
				}	
	
	return returnvalue;
}
//-----------------------------------------------------------------------
short TurnOffModem(void)
{
	short	i;
	char	r[8];
	
	TUTxFlush(tuport2);
	TURxFlush(tuport2);
	TUTxPrintf(tuport2,"AT*P0\n");
	
	for(i=0;i<7;i++)	//Echo
		{
		//TURxGetByteWithTimeout(tuport2,RxTIMEOUT);
		r[i] = TURxGetByteWithTimeout(tuport2,IRDTIMEOUT);
		if(r[i] != '0')
		return -1;
		}
	return 0;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void IridiumGPSPowerON(bool sw)
{
	if(sw == true)PIOSet(IRDGPSPWR);
	if(sw == false)PIOClear(IRDGPSPWR);
}
//**************************************************************************************
//LockOnToGPSTime()
//Another way to lock on to GPS time by checking the consistency of the time.
//**************************************************************************************	
void LockOnToGPSTime()
{
	time_t	utc0=0L, utc1=100L;
	//RTCTimer timer;
	long	offset1=0L, timediff=100L;
	short	numb_lock=0;
	char	time_chr[20];

	//Making sure the GPS time is consistent
	flogf("Trying to time lock on GPS.."); 
	while((utc1-utc0)!=2L || numb_lock<3) //At least 3 locks
	{
	 	//RTCElapsedTimerSetup(&timer);
		utc0 = GetGPSTime(false,time_chr);
		//offset1 = RTCElapsedTime(&timer);
		RTCDelayMicroSeconds(1300000L);
		//Delay(13);	//about 2 sec
		utc1 = GetGPSTime(false,time_chr);
		//temp = utc1-utc0-offset*1/1000000;
		//cprintf("utc1-utc0 = %s\n",ctime(&temp));
		if(utc1-utc0==2L)numb_lock++;
		timediff=(utc1-utc0);
		//DBG(printf("%ld %f %d\n", timediff, (float)offset1*0.000001, numb_lock));
	}
	flogf("TIME LOCKED\n");
	GetGPSTime(true,time_chr);	
}
//-----------------------------------------------------------------------
//***************************************************************************
// GetGPSTime
// Get GPS ZDA fromat
// by Walter Hannah
// Revised by H. Matsumoto 10/06/2004
//***************************************************************************

time_t GetGPSTime(bool set, char *time_chr)
//get GPS time, set the real time clock if set is true
{
	short	i;
	char	r[42]={' '};
	char	buf[3];
	short	hour,min,sec,hsec;
	short	year,month,day;
	ulong	offset;
	RTCTimer timer;
	time_t	UTCsec;
	struct tm gps;
	short	imax = 41;
	
	// send AT Command
	RTCElapsedTimerSetup(&timer);

	TUTxPrintf(tuport2,"AT+PA=8\r"); //Get ZDA format
	TUTxWaitCompletion(tuport2);
	//RTCDelayMicroSeconds(GPSWAITTIME);//Need this so that GPS rec can process
	
	// get response
	r[0]=TURxGetByteWithTimeout(tuport2, 5000);
	for(i=1;i<9;i++)
		{//Skip Echo
		if(TURxGetByteWithTimeout(tuport2,RxTIMEOUT) == -1)
			{
			//LogFailure(11);
			return -1;
			}
		}
	//Start reading ZDA GPS format	
	for(i=1;i<imax;i++)
	{
		if((r[i] = TURxGetByteWithTimeout(tuport2,RxTIMEOUT)) == -1)
		{
			//LogFailure(11);
			return -1;
		}
	}
	
	strncpy(buf,r+15,2);
	hour = atoi(buf);
	strncpy(buf,r+17,2);
	min  = atoi(buf);
	sec  = atoi(r+19);
	day  = atoi(r+25);
	month= atoi(r+28);
	year = atoi(r+31);
	
	gps.tm_year = year - 1900;
	gps.tm_mon  = month - 1;
	gps.tm_mday = day;
	gps.tm_hour = hour - 1;
	gps.tm_min  = min;
	gps.tm_sec  = sec;
	
	UTCsec = mktime(&gps)+1L;
	offset = RTCElapsedTime(&timer);//230msec
	hsec = atoi(r+22) + offset/10000L;
	
	//printf(ctime(&UTCsec));
	RTCDelayMicroSeconds(500000L);
	if(set)
	{
		RTCSetTime(UTCsec, hsec * 400);
		flogf("RTC set at %d %d %d %d:%d:%d.%d\n",year,month,day,hour,min,sec,hsec);
	}
	sprintf(time_chr,"%.2d/%.2d/%.2d %.2d:%.2d:%.2d.%.2d",year,month,day,hour,min,sec,hsec);
	//Delay(10);
	//DBG(cprintf("%d %d %d %d:%d:%d.%d\n",year,month,day,hour,min,sec,hsec));
	fflush(NULL);
	TUTxFlush(tuport2);
	TURxFlush(tuport2);      //Need this for the next TU serial command
	RTCDelayMicroSeconds(10000L);
	//cprintf("%ld\n",offset); //RTC Timing error from GPS 
	//cprintf("%d\n",hsec);

	return UTCsec;
}
//---GetGPSTime()------------------------------------------------------------
//**********************************************************************************
// SwitchAntenna
// Switch the antenna between IRIDIUM and GPS.  Never transmit when GPS is ON.
// Use TPU 1, Pin 22 to tune ON or OFF.  
// Pin 22 OFF = GPS
// Pin 22 ON  = IRIDIUM
// When IRIDIUM transmission/receive is over, switch over to GPS (or turn off 22).
// H. Matsumoto, NOAA 4/05/05
//**********************************************************************************
short SwitchAntenna(char *r)
	{
	short	result=10;
	if(strncmp(r,"GPS",3)==0x00)
		{
		flogf("GPS ANTENNA ON\n");cdrain();coflush();
		//PIOClear(IRDANT);
		PinClear(IRDANT);
		RTCDelayMicroSeconds(500000L);//wait 0.5 sec to settle swtich noise
		return 0;
		}
	if(strncmp(r,"IRD",3)==0x00)
		{
		flogf("IRIDIUM ANTENNA ON\n");cdrain();coflush();
		//PIOSet(IRDANT);
		PinSet(IRDANT);
		RTCDelayMicroSeconds(500000L);
		return 1;
		}
	return -1;
	}//------SwitchAntenna---------------------------------------------------------------

//******************************************************************************
// Send_Platform_ID
// 13-byte header
//	3 byte "???"
//	2 byte crc
//	4 byte Project ID (ASCII)
//	4 byte Platform ID (ASCII)
// crc is for the subsequent 8 bytes data.
// H. Matsumoto, Dec 7, 2004
//******************************************************************************
void Send_Platform_ID(char *ProjID, char *PltfrmID)
{
	int	crc, crc1, crc2;
	unsigned char buf[14], proj[8]; 

	TickleSWSR();
	RTCDelayMicroSeconds(2000000L);	//Put delay here (a few sec )
	sprintf(proj,"%c%c%c%c%c%c%c%c",ProjID[0],ProjID[1],ProjID[2],ProjID[3],
	PltfrmID[0],PltfrmID[1],PltfrmID[2],PltfrmID[3]);
	crc=Calc_Crc(proj, 8);
	crc1=crc;
	crc2=crc;
	sprintf(buf,"???%c%c%c%c%c%c%c%c%c%c",(uchar)((crc1>>8) & 0x00FF),\
	(uchar)(crc2 & 0x00FF),proj[0],proj[1],proj[2],proj[3],proj[4],proj[5]\
	,proj[6],proj[7]);
	//buf[3]=(char)((crc1>>8) & 0x00FF);
	//buf[4]=(char)(crc2 & 0x00FF);
	
	//cprintf("%x%x\n",buf[0],buf[0]);
	//putflush();
	//RTCDelayMicroSeconds(20000L);
	DBG(cprintf("%s\n",buf);RTCDelayMicroSeconds(10000L));

	TUTxFlush(tuport2);
	TURxFlush(tuport2);
	TUTxPrintf(tuport2,"???");
	TUTxPrintf(tuport2,"%c%c",buf[3],buf[4]);
	TUTxWaitCompletion(tuport2);//1.05msec
	TUTxPutBlock(tuport2, proj, 8, 100);
	TUTxWaitCompletion(tuport2);//2.2msec
	RTCDelayMicroSeconds(1000L);
}


//*****************************************************************************
// Send_Blocks
// Send or resend the data. The blocks to be sent are in bitmap field.
//*****************************************************************************
int Send_Blocks(char *bitmap, uchar NumOfBlks, ushort blklngth, ushort lastblklngth, char *fname)
{
uchar 	*buf;
int  	crc_calc;
uchar	blockNum;
uchar	mlen[2];
uchar	crc_buf[2];
long	mlength;
ushort	blklen;
			
	//Open the hydrophone detection file
	if((det_file = fopen(fname,"r")) == NULL)
		{
	 	flogf("Cannot open file\n");//critical error
		return 1;
		}

	crc_calc=0x0000;
	for(blockNum = 1; blockNum <= NumOfBlks; blockNum++)
		{
 		if(blockNum==NumOfBlks)blklngth=lastblklngth;
 		mlength =blklngth+5;		 	//PMEL  IRID block size + 5
		blklen  =blklngth+5;
		itoa(blklen, mlen);				//PMEL  site block length
    	buf = (uchar *) malloc(blklen);
    	fread(buf+5,sizeof(uchar), blklngth, det_file);	//Advance the buffer
		if(bitmap[64-blockNum]!='0')    //Send in reverse order
			{
		  //sprintf(buf,"%c%c",mlen[0],mlen[1]);//block length in reverse order
			buf[0]=mlen[0];						//block length
			buf[1]=mlen[1];
			buf[2]='I';							//data type
			buf[3]=blockNum;
			buf[4]=NumOfBlks;

			crc_calc  = Calc_Crc(buf, blklen);//PMEL site crc include first 5 byte
			//crc_calc  = Calc_Crc(buf+5, blklngth);//Rudic crc does not include the first 5 bytes
			crc_buf[1]= (char)(crc_calc &  0x00FF);
			crc_buf[0]= (char)((crc_calc>>8) & 0x00FF);
			
			//debug to simulate data transmission error at blockNum **
			//if((blockNum == 2 || blockNum==10) && flag<1)
			//	{
			//	crc_buf[1]=(char)(crc_calc + 1);
			//	flag++;
			//	}
			flogf("SENDING BLK #%d %ldBYTES\n",blockNum, mlength);RTCDelayMicroSeconds(14000L);
			cdrain();
			coflush();
			//debug end
			
			//Send a file to IRIDIUM
			TUTxFlush(tuport2);
			TURxFlush(tuport2);
			TUTxPrintf(tuport2,"@@@");
			TUTxWaitCompletion(tuport2);
			TUTxPrintf(tuport2, "%c%c", crc_buf[0], crc_buf[1]);
			//TUTxWaitCompletion(tuport2);
			//RTCDelayMicroSeconds(2000L);
			TUTxPutBlock(tuport2, buf, mlength, 20*blklen);
			TUTxWaitCompletion(tuport2);
			TUTxFlush(tuport2);
			TURxFlush(tuport2);
			ciflush();
			coflush();
			}
    	free(buf);
    }
    fclose(det_file);
return 0;
}// Send_Blocks*******************************************************************
//********************************************************************************
//
// Convert_BitMap_To_CharBuf
//********************************************************************************
void Convert_BitMap_To_CharBuf( ulong val0, ulong val1, char *bin_str)
{
ulong 	remainder;
int 	count,type_size;
short	NumBadBlks;

NumBadBlks=0;
type_size=sizeof(ulong) * 8;

	for(count=0; count<type_size; count++)
	{
		remainder = val0 % 2;
		if(remainder)
			{
			bin_str[count]='1';
			NumBadBlks++;
			}
		else
			bin_str[count]='0';
		val0/=2;
	}
	for(count=type_size; count<type_size*2; count++)
	{
		remainder = val1 % 2;
		if(remainder)
			{
			bin_str[count]='1';
			NumBadBlks++;
			}
		else
			bin_str[count]='0';
		val1/=2;
	}
	//bin_str[count]='\0';
	//Debug
	flogf("\nNO OF BAD BLKS=%d\n",NumBadBlks);RTCDelayMicroSeconds(10000L);
	//debug end
}//******** Convert_BitMap_To_CharBuf**************************************
//*************************************************************************
//  Check if the received chars are "ACK"
//  Returns 1 if "ACK" received,
//          0 not an ACK string.
//*************************************************************************
bool Check_If_ACK(void)
{
char	buf[3];
int     ack;
short	i;
	
	//Read AUX and check if ACK
	//Start reading AUX for message
	TickleSWSR();
	TURxFlush(tuport2);
	//buf[0]=TURxGetByte(tuport2,true);
	buf[0]=TURxGetByteWithTimeout(tuport2,4000);//typical 1.1 sec

	for(i=1;i<3;i++)								//Echo
		{
		buf[i] = TURxGetByteWithTimeout(tuport2, RxTIMEOUT);
		}
	//cprintf("%ld\n",elp);
	//putflush();
		
	DBG(uprintf("%c%c%c\n",buf[0],buf[1],buf[2]));
	RTCDelayMicroSeconds(10000L);
	ack=strncmp(buf,"ACK",3);
	if(ack==0) return true;
	else return false;	
	
}//Check_If_ACK**********************************************************
//*************************************************************************
//  Check if all data received. A "done" string tells the data OK.
//	Returns 1, if "done"
//	        0, if @@@ string.  Resend request.
//		   -1, something else.  
//*************************************************************************
short Check_If_Done_Or_Resent(ulong *val0, ulong *val1)
{
uchar	hbuf[3], buf[11], bfo[8];
short   done, j;
int	    resent;
short	i, buf_size=11;
int	 	crc_rec, crc_chk;						//crc
	
	//Read AUX and check if ACK
	TURxFlush(tuport2);
	//RTCElapsedTimerSetup(&rt);
	flogf("CHECKING IF ANY BAD BLKS\n");RTCDelayMicroSeconds(10000L);
	
	hbuf[0]=TURxGetByte(tuport2,true);
	//hbuf[0]=TURxGetByteWithTimeout(tuport2,40000);//takes 20 seconds for 41 blks (13sec for 2 blks)
	//elp=RTCElapsedTime(&rt);
	for(i=1;i<3;i++)
		{
		hbuf[i] = TURxGetByteWithTimeout(tuport2,IRIDRxTIMEOUT*10);
		TickleSWSR();
		if(hbuf[i] == -1) 
			{
			LogFailure(17);
			i = 3;
			return -1;
			}
		}
	done = strncmp(hbuf,"don",3);
	resent = strncmp(hbuf,"@@",2);

	if(resent==0)//@@@ received, resent request
		{
		//RTCElapsedTimerSetup(&rt);
		buf[0]=TURxGetByte(tuport2,true);
		//buf[0]=TURxGetByteWithTimeout(tuport2,10);//400usec
		//elp=RTCElapsedTime(&rt);
		for(i=1;i<buf_size;i++)
			{
			buf[i] = TURxGetByteWithTimeout(tuport2,IRIDRxTIMEOUT*10);
			TickleSWSR();
			if(buf[i] == '-1') 
				{
				LogFailure(16);
				return -1;
				}
			}

		crc_rec=buf[0]<<8|buf[1]; 					//crc MSB<<8+MSB
		crc_chk= Calc_Crc(buf+2,9);
		if(buf[2] == 'R' && crc_chk == crc_rec)//Header OK, proceed.
			{
			//debug
			DBG(uprintf("%c %d %d %d %d %d %d %d %d\n",buf[2],buf[3],buf[4],buf[5],buf[6],
			buf[7],buf[8],buf[9],buf[10]));
			//debug ends
			for(i=0;i<8;i++)//reverse the order within the byte
				{
				bfo[i]=0x00;//clear
				for(j=0;j<8;j++)bfo[i]|=(uchar)((uchar)buf[i+3]>>j & 0x01)<<(7-j);
				}
			*val0=bfo[0]|((ulong)bfo[1])<<8|((ulong)bfo[2])<<16|((ulong)bfo[3])<<24;
			*val1=bfo[4]|((ulong)bfo[5])<<8|((ulong)bfo[6])<<16|((ulong)bfo[7])<<24;
			}else
			{
			flogf("Resend request garbled\n");RTCDelayMicroSeconds(10000L);
			return 0;
			}
		}
	//DBG(uprintf("%c%c%c\n",hbuf[0],hbuf[1],hbuf[2])); RTCDelayMicroSeconds(10000L);

	if(done ==0)
		{ 
		return 1;  //"done" string is confirmed
		}
	else if(resent ==0)
		{
		return 0; //"@@@" string confirmed - retry the TX
		}
	else return -1;//something else, message garbled	
}//Check_If_Done_Or_Resent
//********************************************************************************************
// Calc_Crc
// Calculate CRC (16 bits long).  Use long integer for calculation, but returns 16-bit int.
// Converted from P. McLane's C code.
// H. Matsumoto 
//*******************************************************************************************
int Calc_Crc(unsigned char *buf, int cnt)
	/* calculate 16-bit CRC of contents of buf */
{
	long accum;
	int i, j;
	accum=0x00000000;
	//char ch;
	//cprintf("%s\n",buf);
	//RTCDelayMicroSeconds(100000L);
	if(cnt <= 0) return 0;
	while( cnt-- )
	{
		accum |= *buf++ & 0xFF;
		for( i=0; i<8; i++ )
		{
			accum <<= 1;
			if(accum & 0x01000000)  accum ^= 0x00102100;
		}
	}
	/* The next 2 lines forces compatibility with XMODEM CRC */
	for(j = 0; j<2; j++)
	{
		accum |= 0 & 0xFF;
		for( i=0; i<8; i++ )
		{
			accum <<= 1;
			if(accum & 0x01000000)  accum ^= 0x00102100;
		}
	}
	//cprintf("%x%x\n",(char)((accum >>16)  & 0x000000FF),(char)((accum >>8) & 0x000000FF));
	//RTCDelayMicroSeconds(100000L);
	return( accum >> 8 );
}
/******************************************************************************
** itoa()
** Convert an interger to 2-byte uchar.  MAB in first, LASB out last.
** H. Matsumoto
******************************************************************************/
void itoa(ushort a, uchar *b)
	{
	b[0] = (a & 0xFF00)>>8;
	b[1] = (a & 0x00FF);
	}
// itoa
//*****************************************************************************
// Write_Header_To_File
// Write location, GMT, system name, target depth, drift in seconds.
// 	
//*****************************************************************************
bool Write_Header_To_File(char *fname, char *LocTimeHeader, ushort TimeSize,\
  		char *DiveHeader, ushort DiveSize, char *DrifChr, ushort DrifSize)
{
short i;
	//Open a file.  Create one, if not there.
	if((det_file = fopen(fname,"w+")) == NULL)
		{
	 	flogf("Cannot open file\n");//critical error
	 	//Stop logging and come up to the surface
		return false;
		}
	//debug
	flogf("CREATING FILE %s\n",fname);
	for(i=0;i<TimeSize;i++)cprintf("%c",*(LocTimeHeader+i));
	cprintf("\n");
	for(i=0;i<DiveSize;i++)cprintf("%c",*(DiveHeader+i));
	cprintf("\n");
	fwrite((void *)LocTimeHeader, sizeof(char), TimeSize, det_file);
	fwrite("\n",sizeof(char),1,det_file);
	fwrite((void *)DiveHeader,sizeof(char),DiveSize, det_file);
	fwrite((void *)DrifChr,sizeof(char),DrifSize, det_file);
	fwrite("\n",sizeof(char),1,det_file);
	fclose(det_file);
	return true;
}
bool Write_Trailer_To_File(char *fname, char *LocTimeHeader, ushort TimeSize,\
  		char *DiveHeader, ushort DiveSize, char *DrifChr, ushort DrifSize)
	{
	//Open the file to append.
	if((det_file = fopen(fname,"a+")) == NULL)
		{
	 	flogf("Cannot open file\n");//critical error
		//Stop logging
		return false;
		}
	//debug
	flogf("APPENDING A NEW DAT FILE TO THE OLD FILE %s\n",fname);
	fwrite((void *)LocTimeHeader, sizeof(char), TimeSize, det_file);
	fwrite("\n",sizeof(char),1,det_file);
	fwrite((void *)DiveHeader,sizeof(char),DiveSize, det_file);
	fwrite((void *)DrifChr,sizeof(char),DrifSize, det_file);
	fwrite("\n",sizeof(char), 1, det_file);
	fclose(det_file);
	return true;
}	
bool Write_SysLocGMT_To_File(char *fname, char *LocTimeHeader, ushort TimeSize)
{
	short i;
	//Open the file.  Append.
	if((det_file = fopen(fname,"w")) == NULL)
		{
	 	flogf("Cannot open file\n");//critical error
		//Stop logging
		return false;
		}
	//debug
	flogf("CREATING A LOCATION FILE %s\n",fname);
	for(i=0;i<TimeSize;i++)cprintf("%c",*(LocTimeHeader+i));
	cprintf("\n");
	fwrite((void *)LocTimeHeader, sizeof(char), TimeSize, det_file);
	fwrite("\n",sizeof(char),1, det_file);
	fclose(det_file);
	return true;
}	
	

