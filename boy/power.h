// power.h
// A-D SYSTEM CURRENT AND VOLTAGE LOGGING
// Changing parameters here will cause problems to program timing.
#include <ADExamples.h>

#define FCHAN 0   // first channel
#define NCHAN 2   // number of channels, just accumulating current in buffers.
#define PITRATE 1 // 1=51ms cycle, 2=102ms,etc..... @ 100us. 250=25ms.
#define PITPERIOD .051 // represents 51ms

#define POWERERROR 1.02
// volts or 11.0 for 15V Battery System of Seaglider
#define MIN_BATTERY_VOLTAGE 11.0
// kiloJoules
#define INITIAL_BATTERY_CAPACITY 5000
#define MINIMUM_BATTERY_CAPACITY INITIAL_BATTERY_CAPACITY * 0.1

#define BITSHIFT 11
// Crucial to ADS Timing of Program. explained in ads power consumption
// calculation excel file
/*
   10: 25.6seconds/file write 843.75 bytes/hour
   11: 51.2secs/file write    421.875bytes/hr
   12: 102.4secs/file         201.937bytes/hr
   13: 204.8secs/file         105.468
   14: 409.6                  52.734
   15: 819.2                  26.367
   16: 1638.4                 13.183
 */


void Delay_AD_Log(short);
void Setup_Acquisition(ushort);
void AD_Write(ushort *);
void AD_Log(void);
ushort GetSystemTimeInt();
bool AD_Check();
ushort Setup_ADS(bool, long, ushort);
float Power_Monitor(ulong, int, ulong *);
void Delay_AD_Log(short Sec);
float Get_Voltage();
float Voltage_Now();
void Open_Avg_File(long);
ushort Return_ADSTIME();
int Get_ADCounter();
bool ADS_Status();
void GetPowerSettings();
void Reset_ADCounter();
void ADSFileName(long);
