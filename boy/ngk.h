// ngk.h
#define NGK_H

typedef enum {
  null_msg=0,
  buoyCmd_msg, buoyRsp_msg, dropCmd_msg, dropRsp_msg, riseCmd_msg, riseRsp_msg,
  statCmd_msg, statRsp_msg, stopCmd_msg, stopRsp_msg, surfCmd_msg, mangled_msg,
  sizeof_msg
} MsgType;

typedef struct NgkInfo {
  char * msgStr[sizeof_msg];    // msg string as if sent to buoy
  char * msgName[sizeof_msg];   // for logs
  // modem
  int delay;                    // modem talk delay (7s)
  int recv[sizeof_msg];         // count
  int send[sizeof_msg];         // count
  int timeout[sizeof_msg];      // count
  MsgType lastRecv;
  MsgType lastSend;
  Serial port;
} NgkInfo;

static MsgType msgParse(char *str);
static void ngkBuoyRsp(void);

MsgType ngkRecv(void);
void ngkInit(void);
char * ngkMsgName(MsgType msg);
void ngkSend(MsgType msg);
void ngkStop(void);
