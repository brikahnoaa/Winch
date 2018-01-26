// ngk.h
// AModemPort and WISPR Transmission

#define MDM_BAUD 4800L
#define BUOY_ID '0'
#define WINCH_ID '1'

typedef enum {
  null_msg=0,
  buoyCmd_msg, buoyRsp_msg, dropCmd_msg, dropRsp_msg, riseCmd_msg, riseRsp_msg,
  statCmd_msg, statRsp_msg, stopCmd_msg, stopRsp_msg, surfCmd_msg, surfRsp_msg,
  sizeof_msg,
} MsgType;

typedef struct NgkInfo {
  bool on;            // ngk motor running
  float boy2ant;      // meters from buoy ctd to ant ctd under still water
  float firstRise;    // Velocity meters/min of the first rise (ascent)
  float lastRise;     // Velocity meters/min of the most recent rise 
  float firstDrop;    // Velocity meters/min of the first drop (descent)
  float lastDrop;     // Velocity meters/min of the most recent drop 
  // int statMotor;      // winch status response X in %W,00,XY
  // int statRope;       // winch status response X in %W,00,XY
} NgkInfo;
extern NgkInfo ngk;

// Tracking number of calls
typedef struct MdmInfo {
  char * message[sizeof_msg];   // msg string
  int delay;                    // # seconds for amodem to transmit msg (7s)
  int recv[sizeof_msg];         // count
  int send[sizeof_msg];         // count
  int timeout[sizeof_msg];      // count
  MsgType expected;             // response expected
  MsgType lastRecv;
  MsgType lastSend;
  Serial port;
} MdmInfo;
extern MdmInfo mdm;

MsgType ngkRecv(MsgType *msg);
bool ngkTimeout(void);
void ngkInit(void);
void ngkSend(MsgType msg);
