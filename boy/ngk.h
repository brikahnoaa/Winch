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
  bool on;              // ngk motor running
  char * msgStr[sizeof_msg];    // msg string as if sent to buoy
  char * msgName[sizeof_msg];   // for logs
  // modem
  int delay;                    // # seconds for amodem to transmit msg (7s)
  int recv[sizeof_msg];         // count
  int send[sizeof_msg];         // count
  int timeout[sizeof_msg];      // count
  MsgType expect;               // response expected
  MsgType lastRecv;
  MsgType lastSend;
  Serial port;
} NgkInfo;

static MsgType msgParse(char *str, MsgType *msg);

bool ngkTimeout(void);
MsgType ngkRecv(MsgType *msg);
void ngkInit(void);
void ngkStop(void);
void ngkSend(MsgType msg);
void ngkMsgName(char *out, MsgType msg);

