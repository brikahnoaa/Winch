// ngk.h

#define MDM_BAUD 4800L
#define BUOY_ID '0'
#define WINCH_ID '1'
#define WINCH_EOL "\r\n"

typedef enum {
  null_msg=0,
  buoyCmd_msg, buoyRsp_msg, dropCmd_msg, dropRsp_msg, riseCmd_msg, riseRsp_msg,
  statCmd_msg, statRsp_msg, stopCmd_msg, stopRsp_msg, surfCmd_msg, mangled_msg,
  sizeof_msg
} MsgType;

typedef struct NgkInfo {
  bool motorOn;         // ngk motor running
  float boy2ant;        // meters from buoy ctd to ant ctd under still water
  float firstRiseV;     // Velocity meters/min of the first rise (ascent)
  float lastRiseV;      // Velocity meters/min of the most recent rise 
  float firstDropV;     // Velocity meters/min of the first drop (descent)
  float lastDropV;      // Velocity meters/min of the most recent drop 
  // int statMotor;     // winch status response X in %W,00,XY
  // int statRope;      // winch status response X in %W,00,XY
} NgkInfo;
extern NgkInfo ngk;

// Tracking number of calls
typedef struct MdmInfo {
  char * msgStr[sizeof_msg];    // msg string as if sent to buoy
  char * msgName[sizeof_msg];
  int delay;                    // # seconds for amodem to transmit msg (7s)
  int recv[sizeof_msg];         // count
  int send[sizeof_msg];         // count
  int timeout[sizeof_msg];      // count
  MsgType expect;               // response expected
  MsgType lastRecv;
  MsgType lastSend;
  Serial port;
} MdmInfo;
extern MdmInfo mdm;

static MsgType msgParse(char *str, MsgType *msg);

bool ngkTimeout(void);
MsgType ngkRecv(MsgType *msg);
void ngkInit(void);
void ngkSend(MsgType msg);
