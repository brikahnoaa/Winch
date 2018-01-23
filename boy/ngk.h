// ngk.h
// AModemPort and WISPR Transmission

#define MDM_BAUD 4800L
#define BUOY_ID "00"
#define WINCH_ID "01"
// 00 or 03? 00 slacks at surface, 03 brakes underwater
#define RISE_CMD "#R," WINCH_ID ",03"
#define SURF_CMD "#R," WINCH_ID ",00"
#define DROP_CMD "#F," WINCH_ID ",00"
#define STOP_CMD "#S," WINCH_ID ",00"
#define STAT_CMD "#W," WINCH_ID ",00"
#define QUIT_RSP "%S," WINCH_ID ",00"

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
  int recv[sizeof_msg];
  int send[sizeof_msg];
  int timeout[sizeof_msg];
  MsgType lastRecv;
  MsgType lastSend;
  Serial port;
  short delay;        // time to transmit msg (7s)
} MdmInfo;
extern MdmInfo mdm;

MsgType ngkRecv(MsgType *msg);
bool msgParse(char *str, MsgType *msg);
void ngkInit(Serial *port);
void ngkSend(MsgType cmd);
