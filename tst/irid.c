// irid.c
#include <main.h>

extern GpsInfo gps;
extern BoyInfo boy;
extern SysInfo sys;

void main(void){
  // Serial port;
  // char c;
  char *buff;
  int l, len, cnt;
  int i, r;
  sysInit();
  mpcInit();
  antInit();
  gpsInit();
  //
  antStart();
  gpsStart();
  //
  len = tst.t2;
  cnt = tst.t1;
  cprintf("\nlength tst.t2=%d, count tst.t1=%d ", len, cnt);
  cprintf("\nbaud gps.rudBaud=%d", gps.rudBaud);
  buff = malloc(len);
  // antSwitch(gps_ant);
  // gpsStats();
  antSwitch(irid_ant);
  if (iridSig()) return;
  if (iridDial()) return;
  if (iridProjHdr()) return;
  for (i=1; i<=cnt; i++) {
    memset(buff, 0, len);
    sprintf(buff, "%d of %d =%d @%d [%d]", 
      i, cnt, len, gps.rudBaud, gps.sendSz);
    buff[len-1] = 'Z';
    r = iridSendBlock(buff, len, i, cnt);
    cprintf("(%d)\n", r);
    // utlDelay(500);
  }
  iridLandResp(all.buf);
  if (strstr(all.buf, "cmds"))
    r = iridLandCmds(all.buf, &l);
  all.buf[l] = 0;
  strcpy(buff, all.buf);
  utlDelay(500);
  utlWrite(gps.port, "done", "");
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
