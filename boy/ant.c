// ant.c - for working with antenna module
#include <main.h>

#define EOL "\r"
#define BAUD 9600L
#define EXEC "<Executed/>"

AntInfo ant;

///
// turn on antenna module, wait until ant responds
// sets: ant.mode .port
void antInit(void) {
  static char *self="antInit";
  short rx, tx;
  DBG();
  ant.me="ant";
  // port
  rx = TPUChanFromPin(ANT_RX);
  tx = TPUChanFromPin(ANT_TX);
  ant.port = TUOpen(rx, tx, BAUD, 0);
  if (ant.port==NULL)
    utlStop("antInit() com1 open fail");
  antDevice(null_dev);
  ant.on = false;
} // antInit

///
// turn on, clean, set params
int antStart(void) {
  static char *self="antStart";
  DBG();
  ant.on = true;
  flogf("\n === ant module start %s", utlDateTime());
  antLogOpen();
  antDevice(cf2_dev);
  PIOClear(ANT_PWR);
  utlDelay(200);
  TURxFlush(ant.port);
  TUTxFlush(ant.port);
  PIOSet(ANT_PWR);
  // get cf2 startup message
  if (!utlReadExpect(ant.port, all.str, "ok", 12))
    flogf("\n%s(): expected ok, saw '%s'", self, all.str);
  DBG1("%s", all.str);
  return 0;
} // antStart

///
// turn off power to antmod 
int antStop() {
  ant.on = false;
  flogf("\n === ant module stop %s", utlDateTime());
  antLogClose();
  antDevice(null_dev);
  PIOClear(ANT_PWR);
  return 0;
} // antStop

///
// open or reopen log file
int antLogOpen(void) {
  static char *self="antLogOpen";
  int r=0;
  if (!ant.on)
    antStart();
  if (!ant.log)
    r = utlLogOpen(&ant.log, ant.me);
  else
    DBG2("%s: log already open", self);
  return r;
} // antLogOpen

///
///
// open or reopen log file
int antLogClose(void) {
  static char *self="antLogClose";
  int r=0;
  if (ant.log)
    r = utlLogClose(&ant.log);
  else
    DBG2("%s: log already closed", self);
  return r;
} // antLogClose

///
// antmod uMPC cf2 and iridium A3LA
// switch between devices on com1, clear pipe
void antDevice(DevType dev) {
  DBG1("antDevice(%s)",(dev==cf2_dev)?"cf2":"a3la");
  if (dev==ant.dev) return;
  utlDelay(SETTLE);
  if (dev==cf2_dev)
    PIOSet(ANT_SEL);
  else if (dev==a3la_dev)
    PIOClear(ANT_SEL);
  else
    return;
  utlDelay(SETTLE);
  TUTxFlush(ant.port);
  TURxFlush(ant.port);
  ant.dev = dev;
  return;
} // antDevice

///
// s39.port = gps.port = antPort()
void antPort(Serial *port) {
  *port = ant.port;
} // antPort

///
// tell antmod to power dev on/off
// should be in gps.c??
void antDevPwr(char c, bool on) {
  DevType currDev=ant.dev;
  DBG0("antDevPwr(%c, %d)", c, on);
  antDevice(cf2_dev);
  if (on)
    TUTxPutByte(ant.port, 3, false);
  else
    TUTxPutByte(ant.port, 4, false);
  TUTxPutByte(ant.port, c, false);
  antDevice(currDev);
} // antDevPwr

///
// should be in gps.c??
void antSwitch(AntType antenna) {
  DevType dev;
  if (antenna==ant.antenna) return;
  DBG1("antSwitch(%s)", (antenna==gps_ant)?"gps":"irid");
  dev = ant.dev;
  antDevice(cf2_dev);
  TUTxPutByte(ant.port, 1, false);        // ^A
  if (antenna==gps_ant) 
    TUTxPutByte(ant.port, 'G', false);
  else
    TUTxPutByte(ant.port, 'I', false);
  antDevice(dev);
  ant.antenna = antenna;
} // antSwitch
    
float antSurfD(void) {
  return ant.surfD;
}

void antAntenna(AntType *antenna) {
  *antenna = ant.antenna;
}
