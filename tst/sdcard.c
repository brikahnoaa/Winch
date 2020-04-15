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
  void *buffer;
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
  }
  exit(0);
  //
}
