// ngk.h
// AModemPort and WISPR Transmission

#define AMODEMBAUD 4800L

typedef enum {
  rise_cmd=0,
  drop_cmd,
  stop_cmd,
  sizeof_cmd,
} CommandType;
typedef enum {
  rise_rsp=0,
  drop_rsp,
  stop_rsp,
  sizeof_rsp,
} RespondType;

typedef struct NgkInfo {
  bool on;            // ngk motor running
  bool pending;       // waiting for comm
  float boy2ant;      // meters from buoy ctd to ant ctd under still water
  float delay;        // seconds after TUTxAcousticModem before action
  float firstRise;    // Velocity meters/min of the first rise (ascent)
  float recentRise;   // Velocity meters/min of the most recent rise 
  float firstDrop;    // Velocity meters/min of the first fall (descent)
  float recentDrop;   // Velocity meters/min of the most recent fall 
} NgkInfo;
extern NgkInfo ngk;

// Tracking number of calls
typedef struct AmodemInfo {
  bool on;
  bool pending;
  int command[sizeof_cmd];
  int respond[sizeof_rsp];
  int timeout[sizeof_cmd];
  Serial port;
} AmodemInfo;
extern AmodemInfo amodem;

void amodemData(void);
void ngkCommand(CommandType);
RespondType ngkRespond(void);
void ngkConsole(void);
void amodemInit(bool);
void ngkStatus(char *string);

