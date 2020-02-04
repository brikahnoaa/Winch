#ifndef QUECOM_H
#define QUECOM_H (0x0010U)

// Dec, 2014 
// Interface between APF9 and WISPR DSP.
/*
 *  Interface between the APF9 and the QUEphone DSP.
 *  Each message starts with a '$' and ends with a '*' and a CR.
 *  The first parameter in each message is a time stamp with the current system time.
 *  10/16/09. Chris Jones and H. Matsumoto
 *  
   $GPS ,%ld, %8.3f,%7.3f* Cr  GPS time, long and lat    apf9->DSP
   $DX?          ,%ld,%ld* Cr  Inq detections            apf9->DSP
   $DXN               ,%d* Cr  Num of detections         DSP->apf9
   $ACK                  * Cr  Send ACK for each line    DSP->apf9
   $NGN               ,%d* Cr  New gain (0-3)            apf9->DSP
   $EXI                  * Cr  End logging               apf9->DSP
   $DET               ,%d* Cr  Detection parameter		 DSP->apf9
   $DFP                  * Cr  Inq disk space   		 apf9->DSP
   $DFP            ,%5.2f* Cr  Reply disk space avail %  DPS->apf9   
 *
 */
#include <time.h>

/* define the name of the detection file */
extern persistent float GPSlat;
extern persistent float GPSlon;

extern persistent char DTX_path[32];
extern persistent int  GoProfile;		//HM 1 profile mode
extern persistent int  Gain;			//HM: Gain of the preamp (0,1,2,3)
extern persistent long Max_Detect_Prof; //HM: Max # of detections per dive
extern persistent int  DspPwrDpt;		//HM: DSP power ON/OFF depth in m

int QuecomWrite(char *str, int len, int Close);
int QuecomRead(char *buf, int bufsize);
int QuecomSetGPS(time_t sec, float lon, float lat);//Send time and location
//int QuecomSetDSC(float press);//Send Descent msg to DSP
//int QuecomSetASC(float press);//Send Ascent msg to DSP
//int QuecomSetPRK(float press);//Send Park mesg to DSP
//int QuecomSetPFL(float press);//Send Profile msg to DSP
//int QuecomSetSDT();
int QuecomSetPMA(int state);
void ResetGoProfile(void); //Reset to GoProfile=0
long QuecomGetDTX(long max_ndtx);//HM 12262010
int QuecomSetNCM(persistent int Gain, persistent int DspPwrDpt);  
int QuecomSetGain(int Gain);
int QuecomDspExit();
float QuecomInqDisk(void);
#endif  /* QUECOM_H */

#include <apf9com.h>
#include <stdio.h>
#include <conio.h>
#include <apf9.h>
#include <apf9icom.h>
#include <config.h>
#include <control.h>
#include <ds2404.h>
#include <eeprom.h>
#include <garmin.h>
#include <logger.h>
#include <math.h>
#include <nmea.h>
#include <unistd.h>
#include <apf9.h>
#include <string.h>
#include <stdlib.h>
#include <descent.h>
#include <seascan.h>
#include <park.h>

//static int verbose = 2; //HM

/* define the name of the detection file */
persistent float GPSlat;
persistent float GPSlon;
persistent char DTX_path[32];
persistent int GoProfile;
persistent int Gain; 				//HM
persistent long Max_Detect_Prof; 	//HM  12/2/2009
persistent int DspPwrDpt;
int HMdebug=0;

/*
 * ---------------------------------------------------------------------------
 * Quecom Write message Write number of bytes (len) from message string
 * (str) to serial port com1 , terminate message with '\c'.
 */
int QuecomWrite(char *str, int len, int Close)
{
   int n;
   
   Com1Enable(9600);
   for (n = 0; n < len; n++) {
      com1.putb(str[n]);
      Wait(3);
   }
   com1.putb(0x0D);
   /* terminate with CR */
   Wait(3);
   if(Close)Com1Disable();
   
   return (n);
}

/*
 * ---------------------------------------------------------------------------
 * Quecom Read Read serial port com1 until the number of bytes (bufsize)
 * are read, or '*' char is found, or the port timeouts.  Timeout works by
 * counting the number of time getb is called, since getb is nonblocking.
 * Returns number of char read.
 */
int QuecomRead(char *buf, int bufsize)
{
   int i, n, go;
   int MaxCount = 10000;	/* max read timeout counter */
   unsigned char byte;
   //char byte;
   static cc FuncName[]="QuecomRead ";
   
   //Com1Enable(9600);
   
   i = 0;
   n = 0;
   go = 1;
   while (go) {
      if (com1.getb(&byte) == 1) {
         buf[n] = byte;		/* save byte in buf */
         n++;			/* increment byte count */
         i = 0;			/* reset timeout counter */
      } else
         i++;			/* increment timeout counter */
      if (byte == '*') {	/* if found terminating char */
         go = 0;		/* quit looping */
         com1.getb(&byte);	/* read the last char, which should be a CR * or NL */
         buf[n] = 0;		/* null terminate the buf */
      }
      /* check if buf size is exceeded */
      if (n >= bufsize) go = 0;
      /* check if timeout count is exceeded */
      if (i >= MaxCount) go = 0;
   }
   Wait(3);     
   if((debuglevel >=2) && (HMdebug==1)){
      static cc msg[]="DSP %s.\n";
      LogEntry(FuncName,msg, buf);
	}
   //Com1Disable();
   return (n);
}

/*
 * ---------------------------------------------------------------------------
 * Set GPS or current RTC time and position //  sec is gps unix time
 */
int QuecomSetGPS(time_t sec, float GPSlon, float GPSlat)
{
   int             bufsize, nwrt;
   char            buf[64];
   static cc       FuncName[] = "QuecomSetGPS()";
   
   /* Quecom write DSP current time and lat/lon mesage */
   bufsize = sprintf(buf, "$GPS,%ld,%8.3f,%7.3f*", sec, GPSlon, GPSlat);
   if ((nwrt = QuecomWrite(buf, bufsize, 1)) != bufsize) {
      static cc       msg[] = "message write error.\n";
      LogEntry(FuncName, msg);
   }
   return (nwrt);
}

/*
 * ---------------------------------------------------------------------------
 * Set Descending mode - DSC
 */
//int QuecomSetDSC(float press)
//{
//   int             bufsize, nwrt;
//   char            buf[16];
//   time_t          sec = time(NULL);
//   static cc       FuncName[] = "QuecomSetDSC()";
   
//   /* Quecom write that float is descending mesage */
//   bufsize = sprintf(buf, "$DSC,%ld,%0.1f*", sec, press);
//   if ((nwrt = QuecomWrite(buf, bufsize, 1)) != bufsize) {
//      static cc       msg[] = "message write error.\n";
//      LogEntry(FuncName, msg);
//   }
//   return (nwrt);
//}

/*
 * ---------------------------------------------------------------------------
 * Set Ascending mode - ASC
 */
//int QuecomSetASC(float press)
//{
//   int             bufsize, nwrt;
//   char            buf[32];
//   time_t          sec = time(NULL);
//   static cc       FuncName[] = "QuecomSetASC()";
   
//   /* Quecom write DSP that float is ascending mesage */
//   bufsize = sprintf(buf, "$ASC,%ld,%0.1f*", sec, press);
//   if ((nwrt = QuecomWrite(buf, bufsize, 1)) != bufsize) {
//      static cc       msg[] = "message write error.\n";
//      LogEntry(FuncName, msg);
//   }
//   return (nwrt);
//}

/*
 * ---------------------------------------------------------------------------
 * Set Parking mode - PRK
 */
//int QuecomSetPRK(float press)
//{
//   int             bufsize, nwrt;
//   char            buf[32];
//   time_t          sec = time(NULL);
//   static cc       FuncName[] = "QuecomSetPRK()";
   
//   /* Quecom write DSP parking mode mesage */
//   bufsize = sprintf(buf, "$PRK,%ld,%0.1f*", sec, press);
//   if ((nwrt = QuecomWrite(buf, bufsize, 1)) != bufsize) {
//      static cc       msg[] = "message write error.\n";
//      LogEntry(FuncName, msg);
//   }
//   return (nwrt);
//}

/*
 * ---------------------------------------------------------------------------
 * Set Profiling mode - PFL
 */
//int QuecomSetPFL(float press)
//{
//   int             bufsize, nwrt;
//   char            buf[32];
//   time_t          sec = time(NULL);
//   static cc       FuncName[] = "QuecomSetPFL()";
   
//   /* Quecom write pressure mesage */
//   bufsize = sprintf(buf, "$PFL,%ld,%0.1f*", sec, press);
//   if ((nwrt = QuecomWrite(buf, bufsize, 1)) != bufsize) {
//      static cc       msg[] = "message write error.\n";
//      LogEntry(FuncName, msg);
//   }
//   return (nwrt);
//}

/*
 * ---------------------------------------------------------------------------
 * Set Surface Detect - SDT
 */
//int QuecomSetSDT()
//{
//   int             bufsize, nwrt;
//   char            buf[32];
//   time_t          sec = time(NULL);
//   static cc       FuncName[] = "QuecomSetSDT()";
   
   /* Quecom write DSP lat/lon mesage */
//   bufsize = sprintf(buf, "$SDT,%ld*", sec);
//   if ((nwrt = QuecomWrite(buf, bufsize, 1)) != bufsize) {
//      static cc       msg[] = "message write error.\n";
//      LogEntry(FuncName, msg);
//   }
//   return (nwrt);
//}


/*
 * ---------------------------------------------------------------------------
 *  Set Piston Move Absolute - PMA // state can be 1 (ON) or 0 (OFF)
 */
int QuecomSetPMA(int state)
{
   int             bufsize, nwrt;
   char            buf[32];
   time_t          sec = time(NULL);
   static cc       FuncName[] = "QuecomSetPMA()";
   
   /* Quecom write DSP Piston movement */
   bufsize = sprintf(buf, "$PMA,%ld,%d*", sec, state);
   if ((nwrt = QuecomWrite(buf, bufsize, 1)) != bufsize){
      static cc       msg[] = "message write error.\n";
      LogEntry(FuncName, msg);
   }
   return (nwrt);
}

/*
 * ---------------------------------------------------------------------------
 *  Reset GoProfile=0
 */
void ResetGoProfile(void)
{
   GoProfile=0; //HM here reset the GoProfile=0
}
/*
 * ---------------------------------------------------------------------------
 * New Quecom detection message. Requests detection record from DSP, reads
 * detection record, then log to file (if WriteDet is enabled). Read up to 5-digit
 * # of detections (<=99999). max_ndtx is the max number of DTX messages
 * that apf9 COM1 can read from the DSP serial. 
 */
long QuecomGetDTX(long max_ndtx)
{
   char buf[64], tmp[12];
   int n, nrd, bufsize;
   time_t sec;
   FILE *fp;
   static cc FuncName[] = "QuecomGetDTX()";
   int blocksize = 59; //message size
   float pres, temp, fm_ici, fm_dur;
   //int fakeDSP=0;  //=1 emulate DSP !=1 reads real DSP output from COM 
   int WriteDet=0;   //write detection on file HM
   int GoProfileEnabled=0;//=1 allows state to change to profile
   
   //parameters added to read more K-M detector output 12/14/2010 HM
    long ndtx, nclk, m_tk, m_th, m_rt;
	int  m_ici, m_dur;
   
   /* create the dtx file name */
   /* encode the date/time */
   //sec = time(NULL); 
   //strftime(tmp, sizeof(tmp), "%y%m%d%H%M", gmtime(&sec));
   //snprintf(DTX_path, sizeof(DTX_path), "%04d.%s.dtx", mission.FloatId, tmp);
   //snprintf(DTX_path,sizeof(DTX_path)-1,"%04u.%03d.dtx",mission.FloatId,PrfIdGet()+1);
   if(WriteDet==1){
	   snprintf(DTX_path,sizeof(DTX_path),"%04u.%03d.dtg",mission.FloatId,PrfIdGet());//HM
	   
	   //Open the DTX file to append
	   fp = fopen(DTX_path, "a+");
	   
	   /* exit if dtx file open failure */
	   if(fp == NULL && (debuglevel >1)) {
		  /* create the message */
		  static cc format[]= "Attempt to open \"%s\" failed \n";
		  /* make the logentry */
		  LogEntry(FuncName,format, DTX_path);
		  return(-1);
	   }
	}
   /* write DX? to DSP */
   //max_ndtx = number of detections allowed to pass on from DSP to apf9i per reading
   //defined in control.c, and DSP output is written on detection file.  If =0, DSP
   //output would not be written.
   sec = time(NULL);
   bufsize = sprintf(buf, "$DX?,%ld,%ld*", sec, max_ndtx);
   if (QuecomWrite(buf, bufsize, 0) != bufsize) {  //Open Com1 and leave it open
      static cc msg[] = "message write error.\n";
      LogEntry(FuncName, msg);
   }
   
   /* Wait for DXN message from the DSP,
    should be 59 char message = $DXN,xxxxx* */
   Wait(70); //12_14_2010 HM -it was 14 before

   strncpy(buf, "    ", 4); //HM make an empty buffer to kill time for timing

   nrd= QuecomRead(buf, blocksize); 
   
   if (debuglevel >=3) { //HM1224
      static cc format[] = "Read %d char: %s\n";
      LogEntry(FuncName, format, nrd, buf);
   }
   
   GetPt(&pres, &temp);
   
   /* If msg type matches, start reading ASC string from DSP, HM */
   if ((strncmp(&buf[1], "DXN", 3) == 0)) {//HM original
    
		//Read      ndet, date   , time   , nclk,m T-K,m Thr,mrati,ICI,mdur
        //     123456789012345678901234567890123456789012345678901234567890
        //    "$DXN,0004,04/01/70,00:47:29,00042,00214,00065,00015,27,068*");
        
		//No of detections
		strncpy(tmp,buf+5,4);
		tmp[4]=0;
		ndtx = atol(tmp);
		//printf("no of dtx %5ld\n", ndtx);

		if (ndtx > 0) {	
			enum State phase = StateGet();
			//total num of clicks
			strncpy(tmp,buf+28,5);
			tmp[5]=0;
			nclk = atol(tmp);
			//printf("no of clks %5d\n", nclk);

			//Mean T-K
			strncpy(tmp,buf+34,5);
			tmp[5]=0;
			m_tk = atol(tmp);
			//printf("mean tk     %5d\n",m_tk);

			//Mean Threshold
			strncpy(tmp,buf+40,5);
			tmp[5]=0;
			m_th = atol(tmp);
			//printf("mean thr    %5d\n", m_th);

			//Mean ratio
			strncpy(tmp,buf+46,5);
			tmp[5]=0;
			m_rt = atol(tmp);
			//printf("mean ratio  %5d\n", m_rt);

			//Mean ici (1/100)
			strncpy(tmp,buf+52,2);
			tmp[2]=0;
			m_ici = atoi(tmp);
			//printf("mean ici    %5d\n", m_ici);

			//Mean duration (1/10 of second)
			strncpy(tmp,buf+55,3);
			tmp[3]=0;
			m_dur = atoi(tmp);
			//printf("m dur       %5d\n", m_dur);
		
			accum_dtx +=ndtx;			//accumulated detections for this profile HM
			
			fm_ici=(float)m_ici/100.;
			fm_dur=(float)m_dur/10.;
			if((phase == DESCENT)||(phase == PARK)||(phase == GODEEP)||(phase==PROFILE)){
				static cc format[] = "Ndtx=%ld,Nclk=%ld,Acm=%ld,MTK=%ld,MTHR=%ld,MRT=%ld,MICI=%0.2f,MDUR=%0.2f,P=%0.1fdB\n";
				LogEntry(FuncName, format, ndtx, nclk,accum_dtx, m_tk, m_th, m_rt, fm_ici, fm_dur,pres);
			}
		}
		//If accumumated detection exceeds, it starts ascending
		if((accum_dtx>=Max_Detect_Prof) && (GoProfileEnabled==1)){
			enum State phase = StateGet();
			if(phase == PARK){
				static cc format[]="Go profile! %ld\n";
				LogEntry(FuncName, format, accum_dtx);	
				GoProfile=1; 		//This changes the float state for the next profile. HM
			}
		}
   
		if ((ndtx > max_ndtx) && (debuglevel >=1) && (max_ndtx !=0L)) {
			static cc format[] = "# of detects = %ld exceeded max reading limit %ld\n";
			LogEntry(FuncName, format, ndtx, max_ndtx);
			ndtx = max_ndtx;
		}

		/* loop over ndtx and read each detection message from DSP */
		if(max_ndtx>0L && WriteDet==1){		//HM =0 do not write a file
			for (n = 0; n < ndtx; n++) {
         
				/* Read detection, read max buffer size */
				nrd = QuecomRead(buf, blocksize);//specify the block size of each detection HM
         
				//if (nrd && verbose) {
				//	static cc format[] = "%s %s\n";
				//	LogEntry(FuncName, format, log_path, buf);
				//}
         
				/* Append each dtx msg to the end of the TX file */
				if (nrd > 1) {

					fprintf(fp,"%s\n",buf);

					/* send ACK back to DSP */
					bufsize = sprintf(tmp, "$ACK*");
					QuecomWrite(tmp, sizeof(tmp), 0);
				}
			}
		fclose(fp); //Close DTX file
		}
	}
	else if (debuglevel>=3){ //HM1224
		/* create the message */
		static cc format[]="No msg from detector.\n";
		/* make the logentry */
		LogEntry(FuncName,format);
	}
   

	Com1Disable();//Now close COM1 from DSP
   return (ndtx);
}
/*
 * ---------------------------------------------------------------------------
 * Send gain and detection criterion command to DSP
 * Revised Dec 2014, HM
 ****************************************************************************/
int QuecomSetNCM(persistent int Gain, persistent int DspPwrDpt)
//Do nothing.  It's here to bridge config.c and quecom to share Gain and 
//DspPwrDpt values
{
   int             bufsize, nwrt;
   char            buf[16];
   static cc       FuncName[] = "QuecomSetNCM()";
   
   /* Quecom write Gain and DSP power On/Off depth */
//   bufsize = sprintf(buf, "$NCGN,%d,D,%d*", Gain, DspPwrDpt);
   bufsize = sprintf(buf, "$NGN,%d*", Gain);
   if ((nwrt = QuecomWrite(buf, bufsize, 1)) != bufsize) {
      static cc       msg[] = "message write error.\n";
      LogEntry(FuncName, msg);
   }
   return (nwrt);
}
/***************************************************************************
QuecomSetGain - Send a new gain value.  It should be send between 20 to 28 sec
after the WISPR is powered up.
***************************************************************************/
int QuecomSetGain(int Gain)
{
   int             bufsize, nwrt;
   char            buf[16];
   static cc       FuncName[] = "QuecomSetGain()";
   
   /* Quecom write Gain */
   bufsize = sprintf(buf, "$NGN,%d*", Gain);
   if ((nwrt = QuecomWrite(buf, bufsize, 1)) != bufsize) {
      static cc       msg[] = "message write error.\n";
      LogEntry(FuncName, msg);
   }
   return (nwrt);
}
//Send a code to gracefully terminate the DSP program
int QuecomDspExit()
{
   int             bufsize, nwrt;
   char            buf[16];
   static cc       FuncName[] = "QuecomDspExit()";
   
   /* Quecom write to DSP com*/
   bufsize = sprintf(buf, "$EXI*");
   if ((nwrt = QuecomWrite(buf, bufsize, 1)) != bufsize) {
      static cc       msg[] = "message write error.\n";
      LogEntry(FuncName, msg);
   }
   return (nwrt);
}
/************************************************************************
* Inquire free disk space to WISPR
*/
float QuecomInqDisk(void)
{
   int         bufsize, blocksize, nwrt, nrd;
   float	   diskused;
   char        buf[16], tmp[5];
   static cc   FuncName[] = "QuecomInqDisk()";
   
   /* Quecom write DSP CF card storage used in %*/
   bufsize = sprintf(buf, "$DFP*");
   if ((nwrt = QuecomWrite(buf, bufsize, 0)) != bufsize) {
      static cc       msg[] = "$DFP message write error.\n";
      LogEntry(FuncName, msg);
    }
   // else {
      //static cc       msg[] = "$DFP message sent.\n";
      //LogEntry(FuncName, msg);
	//}
	
   strncpy(buf, "    ", 4); //HM make an empty buffer to kill time for timing
   blocksize = 11;
   nrd= QuecomRead(buf, blocksize);
   
   if ((debuglevel >=2) && (HMdebug ==1)) { //HM1224
      static cc format[] = "Read %d char: %s\n";
      LogEntry(FuncName, format, nrd, buf);
   }
   
   //GetPt(&pres, &temp);
   
   /* If msg type matches, start reading ASC string from DSP, HM */
   if ((strncmp(&buf[1], "DFP", 3) == 0)) {  
		//disk space available in percentage
		strncpy(tmp,buf+5,5);
		diskused = atof(tmp);
	}
   return (diskused);
}
