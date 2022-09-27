// wsp.c 
#include <utl.h>
#include <pwr.h>
#include <boy.h>
#include <wsp.h>
#include <mpc.h>
#include <tmr.h>

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
  DBG()
  wsp.port = mpcPamPort();
  mpcPamPwr(wsp1_pam, false);
  mpcPamPwr(wsp2_pam, false);
  mpcPamPwr(wsp3_pam, false);
  mpcPamDev(wsp.card);
  wsp.on = false;
} // wspInit

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

///
// select card, turn on
// uses: .card
// sets: wsp.on 
// rets: 0=success >=nextCard
int wspStart(void) {
  int r=0;
  static char *self="wspStart";
  DBG()
  if (wsp.on) wspStop();
  DBG1("\n%s: activating wispr#%d", self, wsp.card)
  mpcPamPwr(wsp.card, true);
  wsp.on = true;
  if (!wsp.log && strlen(wsp.logFile))
    utlLogFile(&wsp.log, wsp.logFile);
  return r;
} // wspStart

///
// stop wsp.card
int wspStop(void) {
  int r=0;
  static char *self="wspStop";
  DBG()
  wsp.on = false;
  if (wsp.log) {
    // close log
    if (close(wsp.log)) 
      utlCloseErr( "wsp.log" );
    wsp.log = 0;
  }
  mpcPamPwr(wsp.card, false);
  return 0;
} // wspStop

///
// turn on and open wispr /mnt/start
int wspOpen(void) {
  int r=0;
  static char *self="wspOpen";
  DBG()
  if (!wsp.on) wspStart();
  if (!utlExpect(wsp.port, all.buf, WSP_OPEN, 20)) {
    utlErr(wsp_err, "wsp start fail");
    r=1;
  }
  return r;
} // wspOpen

///
// close wispr /mnt/start
int wspClose(void) {
  int r=0;
  static char *self="wspClose";
  DBG()
  if (!utlExpect(wsp.port, all.buf, WSP_CLOSE, 12)) {
    flogf("\n%s: did not get %s ", self, WSP_CLOSE);
    r=1;
  }
  wspStop();
  return r;
} // wspClose

///
// calculate rise time
// sets: *riseT = next iridHour, or iridFreq hours interval from midnight
void wspRiseT(time_t *riseT, int iridHour, int iridFreq) {
  int hour;
  time_t r, now;
  struct tm *tmPtr, tmLocal;
  static char *self="wspRiseT";
  DBG()
  // get time, break it down
  time(&now);
  tmPtr = gmtime(&now);
  memcpy(&tmLocal, tmPtr, sizeof(struct tm));
  // figure target hour
  if (iridFreq) {
    // instead of a fixed rise hour, figure next interval
    hour = ((int)(tmLocal.tm_hour/iridFreq)+1) * iridFreq;
  } else {
    hour=iridHour;
  }
  // check the hour, is it past? add one interval
  tmLocal.tm_hour = hour;
  tmLocal.tm_min = 0;
  tmLocal.tm_sec = 0;
  r = mktime(&tmLocal);
  if (r<now) {
    // next day/interval 
    DBG1("r<now");
    // be careful with long math and big ints, do not (time_t) (24*60*60)
    if (iridFreq) {
      r = r + (time_t) iridFreq*60*60;
    } else {
      r = r + (time_t) 24*60*60;
    }
  }
  flogf("\nwspRiseT(): rise at %s", utlDateTimeFmt(r));
  *riseT = r;
  return;
} // wspRiseT

///
// wsp storm check started. interact.
// rets: 1=start 2=RDY 3=spectr 9=stop
int wspStorm(char *buf) {
  static char *rets="1=open 2=RDY 3=predict 8=close";
  char *b;
  static char *self="wspStorm";
  DBG()
  // cmd
  buf[0]=0;
  b=all.str;
  sprintf( b, "%s %s", wsp.spectCmd, wsp.spectFlag );
  if (wsp.spectGain)
    sprintf( b+strlen(b), " -g%d", wsp.spectGain );
  if (wsp.spectLog)
    sprintf( b+strlen(b), " -l %.5s%03d.log", wsp.spectLog, all.cycle );
  // start 
  if (wspOpen()) Exc(1);
  flogf( "\nexec '%s'", b );
  utlWrite( wsp.port, b, EOL );
  // gather
  if (!utlExpect(wsp.port, buf, "RDY", 200)) Exc(2);
  utlWrite(wsp.port, "$WS?*", EOL);
  if (!utlReadWait(wsp.port, buf, 60)) Exc(3);
  flogf("\nwspStorm prediction: %s", buf);
  // ?? add to daily
  // stop
  if (wspClose()) return 9;
  return 0;

  Except
    return(all.exc);
} // wspStorm

///
// if wsp.log else flog
int wspLog(char *str) {
  int r=0;
  if (wsp.log) {
    sprintf(all.str, "%s\n", str);
    if (write(wsp.log, all.str, strlen(all.str))<1) return 1;
  } else {
    flogf("\nwspLog(%s)", str);
  }
  return r;
} // wspLog

///
// set date time on wispr
int wspDateTime(void) {
  static char *self="wspDateTime";
  DBG()
  if (wspOpen()) return 1;
  sprintf(all.str, "date; date -s '%s'; hwclock -w", utlDateTime());
  utlWrite(wsp.port, all.str, EOL);
  if (wspClose()) return 9;
  return 0;
}

///
// run detection program, called from wspDetectH
// if this fails, assume card is bad
// uses: .wisprCmd .wisprFlag .detInt .dutyM all.str 
// sets: *detectM+=
// rets: 1=start 9=stop 10=!wspQuery 20=!wspClose
int wspDetectM(int *detectM, int minutes) {
  static char *rets="1=start 3=FIN 8=close 9=stop 10=query 20=space";
  char *b;
  int r=0, detQ=0;
  float free;
  static char *self="wspDetectM";
  DBGN( "(%d)", minutes )
  // cmd
  b=all.str;
  sprintf( b, "%s %s", wsp.wisprCmd, wsp.wisprFlag );
  if (wsp.wisprGain)
    sprintf( b+strlen(b), " -g%d", wsp.wisprGain );
  if (wsp.wisprLog)
    sprintf( b+strlen(b), " -l %.5s%03d.log", wsp.wisprLog, all.cycle );
  // start
  if (wspOpen()) Exc(1);
  flogf( "\nexec '%s'", b );
  utlWrite( wsp.port, b, EOL );
  // run for minutes; every .detInt, query and reset.
  // query also at end of minutes
  tmrStart(data_tmr, wsp.detInt*60);
  tmrStart(minute_tmr, minutes*60);
  while (!tmrExp(minute_tmr)) {
    if (tmrExp(data_tmr)) {
      // query and reset
      if (wspQuery(&detQ)) Exc(10);
      *detectM += detQ;
      if (wspSpace(&free)) Exc(20);
      flogf("\n%s: disk %3.0f%% free on wispr#%d", self, free, wsp.card);
      tmrStart(data_tmr, wsp.detInt*60);
    } // data_tmr
  } // minute_tmr
  if (wspQuery(&detQ)) Exc(10);
  *detectM += detQ;
  // ?? query diskFree
  // stop
  utlWrite(wsp.port, "$EXI*", EOL);
  if (!utlExpect(wsp.port, all.buf, "FIN", 5)) {
    flogf("\n%s(): expected FIN, got '%s'", self, all.buf);
    Exc(3);
  }
  // ?? add to daily log
  // stop
  if (wspClose()) return 8;
  return 0;

  Except
    wspStop();
    return(all.exc);
} // wspDetectM

///
// run detection for .dutyM minutes at start of hour
// run spectra if .spectRun > 0
// idle if remains<.dutyM*60
// watchdog - hour timer
// uses: .dutyM
// sets: *detectH=
// rets: 1=watchdog 10=!wspDetectM 
int wspDetectH(int *detectH, char *spectr) {
  int r=0, remains, detM=0;
  static char *self="wspDetectH";
  DBG()
  tmrStart(hour_tmr, 60*60+60);   // hour and a minute watchdog
  // enough time?
  wspRemainS(&remains);
  if (remains > wsp.dutyM*60) {
    flogf("\n%s: running WISPR#%d for %d minutes", self, wsp.card, wsp.dutyM);
    r = wspDetectM(&detM, wsp.dutyM);
  } else 
    flogf("\n%s: skipping detection, dutyM%d > remains%d", 
        self, wsp.dutyM, remains);
  *detectH=detM;
  if (r) { // error
    sprintf( all.str, " !! error %d", r );
    flogf( all.str );
    wspLog( all.str );
    return 10;
  }
  if (wsp.spectRun==2) {            // 2==hourly
    wspStorm(spectr);
    wspLog(spectr);
  }
  // ?? log
  if (tmrExp(hour_tmr)) return 1;   // watchdog
  wspRemainS(&remains);
  flogf("\nwspDetectH() idle for %d minutes", remains/60);
  utlNap(remains);
  return 0;
} // wspDetectH

///
// set riseT, run detectH until witching hour
// uses: .spectRun 
// sets: *detect+=
// rets: 0=success ?? 1=WatchDog 11=hour.WD 12=hour.startFail 13=hour.minimum
int wspDetectD(int *detect, char *spectr, int iridHour, int iridFreq) {
  static char *self="wspDetectD";
  float laterH;
  int detH=0, r=0;
  time_t now, riseT;
  DBG()
  flogf("\n%s: setting wispr date/time", self);
  wspDateTime();
  *detect = 0;
  time(&now);
  wspRiseT(&riseT, iridHour, iridFreq);
  laterH = (float)(riseT-now)/60/60;
  flogf("\n  starting wispr detection; end in %3.1f hours", laterH);
  while (time(NULL) < riseT) {
    if (wspDetectH(&detH, spectr)) r+=10;
    *detect += detH;
    flogf("\n%s: %d detections @%s", self, *detect, utlDateTime());
  } // while < riseT
  if (wsp.spectRun==1) 
    wspStorm(all.buf);
  // ?? log
  return r;
} // wspDetectD

///
// seconds remaining in this hour
void wspRemainS(int *remains) {
  struct tm *tim;
  time_t now;
  time(&now);
  tim = gmtime(&now);
  *remains = ((60-tim->tm_min)*60) - tim->tm_sec;
  DBG1("wspRemainS():%ds", *remains)
} // wspRemainS

///
// query detections
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
  DBG1("detected %d", det)
  return 0;
} // wspQuery

///
// wispr detection program, query disk space
int wspSpace(float *free) {
  char *s;
  *free = 0.0;
  utlWrite(wsp.port, "$DFP*", EOL);
  if (!utlReadWait(wsp.port, all.buf, 2)) return 2;
  DBG2("%s", all.buf)
  s = strstr(all.buf, "DFP");
  if (!s) return 1;
  strtok(s, ",");
  s = strtok(NULL, "*");
  *free = atof(s);
  return 0;
} // wspChkCF

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

   $GPS ,%ld, %8.3f,%7.3f*	Cr  GPS time, long and lat    	CF2->WISPR
   $DX?,%ld,%ld*                Cr  Inq detections            	CF2->WISPR
   $DXN ,%d* 			Cr  Num of detections         	WISPR->CF2
   $ACK* 			Cr  Send ACK for each line      WISPR->CF2
   $NGN,%d* 			Cr  New gain (0-3)            	CF2->WISPR
   $EXI* 			Cr  End logging              	CF2->WISPR
   $DET,%d* 			Cr  Detection parameter	        WISPR->CF2
   $DFP* 			Cr  Inq disk space   		CF2->WISPR
   $DFP,%5.2f*                  Cr  Reply disk space avail %    WISPR->CF2

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


