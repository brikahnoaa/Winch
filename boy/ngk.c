// ngk.c
#include <utl.h>
#include <ngk.h>
#include <sys.h>
#include <tmr.h>

#define MDM_BAUD 4800L
#define BUOY_ID '2'
#define WINCH_ID '1'
#define EOL "\r\n"

NgkInfo ngk = {
  { "null",
    // ngk.msgStr[] as if sent to buoy; change ID before sending to winch
    "#B,02,00", "%B,02,00", "#F,02,00", "%F,02,00", "#R,02,03", "%R,02,00",
    "#W,02,00", "%W,02,00", "#S,02,00", "%S,02,00", "#R,02,00", "mangled",
  },
  { "null",
    "buoyCmd", "buoyRsp", "fallCmd", "fallRsp", "riseCmd", "riseRsp",
    "statCmd", "statRsp", "stopCmd", "stopRsp", "surfCmd", "mangled",
  }
}; // remainder of struct is 0 filled

///
// power on amodem
// sets: ngk.port
void ngkInit(void) {
  short mdmRX, mdmTX;
  Serial p;
  DBG0("ngkInit()");
  mdmRX = TPUChanFromPin(MDM_RX);
  mdmTX = TPUChanFromPin(MDM_TX);
  PIORead(MDM_RX_TTL);              // tpu->rs232 is pin 33->48->47
  // Power up the DC-DC for the Acoustic Modem Port
  ngkStop();
  utlDelay(SETTLE);
  ngkStart();
  utlDelay(SETTLE);
  // PIOClear(MDM_TX_TTL);             // tpu->rs232 is pin 35->50->49
  p = TUOpen(mdmRX, mdmTX, MDM_BAUD, 0);
  if (p == 0)
    utlStop("\nERR\t|ngkInit() Bad ngk serial port");
  else {
    TUTxFlush(p);
    TURxFlush(p);
  }
  ngk.port = p;
} // ngkInit

///
// power on
void ngkStart(void){
  PIOSet(MDM_PWR);
} // ngkStart

///
// power off
void ngkStop(void){
  PIOClear(MDM_PWR);
} // ngkStop

///
// send message to winch via amodem
// sets: .send[] .lastSend 
void ngkSend(MsgType msg) {
  char str[128];
  // copy msgStr and change id character // e.g. "#R,0X,00"
  strcpy(str, ngk.msgStr[msg]);
  str[4]=WINCH_ID;
  flogf("\n+ngkSend(%s) at %s", str, utlTime());
  TUTxWaitCompletion(ngk.port);
  TURxFlush(ngk.port);
  utlWrite(ngk.port, str, EOL);
  ngk.send[msg]++;
  ngk.lastSend = msg;
  if (!utlExpect(ngk.port, str, "OK", ngk.delay))
    flogf("\nngkSend() | WARN amodem bad response '%s'", utlNonPrint(str));
} // ngkSend

///
// wait for and log ngk response
MsgType ngkRecvWait(MsgType *msg, int wait) {
  tmrStart(ngk_tmr, wait);
  while (!tmrExp(ngk_tmr))
    if (ngkRecv(msg)!=null_msg)
      break;
  return *msg;
} // ngkRecvWait

///
// ngkRecvWait and return string
char *ngkRecvMsg(int wait) {
  MsgType msg;
  if (ngkRecvWait(&msg, wait)!=null_msg)
    return ngkMsgName(msg);
  else
    return "timeout";
} // ngkRecvWait

///
// get winch message if available and parse it
// respond immediately to stopcmd buoycmd
// uses: ngk.expect
// sets: ngk.on ngk.expect .lastRecv 
// returns: msg
MsgType ngkRecv(MsgType *msg) {
  if (TURxQueuedCount(ngk.port)==0)
    return null_msg;
  utlRead(ngk.port, utlBuf);
  flogf("\n+ngkRecv(%s)", utlBuf);
  *msg = msgParse(utlBuf);
  if (*msg!=mangled_msg) 
    utlWrite(ngk.port, "OK", EOL);
  flogf(" %s %s", ngk.msgName[*msg], utlTime());
  if (*msg==buoyCmd_msg) {     // async, invisible
    ngkBuoyRsp();
    *msg = null_msg;
  }
  return *msg;
} // ngkRecv

///
// match against ngk.msgStr[]
// sets: (*msgP) ngk.recv[]
// returns: msgtype
MsgType msgParse(char *str) {
  MsgType m;
  int len;
  len = strlen(str);      // '%S,00,00\r\n'
  // OK may be mixed in with message, from previous send
  if (strstr(str, "OK") && len<6)
    return null_msg;
  if (len>12)
    flogf(" | WARN msgParse() length=%d", len);
  // match
  for (m=null_msg+1; m<mangled_msg; m++)
    if (strstr(str, ngk.msgStr[m])!=NULL) 
      break;
  ngk.recv[m]++;
  if (m==mangled_msg)           // no match or invalid
    utlErr(ngkParse_err, str);
  return m;
} // msgParse

char * ngkMsgName(MsgType msg) {
  return ngk.msgName[msg];
} // ngkMsgName

///
// buoyRsp buoyRsp, fall and shutdown ??
void ngkBuoyRsp(void) {
  ngkSend(buoyRsp_msg);
  utlDelay(20);
  ngkSend(fallCmd_msg);
  utlSleep();
} // ngkBuoyRsp

///
void ngkFlush(void) { 
  TURxFlush(ngk.port);
  TUTxFlush(ngk.port);
} // ngkFlush
