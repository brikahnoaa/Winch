// winchTst.c - move winch
#include <com.h>
#include <mpc.h>
#include <ngk.h>
#include <sys.h>

#define CTRL_C 3

void winchTstHelp(void);

void winchTstHelp() {
  cprintf(
    "? help q quit ^C quit \n"
    "B buoyRsp F dropRsp R riseRsp S stopRsp W statRsp \n"
    "b buoyCmd f dropCmd r riseCmd s stopCmd w statCmd u surfCmd \n"
    );
}

void main(void){
  char c;
  MsgType msg;
  sysInit();
  mpcInit();
  DBG0("dbg0")
  DBG1("dbg1")
  DBG2("dbg2")
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
        cprintf("\n winch>> '%s' @ %s", ngkMsgName(msg), utlTime());
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

