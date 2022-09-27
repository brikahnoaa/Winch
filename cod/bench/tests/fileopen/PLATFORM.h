
// DEFINE THE TYPE OF PLATFORM
#define PLATFORM LARA
// This structure is primarily for system diagnostics logging. Keeping track of
// phase, alarms, system statuses
typedef struct {

  short PHASE; // 1=AUH, 2=Ascent Profile, 3=Surface Communitcation, 4= Descent
               // Profile
  bool ON;       // While "ON", continue running program
  bool DATA;     // Data is triggered true when the timing interval goes off.
  bool SURFACED; // Set to true when Antenna is estimated to be protruding from
                 // water
  short BUOYMODE;  // 0=stopped 1=ascend 2=descend 3=careful ascent
  float DEPTH;     // Most recent depth measurement from CTD
  float MOORDEPTH; // Depth confirmation at beginning of LARA.PHASE==1. Averaged
                   // Samples
  short TDEPTH; //
  float TOPDEPTH;
  float AVGVEL;
  float PAYOUT;
  short ASCENTTIME;
  short DESCENTTIME;
  short STATUS;
  short CTDSAMPLES;
  bool LOWPOWER;
} SystemStatus;
char *PrintSystemStatus(void);
int Incoming_Data();
void LARA_Recovery();

//#define PLATFORM AUH
//#define PLATFORM SeaGlider
//#define RAOT
//#define RAOB

// LARA PROGRAM
#define PROG_VERSION 3.3 // Keep this up to date!!!

#define POWERLOGGING
#define DEBUG
#define SYSTEMDIAGNOSTICS
#define WISPR
#define IRIDIUM
#define CTDSENSOR
//#define SEAGLIDER
//#define TIMING
//#define ACOUSTICMODEM
//#define BLUETOOTH
#define CONSOLEINPUT
#define REALTIME
#define NIGKWINCH
//#define LARASIM

#define MAX_GPS_CHANGE 1.0
#define MAX_UPLOAD 30000 // bytes
#define MAX_STARTUPS 1000
#define MIN_DETECTION_INTERVAL 10
#define MAX_DETECTION_INTERVAL 60
#define MAX_BLOCK_SIZE 2000
#define MIN_DATAX_INTERVAL 30
#define MAX_DATAX_INTERVAL 2880

#define WTMODE nsStdSmallBusAdj // choose: nsMotoSpecAdj or nsStdSmallBusAdj
#define SYSCLK 16000 // Clock speed: 2000 works 160-32000 kHz Default: 16000

#define WISPRNUMBER 4
#define MAX_DETECTIONS 20
#define MIN_DUTYCYCLE 0
#define MAX_DUTYCYCLE 100
#define MIN_FREESPACE 1.0 // represents the cutoff percentage for WISPR
                          // recording

#ifdef SEAGLIDER
#define MIN_OFF_DEPTH 50
#define MIN_ON_DEPTH 25
#endif

// Enabling REALTIME mode will initiate the call land protocol to transfer
// realtime data.
// It will also use a filenumber based .log system rather than activity log

#ifdef REALTIME
#define RTS(X) X
#else
#define RTS(X)
#endif

// DEBUG
#ifdef DEBUG
#define DBG(X) X
#else
#define DBG(X)
#endif
/*
//CTD
#ifdef CTDSENSOR
   #define CTD(X)    X
#else
   #define CTD(X)
#endif*/

// TIMING
#ifdef TIMING
#define CLK(X) X
#else
#define CLK(X)
#endif

// PowerLogging
#ifdef POWERLOGGING
// LARA has 4 sets of Lithium-Primary Batteries. Rated at 14.4V (Initial at
// 14.74V) and 117Amp-Hour
// This Provides 24MJ of energy!
#define BATTERYLOG

#define POWERERROR 1.02
#define MIN_BATTERY_VOLTAGE                                                    \
  11.0 // volts or 11.0 for 15V Battery System of Seaglider
#define INITIAL_BATTERY_CAPACITY 5000 // kiloJoules
#define MINIMUM_BATTERY_CAPACITY INITIAL_BATTERY_CAPACITY * 0.1

#define PWR(X) X
#define BITSHIFT                                                               \
  11 // Crucial to ADS Timing of Program. explained in ads power consumption
     // calcation excel file
/***************************************************
   10: 25.6seconds/file write 843.75 bytes/hour
   11: 51.2secs/file write    421.875bytes/hr
   12: 102.4secs/file         201.937bytes/hr
   13: 204.8secs/file         105.468
   14: 409.6                  52.734
   15: 819.2                  26.367
   16: 1638.4                 13.183
*************************************************/
#else
#define PWR(X)
#endif

void SystemTest();
