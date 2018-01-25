#include <common.h>
#include <ngk.h>

NgkInfo ngk;
MdmInfo mdm;

/*
 * send message to winch via amodem
 * starts timer, does not set ngk.on
 * sets: mdm.on .send[] .lastSend 
 * uses: mdm.delay
 */
void ngkSend(MsgType cmd) {
  DBG0("ngkSend()")
  TUTxWaitCompletion(mdm.port);
  switch (cmd) {
  case rise_msg:
    TUTxPrintf(mdm.port, RISE_CMD);
    break;
  case surf_msg:
    TUTxPrintf(mdm.port, SURF_CMD);
    break;
  case drop_msg:
    TUTxPrintf(mdm.port, DROP_CMD);
    break;
  case stat_msg:
    TUTxPrintf(mdm.port, STAT_CMD);
    break;
  case stop_msg:
    TUTxPrintf(mdm.port, STOP_CMD);
    break;
  case quit_msg:
    TUTxPrintf(mdm.port, QUIT_RSP);
    break;
  default:
    flogf("\nERR\t|ngkSend bad msg %d", cmd);
    return;
  } // switch
  mdm.send[cmd] += 1;
  if (cmd!=quit_msg)
    mdm.on = true;            // expect response
  mdm.lastSend = cmd;         // used by timeout
  timStart(winch_tim, mdm.delay*2+2);
} // ngkSend

/*
 * get winch message if available and parse it, check timeout
 * sets: ngk.on mdm.on .recv[] .timeout[] scratch (*msg)
 */
MsgType ngkRecv(MsgType *msg) {
  MsgType m = null_msg;                // change this if successful
  if (serRead(mdm.port, scratch)) {
    if (msgParse(scratch, &m)) {
      // m == stop, quit, rise, drop, status, buoy
      // tbd TBD check if lastSend matches?
      mdm.on = false;
      mdm.recv[m] += 1;
      timStop(winch_tim);
      // winch motor
      if (m==rise_msg || m==drop_msg) 
        ngk.on = true;
      if (m==stop_msg || m==quit_msg) 
        ngk.on = false;
    } else {
      // parse fail. already logged by msgParse. wait for timeout? resend?
      // ngkSend( mdm.lastSend );
    }
  } else if (timExp(winch_tim)) {
    // no response and timeout
    mdm.on = false;
    mdm.timeout[lastSend] += 1;
    m = timeout_msg;
  }
  *msg = m;
  return m;
} // ngkRecv

/*
 * void ngkInit(bool)
 */
void ngkInit(Serial &port) {
  short mdmRX, mdmTX;
  Serial p;
  DBG0("mdmInit()");

  mdmRX = TPUChanFromPin(MDM_RX);
  mdmTX = TPUChanFromPin(MDM_TX);
  // Power up the DC-DC for the Acoustic Modem Port
  PIOClear(MDM_PWR);
  delayms(250);
  PIOSet(MDM_PWR); 
  p = TUOpen(mdmRX, mdmTX, MDM_BAUD, 0);
  if (p == 0)
    flogf("\n\t|Bad ngk TUPort\n");
  else {
    TUTxFlush(p);
    TURxFlush(p);
  }
  *port = p;
} // ngkInit

/*
 * validate - count \r and % chars, one of each; length 10
 * parse - based on %R %S %F
 * status - save response values "%W,00,XYcrlf"
 * sets: ngk.statMotor .statRope (*msg)
 */
bool msgParse(char *str, MsgType *msg) {
  MsgType m = null_msg;                // change this if successful
  // validate: len10 ',00,' crlf
  if (strlen(str)==10 
      && memcmp(str+2, "," BUOYID ",", 4)==0 
      && strcmp(str+8, CRLF)==0
      ) {
    if (str[0]=='#') {
      // command
      switch (str[1]) {
      case 'S':
        m = quit_msg;
        ngkSend( quit_msg );
        break;
      } // switch
    } else if (str[0]=='%') {
      // response
      switch str[1] {
      case 'R':
        m = rise_msg; break;
      case 'F':
        m = drop_msg; break;
      case 'S':
        m = stop_msg; break;
      case 'W': // winch status
        m = stat_msg;
        // response values - convert ascii digits to integer
        ngk.statMotor=(int)(str[6]-'0');
        ngk.statRope=(int)(str[7]-'0');
        break;
      case 'B': // buoy status
        ngkSend( buoy_msg );
        m = buoy_msg;
        break;
      } // switch
    } // if % response
    mdm.lastRecv = m;
  } // if well formed
  *msg = m;
  if (m==null_msg) {             // no match or invalid
    flogf("\nERR\t| msgParse() fail, '%s', len=%d cr#=%d %%#=%d", 
      str, len, cr, percent);
    return false
  } else
    return true;
} // msgParse

