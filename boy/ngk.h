// ngk.h
// AModemPort and WISPR Transmission

#define AMODEMBAUD 4800L

typedef enum {
  null_cmd=0,
  rise_cmd,
  drop_cmd,
  stop_cmd,
  sizeof_cmd,
} CommandType;
typedef enum {
  null_rsp=0,
  rise_rsp,
  drop_rsp,
  stop_rsp,
  time_rsp,           // amodem timeout
  sizeof_rsp,
} RespondType;

typedef struct NgkInfo {
  bool on;            // ngk motor running
  float boy2ant;      // meters from buoy ctd to ant ctd under still water
  float firstRise;    // Velocity meters/min of the first rise (ascent)
  float lastRise;     // Velocity meters/min of the most recent rise 
  float firstDrop;    // Velocity meters/min of the first fall (descent)
  float lastDrop;     // Velocity meters/min of the most recent fall 
} NgkInfo;
extern NgkInfo ngk;

// Tracking number of calls
typedef struct AmodemInfo {
  bool on;
  CommandType lastCmd;
  int command[sizeof_cmd];
  int respond[sizeof_rsp];
  int timeout[sizeof_cmd];
  Serial port;
  short delay;        // time to transmit command, get response
  time_t pending;     // expect response by this time
} AmodemInfo;
extern AmodemInfo mdm;

void amodemData(void);
void ngkCommand(CommandType);
RespondType ngkRespond(void);
void ngkConsole(void);
void amodemInit(bool);
void ngkStatus(char *string);

