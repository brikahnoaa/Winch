//******************************************************************************\
// ProvorControl subroutines to give command or receive status from
// PROVOR over the tuport1 serial line.  PROVOR is a half duplex mode. It returns
// what you type in, which makes the program to be complex.  There are delays 
// of reaction, and the delays vary depeindg on what command you are giving.  Note 
// that !G command would not release the PROVOR control until it gets to a specified 
// depth, which caused annoying problem.  If the depth is shallower than the 
// specified depth, after reaching to the bottom it would never release the control
// back to the user.
//
// Jan 5, 2005 HM, NOAA
//******************************************************************************/

#include	<cfxbios.h>		// Persistor BIOS and I/O Definitions
#include	<cfxpico.h>		// Persistor PicoDOS Definitions

#include	<ctype.h>
#include	<errno.h>
#include	<stdarg.h>
#include	<stddef.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<time.h>

#define	PRVRCOM		26  //COM switch pin for PROVOR (1=OFF,0=ON)
#define PRVRRES		27	//Reset PROVOR

PinIODefineTPU(PRVRCOM,		5);//26
PinIODefineTPU(PRVRRES,		6);//27

TUPort	*tuport1; 		//PROVOR COM
short	rxch1,txch1;	//PROVOR COM chan
long	baud1 = 9600L;	//PROVOR COM baud rate
ulong 	PWait = 100000L;//wait in usec before PROVOR COM command
TUChParams ProvParams;

//Routines used for PROVOR control
short InqProvorStatus(short *pStatus, short *lvStatus, short *phase, short *pDur,\
		short *vDur, short *pressure);//Inquire status of PROVOR by ?ST				   
short RecProvorStatus(short *pStatus, short *lvStatus, short *phase, short *pDur,\
		short *vDur, short *pressure);//Just receive status.  Use it after !G				   
void  Delay(short centisecs);
bool  OpenCommProv(void);
bool  CloseCommProv(void);
short BatteryCheck(short *batVolt);
short GetExtPressure(short *extPressure);
short GetReservoirLevel(short *reservLevel);
short GetIntVacuum(short *intVacuum);
short ActivateValve(short valvDuration);
short ActivatePump(short pumpDuration);
short FloatDiagonostic(short *cpuSt, short *intVacuum, short *vacSt, 
	short *reservLevel, short *tankSt, short *batVolt, short *batSt,
	short *extPressure, short *pSensorSt); //More comprehensive check on PROVOR than ?ST
short GoTo(short pDepth, short pError, short pSample);					   
void  itoal(ushort a, uchar b[4]);//change integer to ASC
void  ResetProv(void);
short EmptyReservoir(short duration);
short BalanceReservoir(short duration);
void  LowPowerDelay(ulong Delay);  //Delay is 0.102 sec increment
short StopWhatIsDoing(void);

//*************************************************************************
// GoTo
// Send command to PROVOR to go to a specified parking depth (dBar).
// Input : 	pDepth - Parking depth in dBar (0 to 2000) 
// 			pError - Depth torelance in dBar (50 to 100)
//			pSample- Sampling interval at parking depth (in min, 1-120) 
// It takes about 40 seconds to respond at surface mode, 10min at the bottom
// mode.
// Rev. 1, Jan 4, 2005 HM
//*************************************************************************	
short GoTo(short pDepth, short pError, short pSample)
	{
	short i, errcnt = 0;
	short ich;
	uchar b[64],chk[6], pDepthC[4], pErrorC[4], pSampleC[4];

	//RTCTimer rt;
	//ulong	elp;
	//flogf("!G %d, %d, %d\n",pDepth, pError, pSample);
	itoal(pDepth, pDepthC); //convert 2-byte int to 4-byte asc
	itoal(pError, pErrorC);
	itoal(pSample,pSampleC);
	
	TURxFlush(tuport1);
	RTCDelayMicroSeconds(PWait);
	
	TickleSWSR();
	//PROVOR echoes back
	TUTxPutByte(tuport1,'!',true);
	TURxGetByte(tuport1,true);
	TUTxPutByte(tuport1,'G',true);
	TURxGetByte(tuport1,true);
	TUTxPutByte(tuport1,' ',true);
	TURxGetByte(tuport1,true);
	RTCDelayMicroSeconds(PWait);

	for(i=0;i<4;i++)//Send Depth in (dBAR) or meter
		{
		TUTxPrintf(tuport1,"%d",pDepthC[i]);
		TURxGetByte(tuport1,true);
		}
	TUTxPutByte(tuport1,',',true);//comma
	TURxGetByte(tuport1,true);
		
	for(i=0;i<4;i++)//send error tolerance 50 - 100m
		{
		TUTxPrintf(tuport1,"%d",pErrorC[i]);
		TURxGetByte(tuport1,true);
		}
	TUTxPutByte(tuport1,',',true);//comma
	TURxGetByte(tuport1,true);
		
	for(i=0;i<4;i++)//send sampling rate 1min to 120 min
		{
		TUTxPrintf(tuport1,"%d",pSampleC[i]);
		TURxGetByte(tuport1,true);
		}
	//TickleSWSR();
		
	TUTxPutByte(tuport1,'\r',true);//carriage return
	TURxGetByte(tuport1,true);

	//Get response (83msec typical delay for GO TO action)
	b[0]=TURxGetByte(tuport1,true);
	//RTCElapsedTimerSetup(&rt);
	//for(i=1;i<63;i++)b[i]=TURxGetByte(tuport1,true);
	ich=TURxGetBlock(tuport1,&b[1],62,160);
	//elp=RTCElapsedTime(&rt);
	//cprintf("%ld\n",elp);
	//cprintf("%s %d\n", b, ich);
	//putflush();
	
	if(strstr(b,"ERROR MSG"))//error encountered. Out the loop and try again
		{
		return 10;
		//printf("ERROR MSG\n");
		//putflush();
		}
	
	//check for errors - look for the correct response
	strncpy(chk,"]GO TO",6); 
	for(i=1;i<6;i++)
		if(b[i]!= chk[i])
			errcnt++;
			//printf("%d\n",errcnt);
			//putflush();
	return errcnt;
}
//---------------------GoTo-----------------------------------------------//
//*************************************************************************
// ActivatePump
// Activate pump for specified deci-seconds (4 digits)
// Input 
// 	pumpDuration - Pump activation period in deciseconds
// Rev. 1, Dec. 28, 2004 HM
//*************************************************************************	
short ActivatePump(short pumpDuration)
	{
	short i, errcnt = 0;
	short ich;
	uchar b[12], chk[6], par[4];

	//RTCTimer rt;
	//ulong	elp;

	itoal(pumpDuration, par); //convert 2-byte int to 4-byte asc
	TURxFlush(tuport1);
	RTCDelayMicroSeconds(PWait);
	//PROVOR echoes back
	TUTxPutByte(tuport1,'!',true);
	TURxGetByte(tuport1,true);
	TUTxPutByte(tuport1,'P',true);
	TURxGetByte(tuport1,true);
	TUTxPutByte(tuport1,' ',true);
	TURxGetByte(tuport1,true);
	RTCDelayMicroSeconds(PWait);
	for(i=0;i<4;i++)
		{
		TUTxPrintf(tuport1,"%d",par[i]);
		TURxGetByte(tuport1,true);
		}
	TUTxPutByte(tuport1,'\r',true);
	TURxGetByte(tuport1,true);

	//Response (55msec typical delay for pump action)
	b[0]=TURxGetByte(tuport1,true);
	//RTCElapsedTimerSetup(&rt);
	//for(i=1;i<12;i++)b[i]=TURxGetByte(tuport1,true);
	ich=TURxGetBlock(tuport1,&b[1],11,100);
	//elp=RTCElapsedTime(&rt);
	//cprintf("%ld\n",elp);
	//cprintf("%s %d\n", b, ich);
	//putflush();
	
	if(strstr(b,"ERROR MSG"))//error try again
		{
		return 10;
		}
	
	//check for errors
	strncpy(chk,"]!PUMP",6);
	for(i=1;i<6;i++)
		if(b[i]!= chk[i])
			errcnt++;
			//printf("%d\n",errcnt);
			//putflush();
	return errcnt;
}//-----------------ActivatePump---------------------------------------------//
	
//*************************************************************************
// ActivateValve
// Activate low flow valve for specified deci-seconds.
// input 
//	valveDuration - Valve activation period in deciseconds
// Rev. 1, Dec. 28, 2004 HM
//*************************************************************************	
short ActivateValve(short valveDuration)
	{
	short i, errcnt = 0;
	short ich;
	uchar b[18],chk[10],par[4];

	//RTCTimer rt;
	//ulong	elp;

	//printf("%d\n",valveDuration);
	//putflush();
	itoal(valveDuration, par); //convert 2-byte int to 4-byte asc
	TURxFlush(tuport1);
	RTCDelayMicroSeconds(PWait);

	//PROVOR echoes back
	TUTxPutByte(tuport1,'!',true);
	TURxGetByte(tuport1,true);
	TUTxPutByte(tuport1,'L',true);
	TURxGetByte(tuport1,true);
	TUTxPutByte(tuport1,' ',true);
	TURxGetByte(tuport1,true);
	RTCDelayMicroSeconds(PWait);
	
	for(i=0;i<4;i++)
		{
		TUTxPrintf(tuport1,"%d",par[i]);
		TURxGetByte(tuport1,true);
		}
	TUTxPutByte(tuport1,'\r',true);
	TURxGetByte(tuport1,true);

	//Response (19.2msec delay for reservoir)
	b[0]=TURxGetByte(tuport1,true);
	//RTCElapsedTimerSetup(&rt);
	//for(i=1;i<15;i++)b[i]=TURxGetByte(tuport1,true);
	ich=TURxGetBlock(tuport1,&b[1],15,100);
	//elp=RTCElapsedTime(&rt);
	//cprintf("%ld\n",elp);
	//cprintf("%s %d\n", b, ich);
	//putflush();
	
	if(strstr(b,"ERROR MSG"))//error try again
		{
		return 10;
		}
	
	//check for errors
	sprintf(chk,"]!LOW FLOW");
	for(i=1;i<10;i++)
		if(b[i]!= chk[i])
			errcnt++;
			//printf("%d\n",errcnt);
			//putflush();
	//valveDuration = atoi(&b[11]);//valve active duration in decisecond
	return errcnt;
}//-----------------ActivateValve---------------------------------------------//
//*************************************************************************
// GetReservoirLevel
// Get the reservoir level in cm3 through COM1 from Provor
// Rev. 1, Dec. 28, 2004 HM
//*************************************************************************	
short GetReservoirLevel(short *reservLevel)
	{
	short i, errcnt = 0;
	short ich;
	uchar b[30], chk[8];

	//RTCTimer rt;
	//ulong	elp;
	
	TURxFlush(tuport1);
	RTCDelayMicroSeconds(PWait);
	//PROVOR echoes back
	TUTxPutByte(tuport1,'?',true);
	TURxGetByte(tuport1,true);
	TUTxPutByte(tuport1,'L',true);
	TURxGetByte(tuport1,true);
	TUTxPutByte(tuport1,'E',true);
	TURxGetByte(tuport1,true);
	TUTxPutByte(tuport1,'\r',true);
	TURxGetByte(tuport1,true);

	//Response (2200msec for reservoir)
	b[0]=TURxGetByte(tuport1,true);
	//RTCElapsedTimerSetup(&rt);
	//for(i=1;i<28;i++)b[i]=TURxGetByte(tuport1,true);
	ich=TURxGetBlock(tuport1,&b[1],29,3000);
	//elp=RTCElapsedTime(&rt);
	//cprintf("%ld\n",elp);
	//cprintf("%s %d\n", b, ich);
	//putflush();
	
	if(strstr(b,"ERROR MSG"))//error try again
		{
		return 10;
		}
	
	//check for errors
	sprintf(chk,"]?LEVEL ");
	for(i=1;i<8;i++)
		if(b[i]!= chk[i])
			errcnt++;
			//printf("%d\n",errcnt);
			//putflush();
		if(b[29]!='\r')
	 		errcnt++;
			//printf("%d\n",errcnt);
			//putflush();
	//store values
	*reservLevel = atoi(&b[21]);//returns external pressure in dB
	return errcnt;
}
//---------------------------GetReservoirLevel-----------------------------// 
//*************************************************************************
// GetIntVacuum
// Get the internal vacuum level inside the float in mBars through COM1 
// 
// Rev. 1, Dec. 29, 2004 HM
//*************************************************************************	
short GetIntVacuum(short *intVacuum)
	{
	short i, errcnt;
	short ich;
	uchar b[26],chk[14];

	//RTCTimer rt;
	//ulong	elp;
	
	TURxFlush(tuport1);
	RTCDelayMicroSeconds(PWait);
	//PROVOR echoes back
	TUTxPutByte(tuport1,'?',true);
	TURxGetByte(tuport1,true);
	TUTxPutByte(tuport1,'V',true);
	TURxGetByte(tuport1,true);
	TUTxPutByte(tuport1,'\r',true);
	TURxGetByte(tuport1,true);

	//Response (230msec for internal vacuum)
	b[0]=TURxGetByte(tuport1,true);
	//RTCElapsedTimerSetup(&rt);
	//for(i=1;i<27;i++)b[i]=TURxGetByte(tuport1,true);
	ich=TURxGetBlock(tuport1,&b[1],25,400);
	//elp=RTCElapsedTime(&rt);
	//cprintf("%ld\n",elp);
	//cprintf("%s %d\n", b, ich);
	//putflush();
	
	if(strstr(b,"ERROR MSG"))//error try again
		{
		return 10;
		}
	
	//check for errors
	strncpy(chk,"]?VACCUM LEVEL",14);//French spelling?
	errcnt=0;
	for(i=1;i<14;i++)
		if(b[i] != chk[i])
			errcnt++;
			//cprintf("%d\n",errcnt);
			//putflush();
		if(b[25]!='\r')
	 		errcnt++;
			//cprintf("%d\n",errcnt);
			//putflush();

	*intVacuum = atoi(&b[15]);//returns internal vacuum in mBar
	return errcnt;
}
//---------------------------GetIntVacuum-------------------------------// 
//***********************************************************************
// GetExtPressure
// Reads external pressure in dB from Provor
// Rev. 1 Dec 28, 2004 HM
//***********************************************************************
short GetExtPressure(short *extPressure)
	{
	short i, errcnt = 0;
	short ich;
	uchar b[26],chk[10];

	//RTCTimer rt;
	//ulong	elp;
	
	TURxFlush(tuport1);
	TURxFlush(tuport1);
	RTCDelayMicroSeconds(PWait);

	//PROVOR echoes back
	TUTxPutByte(tuport1,'?',true);
	TURxGetByte(tuport1,true);
	TUTxPutByte(tuport1,'P',true);
	TURxGetByte(tuport1,true);
	TUTxPutByte(tuport1,'R',true);
	TURxGetByte(tuport1,true);
	TUTxPutByte(tuport1,'\r',true);
	TURxGetByte(tuport1,true);

	//Response (1610msec for external pressure - slow)
	b[0]=TURxGetByte(tuport1,true);
	//RTCElapsedTimerSetup(&rt);
	//for(i=1;i<24;i++)b[i]=TURxGetByte(tuport1,true);
	ich=TURxGetBlock(tuport1,&b[1],24,2500);
	//elp=RTCElapsedTime(&rt);
	//cprintf("%ld\n",elp);
	//cprintf("%s %d\n", b, ich);

	if(strstr(b,"ERROR MSG"))//error try again
		{
		return 10;
		}
	
	//check for errors
	sprintf(chk,"]?PRESSURE");
	for(i=1;i<10;i++)
		if(b[i]!=chk[i])
			errcnt++;
		if(b[23]!='\r')
	 		errcnt++;
	
	*extPressure  = atoi(&b[12]);//external pressure in dB
	return errcnt;
}
//-----------------------GetExtPressure---------------------------------//
//************************************************************************
// BatteryCheck
// Check the battery voltage. Returns in mV.
// Dec. 30, 2004 HM, NOAA
//************************************************************************
short BatteryCheck(short *batVolt)
	{
	short i, errcnt = 0;
	short ich;
	uchar b[28],chk[10];

	//RTCTimer rt;
	//ulong	elp;
	
	TUTxFlush(tuport1);
	TURxFlush(tuport1);
	RTCDelayMicroSeconds(PWait);
	//PROVOR echoes back
	TUTxPutByte(tuport1,'?',true);
	TURxGetByte(tuport1,true);
	TUTxPutByte(tuport1,'B',true);
	TURxGetByte(tuport1,true);
	TUTxPutByte(tuport1,'\r',true);
	TURxGetByte(tuport1,true);

	//Response (550msec delay for voltage)
	b[0]=TURxGetByte(tuport1,true);
	//RTCElapsedTimerSetup(&rt);
	//for(i=1;i<28;i++)b[i]=TURxGetByte(tuport1,true);
	ich=TURxGetBlock(tuport1,&b[1],27,1000);
	//elp=RTCElapsedTime(&rt);
	//cprintf("%ld\n",elp);
	//cprintf("%s %d\n", b, ich);

	if(strstr(b,"ERROR MSG"))//error try again
		{
		return 10;
		}
	
	//RTCDelayMicroSeconds(10000L);
	//check for errors
	sprintf(chk,"]?BATTERY");
	for(i=0;i<9;i++)
		if(b[i]!=chk[i])
			errcnt++;
		if(b[26]!='\r')
	 		errcnt++;
	
	//store values
	*batVolt  = atoi(&b[18]);//Battery voltage in mV
	//cprintf("Battery Voltage %dmV\n",*batVolt);
	//putflush();
	return errcnt;
}
//------------------BatteryCheck-------------------------------------------
//*************************************************************************
// InqProvorStatus
// Inquire the status of PROVOR float
//	pStatus 	Status of pump - 0 or 1 (ON)
//	vStatus  	Status of low-flow valve - 0 or 1 (ON)
//	prevPhase  	Previous phase
//	phase 		(0-9)
//	pDur  		5-digit duration of pump activation in decisec 
//	vDur  		5-digit duration of low flow valve in decisec
//	pressure 	pressure in dBARs
//  returns 0 if no errors.
//
// Dec. 30, 2004 HM NOAA
//*************************************************************************
short InqProvorStatus(short *pStatus, short *lvStatus, short *phase, short *pDur,\
		short *vDur, short *pressure)				   
{
	short i, errcnt = 0;
	short ich;
	uchar b[34],chk[10];
	//uchar	rc;
	//RTCTimer rt;
	//ulong elp;
	TURxFlush(tuport1);
	TUTxFlush(tuport1);
	RTCDelayMicroSeconds(PWait);
	//PROVOR echoes back
	TUTxPutByte(tuport1,'?',true);
	TURxGetByte(tuport1,true);
	TUTxPutByte(tuport1,'S',true);
	TURxGetByte(tuport1,true);
	TUTxPutByte(tuport1,'T',true);
	TURxGetByte(tuport1,true);
	TUTxPutByte(tuport1,'\r',true);
	TURxGetByte(tuport1,true);
	//cprintf("%s\r", init);
	//RTCDelayMicroSeconds(1000L);

	//Wait for the response. Delay is ~640msec
	//RTCElapsedTimerSetup(&rt);
	TickleSWSR();
	//b[0]=TURxGetByte(tuport1,true);//9/09/05 HM
	b[0]=TURxGetByteWithTimeout(tuport1,7500);//9/09/05 HM
	//for(i=1;i<33;i++)b[i]=TURxGetByte(tuport1,true);
	ich=TURxGetBlock(tuport1,&b[1],33,1000);
	//elp=RTCElapsedTime(&rt);
	//cprintf("%ld\n",elp);

	if(strstr(b,"ERROR MSG"))//error. Out the loop and try again
		{
		return 10;
		}
	
	//RTCDelayMicroSeconds(1000L);
	//check for errors
	sprintf(chk,"]?STATUS ");
	for(i=0;i<9;i++)
		if(b[i]!=chk[i])
			errcnt++;
		if(b[31]!='\r')
	 		errcnt++;
	 		//printf("%d\n",errcnt);
	 		//putflush();
	
	//returns values
	*pStatus   = atoi(&b[9]); //Status of Pump 0 or 1 (ON)
	*lvStatus  = atoi(&b[11]);//Status of low flow valve 0 or 1(ON)
	*phase     = atoi(&b[25]);//Current phase in one byte (0-9)
	*pDur      = atoi(&b[13]);//5 dig -duration of pump activation in decisec 
	*vDur      = atoi(&b[19]);//5 digit duration of low flow valve in decisec
	*pressure  = atoi(&b[27]);//pressure in dBARs
	//uprintf("Status %d,%d,%05d,%05d,%d,%04d\n",*pStatus,*lvStatus,*pDur,*vDur,*phase,*pressure);
	//putflush();
	
	return errcnt;
}//-----------------------ProvorStatusInq--------------------------------------
//*****************************************************************************
// Receive status report from PROVOR after !Go command.  If no character received
// for 1000msec, returns error.  It also checks if there is an ERROR MSG.
// Returns "pStatus","lvStatus","phase","pDur","vDur",and "pressure".
// H. Matsumoto 01/27/05 NOAA
//*****************************************************************************   
short RecProvorStatus(short *pStatus, short *lvStatus, short *phase, short *pDur,\
		short *vDur, short *pressure)				   
{
	short i, errcnt = 0;
	short ich;
	uchar b[34],chk[10];

	b[0]=TURxGetByte(tuport1,true);
	//for(i=0;i<33;i++)b[i]=TURxGetByte(tuport1,true);
	ich=TURxGetBlock(tuport1,&b[1],33,1300); // Limit to 2000msec
	//flogf("%s\n",b);//debug Sept/05
	//putflush();//debug
	
	if(strstr(b,"ERROR MSG"))//error. 
		{
		return 10;
		}
	//RTCDelayMicroSeconds(1000L);

	//check for errors
	sprintf(chk,"]?STATUS ");
	for(i=0;i<9;i++)
		if(b[i]!=chk[i])
			errcnt++;
		if(b[31]!='\r')
	 		errcnt++;
	 		//printf("%d\n",errcnt);
	 		//putflush();
	
	//returns values
	if(errcnt<2)
		{
		*pStatus   = atoi(&b[9]); //Status of Pump 0 or 1 (ON)
		*lvStatus  = atoi(&b[11]);//Status of low flow valve 0 or 1(ON)
		*phase     = atoi(&b[25]);//Current phase in one byte (0-9)
		*pDur      = atoi(&b[13]);//5 dig - duration of pump activation in decisec 
		*vDur      = atoi(&b[19]);//5 digit duration of low flow valve in decisec
		*pressure  = atoi(&b[27]);//pressure in dBARs
		}
	//flogf("Status %d,%d,%05d,%05d,%d,%04d\n",*pStatus,*lvStatus,*pDur,*vDur,*phase,*pressure);
	//putflush();
	
	return errcnt;
}//-----------------------Status--------------------------------------

//*************************************************************************
// FloatDiagonostic
// Reports comprehensive status of PROVOR float
// cpuSt		- CPU Status (1=good, 0= bad)
// intVacuum, 	- internal vacuum
// vacSt,		- vacuum status (1=good, 0=bad)
// reservLevel, - oil reservoir level
// tankSt,		- reservoir status
// batVolt, 	- battery voltage
// batSt,		- battery status
// extPressure, - external pressure
// pSensorSt	- pressure sensor status
// Dec. 30, 2004 HM NOAA
//*************************************************************************
short FloatDiagonostic(short *cpuSt,short *intVacuum, short *vacSt, 
	short *reservLevel, short *tankSt,short *batVolt, short *batSt,
	short *extPressure, short *pSensorSt)				   
{
	short i, errcnt = 0;
	//short ich;
	uchar b[152],chk[8],status[4];
	//uchar	rc;
	//RTCTimer rt;
	//ulong elp;
	char ref[4]="FAIL";
	int  same;
	
	TURxFlush(tuport1);
	TUTxFlush(tuport1);
	RTCDelayMicroSeconds(PWait);
	//PROVOR echoes back
	TUTxPutByte(tuport1,'!',true);
	TURxGetByte(tuport1,true);
	TUTxPutByte(tuport1,'C',true);
	TURxGetByte(tuport1,true);
	TUTxPutByte(tuport1,'\r',true);
	TURxGetByte(tuport1,true);
	
	//cprintf("%s\r", init);
	//RTCDelayMicroSeconds(1000L);

	//Response delay 1.390sec
	//RTCElapsedTimerSetup(&rt);
	b[0]=TURxGetByte(tuport1,true);
	for(i=1;i<151;i++)b[i]=TURxGetByte(tuport1,true);
	//ich=TURxGetBlock(tuport1,&b[1],151,1500);
	//elp=RTCElapsedTime(&rt);
	//cprintf("%ld\n",elp);
	
	//putflush();
	//cprintf("%s\n",b);
	//putflush();

	if(strstr(b,"ERROR MSG"))//error try again
		{
		return 10;
		}
	
	//RTCDelayMicroSeconds(1000L);
	//check for errors
	strncpy(chk,"]!CHECK",7);
	errcnt=0;
	for(i=1;i<7;i++)if(b[i] != chk[i]) errcnt++;
	//ptr=strstr(b,chk);
	//if(ptr)errcnt=0;
	//cprintf("errcount =%d\n", errcnt);
	//putflush();
	
	//returns values
	strncpy(status, &b[33], 4); //CPU status OK or FAIL
	//cprintf("%s\n",status);
	//putflush();
	same=strncmp(status,ref,4);
	if(same!=0)
		{
		*cpuSt=1;
		//cprintf("CPU OK\n");
		//putflush();
		}else 
		{
		*cpuSt=0;
		//cprintf("CPU Failed\n");
		//putflush();
		}
	strncpy(status,&b[57],4); //Vacuum status OK or FAIL
	//cprintf("%s\n",status);
	//putflush();
	same=strncmp(status,ref,4);
	if(same!=0)
		{
		*vacSt=1;
		//cprintf("Vacuum OK\n");
		//putflush();
		}else 
		{
		*vacSt=0;
		//cprintf("Vacuum Failed\n");
		//putflush();
		}
		
	for(i=0;i<4;i++)status[i] = b[83+i]; //Tank status OK or FAIL
	same=strncmp(status,ref,4);
	if(same!=0)
		{
		*tankSt=1;
		//cprintf("Tank OK\n");
		//putflush();
		}else 
		{
		*tankSt=0;
		//cprintf("Tank Failed\n");
		//putflush();
		}
		
	for(i=0;i<4;i++)status[i] = b[111+i]; //Voltage OK or FAIL
	same=strncmp(status,ref,4);
	if(same!=0)
		{
		*batSt=1;
		//cprintf("Voltage OK\n");
		//putflush();
		}else 
		{
		*batSt=0;
		//cprintf("Voltage Failed\n");
		//putflush();
		}
		
	for(i=0;i<4;i++)status[i] = b[146+i]; //Pressure sensor OK or FAIL
	same=strncmp(status,ref,4);
	if(same!=0)
		{
		*pSensorSt=1;
		//cprintf("Pressure sensor OK\n");
		//putflush();
		}else 
		{
		*pSensorSt=0;
		//cprintf("Pressure sensor Failed\n");
		//putflush();
		}
	*intVacuum  = atoi(&b[45]);//internal vacuum in mBAr
	*reservLevel= atoi(&b[73]);//tank reservoir level in cm3
	*batVolt    = atoi(&b[101]);//battery voltage in mV 
	*extPressure= atoi(&b[132]);//pressure in cBar
	//cprintf("%04d,%04d,%05d,%05d\n",*intVacuum,*reservLevel,*batVolt,*extPressure);
	//putflush();
	
	return errcnt;
}
//--------------------FloatDiagonostic---------------------------------------------
//**********************************************************************************
// OpenCommProv
// Open com port for PROVOR using CF2 TPU I/O (33,34).  Enable pin 26 to PROVOR com
// to open.  Enable MAX322 transmitter with pin 29 and 30. 
// Jan 5, 2005, HM
//**********************************************************************************
bool OpenCommProv(void)
{
	bool status=true;
	flogf("OPENING PROV COM\n");RTCDelayMicroSeconds(10000L);
	PinSet(29);					// enable MAX322 transmitter (/OFF)
	PinClear(30);				// enable MAX322 receivers (/EN)
	RTCDelayMicroSeconds(100000L);
	TUInit(calloc, free);
	RTCDelayMicroSeconds(10000L);
	rxch1 = TPUChanFromPin(33);	// AUX1 RS232 receiver on PicoDAQ2 board
	txch1 = TPUChanFromPin(34);	// AUX1 RS232 transmitter on PicoDAQ2 board
	LowPowerDelay(10L);
	//memcpy(&ProvParams,TUGetDefaultParams(),sizeof(TUChParams));
	//ProvParams.baud=baud1;
	//tuport1= TUOpen(rxch1,txch1,baud1, &ProvParams);
	tuport1= TUOpen(rxch1,txch1,baud1, 0);
	RTCDelayMicroSeconds(10000L);
 	if (tuport1 == 0 )
	   {
		cprintf("\n!!! Unexpected error opening TU1 channel\n");
	   	BIOSReset();
	   }  
	PinSet(PRVRCOM);			// enable PROVOR side COM first
	RTCDelayMicroSeconds(10000L);
	
	SCIRxSetBuffered(true);		// switch SCI to buffered receive mode
	RTCDelayMicroSeconds(10000L);
	SCITxSetBuffered(true);		// switch SCI to buffered transmit mode
	Delay(40);	//Wait for 4 sec until Provor COM is ready
	
	TURxFlush(tuport1);
	TUTxFlush(tuport1);
	return status;
}
//----------------OpenCommProv-------------------------------------------------
//***********************************************************************
// CloseCommProv
// Close PROVOR com
// Jan 5, 2005, HM, NOAA
//***********************************************************************
bool CloseCommProv(void)
{
	bool status=false;
	PinClear(PRVRCOM);		// Shut down PROVOR com
	RTCDelayMicroSeconds(10000L);
	
	if(tuport1)
		{
		TURxFlush(tuport1);
		//TUTxFlush(tuport1);
		flogf("CLOSING PROVOR COM\n");cdrain();coflush();RTCDelayMicroSeconds(10000L);
		TUClose(tuport1);
		TURelease();
		}
	PinClear(29);			// disable MAX322 transmitter (/OFF)
	PinSet(30);				// disable MAX322 receivers (/EN)
	RTCDelayMicroSeconds(2000000L);
	return status;
}
//***********************************************************************
// Delay
// Delay for a specified centiseconds
// Jan 5, 2005, HM, NOAA
//***********************************************************************
void Delay(short centisecs)
//delay this many hundredths of a second
{
	ushort i;
	for(i=0;i<100*centisecs;i++)	
	   Delay1ms();
}
//******************************************************************************
// itoal()
// Convert a 4-digit interger to 4-byte uchar. MSB in first, LSB last out.
// H. Matsumoto
//*****************************************************************************/
void itoal(ushort a, uchar b[4])
	{
	b[0] = (a/1000);
	b[1] = (a-b[0]*1000)/100;
	b[2] = (a-b[0]*1000-b[1]*100)/10;
	b[3] = a-b[0]*1000-b[1]*100-b[2]*10;
	}
//******************************************************************************
// ResetProv
//******************************************************************************
void ResetProv()
{
	PinSet(PRVRRES);RTCDelayMicroSeconds(1000000L);
	PinClear(PRVRRES);
}
//******************************************************************************
// EmptyReservoir
// Check the reservoir level and empty out if not "zero."
//******************************************************************************
short EmptyReservoir(short duration)
{
	short	result;
	short   reservLevel,i,numCheck,length, increment;
	increment=30;       //in deciseconds (ds) = seconds x 10
	result=10;
	while(result>1) result = GetReservoirLevel(&reservLevel);
	flogf("RESERVOIR LEVEL %d cm3\n", reservLevel);cdrain();coflush();RTCDelayMicroSeconds(10000L);
	if(reservLevel==0) return reservLevel;
	else
		{
		ActivatePump(duration);			//Activitate pump for duration sec
		if(duration<increment)duration=increment;
		numCheck=duration/increment;			//50ds increment
		length=0;
		i=0;
		while(i<numCheck)
			{
			LowPowerDelay((long)increment);
			length +=increment;
			i++;
			result=10;
			while(result>1) result = GetReservoirLevel(&reservLevel);
			flogf("RESERVOIR LEVEL %d cm3\n", reservLevel);cdrain();coflush();RTCDelayMicroSeconds(10000L);
			if(reservLevel==0){StopWhatIsDoing();i=numCheck;}
			}
		length=length/10;
		flogf("PUMP ACTIVATED FOR %d SEC\n", length);cdrain();coflush();
		LowPowerDelay(duration);	//give a rest to a pump
		return reservLevel;
		}
}	
//*************************************************************************
//Stop what RPOVOR is doing
//July 6, 2006 HM
//*************************************************************************	
short StopWhatIsDoing(void)
	{
	short i, errcnt = 0;
	short ich;
	uchar b[10], chk[6];

	TURxFlush(tuport1);
	RTCDelayMicroSeconds(PWait);
	//PROVOR echoes back
	TUTxPutByte(tuport1,'!',true);
	TURxGetByte(tuport1,true);
	TUTxPutByte(tuport1,'S',true);
	TURxGetByte(tuport1,true);
	TUTxPutByte(tuport1,'T',true);
	TURxGetByte(tuport1,true);
	RTCDelayMicroSeconds(PWait);
	TUTxPutByte(tuport1,'\r',true);
	TURxGetByte(tuport1,true);

	//Response (55msec typical delay for pump action)
	b[0]=TURxGetByte(tuport1,true);
	//RTCElapsedTimerSetup(&rt);
	//for(i=1;i<12;i++)b[i]=TURxGetByte(tuport1,true);
	ich=TURxGetBlock(tuport1,&b[1],9,100);
	//elp=RTCElapsedTime(&rt);
	//cprintf("%ld\n",elp);
	cprintf("%s %d\n", b, ich);
	putflush();
	
	if(strstr(b,"ERROR MSG"))//error try again
		{
		return 10;
		}
	
	//check for errors
	strncpy(chk,"]!STOP",6);
	for(i=1;i<6;i++)
		if(b[i]!= chk[i])
			errcnt++;
			//printf("%d\n",errcnt);
			//putflush();
	return errcnt;
}//-----------------StopWhatIsDoing---------------------------------------------//

//******************************************************************************
// BalanceReservoir
// Check the reservoir level and empty out until it changes from 0 to 1905.
//******************************************************************************
short BalanceReservoir(short duration)
{
	short	result;
	short   reservLevel,i,numCheck,length, increment;
	short	pStatus, lvStatus, phase, pDur, vDur, pressure;
	increment=10;       //in deciseconds=ds
	result=10;
	while(result>1) result = GetReservoirLevel(&reservLevel);
	flogf("RESERVOIR LEVEL %d cm3\n", reservLevel);cdrain();coflush();RTCDelayMicroSeconds(10000L);
	if(reservLevel!=0) return reservLevel;
	else
		{
		ActivateValve(duration);			//Activitate pump for duration sec
		if(duration<increment)duration=increment;
		numCheck=duration/increment;			//30ds increment
		length=0;
		i=0;
		while(i<numCheck)
			{
			LowPowerDelay((long)increment);
			//RTCDelayMicroSeconds(3000000L);
			length +=increment;
			result=10;
			i++;
			while(result>1) result = GetReservoirLevel(&reservLevel);
			flogf("RESERVOIR LEVEL %d cm3\n", reservLevel);cdrain();coflush();RTCDelayMicroSeconds(10000L);
			InqProvorStatus(&pStatus, &lvStatus, &phase, &pDur, &vDur, &pressure);				   
			if(lvStatus == 0) i=numCheck;
			if(reservLevel!=0){StopWhatIsDoing();i=numCheck;}
			}
		flogf("VALVE OPENED FOR %d SEC\n", length/10);cdrain();coflush();
		LowPowerDelay(20L);	//give a rest
		return reservLevel;
		}
}	