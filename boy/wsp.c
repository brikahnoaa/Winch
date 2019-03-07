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
// algor: set riseT, wspHour, (storm check), check time v riseT
// wspr runs for a clock hour (or part of) between checks
// pass gain, gpstime at power up, start on command line
//
// wspInit selects one card, but all off
// wspStart powers wsp.cardUse

///
// sets: wsp.port .wspPending
void wspInit(void) {
  DBG0("wspInit()")
  wsp.port = mpcPamPort();
  PIOClear(PAM_12);
  PIOClear(PAM_34);
  tmrStop(wsp_tmr);
  mpcPamDev(wsp1_pam);
  mpcPamPulse(WISPR_PWR_OFF);
  mpcPamDev(wsp2_pam);
  mpcPamPulse(WISPR_PWR_OFF);
  mpcPamDev(wsp3_pam);
  mpcPamPulse(WISPR_PWR_OFF);
  mpcPamDev(null_pam);
  wsp.on = false;
} // wspInit

///
// turn on, disk free > wsp.freeMin
// ?? needs more checks
// ?? increment to next card if data full
// sets: wsp.on .card
// rets: 0=success >=nextCard
int wspStart(int card) {
  DBG0("wspStart()")
  if (strlen(wsp.logFile))
    wsp.log = utlLogFile(wsp.logFile);
  // select, power on
  mpcPamDev(card);
  mpcPamPulse(WISPR_PWR_ON);
  // expect df output
  // utlExpect(wsp.port, utlBuf, "/mnt", 40);
  // ?? check free disk, maybe increment card
  // flogf("\n%s\n", utlBuf);
  wsp.on = true;
  return 0;
} // wspStart

///
// stop wsp.cardUse
int wspStop(void) {
  int i, r=1;
  // try for graceful shutdown, 3x
  if (wsp.on) {
    for (i=0;i<3;i++) {
      utlWrite(wsp.port, "$EXI*", EOL);
      if (utlExpect(wsp.port, utlBuf, "FIN", 5)) {
        // success
        r=0;
        break;
      }
    }
    if (wsp.storm) {
      wspStorm(utlBuf);
      wspLog(utlBuf);
    }
    if (!utlExpect(wsp.port, utlBuf, "done", 10))
      flogf("\nwspStop\t| err: expected 'done', not seen...");
    wsp.on = false;
  } // wisp.on
  mpcPamPulse(WISPR_PWR_OFF);
  if (wsp.log) {
    // close log
    if (close(wsp.log)) utlCloseErr( "wsp.log" );
    wsp.log = 0;
  }
  return r;
} // wspStop

///
// send gps, gain to wispr
int wspSetup(char *gps, int gain) {
// ??
  if (*gps) return gain;
  return 0;
} // wspSetup

///
// calculate rise time
// rets: today at riseHour, or if past add 24hrs
time_t wspRiseT(int riseHour) {
  time_t r, now;
  struct tm *tmPtr, tmLoc;
  // get time, break it down
  time(&now);
  tmPtr = gmtime(&now);
  memcpy(&tmLoc, tmPtr, sizeof(struct tm));
  // check the hour, is it past?
  tmLoc.tm_hour = riseHour;
  r = mktime(&tmLoc);
  if (r<now) {
    // next day same hour
    r += 24*60*60;
  }
  return r;
} // wspRiseT

///
// wsp storm check started. interact.
int wspStorm(char *buf) {
  DBG0("wspStorm()")
  if (!utlExpect(wsp.port, buf, "RDY", 200)) return 1;
  utlWrite(wsp.port, "$WS?*", EOL);
  utlReadWait(wsp.port, buf, 10);
  flogf("\nwspStorm prediction: %s", buf);
  return 0;
} // wspStorm

///
// ?? replace with log to wsp.log
void wspLog(char *str) {
  if (wsp.log) {
    sprintf(utlStr, "%s\n", str);
    write(wsp.log, utlStr, strlen(utlStr));
  } else {
    flogf("\nwspLog(%s)", str);
  }
} // wspLog

///
// test routine, run detection for minutes
int wspDetectMin(int minutes, int *detect) {
  int r;
  tmrStart(minute_tmr, minutes*60);
  while (!tmrExp(minute_tmr)) {}
  r = wspQuery(detect);
  wspStop();
  return r;
} // wspDetectMin

///
// run detection for each hour until witching hour
// rets: 0=success 1=WatchDog 11=hour.WD 12=hour.startFail 13=hour.minimum
int wspDetectDay(int *detections) {
  struct tm *tim;
  time_t secs;
  int i, r, currHr, doneHr, waitHr;
  DBG0("wspDetectDay()")
  doneHr = wsp.hour;
  // tim->tm_hour tim->tm_min tim->tm_sec
  time(&secs);
  tim = gmtime(&secs);
  currHr = tim->tm_hour;
  // wrap around math, 24hr diff
  waitHr = (24+doneHr-currHr)%24;
  flogf("\n starting wispr detection; end in hour %d", doneHr);
  flogf(", %d hours to run", waitHr);
  utlLogTime();
  tmrStart(day_tmr, waitHr*60*60);
  for (i=0;i<waitHr;i++) {
    r = wspDetectHour(detections);
    if (r) return 10+r;
    // watchdog
    if (tmrExp(day_tmr)) return 1;
  }
  return 0;
} // wspDetectDay

///
// run detection for wsp.duty minutes at end of hour
// rets: 0=success 1=watchdog 2=startFail 3=minimum
int wspDetectHour(int *detections) {
  struct tm *tim;
  time_t secs;
  int min, hour, remains, rest, duty, detect;
  DBG0("wspDetectHour()")
  min = 60;   // when not testing, min=60
  duty = wsp.duty; 
  tmrStart(hour_tmr, 60*60+60);   // hour and a minute
  // tim->tm_hour tim->tm_min tim->tm_sec
  time(&secs);
  tim = gmtime(&secs);
  hour = tim->tm_hour;
  remains = 60 - tim->tm_min;
  // need at least wsp.minimum to start/stop
  if (remains<=wsp.minimum) {
    utlNap(remains*min);
    return 0;
  }
  // rest first
  if (remains>duty) {
    rest = remains-duty;
    flogf("\nwispr:\t| rest for %d min", rest);
    utlLogTime();
    utlNap(rest*min);
  } else {
    // no time for full duty
    duty = remains;
  }
  // duty calls
  flogf("\nwispr:\t| run for %d min", duty);
  utlLogTime();
  // ?? not handled right on return
  if (wspStart(wsp.cardUse)) return 2;
  while (duty>0) {
    if (duty>wsp.detInt) 
      utlNap(wsp.detInt*min);
    else
      utlNap(duty*min);
    duty -= wsp.detInt;
    wspQuery(&detect);
    flogf(" [%d]", detect);
    *detections += detect;
    // watchdog
    if (tmrExp(hour_tmr)) break;
  } // duty
  wspStop();
  // wait until we are on to next hour
  while (hour==tim->tm_hour) {
    time(&secs);
    tim = gmtime(&secs);
    utlNap(5);
    // watchdog
    if (tmrExp(hour_tmr)) break;
  }
  // need to capture stats ??
  return 0;
} // wspDetectHour

///
// query detections
// rets: 0=success 1=badData
int wspQuery(int *det) {
  char *s, query[32];
  TURxFlush(wsp.port);
  *det = 0;
  sprintf(query, "$DX?,%d*", wsp.detMax);
  utlWrite(wsp.port, query, EOL);
  utlReadWait(wsp.port, utlBuf, 16);
  wspLog(utlBuf);
  // total det
  s = strtok(utlBuf, "$,");
  if (!strstr(s, "DXN")) return 1;
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
  if (!utlReadWait(wsp.port, utlBuf, 2)) return 2;
  DBG2("%s", utlBuf)
  s = strstr(utlBuf, "DFP");
  if (!s) return 1;
  strtok(s, ",");
  s = strtok(NULL, "*");
  *free = atof(s);
  return 0;
} // wspChkCF

/*
 * wispr doc <winchProj\LARASystem5.doc>
 *
   $GPS ,%ld, %8.3f,%7.3f*	Cr  GPS time, long and lat    	CF2->WISPR
   $DX?,%ld,%ld*                	Cr  Inq detections            	CF2->WISPR
   $DXN ,%d* 			Cr  Num of detections         	WISPR->CF2
   $ACK* 			Cr  Send ACK for each line    WISPR->CF2
   $NGN,%d* 			Cr  New gain (0-3)            	CF2->WISPR
   $EXI* 			Cr  End logging              	CF2->WISPR
   $DET,%d* 			Cr  Detection parameter	WISPR->CF2
   $DFP* 			Cr  Inq disk space   		CF2->WISPR
   $DFP,%5.2f* 		Cr  Reply disk space avail %  WISPR->CF2

The followings are descriptions of the wispr command line parameters.  
Options:           	DESCRIPTION                                	DEFAULT
-M {mode}   	Processing mode number                     	 [ 1 ]*
-T {secs}   	Size of ADC data buffers in seconds     	[up to 8 seconds]
-F {level}        	Sets flac compression level                	[ 2, 3]
-s {bitshift}     	Sets data bitshift                         	[ 8 ]
-o {organization} 	Organization in FLAC metadata string       	[ none ]
-b {number}       	Number of data buffers per file           	[10 ]
-n {nclick}       	Min number of click for detection         	[ 10 ]
-q {nbufs}        	data snipet option (not implemented yet)   	[ ? ]
-i {number}       	Number buffers to skip between file        	[ 10 ]
-p {prefix}       	Data file name prefix                      	[ wispr_ ]
-l {filename}     	Log file name                              	[no log file ]
-v {level}        	Verbose level (0=none)                     	[ 0 ]
-L                	Enable LEDs                                	[ disabled ]
-r                	Request GPS time, lat/lon and gain at start	[ disabled ] 
	wait GPS signal for 17 sec, for free disk space 57 sec
-W                	Simulated test detection mode              	[ disabled ] 
-f	sampling frequency 62500, 93750, 125000	 [125000]
	If wispr_kw (Killer whale) 93750 is default
-g	gain 0, 1, 2, 3 (additonal gain with 6dB incr)  [0]
-h	print help
-C	number of files to record
x	cpu usage time

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

#/bin/wispr_bw -T8 -b15 -M2 -F2 -l wispr.log -v2 -r -g0  
#/bin/wispr_bw -T8 -b15 -M2 -F2 -v2 -g0  
#/bin/wispr_bw -l wispr.log -v2 -T8 -b15 -M2 -F2 -g0 -r
#/bin/wispr_bw -l wispr.log -v2 -T8 -b15 -M2 -F2 -g0 -r -W

/bin/wispr_bw -l wispr.log -v2 -T8 -b15 -M2 -F2 -g0 -r
# remount and run spector
mount /dev/sda1 /mnt
/bin/spectrogram -T10 -C12 -v1 -n512 -o256 -g0 -t20 -l noise.log

 *
 */


