// winching.c - move winch
#include <com.h>
#include <winching.h>
#include <ngk.h>

#define CTRL_C 3

void winchingHelp() {
  cprintf("\n b=buoy status, f=fall, r=rise, s=stop, u=up(00), w=winch status"
         "\n lower case (r) is command, upper case (R) is response"
         "\n ?=help q=quit ^C=quit"
         "\n");
}

void winchingMain(void){
  char c;
  MsgType msg;
  winchingHelp();
  while (true) { // command
    ciflush();
    cprintf("\nCommand: ");
    cdrain();

    while (true) { // input
      // amodem
      if (ngkRecv(&msg)!=null_msg) {
        cprintf("\n winch>> '%s' @ %s", mdm.msgName[msg], clockTime(scratch));
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
        case '?': winchingHelp(); break;
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
