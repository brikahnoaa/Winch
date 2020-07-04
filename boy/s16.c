// s16.c sbe16
#include <main.h>

#define EOL "\r"
#define EXEC "S>"

S16Info s16;

///
// sets: s16.port .s16Pending
void s16Init(void) {
  static char *self="s16Init";
  DBG();
  s16.port = mpcPamPort();
  if (dbg.test) s16.pumpMode=0;
  s16Start();
  utlWrite(s16.port, "DelayBeforeSampling=0", EOL);
  utlReadWait(s16.port, all.str, 1);   // echo
  //// utlWrite(s16.port, "TxSampleNum=N", EOL);
  //// utlReadExpect(s16.port, all.str, EXEC, 2);
  utlWrite(s16.port, "txRealTime=n", EOL);
  utlReadExpect(s16.port, all.str, EXEC, 2);
  if (s16.initStr) {
    utlWrite(s16.port, s16.initStr, EOL);
    utlReadWait(s16.port, all.str, 1);   // echo
  }
  s16Stop();
} // s16Init

///
// ret: 0=ok 1=not
int s16Start(void) {
  static char *self="s16Start";
  if (s16.on) // verify
    if (s16Prompt()) {
      s16Sample();
      return 0;
    } else {
      flogf("\n%s(): ERR sbe16, no response", self);
      return 1;
    }
  s16.on = true;
  if (s16.logging) s16LogOpen();
  flogf("\n === buoy sbe16 start %s", utlDateTime());
  mpcPamPwr(sbe16_pam, true);
  tmrStop(s16_tmr);
  if (!s16Prompt()) {
    utlErr(s16_err, "s16: no prompt");
    return 2;
  }
  // 0=no 1=.5sec 2=during
  sprintf(all.str, "pumpmode=%d", s16.pumpMode);
  utlWrite(s16.port, all.str, EOL);
  utlReadWait(s16.port, all.str, 2);   // echo
  sprintf(all.str, "datetime=%s", utlDateTimeSBE());
  utlWrite(s16.port, all.str, EOL);
  utlReadWait(s16.port, all.str, 2);   // echo
  if (s16.startStr) {
    utlWrite(s16.port, s16.startStr, EOL);
    utlReadWait(s16.port, all.str, 2);   // echo
  }
  s16Sample();
  return 0;
} // s16Start

///
int s16Stop(void){
  static char *self="s16Stop";
  flogf("\n === buoy sbe16 stop %s", utlDateTime());
  antLogClose();
  mpcPamPwr(sbe16_pam, false);
  s16.on = false;
  return 0;
} // s16Stop

///
// open or reopen log file
int s16LogOpen(void) {
  static char *self="s16LogOpen";
  int r=0;
  if (!s16.on)
    s16Start();
  if (!s16.log)
    r = utlLogOpen(&s16.log, "s16");
  else
    DBG2("%s: log already open", self);
  return r;
} // s16LogOpen

///
///
// open or reopen log file
int s16LogClose(void) {
  static char *self="s16LogClose";
  int r=0;
  if (s16.log)
    r = utlLogClose(&s16.log);
  else
    DBG2("%s: log already closed", self);
  return r;
} // s16LogClose

///
// sbe16
// s16Prompt - poke buoy CTD, look for prompt
bool s16Prompt(void) {
  DBG1("s16Pr");
  if (s16Pending()) 
    s16DataWait();
  s16Flush();
  utlWrite(s16.port, "", EOL);
  if (utlReadExpect(s16.port, all.str, EXEC, 5))
    return true;
  s16Stop();
  s16Start();
  utlWrite(s16.port, "", EOL);
  if (utlReadExpect(s16.port, all.str, EXEC, 5))
    return true;
  return false;
} // s16Prompt

///
// data waiting
bool s16Data() {
  int r;
  r=TURxQueuedCount(s16.port);
  if (r)
    tmrStop(s16_tmr);
  return r>0;
} // s16Data

///
// wait for data or not pending (timeout)
bool s16DataWait(void) {
  static char *self="s16DataWait";
  DBG();
  do if (s16Data()) 
    return true;
  while (s16Pending());
  flogf(" %s:fail", self);
  return false;
} // s16DataWait

///
// poke s16 to get sample, set interval timer 
// sets: s16_tmr
void s16Sample(void) {
  static char *self="s16Sample";
  if (s16Pending()) return;
  DBG();
  // flush old data, check for sleep message and wake if needed
  if (s16Data()) {
    utlRead(s16.port, all.str);
    if (strstr(all.str, "time out"))
      s16Prompt();      // wakeup
  } // s16Data()
  utlWrite(s16.port, s16.takeSamp, EOL);
  // get echo 
  utlReadWait(s16.port, all.str, 1);
  tmrStart(s16_tmr, s16.timer);
} // s16Sample

///
// sample, read data, log to file
// sets: .temp .depth 
bool s16Read(void) {
  char *p0, *p1, *p2, *p3;
  static char *self="s16Read";
  DBG();
  if (!s16Data()) return false;
  // utlRead(s16.port, all.str);
  p0 = utlReadExpect(s16.port, all.str, EXEC, 2);
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
  DBG1("= %4.2", s16.depth);
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
  if (s16Data())
    s16Read();
  return s16.depth;
} // s16Depth

///
float s16Temp(void) {
  if (s16Data())
    s16Read();
  return s16.temp;
} // s16Temp

///
// get science, clear log
void s16GetSamples(void) {
  int len1=sizeof(all.str);
  int len2=len1, len3=len1;
  int total=0;
  if (s16.logging) s16LogOpen();
  flogf("\n+s16GetSamples()");
  s16Prompt();          // wakeup
  utlWrite(s16.port, "GetSamples:", EOL);
  while (len1==len3) {
    // repeat until less than a full buf
    len2 = (int) TURxGetBlock(s16.port, all.str, (long) len1, (short) 1000);
    len3 = write(s16.log, all.str, len2);
    if (len2!=len3) 
      flogf("\nERR\t| s16GetSamples() could not write s16.log");
    flogf("+[%d]", len3);
    total += len3;
  } // while ==
  s16LogClose();
  flogf(" = %d bytes to s16.log", total);
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
      c=(char)cgetc();
      cputc(c);
      cputc(' ');
      switch (c) {
      case '?':
        printf("s16.on=%d .log=%d .depth=%3.1f .temp=%3.1f\n",
            s16.on, s16.log, s16.depth, s16.temp);
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
            c=(char)TURxGetByte(s16.port,false);
            cputc(c);
          }
        }
        break;
      }
    }
  }
} // s16Test

//
// general note: s16 wants \r only for input, outputs \r\n
// Bug:! TUTxPrint translates \n into \r\n, which sorta kinda works if lucky
//
// \r input, echos input.  \r\n before next output.
// pause 0.33s between \rn and s> prompt.
// wakeup takes 1.045s, writes extra output "SBE 16plus\r\nS>"
// pause between ts\r\n and result = 4.32s
// sbe16 response is just over 3sec in sync, well over 4sec in command
// NOTE - sbe16 does not echo while logging, must get prompt before STOP
