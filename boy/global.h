// static global - seen only in file where declared
// static local - retains value between func calls


/*
 * SYSTEM PARAMETER STRUCTURES, globals
 */


/* test */
#include <stdio.h>
#include <stdlib.h>
typedef char bool;              // pico thing
typedef int TUPort;
#define false 0
#define true  1
/* test */


#include<define.h>

// ant
typedef struct AntennaData {
  bool surfaced;        // Set to true when Antenna is floating
  float ctdPos;         // depth of CTD when floating
  char gpsLong[20];     // 123:45.6789 W
  char gpsLat[20];      // 45:67.8900 N
  TUPort *port;
} AntennaData;

// System Parameters//Always defined // used as MPC.name
typedef struct BuoyData {
  char progname[20]; // added HM
  char projID[6];    // rudicsland
  char pltfrmID[6];  // rudicsland
  long filenum; // current number for filename ####.dat ####.log
  short starts;
  short maxStarts; //
  short detInt;   //-D      //Minutes   //WISPR DET INTERVAL
  short dataInt; // VEE:DATAXINTERVAL_NAME
  short phase; // 1=WISPR, 2=Ascent, 3=Surface, 4=Descent, 5=deployment
  short phaseStart; // time this phase started
  bool on;       // While "ON", continue running program
  float depth;     // Most recent depth measurement from sbe16
  float moorDepth; // Depth at beginning of LARA.PHASE==1. Averaged Samples
  float avgVel;
  TUPort *port; // port for antenna module and ctd
} BuoyData;

// ctd
typedef struct CtdData {
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
  float delay; // seconds after TUTxAcousticModem before action
  float rrate;  // Velocity in meters/minute of the rise (ascent) rate
  float frate;  // Velocity in Meters/minute of the fall (descent) rate
  TUPort *port;
} WinchData;

typedef struct WisprData {
  short gain; //-g 0-3
  short num;    // number of WISPR Boards installed
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
