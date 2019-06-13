// iri.h
#ifndef H_IRI
#define H_IRI

typedef struct GpsStats {
  char lat[32];             // 45:67.8900N
  char lng[32];             // 123:45.6789W
  char date[32];            // Date=06-06-2019
  char time[32];            // Time=21:25:10.000
} GpsStats;

typedef struct IriInfo {
  bool setTime;             // flag, reset time via gps, starts true
  bool logging;             // make a log of comm
  char me[4];
  char phoneNum[16];        // (0088160000519)
  char platform[16];        // (LR01)
  char project[16];         // (QUEH)
  int blockSz;              // how many bytes to upload in a file block (1024)
  int fileCnt;              // how many files to upload in a connection (2)
  int fileMaxKB;            // how many kilobytes max in a file (64K)
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
} IriInfo;

typedef struct IriData {
  GpsStats *stats1;          // we read stats, repeat
  GpsStats *stats2;          // ... until two match
  Serial port;
  char *block;              // offset into buf for file transfer
  char *buf;                // buffer for file transfer
  char projHdr[16];         // rudicsland
  int blockSz;              // size of *block - verify GspInfo.blockSz
  int log;                  // log of file transfers
  int rudUsec;              // microsec delay calculated from rudBaud/sendSz
  int sats;
  int signal;
} IriData;

static bool iriSetTime(GpsStats *stats);
static int iriCRC(char *buf, int cnt);
static int iriPrompt(void);
static int iriSats(void);
static int iriSend(char *buff, long len);

int iriDateTime(GpsStats *stats);
int iriDial(void);
int iriLandCmds(char *buff);
int iriLandResp(char *buff);
int iriLatLng(GpsStats *stats);
int iriProcessCmds(char *buff);
int iriProjHello(char *buff);
int iriSendBlock(int bsiz, int bnum, int btot);
int iriSendFile(char *fname);
int iriSig(void);
int iriStart(void);
void iriHup(void);
void iriInit(void);
void iriStop(void);

#endif
