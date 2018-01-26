#include <common.h>
#include <ngk.h>

NgkInfo ngk;
MdmInfo mdm = {
  { "",
    // %B and %W are shorter, last two digits are status
    "#B,01,00", "%B,01,", "#F,01,00", "%F,01,00", "#R,01,00", "%R,01,00",
    "#W,01,00", "%W,01,", "#S,01,00", "%S,01,00", "#R,01,03", "%R,01,00",
    "",
    "",
  },
  7
} // remainder of struct is 0 filled

/*
 * send message to winch via amodem
 * starts timer, does not set ngk.on
 * sets: mdm.on .send[] .lastSend 
 * uses: mdm.delay
 */
void ngkSend(MsgType msg) {
  DBG0("ngkSend()")
  TUTxWaitCompletion(mdm.port);
  serWrite(mdm.port, mdm.message[msg]);
  mdm.send[msg]++;
  if (msg!=quit_msg)
    mdm.on = true;            // expect response
  mdm.lastSend = msg;         // used by timeout
  timStart(winch_tim, mdm.delay*2+2);
} // ngkSend

/*
 * get winch message if available and parse it, check timeout
 * sets: ngk.on mdm.expected .lastRecv .recv[] .timeout[] scratch (*msg)
 */
MsgType ngkRecv(MsgType *msg) {
  MsgType m = null_msg;                // change this if successful
  if (serRead(mdm.port, scratch)) {
    if (msgParse(scratch, &m)) {
      // m == stop, quit, rise, drop, status, buoy
      // tbd TBD check if lastSend matches?
      mdm.on = false;
      mdm.recv[m]++;
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
    mdm.timeout[lastSend]++;
    m = timeout_msg;
  }
  *msg = m;
  return m;
} // ngkRecv

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
 * match against mdm.message[] strings
 * sets: (*msg)
 */
bool msgParse(char *str, MsgType *msg) {
  MsgType m;
  // count down to null_msg, that means not found
  for (m=timeout_msg-1; m>null_msg; m--) 
    if (strstr(str, mdm.message[m]!=NULL) 
      break;
  *msg = m;
  if (m==null_msg) {             // no match or invalid
    flogf("\nERR\t| msgParse() fail, '%s', len=%d", str, strlen(str));
    return false
  } else
    return true;
} // msgParse

