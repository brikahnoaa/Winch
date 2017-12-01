// winch.h
// AModemPort and WISPR Transmission

#define NIGKMINDEPTH 6
#define AMODEMBAUD 4800L

typedef struct WinchInfo {
  bool pending;       // expecting response
  float delay;        // seconds after TUTxAcousticModem before action
  float firstRise;    // Velocity in meters/minute of the rise (ascent) rate
  float lastRise;     // Velocity in meters/minute of the rise (ascent) rate
  float firstFall;    // Velocity in meters/minute of the rise (ascent) rate
  float lastFall;     // Velocity in meters/minute of the rise (ascent) rate
} WinchInfo;
extern WinchInfo winch;

// Tracking number of calls
typedef struct AmodemInfo {
  bool off;
  short ascentCalls;
  short ascentRcv;
  short descentCalls;
  short descentRcv;
  short stopCalls;
  short stopRcv;
  short buoyRcv;
  short winchCalls;
  Serial port;
} AmodemInfo;
extern AmodemInfo amodem;

void AModemData();
ulong WinchAscend();
ulong WinchDescend();
ulong WinchStop();
void BuoyStatus();
void WinchConsole();
void amodemInit(bool);
void GetWinchSettings();
ulong WinchStatus();
void WinchMonitor(int);

