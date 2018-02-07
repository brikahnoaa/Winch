// ngk.c
#include <com.h>
#include <ngk.h>
#include <tmr.h>

MdmInfo mdm = {
  { "null", 
    // mdm.msgStr[] as if sent to buoy; change ID before sending to winch
    // %B and %W are shorter, last two digits are status
    "#B,00,00", "%B,00,", "#F,00,00", "%F,00,00", "#R,00,00", "%R,00,00",
    "#W,00,00", "%W,00,", "#S,00,00", "%S,00,00", "#R,00,03", "mangled",
  },
  { "null",
    "buoyCmd", "buoyRsp", "dropCmd", "dropRsp", "riseCmd", "riseRsp",
    "statCmd", "statRsp", "stopCmd", "stopRsp", "surfCmd", "mangled",
  },
  7 // delay
}; // remainder of struct is 0 filled
NgkInfo ngk;

//
// power on amodem
// sets: mdm.port
//
void ngkInit(void) {
  short mdmRX, mdmTX;
  Serial p;
  DBG0("ngkInit()");
  // Power up the DC-DC for the Acoustic Modem Port
  PIOClear(MDM_PWR);
  delayms(250);
  PIOSet(MDM_PWR); 
  delayms(250);
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
  mdm.port = p;
} // ngkInit

//
// send message to winch via amodem
// starts winch_tmr, does not set ngk.on (see ngkRecv)
// sets: mdm.expect .send[] .lastSend 
// uses: mdm.delay
//
void ngkSend(MsgType msg) {
  char str[12];
  flogf("\nngkSend(%s) at %s", mdm.msgName[msg], clockTime(scratch));
  strcpy(str, mdm.msgStr[msg]);
  strcat(str, WINCH_EOL);
  // set winch id "#R,0X,00"
  str[4]=WINCH_ID;
  TUTxWaitCompletion(mdm.port);
  serWrite(mdm.port, str);
  mdm.send[msg]++;
  mdm.lastSend = msg;         // used by timeout
  switch (msg) {
  case dropCmd_msg:
    mdm.expect = dropRsp_msg;
    break;
  case riseCmd_msg:
    mdm.expect = riseRsp_msg;
    break;
  case statCmd_msg:
    mdm.expect = statRsp_msg;
    break;
  case stopCmd_msg:
    mdm.expect = stopRsp_msg;
    break;
  case surfCmd_msg:
    mdm.expect = riseRsp_msg;
    break;
  default:
    mdm.expect = 0;
  }
  if (mdm.expect!=null_msg)
    tmrStart(winch_tmr, mdm.delay*2+2);
} // ngkSend

//
// get winch message if available and parse it
// ?? respond to stopcmd buoycmd
// sets: ngk.on mdm.expect .lastRecv .recv[] scratch (*msg)
// returns: false if no message
//
MsgType ngkRecv(MsgType *msg) {
  MsgType m = null_msg;
  char msgStr[BUFSZ];
  if (serRead(mdm.port, msgStr)==0) 
    return null_msg;
  if (msgParse(msgStr, &m)==mangled_msg) 
    return mangled_msg;
  flogf("\n\t|ngkRecv(%s) at %s", mdm.msgName[m], clockTime(scratch));
  mdm.recv[m]++;
  if (m==buoyCmd_msg) {                // buoyCmd could happen anytime
    ngkSend(buoyRsp_msg);
    return null_msg;
  }
  if (m==stopCmd_msg) {
    // surfaced or jammed
    ngkSend(stopRsp_msg);
  }
  if (mdm.expect) { // expecting a msg
    if (mdm.expect!=m)
      flogf(" (expecting %s)", mdm.msgName[mdm.expect]);
    mdm.expect = 0;
    tmrStop(winch_tmr);
  }
  // winch motor
  if (m==riseRsp_msg || m==dropRsp_msg) 
    ngk.motorOn = true;
  if (m==stopCmd_msg || m==stopRsp_msg) 
    ngk.motorOn = false;
  // 
  *msg = m;
  return m;
} // ngkRecv

bool ngkTimeout(void) {
  if (mdm.expect && tmrExp(winch_tmr)) {
    mdm.expect = null_msg;
    mdm.timeout[mdm.expect]++;
    return true;
  } else
    return false;
} // ngkTimeout

//
// match against mdm.msgStr[]
// sets: (*msg)
// returns: msg
//
MsgType msgParse(char *str, MsgType *msg) {
  MsgType m = mangled_msg;
  int len;
  len = crlfTrim(str);
  if (len!=8) 
    flogf("\nERR\t|msgParse() wrong msg length = %d", len);
  for (m=null_msg+1; m<mangled_msg; m++)
    if (strstr(str, mdm.msgStr[m])!=NULL) 
      break;
  if (m==mangled_msg)             // no match or invalid
    flogf("\nERR\t|msgParse(%s) fail", str);
  *msg = m;
  return m;
} // msgParse

