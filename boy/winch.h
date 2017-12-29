// winch.h
// AModemPort and WISPR Transmission

#define NIGKMINDEPTH 6
#define AMODEMBAUD 4800L

typedef struct WinchInfo {
  bool on;            // winch motor running
  float boy2ant;      // meters from buoy ctd to ant ctd under still water
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

void aModemData(void);
ulong winchAscend(void);
ulong winchDescend(void);
ulong winchStop(void);
void buoyStatus(void);
void winchConsole(void);
void amodemInit(bool);
void getWinchSettings(void);
ulong winchStatus(void);
void winchMonitor(int);

