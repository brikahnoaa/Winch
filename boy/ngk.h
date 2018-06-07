// ngk.h
#define NGK_H

typedef enum {
  null_msg=0,
  buoyCmd_msg, buoyRsp_msg, fallCmd_msg, fallRsp_msg, riseCmd_msg, riseRsp_msg,
  statCmd_msg, statRsp_msg, stopCmd_msg, stopRsp_msg, surfCmd_msg, mangled_msg,
  sizeof_msg
} MsgType;

typedef struct NgkInfo {
  char * msgStr[sizeof_msg];    // msg string as if sent to buoy
  char * msgName[sizeof_msg];   // for logs
  // modem
  int recv[sizeof_msg];         // count
  int send[sizeof_msg];         // count
  int timeout[sizeof_msg];      // count
  MsgType lastRecv;
  MsgType lastSend;
  Serial port;
} NgkInfo;

static MsgType msgParse(char *str);
static void ngkBuoyRsp(void);

MsgType ngkRecv(MsgType *msg);
MsgType ngkRecvWait(MsgType *msg, int wait);
char *ngkRecvMsg(int wait);
char *ngkMsgName(MsgType msg);
void ngkInit(void);
void ngkSend(MsgType msg);
void ngkStart(void);
void ngkStop(void);
