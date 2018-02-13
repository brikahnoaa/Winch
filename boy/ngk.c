// ngk.c
#include <com.h>
#include <ngk.h>
#include <tmr.h>

NgkInfo ngk = {
  false,
  { "null",
    // ngk.msgStr[] as if sent to buoy; change ID before sending to winch
    // %B and %W are shorter, last two digits are status
    "#B,00,00", "%B,00,", "#F,00,00", "%F,00,00", "#R,00,03", "%R,00,00",
    "#W,00,00", "%W,00,", "#S,00,00", "%S,00,00", "#R,00,00", "mangled",
  },
  { "null",
    "buoyCmd", "buoyRsp", "dropCmd", "dropRsp", "riseCmd", "riseRsp",
    "statCmd", "statRsp", "stopCmd", "stopRsp", "surfCmd", "mangled",
  }
}; // remainder of struct is 0 filled

//
// power on amodem
// sets: ngk.port
//
void ngkInit(void) {
  short mdmRX, mdmTX;
  Serial p;
  DBG0("ngkInit()");
  // Power up the DC-DC for the Acoustic Modem Port
  PIOClear(MDM_PWR);
  delayms(RS232_SETTLE);
  PIOSet(MDM_PWR); 
  delayms(RS232_SETTLE);
  PIORead(MDM_RX_TTL);              // tpu->rs232 is pin 33->48->47
  PIOClear(MDM_TX_TTL);             // tpu->rs232 is pin 35->50->49
  mdmRX = TPUChanFromPin(MDM_RX);
  mdmTX = TPUChanFromPin(MDM_TX);
  p = TUOpen(mdmRX, mdmTX, MDM_BAUD, 0);
  if (p == 0)
    flogf("\nERR\t|ngkInit() Bad ngk serial port");
  else {
    TUTxFlush(p);
    TURxFlush(p);
    serWrite(p, "\n");      // ??
  }
  ngk.port = p;
} // ngkInit

void ngkStop(void){
  PIOClear(MDM_PWR);
}

//
// send message to winch via amodem
// starts winch_tmr, does not set ngk.on (see ngkRecv)
// sets: ngk.expect .send[] .lastSend 
// uses: ngk.delay
//
void ngkSend(MsgType msg) {
  char str[12];
  flogf("\nngkSend(%s) at %s", ngk.msgName[msg], clockTime(scratch));
  strcpy(str, ngk.msgStr[msg]);
  strcat(str, WINCH_EOL);
  // set winch id "#R,0X,00"
  str[4]=WINCH_ID;
  TUTxWaitCompletion(ngk.port);
  serWrite(ngk.port, str);
  ngk.send[msg]++;
  ngk.lastSend = msg;         // used by timeout
  switch (msg) {
  case dropCmd_msg:
    ngk.expect = dropRsp_msg;
    break;
  case riseCmd_msg:
    ngk.expect = riseRsp_msg;
    break;
  case statCmd_msg:
    ngk.expect = statRsp_msg;
    break;
  case stopCmd_msg:
    ngk.expect = stopRsp_msg;
    break;
  case surfCmd_msg:
    ngk.expect = riseRsp_msg;
    break;
  default:
    ngk.expect = null_msg;
  }
  if (ngk.expect!=null_msg)
    tmrStart(winch_tmr, ngk.delay*2+2);
} // ngkSend

//
// get winch message if available and parse it
// ?? respond to stopcmd buoycmd
// uses: ngk.expect
// sets: ngk.on ngk.expect .lastRecv scratch (*msg)
// returns: false if no message
//
bool ngkRecv(MsgType *msgP) {
  char msgStr[BUFSZ];
  MsgType m;
  if (serRead(ngk.port, msgStr)==0) {
    if (ngkTimeout()) {
      *msgP = timeout_msg;
    else
      *msgP = null_msg;
    return false;
  }
  if (!msgParse(msgP, msgStr))    // mangled
    return false;
  m = *msgP;
  if (m==buoyCmd_msg) {
    // async, handled special. Does not change .expect
    ngkBuoyRsp();
    *msgP = null_msg;
    return false;
  }
  // normal message
  flogf("\n\t|ngkRecv(%s) at %s", ngk.msgName[m], clockTime(scratch));
  if (m==stopCmd_msg)             // surfaced or jammed
    ngkSend(stopRsp_msg);
  // winch motor
  if (m==riseRsp_msg || m==dropRsp_msg) 
    ngk.on = true;
  if (m==stopCmd_msg || m==stopRsp_msg) 
    ngk.on = false;
  if (ngk.expect==null_msg) 
    return true;     
  // we are expecting a msg
  tmrStop(winch_tmr);
  if (ngk.expect==m) {
    ngk.expect = null_msg;
    return true;
  } else { 
    flogf(" (expected %s)", ngk.msgName[ngk.expect]);
    ngk.expect = null_msg;
    return false;
  }
} // ngkRecv

bool ngkTimeout(void) {
  if (ngk.expect && tmrExp(winch_tmr)) {
    ngk.timeout[ngk.expect]++;
    ngk.expect = null_msg;
    return true;
  } else
    return false;
} // ngkTimeout

//
// match against ngk.msgStr[]
// sets: (*msgP) ngk.recv[]
// returns: success
//
bool msgParse(MsgType *msgP, char *str) {
  MsgType m;
  int len;
  len = crlfTrim(str);
  if (len!=8) 
    flogf(" | ERR msgParse() length %d", len);
  for (m=null_msg+1; m<mangled_msg; m++)
    if (strstr(str, ngk.msgStr[m])!=NULL) 
      break;
  ngk.recv[m]++;
  *msgP = m;
  if (m==mangled_msg) {           // no match or invalid
    flogf(" | ERR msgParse(%s) fail", str);
    return false;
  } else
    return true;
} // msgParse

void ngkMsgName(char *out, MsgType msg) {
  strcpy(out, ngk.msgName[msg]);
  return;
}

void ngkExpect(MsgType msg) {
  ngk.expect = msg;
}
