#include <common.h>
#include <ngk.h>

bool msgParse(char *str, MsgType *msg);

// mdm.message[] strings, as if sent to buoy; change ID before sending to winch
MdmInfo mdm = {
  { "null",
    // %B and %W are shorter, last two digits are status
    "#B,00,00", "%B,00,", "#F,00,00", "%F,00,00", "#R,00,00", "%R,00,00",
    "#W,00,00", "%W,00,", "#S,00,00", "%S,00,00", "#R,00,03", "surfRsp",
    "sizeof",
  },
  7
} // remainder of struct is 0 filled
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
 * starts timer, does not set ngk.on
 * sets: mdm.on .send[] .lastSend 
 * uses: mdm.delay
 */
void ngkSend(MsgType msg) {
  char m[12];
  DBG0("ngkSend(%d)",msg)
  strcpy(m, mdm.message[msg]);
  strcat(m, "\n");
  // set id "#R,0X,00"
  m[4]=WINCH_ID;
  TUTxWaitCompletion(mdm.port);
  serWrite(mdm.port, m);
  mdm.send[msg]++;
  if (msg!=quit_msg)
    mdm.on = true;            // expect response
  mdm.lastSend = msg;         // used by timeout
  timStart(winch_tim, mdm.delay*2+2);
} // ngkSend

/*
 * get winch message if available and parse it
 * sets: ngk.on mdm.expected .lastRecv .recv[] scratch (*msg)
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
  }
  *msg = m;
  return m;
} // ngkRecv

bool ngkTimeout(void) {
  if (mdm.expected && timExp(winch_tim)) {
    mdm.expected = null_msg;
    mdm.timeout[mdm.expected]++;
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
    if (strstr(str, mdm.message[m]!=NULL) 
      break;
  if (m==sizeof_msg) 
    m = null_msg;
  *msg = m;
  if (m==null_msg) {             // no match or invalid
    flogf("\nERR\t| msgParse() fail, '%s', len=%d", str, strlen(str));
    return false
  } else
    return true;
} // msgParse

