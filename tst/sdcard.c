// sdcard
#include <test.h>

extern BoyInfo boy;
extern SysInfo sys;

// bool CFCardDetect(void);
// void CFEnable(bool on);
// void *CFGetDriver(void);
// short ATACapacity(ATADvr iodvr, ulong *sectors, ushort *spt, ushort *heads, void **info);
// short ATAReadSectors(ATADvr iodvr, ulong sector, void *buffer, short count);
// short ATAWriteSectors(ATADvr iodvr, ulong sector, void *buffer, short count);
//

void main(void){
  int sdGB=32;
  short ok;
  ATADvr iodvr;
  //
  ulong sectors;
  ushort spt;
  ushort heads;
  void *info;
  //
  ulong sector;
  uchar *buffer;
  short count;
  //
  sysInit();
  mpcInit();
  iodvr=CFGetDriver();
  //
  cprintf("\n sdcard read direct access \n");
  //
  if (CFCardDetect()) {
    CFEnable(true);
    ok=ATACapacity(iodvr, &sectors, &spt, &heads, &info);
    cprintf("card has %ld sectors   (status %d)\n", sectors, ok);
    buffer=malloc(512*4);
    memset(buffer, 0xA5, 512*4);
    ok=ATAWriteSectors(iodvr, sectors-3, buffer, 4);
    if (ok) cprintf("write err   (status %d)\n", ok);
    memset(buffer, 0, 512*4);
    ok=ATAReadSectors(iodvr, sectors-3, buffer, 4);
    if (ok) cprintf("read err   (status %d)\n", ok);
    // any zeros?
    for (i=0; i<512*4; i++)
      if (buffer[i]==0) {
        cprintf("zero err   (status %d)\n", ok);
        break;
      }
  }
  exit(0);
  //
}
