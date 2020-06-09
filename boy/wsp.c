// wsp.c 
#include <main.h>

#define EOL "\r"

WspInfo wsp;

///
// run wspr until we pass the target time .riseT
// algor: set riseT, wspH, (storm check), check time v riseT
// wspr runs for a clock hour (or part of) between checks
// pass gain, gpstime at power up, start on command line
//
// wspInit selects no card, but all off
// wspStart powers wsp.card

///
// reset, all pams off
// sets: wsp.port .wspPending
void wspInit(void) {
  static char *self="wspInit";
  DBG();
  wsp.port = mpcPamPort();
  mpcPamPwr(wsp1_pam, false);
  mpcPamPwr(wsp2_pam, false);
  mpcPamPwr(wsp3_pam, false);
  mpcPamDev(wsp.card);
  wsp.on = false;
} // wspInit

#define WSP_START_OPEN "<wispr>"
#define WSP_START_SEC 20
///
// select card, turn on
// uses: .card
// sets: wsp.on 
// rets: 0=success >=nextCard
int wspStart(void) {
  static char *self="wspStart";
  static char *rets="1=!log 2=!<wispr> 3=!date";
  DBG();
  if (wsp.on) wspStop();
  DBG1("\n%s: activating wispr#%d", self, wsp.card);
  mpcPamPwr(wsp.card, true);
  wsp.on = true;
  if (!wsp.log)
    if (utlLogOpen(&wsp.log, "wsp")) raise(1);
  if (!utlReadExpect(wsp.port, all.buf, WSP_START_OPEN, WSP_START_SEC))
    raise(2);
  if (wspDateTime()) raise(3);
  return 0;
} // wspStart

#define WSP_START_CLOSE "</wispr>"
///
// stop wsp.card
int wspStop(void) {
  int r=0;
  static char *self="wspStop";
  DBG();
  wsp.on = false;
  if (wsp.log) 
    utlLogClose(&wsp.log);
  mpcPamPwr(wsp.card, false);
  return 0;
} // wspStop

///
// set date time on wispr
int wspDateTime(void) {
  static char *self="wspDateTime";
  static char *rets="1=off 2=!open 3=!close";
  DBG();
  if (!wsp.on) raise(1);
  if (wspOpen()) raise(2);
  sprintf(all.str, "date -s '%s'; hwclock -w", utlDateTime());
  utlWrite(wsp.port, all.str, EOL);
  if (wspClose()) raise(3);
  return 0;
}

///
// wspr is giving us xml style <cmd> </cmd>
#define WSP_CMD_OPEN "<cmd>"
#define WSP_CMD_SEC 20
int wspOpen(void) {
  int r=0;
  static char *self="wspOpen";
  static char *rets="1=off 2=!<cmd>";
  DBG();
  if (!wsp.on) raise(1);
  if (!utlReadExpect(wsp.port, all.buf, WSP_CMD_OPEN, WSP_CMD_SEC)) 
    raise(2);
  return 0;
} // wspOpen

///
// close wispr /mnt/start
// wspr is giving us xml style <cmd> </cmd>
#define WSP_CMD_CLOSE "</cmd>"
int wspClose(void) {
  int r=0;
  static char *self="wspClose";
  static char *rets="1=off 2=!</wispr>";
  DBG();
  if (!wsp.on) raise(1);
  if (!utlReadExpect(wsp.port, all.buf, WSP_CMD_CLOSE, WSP_CMD_SEC)) 
    raise(2);
  return 0;
} // wspClose

///
// assumes started.
int wspCmd(char *out, char *cmd) {
  static char *self="wspCmd";
  static char *rets="1=!open 2=WSP_CMD_SEC timeout";
  char *r;
  DBG();
  out[0]=0;
  if (wspOpen()) raise(1);
  cprintf("sending to wispr: %s \n", cmd);
  utlWrite( wsp.port, cmd, EOL );
  r = utlReadExpect( wsp.port, all.buf, WSP_CMD_CLOSE, WSP_CMD_SEC );
  if (r) 
    flogf("\n%s\n", all.buf);
  else
    raise(2);
  return 0;
} // wspCmd

///
// wsp storm check. interact. assumes already started.
// rets: 1=open 2=RDY 3=predict 8=close
int wspStorm(char *buf) {
  static char *self="wspStorm";
  static char *rets="1=open 2=RDY 3=predict 9=close";
  DBG();
  // cmd
  buf[0]=0;
  sprintf( all.str, "%s %s %s -l %.5s%03d.log", 
      wsp.spectCmd, wsp.spectFlag, wsp.spectGain, wsp.spectLog, all.cycle );
  // start 
  if (wspOpen()) raise(1);
  flogf( "\nexec '%s'", all.str );
  utlWrite( wsp.port, all.str, EOL );
  // gather
  if (!utlReadExpect(wsp.port, buf, "RDY", 200)) raise(2);
  utlWrite(wsp.port, "$WS?*", EOL);
  if (utlReadWait(wsp.port, buf, 60)) raise(3);
  flogf("\nwspStorm prediction: %s", buf);
  // ?? add to daily, eng log
  // ?? parse - we need to decide rise or not
  if (wspClose()) raise(9);
  return 0;
} // wspStorm

///
// if wsp.log else flog
int wspLog(char *str) {
  static char *self="wspLog";
  static char *rets="1=!open 2=!write";
  int r=0;
  if (wsp.log) {
    sprintf(all.str, "%s\n", str); // ?? risky
    if (write(wsp.log, all.str, strlen(all.str))<1) return 1;
  } else {
    flogf("\nwspLog(%s)", str);
  }
  return r;
} // wspLog

///
// stub
int wspDetect(WspData *wspd, int minutes) {
  int x;
  wspDetectM(&x, minutes);
  wspd=null;
  return 0;
}

///
// run detection program for minutes, take a nap during
// if this fails, assume card is bad; does not call wspStop
// uses: .wisprCmd .wisprFlag .detInt .dutyM all.str 
// sets: *detectM+=
// rets: 1=start 3=FIN 8=close 9=stop 10=query 20=space
int wspDetectM(int *detectM, int minutes) {
  static char *self="wspDetectM";
  static char *rets="1=start 3=FIN 8=close 9=stop 10=query 20=space";
  int r=0, detQ=0;
  DBGN( "(%d)", minutes );
  // cmd
  sprintf( all.str, "%s %s %s -l %.5s%03d.log", 
      wsp.wisprCmd, wsp.wisprFlag, wsp.wisprGain, wsp.wisprLog, all.cycle );
  if (wsp.wisprTest)
    sprintf( all.str+strlen(all.str), " -W" );
  // start
  if (wspOpen()) raisex(1);
  flogf( "\nexec '%s'", all.str );
  utlWrite( wsp.port, all.str, EOL );
  // run for minutes // query at end 
  pwrSleep(minutes*60);
  if (wspQuery(&detQ)) raisex(10);
  *detectM += detQ;
  // ?? query disk free
  // stop
  utlWrite(wsp.port, "$EXI*", EOL);
  if (!utlReadExpect(wsp.port, all.buf, "FIN", 5)) {
    flogf("\n%s(): expected FIN, got '%s'", self, all.buf);
    raisex(3);
  }
  // ?? add to daily log
  // stop
  if (wspClose()) raisex(8);
  return 0;
  //
  except: {
    wspClose();
    return(dbg.except);
  }
} // wspDetectM

///
// query detections from running wspr
// rets: 0=success 1=badData
int wspQuery(int *det) {
  char *s, query[32];
  TURxFlush(wsp.port);
  *det = 0;
  sprintf(query, "$DX?,%d*", wsp.detMax);
  utlWrite(wsp.port, query, EOL);
  utlReadWait(wsp.port, all.buf, 16);
  wspLog(all.buf);
  // total det
  s = strtok(all.buf, "$,");
  if (!strstr(s, "DXN")) {
    flogf("\n%s: ERR bad detections response");
    return 1;
  }
  s = strtok(NULL, ",");
  *det = atoi(s);
  DBG1("detected %d", det);
  return 0;
} // wspQuery

///
// query disk space from running wspr
int wspSpace(float *free) {
  char *s;
  *free = 0.0;
  utlWrite(wsp.port, "$DFP*", EOL);
  if (utlReadWait(wsp.port, all.buf, 2)) return 2;
  DBG2("%s", all.buf);
  s = strstr(all.buf, "DFP");
  if (!s) return 1;
  strtok(s, ",");
  s = strtok(NULL, "*");
  *free = atof(s);
  return 0;
} // wspChkCF

///
// change cards if we have another one
// rets: 0=success >=fail
int wspCardSwap(void) {
  if (wsp.card<=1) {
    wsp.card=0;
    utlErr(wsp_err, "No more wispr cards");
    return 1;
  } else {
    wsp.card-=1;
    flogf( "\nwspCardSwap() -> %d", wsp.card );
    return 0;
  }
} // wspCardSwap

/*
 * wispr doc <winchProj\LARASystem5.doc>
 *
One single program can be operated in the following 5 different modes.
*Modes
- Mode 1: Record continuously, no detection functionality
- Mode 2: Record continuously with detection functionality
- Mode 3: Record intermittently. 
  Skip specified number of buffers between files. 
- Mode 4: Run beak whale detection function only. 
  Process incoming data continuously with the detection function
  but only write a file when a detection appeared.
- Mode 5: Run beak whale detection function and record data intermittently.
  Same idea as mode 4 + record data intermittently. 
  How many encounters were missed or to monitor noise levels regularly.
**-r	Send interrupt to ext processor to receive GPS time, lat/lon
  and gain during the first 17-second start up, and wait for 5 sec. 
  Default is to receive the GPS time, lat/lon and gain without interrupt. 
  Send % of free space of CF card 40 sec later  (57 sec after start up).

***Simulated test detection mode (bench test only)
  Test the detection performance by overwriting AD buffer content with 
  a short wave file of a specfic species clicks/calls before processing 
  the buffer.  File name has to be bw_test.wav.  
  Detection results are stored in detections.dtx.

commands end with \r
   $GPS ,%ld, %8.3f,%7.3f*	GPS time, long and lat    	CF2->WISPR
   $DX?,%ld,%ld*            Inq detections            	CF2->WISPR
   $DXN,%d* 			    Num of detections         	WISPR->CF2
   $ACK* 			        Send ACK for each line      WISPR->CF2
   $NGN,%d* 			    New gain (0-3)            	CF2->WISPR
   $EXI* 			        End logging              	CF2->WISPR
   $DET,%d* 			    Detection parameter	        WISPR->CF2
   $DFP* 			        Inq disk space   		    CF2->WISPR
   $DFP,%5.2f*              Reply disk space avail %    WISPR->CF2

The followings are descriptions of the wispr command line parameters.  
Options:           	DESCRIPTION                                	DEFAULT
-M {mode}   	Processing mode number                     	[ 1 ]*
-T {secs}   	Size of ADC data buffers in seconds     	[ up to 8 secs ]
-F {level}      Sets flac compression level                	[ 2, 3]
-s {bitshift}   Sets data bitshift                         	[ 8 ]
-o {org}        Organization in FLAC metadata string       	[ none ]
-b {number}     Number of data buffers per file           	[ 10 ]
-n {nclick}     Min number of click for detection         	[ 10 ]
-q {nbufs}      data snipet option (not implemented yet)   	[ ? ]
-i {number}     Number buffers to skip between file        	[ 10 ]
-p {prefix}     Data file name prefix                      	[ wispr_ ]
-l {filename}   Log file name                              	[ no log file ]
-v {level}      Verbose level (0=none)                     	[ 0 ]
-L              Enable LEDs                                	[ disabled ]
-r              Request GPS time, lat/lon and gain at start	[ disabled ] 
	wait GPS signal for 17 sec, for free disk space 57 sec
-W              Simulated test detection mode              	[ disabled ] 
-f	        sampling frequency 62500, 93750, 125000	        [ 125000 ]
	If wispr_kw (Killer whale) 93750 is default
-g	        gain 0, 1, 2, 3 (additonal gain with 6dB incr)  [0]
-h	        print help
-C	        number of files to record
x	        cpu usage time


#/bin/wispr_bw -T8 -b15 -M2 -F2 -l wispr.log -v2 -r -g0  
#/bin/wispr_bw -T8 -b15 -M2 -F2 -v2 -g0  
#/bin/wispr_bw -l wispr.log -v2 -T8 -b15 -M2 -F2 -g0 -r
#/bin/wispr_bw -l wispr.log -v2 -T8 -b15 -M2 -F2 -g0 -r -W

/bin/wispr_bw -l wispr.log -v2 -T8 -b15 -M2 -F2 -g0 -r
/bin/wispr_bw -l wispr.log -v2 -T8 -b15 -M2 -F2 -W
# remount and run spector
mount /dev/sda1 /mnt
/bin/spectrogram -T10 -C12 -v1 -n512 -o256 -g0 -t20 -l noise.log
72/02/16 09:49:30 Wait for 20 sec CF2 inquiry $WS?*

 *
 */


