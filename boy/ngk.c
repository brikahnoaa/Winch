// ngk.c
#include <utl.h>
#include <ngk.h>
#include <tmr.h>

#define MDM_BAUD 4800L
#define BUOY_ID '2'
#define WINCH_ID '1'
#define EOL "\r\n"

NgkInfo ngk = {
  { "null",
    // ngk.msgStr[] as if sent to buoy; change ID before sending to winch
    // %B and %W are shorter, last two digits are status
    "#B,02,00", "%B,02,", "#F,02,00", "%F,02,00", "#R,02,03", "%R,02,00",
    "#W,02,00", "%W,02,", "#S,02,00", "%S,02,00", "#R,02,00", "mangled",
  },
  { "null",
    "buoyCmd", "buoyRsp", "dropCmd", "dropRsp", "riseCmd", "riseRsp",
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
  PIOClear(MDM_PWR);
  utlDelay(RS232_SETTLE);
  PIOSet(MDM_PWR);
  utlDelay(RS232_SETTLE);
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

void ngkStop(void){
  PIOClear(MDM_PWR);
}

///
// send message to winch via amodem
// sets: .send[] .lastSend 
void ngkSend(MsgType msg) {
  char str[12];
  // copy msgStr and change id character // e.g. "#R,0X,00"
  strcpy(str, ngk.msgStr[msg]);
  str[4]=WINCH_ID;
  flogf("\n+ngkSend(%s) at %s", str, utlTime());
  TUTxWaitCompletion(ngk.port);
  utlWrite(ngk.port, str, EOL);
  ngk.send[msg]++;
  ngk.lastSend = msg;
  if (msg==dropCmd_msg || msg==riseCmd_msg 
   || msg==stopCmd_msg || msg==surfCmd_msg) 
    tmrStart(winch_tmr, ngk.delay*2+1);
  str[0] = 0;
  utlReadWait(ngk.port, str, 2);
  // str should include "OK"
  if (strstr(str, "OK")==NULL)
    flogf("\n\t| \nngkSend() amodem bad response '%s'", str);
} // ngkSend

///
// get winch message if available and parse it
// respond immediately to stopcmd buoycmd
// uses: ngk.expect
// sets: ngk.on ngk.expect .lastRecv 
// returns: msg
MsgType ngkRecv() {
  MsgType msg;
  if (utlRead(ngk.port, utlBuf)==0) 
    return null_msg;
  msg = msgParse(utlBuf);
  flogf("\n+ngkRecv(%s) at %s", ngk.msgName[msg], utlTime());
  if (msg==buoyCmd_msg) {     // async, invisible
    ngkBuoyRsp();
    return null_msg;
  }
  if (msg==dropRsp_msg || msg==riseRsp_msg || msg==stopRsp_msg) 
    tmrStop(winch_tmr);
  if (msg==stopCmd_msg)
    ngkSend(stopRsp_msg);
  return msg;
} // ngkRecv

///
// match against ngk.msgStr[]
// sets: (*msgP) ngk.recv[]
// returns: msgtype
MsgType msgParse(char *str) {
  MsgType m;
  int len;
  len = utlTrim(str);
  if (len!=8) 
    flogf(" | ERR msgParse() length %d", len);
  for (m=null_msg+1; m<mangled_msg; m++)
    if (strstr(str, ngk.msgStr[m])!=NULL) 
      break;
  ngk.recv[m]++;
  if (m==mangled_msg)           // no match or invalid
    utlErr(ngkParse_err, str);
  else
    utlWrite(ngk.port, "OK", EOL);
  return m;
} // msgParse

char * ngkMsgName(MsgType msg) {
  return ngk.msgName[msg];
}

///
// construct ?? and send buoy status response
void ngkBuoyRsp(void) {
  ngkSend( buoyRsp_msg);
}
