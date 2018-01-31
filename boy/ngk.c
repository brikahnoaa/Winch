// ngk.c
#include <com.h>
#include <ngk.h>
#include <tmr.h>

// mdm.message[] strings, as if sent to buoy; change ID before sending to winch
MdmInfo mdm = {
    // %B and %W are shorter, last two digits are status
  { "null",
    "#B,00,00", "%B,00,", "#F,00,00", "%F,00,00", "#R,00,00", "%R,00,00",
    "#W,00,00", "%W,00,", "#S,00,00", "%S,00,00", "#R,00,03", "surfRsp",
  },
  { "null",
    "buoyCmd", "buoyRsp", "dropCmd", "dropRsp", "riseCmd", "riseRsp",
    "statCmd", "statRsp", "stopCmd", "stopRsp", "surfCmd", "surfRsp"
  },
  7
}; // remainder of struct is 0 filled
NgkInfo ngk;

/*
 * power on amodem, initialize strings
 * sets: mdm.port
 */
void ngkInit(void) {
  short mdmRX, mdmTX;
  Serial p;
  DBG0("ngkInit()");

  // Power up the DC-DC for the Acoustic Modem Port
  PIOClear(MDM_PWR);
  delayms(100);
  PIOSet(MDM_PWR); 
  delayms(100);
  mdmRX = TPUChanFromPin(MDM_RX);
  mdmTX = TPUChanFromPin(MDM_TX);
  p = TUOpen(mdmRX, mdmTX, MDM_BAUD, 0);
  if (p == 0)
    flogf("\nERR\t|ngkInit() Bad ngk serial port");
  else {
    TUTxFlush(p);
    TURxFlush(p);
  }
  mdm.port = p;
} // ngkInit

/*
 * send message to winch via amodem
 * starts winch_tmr, does not set ngk.on (see ngkRecv)
 * sets: mdm.expect .send[] .lastSend 
 * uses: mdm.delay
 */
void ngkSend(MsgType msg) {
  char str[12];
  flogf("\nngkSend(%s) at %s", mdm.name[msg], clockTime(scratch));
  strcpy(str, mdm.message[msg]);
  strcat(str, "\n");
  // set winch id "#R,0X,00"
  str[4]=WINCH_ID;
  TUTxWaitCompletion(mdm.port);
  serWrite(mdm.port, str);
  mdm.send[msg]++;
  mdm.lastSend = msg;         // used by timeout
  switch (msg) {
  case dropCmd_msg:
    mdm.expect = dropRsp_msg;
  case riseCmd_msg:
    mdm.expect = riseRsp_msg;
  case statCmd_msg:
    mdm.expect = statRsp_msg;
  case stopCmd_msg:
    mdm.expect = stopRsp_msg;
  case surfCmd_msg:
    mdm.expect = surfRsp_msg;
  default:
    mdm.expect = null_msg;
  }
  if (mdm.expect!=null_msg)
    tmrStart(winch_tmr, mdm.delay*2+2);
} // ngkSend

/*
 * get winch message if available and parse it
 * ?? respond to stopcmd buoycmd
 * sets: ngk.on mdm.expect .lastRecv .recv[] scratch (*msg)
 */
MsgType ngkRecv(MsgType *msg) {
  MsgType m = null_msg;                // change this if successful
  char msgStr[BUFSZ], *ptr;
  int len = 0;
  if (serRead(mdm.port, msgStr)) {
    if (len==10)
      // strip anything past CRLF
      if ((ptr = strpbrk(msgStr, "\r\n"))!=NULL) 
        *ptr = 0;
      else
        flogf("\nERR\t|ngkRecv() no CRLF");
    else
      flogf("\nERR\t|ngkRecv() wrong msg length = %d", len);
    flogf("\n\t|ngkRecv(%s)", msgStr);
    if (msgParse(msgStr, &m)) {
      flogf("->%s at %s", mdm.name[m], clockTime(scratch));
      // tbd TBD check if lastSend matches?
      if (mdm.expect!=m) {
        flogf("\nERR\t| (expecting %s)", mdm.name[mdm.expect]);
      }
      mdm.expect = null_msg;
      mdm.recv[m]++;
      tmrStop(winch_tmr);
      // winch motor
      if (m==riseRsp_msg || m==dropRsp_msg) 
        ngk.on = true;
      if (m==stopCmd_msg || m==stopRsp_msg) 
        ngk.on = false;
    } else {
      // parse fail. already logged by msgParse. wait for timeout? resend?
      // ??
      // ngkSend( mdm.lastSend );
    }
  }
  *msg = m;
  return m;
} // ngkRecv

bool ngkTimeout(void) {
  if (mdm.expect!=null_msg && tmrExp(winch_tmr)) {
    mdm.expect = null_msg;
    mdm.timeout[mdm.expect]++;
    return true;
  } else
    return false;
} // ngkTimeout

/*
 * match against mdm.message[] strings
 * sets: (*msg)
 */
bool msgParse(char *str, MsgType *msg) {
  MsgType m;
  // search down until null_msg, that means not found
  for (m=null_msg+1; m<sizeof_msg; m++) 
    if (strstr(str, mdm.message[m])!=NULL) 
      break;
  if (m==sizeof_msg) 
    m = null_msg;
  *msg = m;
  if (m==null_msg) {             // no match or invalid
    flogf("\nERR\t| msgParse() fail, '%s', len=%d", str, strlen(str));
    return false;
  } else
    return true;
} // msgParse

