// iri.h
#ifndef H_IRI
#define H_IRI

typedef struct GpsStats {
  char lat[32];             // 45:67.8900N
  char lng[32];             // 123:45.6789W
  char date[32];            // Date=06-06-2019
  char time[32];            // Time=21:25:10.000
} GpsStats;

/// This will be malloc'd as iri.blockSz+sizeof(IriBuff), irid.buf=pointer 
typedef struct IriBuff {
  char leader[3];           // @@@
  int checksum;             // 2 bytes
  int length;               // 2 bytes
  char blockType;           // T=text B=binary Z=zip
  short bnum;               // 1 char, this block number
  short btot;               // 1 char, total blocks
  char block[];             // array of bytes == iri.blockSz
} IriBuff;

typedef struct IriProjHdr {
  char leader[3];           // ???
  int checksum;             // two bytes
  char project[4];          // QUEH
  char platform[4];         // LR01
  char terminator;          // 0
} IriProjHdr;

typedef struct IriInfo {
  bool setTime;             // flag, reset time via gps, starts true
  char phoneNum[16];        // (0088160000519)
  char platform[16];        // (LR01)
  char project[16];         // (QUEH)
  int fileCnt;              // how many files to upload in a connection (2)
  int fileMaxKB;            // how many kilobytes max in a file (64)
  int filePause;            // wait sec between sending "data" and data (1)
  int hdrPause;             // wait sec for rudics header response (20)
  int hdrTry;               // header retry (3)
  int hupMs;                // ms +++ ms for HUP (2000)
  int redial;               // how many calls to make (5)
  int rudBaud;              // effective baud rate for rudics (2400)
  int rudResp;              // wait secs for respond to a block (30)
  int sendSz;               // send some chars, then wait rudUsec (64)
  int signalMin;            // min iri signal level to try transmit (2)
  int timeout;              // seconds for steps like signal, sats (60)
  long blockSz;              // how many bytes to upload in a file block (1024)
} IriInfo;

typedef struct IriData {
  GpsStats *stats1;         // we read stats, repeat
  GpsStats *stats2;         // ... until two match
  Serial port;
  IriBuff *buf;             // malloc( iri.blockSz+sizeof(IriBuff) )
  IriProjHdr *projHdr;      // rudicsland
  int log;
  int rudUsec;              // microsec delay calculated from rudBaud
  int sats;
  int signal;
  long blockSz;             // size of *block - verify GpsInfo.blockSz
} IriData;

static int iriCRC(char *buf, int cnt);
int iriLandCmds(char *buff);
static int iriProcessCmds(char *buff);
static int iriPrompt(void);
static int iriSats(void);
int iriSendBlock(int bsiz, int bnum, int btot);

bool iriSetTime(GpsStats *stats);
int iriDateTime(GpsStats *stats);
int iriDial(void);
int iriLandResp(char *buff);
int iriLatLng(GpsStats *stats);
int iriProjHello(char *buff);
int iriSendFile(char *fname);
int iriSig(void);
int iriStart(void);
void iriData(void);
void iriHup(void);
void iriInit(void);
void iriStop(void);

#endif
