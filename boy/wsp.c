// wsp.c 
#include <main.h>

#define EOL "\r"

WspInfo wsp;

///
// wspInit selects no card, but all off
// wspStart powers wsp.card and selects serial

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

///
// select card, turn on
// uses: .card
// sets: wsp.on 
// rets: 0=success >=nextCard
int wspStart(void) {
  static char *self="wspStart";
  static char *rets="1=!wsp.port 2=!<wispr> 3=!date";
  DBG();
  if (wsp.port) TURxFlush(wsp.port); // flush
  else raise(1);
  if (wsp.on || wsp.log) wspStop();
  DBG1("\n%s: activating wispr#%d", self, wsp.card);
  mpcPamPwr(wsp.card, true);
  wsp.on = true;
  if (wsp.logging) 
    utlLogOpen(&wsp.log, "wsp"); 
  if (utlGetTagSecs(wsp.port, all.str, "<wispr>", 20))
    raise(2);
  if (wspDateTime()) 
    raise(3);
  return 0;
} // wspStart

///
// stop wsp.card
// sets: wsp.on 
int wspStop(void) {
  static char *self="wspStop";
  static char *rets="1=!logclose 2=!<wispr>";
  int r;
  DBG();
  if (wsp.log) 
    utlLogClose(&wsp.log);
  // should be one last open/close pair
  if (!wspOpen())
    // blank line, continues
    utlWrite( wsp.port, "", EOL );
  wspClose();
  r = utlGetTagSecs(wsp.port, all.str, "</wispr>", 10);
  utlNap(1);
  wsp.on = false;
  mpcPamPwr(wsp.card, false);
  if (r) raise(2);
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
// wspr is giving us xml style <wspr> <cmd> </cmd> </wspr>
int wspOpen(void) {
  static char *self="wspOpen";
  static char *rets="1=off 2=!<cmd>";
  DBG();
  if (!wsp.on) raise(1);
  if (utlGetTagSecs(wsp.port, all.str, "<cmd>", 12)) 
    raise(2);
  return 0;
} // wspOpen

///
// wspr is giving us xml style <cmd> </cmd>
int wspClose(void) {
  static char *self="wspClose";
  static char *rets="1=off 2=!</cmd>";
  DBG();
  if (!wsp.on) raise(1);
  if (utlGetTagSecs(wsp.port, all.str, "</cmd>", 12)) 
    raise(2);
  return 0;
} // wspClose

///
// assumes started. waits seconds for response before timeout
// sets: *out
int wspCmd(char *out, char *cmd, int seconds) {
  static char *self="wspCmd";
  static char *rets="1=!open 2=timeout";
  DBG();
  out[0]=0;
  if (wspOpen()) raise(1);
  flogf("\n%s: sending to wispr: %s \n", self, cmd);
  utlWrite( wsp.port, cmd, EOL );
  if (utlGetTagSecs( wsp.port, out, "</cmd>", seconds ))
    raise(2);
  flogf("\n%s\n", out);
  return 0;
} // wspCmd

///
// wsp storm check. interact. assumes already started.
// rets: 1=open 2=RDY 3=predict 8=close
int wspSpectr(char *buf) {
  static char *self="wspSpectr";
  static char *rets="1=open 2=RDY 3=predict 9=close";
  DBG();
  // cmd
  buf[0]=0;
  // start 
  if (wspOpen()) raisex(1);
  // wispr adds /mnt/ to log path
  sprintf( all.str, "%s %s %s -l log/%.5s%03d.log", 
      wsp.spectCmd, wsp.spectFlag, wsp.spectGain, wsp.spectLog, all.cycle );
  flogf( "\n%s: %s", self, all.str );
  utlWrite( wsp.port, all.str, EOL );
  // gather
  if (utlGetTagSecs(wsp.port, buf, "RDY", 200)) 
    raisex(2);
  utlWrite(wsp.port, "$WS?*", EOL);
  if (!utlReadWait(wsp.port, buf, 60)) 
    raisex(3);
  flogf("\n%s() prediction: %s", self, buf);
  // ?? add to daily, eng log
  // ?? parse - we need to decide rise or not
  if (wspClose()) raise(9);
  return 0;
  //
  except: {
    utlWrite(wsp.port, "$EXI*", EOL);
    wspClose();
    return(dbg.except);
  }
} // wspStorm

///
// writes to log file if it is open
int wspLog(char *str) {
  static char *self="wspLog";
  static char *rets="1=!open 2=!write";
  if (wsp.log) {
    sprintf(all.str, "%s\n", str); // ?? risky
    if (write(wsp.log, all.str, strlen(all.str))<1) 
      raise(2);
  } 
  return 0;
} // wspLog

///
// stub
int wspDetect(WspData *wspd, int minutes) {
  float f;
  int x;
  wspDetectM(&x, &f, minutes);
  wspd=null;
  return 0;
}

///
// run detection program for minutes, take a nap during
// if this fails, assume card is bad; does not call wspStop
// uses: .wisprCmd .wisprFlag .detInt .dutyM all.str 
// sets: *detectQ=detections
// rets: 1=start 2=lowDisk 3=FIN 8=close 10=query 20=space
int wspDetectM(int *detected, float *free, int minutes) {
  static char *self="wspDetectM";
  static char *rets="1=open 3=!FIN 8=close 9=stop 10=query 20=space";
  DBGN( "(%d)", minutes );
  if (wsp.diskFree>0 && wsp.diskFree<wsp.diskMin) raise(2);
  if (wspOpen()) raisex(1);
  // wispr adds /mnt/ to log path
  sprintf( all.str, "%s %s %s -l log/%.5s%03d.log", 
      wsp.wisprCmd, wsp.wisprFlag, wsp.wisprGain, wsp.wisprLog, all.cycle );
  if (wsp.wisprTest)
    sprintf( all.str+strlen(all.str), " -W" );
  flogf( "\n%s: %s", self, all.str );
  utlWrite( wsp.port, all.str, EOL );
  // run for minutes // power low 
  TUTxWaitCompletion( wsp.port );
  pwrNap(minutes*60);
  // detect, free queries 
  if (wspQuery(detected)) raisex(10);
  if (wspSpace(free)) raisex(20);
  wsp.diskFree = *free;
  // stop
  utlWrite(wsp.port, "$EXI*", EOL);
  if (utlGetTagSecs(wsp.port, all.str, "FIN", 5)) {
    flogf("\n%s(): expected FIN, got '%s'", self, all.str);
    raisex(3);
  }
  // stop
  if (wspClose()) raise(8);
  return 0;
  //
  except: {
    utlWrite(wsp.port, "$EXI*", EOL);
    wspClose();
    return(dbg.except);
  }
} // wspDetectM

///
// query detections from running wspr
// rets: 0=success 1=badData
int wspQuery(int *det) {
  static char *self="wspQuery";
  static char *rets="1=noResp 2=badResp";
  char *s, query[32];
  TURxFlush(wsp.port);
  *det = 0;
  sprintf(query, "$DX?,%d*", wsp.detMax);
  utlWrite(wsp.port, query, EOL);
  if (!utlReadWait(wsp.port, all.buf, 16))
    raise(1);
  wspLog(all.buf);
  DBG2("%s", all.buf);
  // total det
  s = strtok(all.buf, "$,");
  if (!strstr(s, "DXN")) 
    raise(2);
  s = strtok(NULL, ",");
  *det = atoi(s);
  DBG1("detected %d", *det);
  return 0;
} // wspQuery

///
// query disk space from running wspr
int wspSpace(float *free) {
  static char *self="wspSpace";
  static char *rets="1=noResp 2=badResp";
  char *s;
  *free = 0.0;
  utlWrite(wsp.port, "$DFP*", EOL);
  if (!utlReadWait(wsp.port, all.str, 60)) 
    raise(1);
  wspLog(all.str);
  DBG2("%s", all.str);
  s = strstr(all.str, "DFP");
  if (!s) raise(2);
  strtok(s, ",");
  s = strtok(NULL, "*");
  *free = atof(s);
  DBG1("freespace %4.2f", *free);
  return 0;
} // wspSpace

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


