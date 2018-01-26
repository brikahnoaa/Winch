// winching.c - move winch
#include <common.h>
#include <ngk.h>
#include <timer.h>

#define CTRL_C 3

void printHelp() {
  printf("\n b=buoy status, f=fall, r=rise, s=stop, u=up(00), w=winch status"
         "\n lower case (r) is command, upper case (R) is response");
}

void main(void){
  char c;
  mpcInit();
  ngkInit();
  while (true) {
    ciflush();
    printf("\nCommand: ");

    while (true) {
      // amodem
      if (TURxQueuedCount(ngk.port)) {
        ngkRecv(&msg);
        clockTime(scratch);
        printf("\n winch>> '%s' @ %s", mdm.message[msg], scratch);
        break; // while
      } // if mdm
      // keyboard
      if (cgetq()) {
        c = cgetc();
        cputc(c);
        switch (c) {
        case CTRL_C: BIOSResetToPicoDOS();
        case '?': printHelp(); break;
        case 'B': ngkSend(buoyRsp_msg); break;
        case 'F': ngkSend(dropRsp_msg); break;
        case 'R': ngkSend(riseRsp_msg); break;
        case 'S': ngkSend(stopRsp_msg); break;
        case 'U': ngkSend(surfRsp_msg); break;
        case 'W': ngkSend(statRsp_msg); break;
        case 'b': ngkSend(buoyCmd_msg); break;
        case 'f': ngkSend(dropCmd_msg); break;
        case 'r': ngkSend(riseCmd_msg); break;
        case 's': ngkSend(stopCmd_msg); break;
        case 'u': ngkSend(surfCmd_msg); break;
        case 'w': ngkSend(statCmd_msg); break;
        default: printf("??");
        } // switch
        break; // while
      } // if key
    } // while
  } // while
}
