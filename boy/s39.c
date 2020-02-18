// s39.c sbe39
#include <main.h>

#define EOL "\r"
#define EXEC "<Executed/>"

S39Info s39;

///
// sets: s39.port .s39Pending
void s39Init(void) {
  static char *self="s39Init";
  DBG();
  s39.me="s39";
  s39.port = mpcPamPort();
  // if (dbg.test) s39.pumpMode=0;
  s39Start();
  utlWrite(s39.port, "DelayBeforeSampling=0", EOL);
  utlReadWait(s39.port, all.str, 1);   // echo
  if (s39.initStr) {
    utlWrite(s39.port, s39.initStr, EOL);
    utlReadWait(s39.port, all.str, 1);   // echo
  }
  utlWrite(s39.port, "stop", EOL);
  utlReadWait(s39.port, all.str, 1);   // echo
  s39Stop();
} // s39Init

///
int s39Start(void) {
  static char *self="s39Start";
  if (s39.on) // verify
    if (s39Prompt()) {
      s39Sample();
      return 0;
    } else {
      flogf("\n%s(): ERR sbe39, no response", self);
      return 1;
    }
  s39.on = true;
  s39LogOpen();
  flogf("\n === buoy sbe39 start %s", utlDateTime());
  mpcPamPwr(sbe39_pam, true);
  tmrStop(s39_tmr);
  if (!s39Prompt())
    utlErr(s39_err, "s39: no prompt");
  // 0=no 1=.5sec 2=during
  sprintf(all.str, "pumpmode=%d", s39.pumpMode);
  utlWrite(s39.port, all.str, EOL);
  utlReadWait(s39.port, all.str, 2);   // echo
  sprintf(all.str, "datetime=%s", utlDateTimeSBE());
  utlWrite(s39.port, all.str, EOL);
  utlReadWait(s39.port, all.str, 2);   // echo
  if (s39.startStr) {
    utlWrite(s39.port, s39.startStr, EOL);
    utlReadWait(s39.port, all.str, 2);   // echo
  }
  s39Sample();
  return 0;
} // s39Start

///
int s39Stop(void){
  static char *self="s39Stop";
  flogf("\n === buoy sbe39 stop %s", utlDateTime());
  antLogClose();
  if (s39.auton) s39Auton(false);
  mpcPamPwr(sbe39_pam, false);
  s39.on = false;
  return 0;
} // s39Stop

///
// open or reopen log file
int s39LogOpen(void) {
  static char *self="s39LogOpen";
  int r=0;
  if (!s39.on)
    s39Start();
  if (!s39.log)
    r = utlLogOpen(&s39.log, s39.me);
  else
    DBG2("%s: log already open", self);
  return r;
} // s39LogOpen

///
///
// open or reopen log file
int s39LogClose(void) {
  static char *self="s39LogClose";
  int r=0;
  if (s39.log)
    r = utlLogClose(&s39.log);
  else
    DBG2("%s: log already closed", self);
  return r;
} // s39LogClose

///
// sbe39
// s39Prompt - poke buoy CTD, look for prompt
bool s39Prompt(void) {
  DBG1("cPt");
  if (s39Pending()) 
    s39DataWait();
  s39Flush();
  utlWrite(s39.port, "", EOL);
  // looking for S> at end
  if (utlReadExpect(s39.port, all.str, EXEC, 5))
    return true;
  // try again after break
  s39Break();
  utlWrite(s39.port, "", EOL);
  if (utlReadExpect(s39.port, all.str, EXEC, 5))
    return true;
  return false;
} // s39Prompt

///
// reset, exit sync mode
void s39Break(void) {
  static char *self="s39Break";
  DBG();
  TUTxBreak(s39.port, 5000);
} // s39Break

///
// data waiting
bool s39Data() {
  int r;
  DBG2("cDa");
  r=TURxQueuedCount(s39.port);
  if (r)
    tmrStop(s39_tmr);
  return r>0;
} // s39Data

///
// wait for data or not pending (timeout)
bool s39DataWait(void) {
  static char *self="cDW";
  DBG();
  do if (s39Data()) 
    return true;
  while (s39Pending());
  flogf(" %s:fail", self);
  return false;
} // s39DataWait

///
// poke s39 to get sample, set interval timer (ignore s39.auton)
// sets: s39_tmr
void s39Sample(void) {
  if (s39Pending()) return;
  DBG1("cSam");
  // flush old data, check for sleep message and prompt if needed
  if (s39Data()) {
    utlRead(s39.port, all.str);
    if (strstr(all.str, "time out"))
      s39Prompt();      // wakeup
  } // s39Data()
  if (!s39.auton && s39.sampStore)
    utlWrite(s39.port, "TSSon", EOL);
  else
    utlWrite(s39.port, "TS", EOL);
  // get echo // NOTE - sbe39 does not echo while auton
  if (!s39.auton)
    utlReadWait(s39.port, all.str, 1);
  tmrStart(s39_tmr, s39.timer);
} // s39Sample

///
// sample, read data, log to file
// sets: .temp .depth 
bool s39Read(void) {
  char *p0, *p1, *p2, *p3;
  static char *self="s39Read";
  DBG();
  if (!s39Data()) return false;
  // utlRead(s39.port, all.str);
  p0 = utlReadExpect(s39.port, all.str, EXEC, 2);
  if (!p0) {
    utlErr(s39_err, "s39Read: no S>");
    return false;
  } // not data
  if (s39.log) 
    write(s39.log, all.str, strlen(all.str)-2); // no S>
  // Temp, conductivity, depth, fluromtr, PAR, salinity, time
  // ' 20.6538,  0.01145,    0.217,   0.0622, 01 Aug 2016 12:16:50\r\n'
  // note: leading # in syncmode '# 20.6...'
  // note: picks up trailing S> prompt if not in syncmode
  p0 = all.str;
  p1 = strtok(p0, "\r\n#, ");
  if (!p1) return false;
  s39.temp = atof( p1 );
  p2 = strtok(NULL, ", "); 
  if (!p2) return false;
  // s39.cond = atof( p2 );
  p3 = strtok(NULL, ", ");
  if (!p3) return false;
  s39.depth = atof( p3 );
  if (s39.temp==0.0 && s39.depth==0.0) {
    utlErr(ant_err, "antRead: null values");
    return false;
  }
  DBG1("= %4.2", s39.depth);
  s39.sampT = time(0);
  s39Sample();
  return true;
} // s39Read

///
// tmrOn ? s39Pending. tmrExp ? err
bool s39Pending(void) {
  return (tmrOn(s39_tmr));
}

///
float s39Depth(void) {
  DBG1("cDep");
  if (s39Data())
      s39Read();
  return s39.depth;
} // s39Depth

///
// NOTE - sbe39 does not echo while logging, but it does S> prompt
// must get prompt after log starts, before STOP 
// "start logging at = 08 Jul 2018 05:28:29, sample interval = 10 seconds\r\n"
// sets: .auton
// rets: 0=good 1=off 2=badResponse
int s39Auton(bool auton) {
  int r=0;
  flogf("\ns39Auton(%s)", auton?"true":"false");
  if (!s39.on) {
    r = 1;
    s39Start();
  }
  if (auton) {
    // note - initlogging may be done at end of s39GetSamples
    if (s39Pending())
      s39DataWait();
    s39Prompt();
    sprintf(all.str, "sampleInterval=%d", s39.sampInter);
    utlWrite(s39.port, all.str, EOL);
    utlReadExpect(s39.port, all.str, EXEC, 2);
    utlWrite(s39.port, "txRealTime=n", EOL);
    utlReadExpect(s39.port, all.str, EXEC, 2);
    utlWrite(s39.port, "startnow", EOL);
    if (!utlReadExpect(s39.port, all.str, "start logging", 4)) {
      r = 1;
      utlErr(s39_err, "s39Auton: expected 'start logging'");
    }
    // s39Prompt();
  } else {
    // turn off
    s39Prompt();
    // utlWrite(s39.port, "stop", EOL);
    // utlReadExpect(s39.port, all.str, EXEC, 2);
    utlWrite(s39.port, "stop", EOL);
    if (!utlReadExpect(s39.port, all.str, "logging stopped", 4)) {
      flogf("\nERR\t| expected 'logging stopped', retry...");
      utlWrite(s39.port, "stop", EOL);
      if (!utlReadExpect(s39.port, all.str, "logging stopped", 4)) {
        r=2;
        flogf("\nERR\t| got '%s'", all.str);
        utlErr(s39_err, "expected 'logging stopped'");
      }
    }
  } // if auton
  s39.auton = auton;
  return r;
} // s39Auton

///
// get science, clear log
void s39GetSamples(void) {
  int len1=sizeof(all.str);
  int len2=len1, len3=len1;
  int total=0;
  s39LogOpen();
  flogf("\n+s39GetSamples()");
  s39Prompt();          // wakeup
  utlWrite(s39.port, "GetSamples:", EOL);
  while (len1==len3) {
    // repeat until less than a full buf
    len2 = (int) TURxGetBlock(s39.port, all.str, (long) len1, (short) 1000);
    len3 = write(s39.log, all.str, len2);
    if (len2!=len3) 
      flogf("\nERR\t| s39GetSamples() could not write %s.log", s39.me);
    flogf("+[%d]", len3);
    total += len3;
  } // while ==
  s39LogClose();
  if (s39.sampClear) {
    utlWrite(s39.port, "initLogging", EOL);
    utlReadExpect(s39.port, all.str, "verify", 2);
    utlWrite(s39.port, "initLogging", EOL);
    utlReadExpect(s39.port, all.str, EXEC, 2);
  }
  flogf(" = %d bytes to %s.log", total, s39.me);
} // s39GetSamples

///
// flush input - clears timeout
void s39Flush(void) {
  TURxFlush(s39.port);
} // s39Flush

///
// loop to test sbe39
void s39Test(void) {
  bool b;
  char c;
  float f;
  int i;
  b=false; c=0; f=0.0; i=0;
  printf("\n%c s39 functions\n");
  printf("q:quit ?:info "
      "d:data w:dataWait p:prompt r:read s:sample "
      "i:init o:start x:stop t:talk"
      "\n");
  while (c != 'q') {
    if (cgetq()) {
      c=(char)cgetc();
      cputc(c);
      cputc(' ');
      switch (c) {
      case '?':
        printf("s39.on=%d .log=%d .auton=%d .depth=%3.1f .temp=%3.1f\n",
            s39.on, s39.log, s39.auton, s39.depth, s39.temp);
        break;
      case 'd':
        b=s39Data();
        printf("s39Data %s\n", b?"true":"false");
        break;
      case 'w':
        b=s39DataWait();
        printf("s39DataWait %s\n", b?"true":"false");
        break;
      case 'p':
        b=s39Prompt();
        printf("s39Prompt %s\n", b?"true":"false");
        break;
      case 'r':
        b=s39Read();
        printf("s39Read %s\n", b?"true":"false");
        break;
      case 'i':
        s39Init();
        printf("s39Init ok\n");
        break;
      case 's':
        s39Sample();
        printf("s39Sample ok\n");
        break;
      case 'o':
        s39Start();
        printf("s39Start ok\n");
        break;
      case 'x':
        s39Stop();
        printf("s39Stop ok\n");
        break;
      case 't':
        flogf("\nTalk to sbe39 - Press Q to exit\n");
        while (true) {
          if (cgetq()) {
            c=cgetc();
            if (c=='Q') break;
            TUTxPutByte(s39.port,c,false);
          }
          if (TURxQueuedCount(s39.port)) {
            c=(char)TURxGetByte(s39.port,false);
            cputc(c);
          }
        }
        break;
      }
    }
  }
} // s39Test

// general note: s39 wants \r only for input, outputs \r\n
// Bug:! TUTxPrint translates \n into \r\n, which sorta kinda works if lucky
//
// \r input, echos input.  \r\n before next output.
// pause 0.33s between \rn and s> prompt.
// wakeup takes 1.045s, writes extra output "SBE 16plus\r\nS>"
// pause between ts\r\n and result = 4.32s
// sbe16 response is just over 3sec in sync, well over 4sec in command
// NOTE - sbe16 does not echo while logging, must get prompt before STOP

