// s16.c sbe16
#include <main.h>

#define EOL "\r"
#define EXEC "S>"

S16Info s16;

// general note: s16 wants \r only for input, outputs \r\n
// Bug:! TUTxPrint translates \n into \r\n, which sorta kinda works if lucky
//
// \r input, echos input.  \r\n before next output.
// pause 0.33s between \rn and s> prompt.
// wakeup takes 1.045s, writes extra output "SBE 16plus\r\nS>"
// pause between ts\r\n and result = 4.32s
// sbe16 response is just over 3sec in sync, well over 4sec in command
// NOTE - sbe16 does not echo while logging, must get prompt before STOP

///
// sets: s16.port .s16Pending
void s16Init(void) {
  static char *self="s16Init";
  DBG()
  s16.me="s16";
  s16.port = mpcPamPort();
  s16Start();
  utlWrite(s16.port, "DelayBeforeSampling=0", EOL);
  utlReadWait(s16.port, all.str, 1);   // echo
  utlWrite(s16.port, "stop", EOL);
  utlReadWait(s16.port, all.str, 1);   // echo
  s16Stop();
} // s16Init

///
int s16Start(void) {
  static char *self="s16Start";
  if (s16.on) // verify
    if (s16Prompt()) {
      s16Sample();
      return 0;
    } else {
      flogf("\n%s(): ERR sbe16, expected prompt", self);
      return 1;
    }
  s16.on = true;
  flogf("\n === buoy sbe16 start %s", utlDateTime());
  mpcPamPwr(sbe16_pam, true);
  tmrStop(s16_tmr);
  if (!s16Prompt())
    utlErr(s16_err, "s16: no prompt");
  sprintf(all.str, "datetime=%s", utlDateTimeS16());
  utlWrite(s16.port, all.str, EOL);
  utlReadWait(s16.port, all.str, 2);   // echo
  s16Sample();
  return 0;
} // s16Start

///
int s16Stop(void){
  static char *self="s16Stop";
  flogf("\n === buoy sbe16 stop %s", utlDateTime());
  utlLogClose(&s16.log);
  if (s16.auton)
    s16Auton(false);
  mpcPamPwr(sbe16_pam, false);
  s16.on = false;
  return 0;
} // s16Stop

///
// open or reopen log file
int s16LogOpen(void) {
  int r=0;
  if (!s16.log)
    r = utlLogOpen(&s16.log, s16.me);
  return r;
} // s16LogOpen

///
///
// open or reopen log file
int s16LogClose(void) {
  int r=0;
  if (!s16.log)
    r = utlLogClose(&s16.log);
  return r;
} // s16LogClose

///
// sbe16
// s16Prompt - poke buoy CTD, look for prompt
bool s16Prompt(void) {
  DBG1("cPt")
  if (s16Pending()) 
    s16DataWait();
  s16Flush();
  utlWrite(s16.port, "", EOL);
  // looking for S> at end
  if (utlExpect(s16.port, all.str, EXEC, 5))
    return true;
  // try again after break
  s16Break();
  utlWrite(s16.port, "", EOL);
  if (utlExpect(s16.port, all.str, EXEC, 5))
    return true;
  return false;
} // s16Prompt

///
// reset, exit sync mode
void s16Break(void) {
  static char *self="s16Break";
  DBG()
  TUTxBreak(s16.port, 5000);
} // s16Break

///
// data waiting
bool s16Data() {
  int r;
  DBG2("cDa")
  r=TURxQueuedCount(s16.port);
  if (r)
    tmrStop(s16_tmr);
  return r>0;
} // s16Data

///
// wait for data or not pending (timeout)
bool s16DataWait(void) {
  static char *self="cDW";
  DBG()
  do if (s16Data()) 
    return true;
  while (s16Pending());
  flogf(" %s:fail", self);
  return false;
} // s16DataWait

///
// poke s16 to get sample, set interval timer (ignore s16.auton)
// sets: s16_tmr
void s16Sample(void) {
  if (s16Pending()) return;
  DBG1("cSam")
  // flush old data, check for sleep message and prompt if needed
  if (s16Data()) {
    utlRead(s16.port, all.str);
    if (strstr(all.str, "time out"))
      s16Prompt();      // wakeup
  } // s16Data()
  if (!s16.auton && s16.sampStore)
    utlWrite(s16.port, "TSSon", EOL);
  else
    utlWrite(s16.port, "TS", EOL);
  // get echo // NOTE - sbe16 does not echo while auton
  if (!s16.auton)
    utlReadWait(s16.port, all.str, 1);
  tmrStart(s16_tmr, s16.delay);
} // s16Sample

///
// sample, read data, log to file
// sets: .temp .depth 
bool s16Read(void) {
  char *p0, *p1, *p2, *p3;
  static char *self="s16Read";
  DBG()
  if (!s16Data()) return false;
  // utlRead(s16.port, all.str);
  p0 = utlExpect(s16.port, all.str, EXEC, 2);
  if (!p0) {
    utlErr(s16_err, "s16Read: no S>");
    return false;
  } // not data
  if (s16.log) 
    write(s16.log, all.str, strlen(all.str)-2); // no S>
  // Temp, conductivity, depth, fluromtr, PAR, salinity, time
  // ' 20.6538,  0.01145,    0.217,   0.0622, 01 Aug 2016 12:16:50\r\n'
  // note: leading # in syncmode '# 20.6...'
  // note: picks up trailing S> prompt if not in syncmode
  p0 = all.str;
  p1 = strtok(p0, "\r\n#, ");
  if (!p1) return false;
  s16.temp = atof( p1 );
  p2 = strtok(NULL, ", "); 
  if (!p2) return false;
  // s16.cond = atof( p2 );
  p3 = strtok(NULL, ", ");
  if (!p3) return false;
  s16.depth = atof( p3 );
  if (s16.temp==0.0 && s16.depth==0.0) {
    utlErr(ant_err, "antRead: null values");
    return false;
  }
  DBG1("= %4.2", s16.depth)
  s16.sampT = time(0);
  s16Sample();
  return true;
} // s16Read

///
// tmrOn ? s16Pending. tmrExp ? err
bool s16Pending(void) {
  return (tmrOn(s16_tmr));
}

///
float s16Depth(void) {
  DBG1("cDep")
  if (s16Data())
      s16Read();
  return s16.depth;
} // s16Depth

///
// NOTE - sbe16 does not echo while logging, but it does S> prompt
// must get prompt after log starts, before STOP 
// "start logging at = 08 Jul 2018 05:28:29, sample interval = 10 seconds\r\n"
// sets: .auton
// rets: 0=good 1=off 2=badResponse
int s16Auton(bool auton) {
  int r=0;
  flogf("\ns16Auton(%s)", auton?"true":"false");
  if (!s16.on) {
    r = 1;
    s16Start();
  }
  if (auton) {
    // note - initlogging may be done at end of s16GetSamples
    if (s16Pending())
      s16DataWait();
    s16Prompt();
    sprintf(all.str, "sampleInterval=%d", s16.sampleInt);
    utlWrite(s16.port, all.str, EOL);
    utlExpect(s16.port, all.str, EXEC, 2);
    utlWrite(s16.port, "txRealTime=n", EOL);
    utlExpect(s16.port, all.str, EXEC, 2);
    utlWrite(s16.port, "startnow", EOL);
    if (!utlExpect(s16.port, all.str, "start logging", 4)) {
      r = 1;
      utlErr(s16_err, "s16Auton: expected 'start logging'");
    }
    // s16Prompt();
  } else {
    // turn off
    s16Prompt();
    // utlWrite(s16.port, "stop", EOL);
    // utlExpect(s16.port, all.str, EXEC, 2);
    utlWrite(s16.port, "stop", EOL);
    if (!utlExpect(s16.port, all.str, "logging stopped", 4)) {
      flogf("\nERR\t| expected 'logging stopped', retry...");
      utlWrite(s16.port, "stop", EOL);
      if (!utlExpect(s16.port, all.str, "logging stopped", 4)) {
        r=2;
        flogf("\nERR\t| got '%s'", all.str);
        utlErr(s16_err, "expected 'logging stopped'");
      }
    }
  } // if auton
  s16.auton = auton;
  return r;
} // s16Auton

///
// get science, clear log
void s16GetSamples(void) {
  int len1=sizeof(all.str);
  int len2=len1, len3=len1;
  int total=0;
  s16LogOpen();
  flogf("\n+s16GetSamples()");
  s16Prompt();          // wakeup
  utlWrite(s16.port, "GetSamples:", EOL);
  while (len1==len3) {
    // repeat until less than a full buf
    len2 = (int) TURxGetBlock(s16.port, all.str, (long) len1, (short) 1000);
    len3 = write(s16.log, all.str, len2);
    if (len2!=len3) 
      flogf("\nERR\t| s16GetSamples() could not write %s.log", s16.me);
    flogf("+[%d]", len3);
    total += len3;
  } // while ==
  s16LogClose();
  if (s16.sampClear) {
    utlWrite(s16.port, "initLogging", EOL);
    utlExpect(s16.port, all.str, "verify", 2);
    utlWrite(s16.port, "initLogging", EOL);
    utlExpect(s16.port, all.str, EXEC, 2);
  }
  flogf(" = %d bytes to %s.log", total, s16.me);
} // s16GetSamples

///
// flush input - clears timeout
void s16Flush(void) {
  TURxFlush(s16.port);
} // s16Flush

///
// loop to test sbe16
void s16Test(void) {
  bool b;
  char c;
  float f;
  int i;
  b=false; c=0; f=0.0; i=0;
  printf("\n%c s16 functions\n");
  printf("q:quit ?:info "
      "d:data w:dataWait p:prompt r:read s:sample "
      "i:init o:start x:stop t:talk"
      "\n");
  while (c != 'q') {
    if (cgetq()) {
      c=cgetc();
      cputc(c);
      cputc(' ');
      switch (c) {
      case '?':
        printf("s16.on=%d .log=%d .auton=%d .depth=%3.1f .temp=%3.1f\n",
            s16.on, s16.log, s16.auton, s16.depth, s16.temp);
        break;
      case 'd':
        b=s16Data();
        printf("s16Data %s\n", b?"true":"false");
        break;
      case 'w':
        b=s16DataWait();
        printf("s16DataWait %s\n", b?"true":"false");
        break;
      case 'p':
        b=s16Prompt();
        printf("s16Prompt %s\n", b?"true":"false");
        break;
      case 'r':
        b=s16Read();
        printf("s16Read %s\n", b?"true":"false");
        break;
      case 'i':
        s16Init();
        printf("s16Init ok\n");
        break;
      case 's':
        s16Sample();
        printf("s16Sample ok\n");
        break;
      case 'o':
        s16Start();
        printf("s16Start ok\n");
        break;
      case 'x':
        s16Stop();
        printf("s16Stop ok\n");
        break;
      case 't':
        flogf("\nTalk to sbe16 - Press Q to exit\n");
        while (true) {
          if (cgetq()) {
            c=cgetc();
            if (c=='Q') break;
            TUTxPutByte(s16.port,c,false);
          }
          if (TURxQueuedCount(s16.port)) {
            c=TURxGetByte(s16.port,false);
            cputc(c);
          }
        }
        break;
      }
    }
  }
} // s16Test
