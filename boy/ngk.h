// ngk.h
// AModemPort and WISPR Transmission

#define AMODEMBAUD 4800L
#define BUOYID "00"
#define WINCHID "01"
// 00 or 03? 00 slacks at surface, 03 brakes underwater
#define RISE_CMD "#R," WINCHID ",03"
#define SURF_CMD "#R," WINCHID ",00"
#define DROP_CMD "#F," WINCHID ",00"
#define STOP_CMD "#S," WINCHID ",00"
#define STAT_CMD "#W," WINCHID ",00"
#define QUIT_RSP "%S," WINCHID ",00"

typedef enum {
  null_msg=0,
  rise_msg,           // rise and brake on stop
  surf_msg,           // rise to surface, no brake
  drop_msg,
  stop_msg,           // tell winch to stop
  quit_msg,           // winch stops itself
  stat_msg,           // winch status
  buoy_msg,           // buoy status (from deck unit, via winch)
  timeout_msg,
  sizeof_msg,
} MsgType;

typedef struct NgkInfo {
  bool on;            // ngk motor running
  float boy2ant;      // meters from buoy ctd to ant ctd under still water
  float firstRise;    // Velocity meters/min of the first rise (ascent)
  float lastRise;     // Velocity meters/min of the most recent rise 
  float firstDrop;    // Velocity meters/min of the first drop (descent)
  float lastDrop;     // Velocity meters/min of the most recent drop 
  int statMotor;      // status response X in %W,00,XY
  int statRope;       // status response X in %W,00,XY
} NgkInfo;
extern NgkInfo ngk;

// Tracking number of calls
typedef struct MdmInfo {
  bool on;            // expect response 
  MsgType lastSend;
  MsgType lastRecv;
  int send[sizeof_msg];
  int recv[sizeof_msg];
  int timeout[sizeof_msg];
  Serial port;
  short delay;        // time to transmit msg (7s)
} MdmInfo;
extern MdmInfo mdm;

void amodemData(void);
void ngkSend(MsgType);
MsgType ngkRecv(void);
void ngkConsole(void);
void amodemInit(bool);
void ngkStatus(char *string);

