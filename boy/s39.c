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
  s39.port = antPort();
  //// if (dbg.test) s16.pumpMode=0;
  s39Start();
  utlWrite(s39.port, "DelayBeforeSampling=0", EOL);
  utlReadWait(s39.port, all.str, 1);   // echo
  utlWrite(s39.port, "TxSampleNum=N", EOL);
  utlReadExpect(s39.port, all.str, EXEC, 2);
  utlWrite(s39.port, "txRealTime=n", EOL);
  utlReadExpect(s39.port, all.str, EXEC, 2);
  if (s39.initStr) {
    utlWrite(s39.port, s39.initStr, EOL);
    utlReadWait(s39.port, all.str, 1);   // echo
  }
  s39Stop();
} // s39Init

///
// ret: 0=ok 1=not
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
  if (s39.logging) s39LogOpen();
  flogf("\n === buoy sbe39 start %s", utlDateTime());
  antDevPwr('S', true);
  tmrStop(s39_tmr);
  if (!s39Prompt()) {
    utlErr(s39_err, "s39: no prompt");
    return 2;
  }
  //// // 0=no 1=.5sec 2=during
  //// sprintf(all.str, "pumpmode=%d", s16.pumpMode);
  //// utlWrite(s16.port, all.str, EOL);
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
  antDevPwr('S', false);
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
    r = utlLogOpen(&s39.log, "s39");
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
  DBG1("s39Pr");
  if (s39Pending()) 
    s39DataWait();
  s39Flush();
  utlWrite(s39.port, "", EOL);
  if (utlReadExpect(s39.port, all.str, EXEC, 5))
    return true;
  s39Stop();
  s39Start();
  utlWrite(s39.port, "", EOL);
  if (utlReadExpect(s39.port, all.str, EXEC, 5))
    return true;
  return false;
} // s39Prompt

///
// data waiting
bool s39Data() {
  int r;
  r=TURxQueuedCount(s39.port);
  if (r)
    tmrStop(s39_tmr);
  return r>0;
} // s39Data

///
// wait for data or not pending (timeout)
bool s39DataWait(void) {
  static char *self="s39DataWait";
  DBG();
  do if (s39Data()) 
    return true;
  while (s39Pending());
  flogf(" %s:fail", self);
  return false;
} // s39DataWait

///
// poke s39 to get sample, set interval timer 
// sets: s39_tmr
void s39Sample(void) {
  static char *self="s39Sample";
  if (s39Pending()) return;
  DBG();
  // flush old data, check for sleep message and prompt if needed
  if (s39Data()) {
    utlRead(s39.port, all.str);
    if (strstr(all.str, "time out"))
      s39Prompt();      // wakeup
  } // s39Data()
  utlWrite(s39.port, s39.takeSamp, EOL);
  // get echo
  utlReadWait(s39.port, all.str, 1);
  tmrStart(s39_tmr, s39.timer);
} // s39Sample

///
// read data, store, log, sample
// differs slightly from s16Read
// sets: .temp .depth 
char *s39Read(void) {
  char *p0, *p1, *p2;
  static char *self="s39Read";
  DBG();
  if (!s39Data()) return null;
  // utlRead(s39.port, all.str);
  p0 = utlReadExpect(s39.port, all.str, EXEC, 2);
  if (!p0) { // not data
    sprintf(all.buf, "%s: no %s in %s", self, EXEC, all.str);
    utlErr(s39_err, all.buf);
    return null;
  } 
  p0 = 0; // trim off trailing prompt
  if (s39.log) 
    write(s39.log, all.str, strlen(all.str));
  DBG2("%s", all.str);
  // Temp, depth, date, time
  // ' 20.6538,  0.217,   01 Aug 2016, 12:16:50\r\n'
  // note: leading # in syncmode '# 20.6...'
  // note: picks up trailing S> prompt if not in syncmode
  strcpy(all.buf, all.str);
  p0 = all.buf;
  p1 = strtok(p0, "\r\n#, ");
  if (!p1) return null;
  s39.temp = atof( p1 );
  // strtok(NULL, ", "); // skip one
  p2 = strtok(NULL, ", "); 
  if (!p2) return null;
  s39.depth = atof( p2 );
  if (s39.temp==0.0 && s39.depth==0.0) {
    utlErr(ant_err, "antRead: null values");
    return null;
  }
  s39.sampT = time(0);
  s39Sample();
  return all.str;
} // s39Read

///
// tmrOn ? s39Pending. tmrExp ? err
bool s39Pending(void) {
  return (tmrOn(s39_tmr));
}

///
float s39Depth(void) {
  if (s39Data())
    s39Read();
  return s39.depth;
} // s39Depth

///
float s39Temp(void) {
  if (s39Data())
    s39Read();
  return s39.temp;
} // s39Temp

///
// get science, clear log
void s39GetSamples(void) {
  int len1=sizeof(all.str);
  int len2=len1, len3=len1;
  int total=0;
  if (s39.logging) s39LogOpen();
  flogf("\n+s39GetSamples()");
  s39Prompt();          // wakeup
  utlWrite(s39.port, "GetSamples:", EOL);
  while (len1==len3) {
    // repeat until less than a full buf
    len2 = (int) TURxGetBlock(s39.port, all.str, (long) len1, (short) 1000);
    len3 = write(s39.log, all.str, len2);
    if (len2!=len3) 
      flogf("\nERR\t| s39GetSamples() could not write s39.log");
    flogf("+[%d]", len3);
    total += len3;
  } // while ==
  s39LogClose();
  flogf(" = %d bytes to s39.log", total);
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
        printf("s39.on=%d .log=%d .depth=%3.1f .temp=%3.1f\n",
            s39.on, s39.log, s39.depth, s39.temp);
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

