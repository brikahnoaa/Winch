#ifndef QUECOM_H
#define QUECOM_H (0x0010U)
// Dec, 2015
// Replaced the complicated DspOn process with DspStart(); You have to choose the 
// right wait period for the WISPR boot process and disk size to get the free space
// and send the GPS time and gain from Apf9 to WISPR.
// Added a counter in QuecomGetDTX to count No_Response from WISPR.  If >=3 no 
// responses, it toggles the WISPR power and restart the program. 
//  
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

//extern persistent char DTX_path[32];
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
