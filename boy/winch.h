// winch.h
// AModemPort and WISPR Transmission

#define NIGKMINDEPTH 6
#define AMODEMBAUD 4800L

typedef struct WinchInfo {
  bool on;            // winch motor running
  float delay;        // seconds after TUTxAcousticModem before action
  float firstRise;    // Velocity meters/min of the first rise (ascent)
  float lastRise;     // Velocity meters/min of the most recent rise 
  float firstFall;    // Velocity meters/min of the first fall (descent)
  float lastFall;     // Velocity meters/min of the most recent fall 
} WinchInfo;
extern WinchInfo winch;

// Tracking number of calls
typedef struct AmodemInfo {
  bool on;
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

void AModemData(void);
ulong WinchAscend(void);
ulong WinchDescend(void);
ulong WinchStop(void);
void BuoyStatus(void);
void WinchConsole(void);
void amodemInit(bool);
void GetWinchSettings(void);
ulong WinchStatus(void);
void WinchMonitor(int);

