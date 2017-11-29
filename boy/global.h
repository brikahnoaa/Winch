// static global - seen only in file where declared
// static local - retains value between func calls


/*
 * SYSTEM PARAMETER STRUCTURES, globals
 */

#define BUFSZ 1024

typdef unsigned long Time;

// ant
typedef struct AntennaData {
  bool off;
  bool surfaced;        // Set to true when Antenna is floating
  float ctdPos;         // depth of CTD when floating
  char gpsLong[20];     // 123:45.6789 W
  char gpsLat[20];      // 45:67.8900 N
  TUPort *port;
} AntennaData;

// System Parameters//Always defined // used as MPC.name
typedef struct BuoyData {
  bool off;
  int deviceID;           // DEVA=1 = antenna, DEVB=2 = buoy ctd
  char programName[20]; // added HM
  char projectID[6];    // rudicsland
  char platformID[6];  // rudicsland
  long filenumber; // current number for filename ####.dat ####.log
  long starts;
  long startsMax; 
  int dataInt; // VEE:DATAXINTERVAL_NAME
  int phase; // 1=WISPR, 2=Ascent, 3=Surface, 4=Descent, 5=deployment
  int phaseInitial; // normal start in this phase (2)
  Time phaseStart; // time this phase started
  Time onStart;    // startup time
  float depth;     // Most recent depth measurement from sbe16
  float moorDepth; // Depth at beginning of LARA.PHASE==1. Averaged Samples
  float avgVel;
} BuoyData;

// ctd
typedef struct CtdData {
  bool off;
  bool syncMode;  // off = polled
  bool pending;   // polled request pending
  short delay;    // Delay in seconds between polled samples
  TUPort *port;    // same as ant.port
} CtdData;

// IRIDIUM Structure Parameters
typedef struct IridiumData {
  char phone[14]; // Rudics phone number 13 char long
  short minSigQ;  // Min Irid signal quality to proceed
  short maxCalls; // Maximum Iridium calls per session
  short maxUpl;   // Max upload try per call
  short warmup; // IRID GPS Unit warm up in sec
  short offset;   // GPS and UTC time offset in sec
  short rest;     // Rest period for Iridium to call again
  short callHour; // Hour at which to call
  short callMode; // 0==call on Dataxinterval, 1== call at set hour everyday.
} IridiumData;

typedef struct PowerData {
  char batCap[9];
  short batLog;    // t logging change in battery capacity
  char minVolt[6]; //-v %.2f  minimum system voltage
} PowerData;

typedef struct WinchData {
  bool off;
  bool pending;       // expecting response
  float delay;        // seconds after TUTxAcousticModem before action
  float firstRise;    // Velocity in meters/minute of the rise (ascent) rate
  float lastRise;     // Velocity in meters/minute of the rise (ascent) rate
  float firstFall;    // Velocity in meters/minute of the rise (ascent) rate
  float lastFall;     // Velocity in meters/minute of the rise (ascent) rate
  TUPort *port;
} WinchData;

typedef struct WisprData {
  bool off;
  short gain; //-g 0-3
  short num;    // number of WISPR Boards installed
  short detInt;   //-D      //Minutes   //WISPR DET INTERVAL
  short detMax; // Maximum Number of Detections to return
  short detNum; // Number of detections per one call to initiate #REALTIME call
  short dutycycl; // Duty cycle of recorder during one detection interval
  TUPort *port;
} WisprData;

// global externals
extern char scratch[], stringin[], stringout[];
extern char WriteBuffer[];

extern AntennaData ant;
extern BuoyData boy;
extern CtdData ctd;
extern IridiumData irid;
extern PowerData power;     // was ads
extern WinchData winch;
extern WisprData wisp;
