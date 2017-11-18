// global externals
extern char scratch[], stringin[], stringout[];
extern char WriteBuffer[];

// static global - seen only in file where declared
// static local - retains value between func calls


/*
 * SYSTEM PARAMETER STRUCTURES, globals
 */
//CTD
typedef struct {
  bool syncMode;
  short delay;          //Delay in seconds between polled samples
  TUPort port;
} CTDParameters;

// System Parameters//Always defined // used as MPC.name
typedef struct {
  char PROGNAME[20]; // added HM
  char LONG[17];     // 123:45.67 West
  char LAT[17];      // 45:67.8900 North
  char PROJID[6];    // rudicsland
  char PLTFRMID[6];  // rudicsland
  char LOGFILE[13]; // File Name: activity.log
  long FILENUM; // current number for 00000000.dat
  short STARTUPS;
  short STARTMAX; //-s
  short DETINT;   //-D      //Minutes   //WISPR DET INTERVAL
  short DATAXINT; // VEE:DATAXINTERVAL_NAME
} SystemParameters;

// This structure is System status, used as LARA.*
typedef struct {
  short PHASE; // 1=WISPR, 2=Ascent, 3=Surface, 4=Descent, 5=deployment
  bool ON;       // While "ON", continue running program
  bool DATA;     // ?? Data is triggered true when the timing interval goes off.
  bool SURFACED; // Set to true when Antenna is estimated to be above water
  short BUOYMODE;  // 0=stopped 1=ascend 2=descend 3=careful ascent
  float DEPTH;     // Most recent depth measurement from CTD
  float MOORDEPTH; // Depth at beginning of LARA.PHASE==1. Averaged Samples
  float TOPDEPTH;
  float TDEPTH;   // purpose unknown
  float AVGVEL;
  short ASCENTTIME;
  short DESCENTTIME;
  short STATUS;
  bool LOWPOWER;
  bool RESTART;
  TUPort AntModport; // port for antenna module
  TUPort CTDport; // port for the buoy ctd
  bool CTDsync; // ctd sync mode
} SystemStatus;

typedef struct {
  char BATCAP[9];
  short BATLOG;    // t logging change in battery capacity
  char MINVOLT[6]; //-v %.2f  minimum system voltage
} PowerParameters;

typedef struct {
  short GAIN; //-g 0-3
  // short    MODE;   //-M 1-5 in WISPR Start Script
  short NUM;    // Depends on the number of WISPR Boards installed
  short DETMAX; // Maximum Number of Detections to return
  short DETNUM; // Number of detections per one call to initiate #REALTIME call
                // to land.
  short DUTYCYCL; // Duty cycle of recorder during one detection interval
  // short    ON;      //Power to the WISPR 1=on 0=off
} WISPRParameters;

// IRIDIUM Structure Parameters
typedef struct {
  char PHONE[14]; // Rudics phone number 13 char long
  short MINSIGQ;  // Min Irid signal quality to proceed
  short MAXCALLS; // Maximum Iridium calls per session
  short MAXUPL;   // Max upload try per call
  short WARMUP; // IRID GPS Unit warm up in sec//Does this really need to be in
                // here?
  short ANTSW;    //=1: antenna switch; =0: no antenna switch
  short OFFSET;   // GPS and UTC time offset in sec
  short REST;     // Rest period for Iridium to call again
  short CALLHOUR; // Hour at which to call
  short CALLMODE; // 0==call on Dataxinterval, 1== call at set hour everyday.
  bool LOWFIRST;  // send file with lowest value first
} IridiumParameters;

typedef struct {
  short TDEPTH; // CTD depth at optimal position for iridium/gps comms (Antenna
                // at surface when winch cable angle <10')
  short DELAY; // Time in seconds post TUTxAcousticModem when the Winch actually
               // accepts character. Strictly for timing & Calculation of cable
               // length
  short RRATE;  // Velocity in meters/minute of the rise (ascent) rate
  short FRATE;  // Velocity in Meters/minute of the fall (descent) rate
  short ANTLEN; // Length from CTD to antenna. More specifically: From CTD Depth
                // Sensor to water surface when antenna is out of water.
  short PROFILES; // Keep record of number of profiles
  short RECOVERY; // If 1, call in repeatedly @ specified interval. 'A' reset to
                  // 30minutes.
} WINCHParameters;

// Tracking number of calls
typedef struct {
  short ASCENTCALLS;
  short ASCENTRCV;
  short DESCENTCALLS;
  short DESCENTRCV;
  short STOPCALLS;
  short STOPRCV;
  short BUOYRCV;
  short WINCHCALLS;
} WinchCalls;

