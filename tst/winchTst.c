// winchTst.c - move winch
#include <com.h>
#include <ngk.h>

#define CTRL_C 3
extern NgkInfo ngk;

void antStop(void);
void boyStop(void);
void ctdStop(void);
void pwrStop(void);
void wspStop(void);
void antInit(void);
void boyInit(void);
void ctdInit(void);
void pwrInit(void);
void wspInit(void);
void winchTstHelp(void);

void winchTstHelp() {
  cprintf("\n b=buoy status, f=fall, r=rise, s=stop, u=up(00), w=winch status"
         "\n lower case (r) is command, upper case (R) is response"
         "\n ?=help q=quit ^C=quit"
         "\n");
}

void main(void){
  char c;
  MsgType msg;
  winchTstHelp();
  ngkInit();
  while (true) { // command
    ciflush();
    cprintf("\nCommand: ");
    cdrain();

    while (true) { // input
      // amodem
      msg = ngkRecv();
      if (msg!=null_msg) {
        cprintf("\n winch>> '%s' @ %s", ngk.msgName[msg], clockTime(scratch));
        break; // while input
      } 
      // keyboard
      if (cgetq()) {
        c = cgetc();
        cputc(c);
        cputc('\n');
        switch (c) {
        case CTRL_C: BIOSResetToPicoDOS();
        case 'q': BIOSResetToPicoDOS();
        case '?': winchTstHelp(); break;
        case 'B': ngkSend(buoyRsp_msg); break;
        case 'F': ngkSend(dropRsp_msg); break;
        case 'R': ngkSend(riseRsp_msg); break;
        case 'S': ngkSend(stopRsp_msg); break;
        case 'W': ngkSend(statRsp_msg); break;
        case 'b': ngkSend(buoyCmd_msg); break;
        case 'f': ngkSend(dropCmd_msg); break;
        case 'r': ngkSend(riseCmd_msg); break;
        case 's': ngkSend(stopCmd_msg); break;
        case 'u': ngkSend(surfCmd_msg); break;
        case 'w': ngkSend(statCmd_msg); break;
        default: cprintf("??");
        } // switch
        break; // while input
      } // if key
    } // while input
  } // while command
}

void antStop(void){}
void boyStop(void){}
void ctdStop(void){}
void pwrStop(void){}
void wspStop(void){}
void antInit(void){}
void boyInit(void){}
void ctdInit(void){}
void pwrInit(void){}
void wspInit(void){}
