// iridFile.c
#include <main.h>

extern GpsInfo gps;
extern GpsData gpsd;
extern BoyInfo boy;
extern SysInfo sys;

void main(void){
  // Serial port;
  // char c;
  char *buff;
  int len, cnt;
  int i, r;
  i=0;
  sysInit();
  mpcInit();
  antInit();
  gpsInit();
  //
  antStart();
  gpsStart();
  //
  len = dbg.t2;
  cnt = dbg.t1;
  cprintf("\nlength dbg.t2=%d, count dbg.t1=%d ", len, cnt);
  cprintf("\nbaud gps.rudBaud=%d", gps.rudBaud);
  buff = malloc(len);
  // antSwitch(gps_ant);
  // gpsStats();
  antSwitch(irid_ant);
  if (iridSig()) return;
  if (iridDial()) return;
  if (iridProjHdr()) return;
  /*
  for (i=1; i<=cnt; i++) {
    memset(buff, 0, len);
    sprintf(buff, "%d of %d =%d @%d [%d]", 
      i, cnt, len, gps.rudBaud, gps.sendSz);
    buff[len-1] = 'Z';
    r = iridSendBlock(buff, len, i, cnt);
    cprintf("(%d)\n", r);
    // utlDelay(500);
  }
   */
  iridSendFile("test\\test.log");
  iridLandResp(all.buf);
  if (strstr(all.buf, "cmds"))
    r = iridLandCmds(all.buf);
  strcpy(buff, all.buf);
  utlDelay(500);
  utlWrite(gpsd.port, "done", "");
  utlDelay(500);
  iridHup();
  iridSig();
  flogf("\n%s\n", utlTime());
  flogf("\nsetting '%s'", utlNonPrint(buff));
  cfgString(buff);
  flogf("\nsys.program = %s", sys.program);
  // antSwitch(gps_ant);
  // gpsStats();
  gpsStop();
  antStop();
}
