#ifndef QUECOM_H
#define QUECOM_H (0x0010U)

// Dec, 2014 
// Interface between APF9 and WISPR DSP.
/*
 *  Interface between the APF9 and the Quephone DSP.
 *  Each message starts with a '$' and ends with a '*' and a CR.
 *  The first parameter in each message is a time stamp with the current system time.
 *  Created by chris jones (UW-APL) on 10/16/09.
 *
 */
#include <time.h>

/* define the name of the detection file */
extern persistent float GPSlat;
extern persistent float GPSlon;

extern persistent char DTX_path[32];
extern persistent int  GoProfile;
extern persistent int  Gain;
extern persistent long Max_Detect_Prof; //HM  11/2/2009
extern persistent int  DspPwrDpt;

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
