// ngk.h
// AModemPort and WISPR Transmission

#define AMODEMBAUD 4800L

typedef struct NgkInfo {
  bool on;            // ngk motor running
  bool pending;       // waiting for comm
  float boy2ant;      // meters from buoy ctd to ant ctd under still water
  float delay;        // seconds after TUTxAcousticModem before action
  float firstRise;    // Velocity meters/min of the first rise (ascent)
  float lastRise;     // Velocity meters/min of the most recent rise 
  float firstFall;    // Velocity meters/min of the first fall (descent)
  float lastFall;     // Velocity meters/min of the most recent fall 
} NgkInfo;
extern NgkInfo ngk;

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
  short ngkCalls;
  Serial port;
} AmodemInfo;
extern AmodemInfo amodem;

void aModemData(void);
ulong ngkAscend(void);
ulong ngkDescend(void);
ulong ngkStop(void);
void buoyStatus(void);
void ngkConsole(void);
void amodemInit(bool);
void getNgkSettings(void);
ulong ngkStatus(void);
void ngkMonitor(int);

