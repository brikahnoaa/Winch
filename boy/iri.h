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
  bool setTime;             // flag, reset time via gps (true)
  bool logging;             // make a log of comm via rudics (false)
  char me[4];               // (iri)
  char phoneNum[16];        // (0088160000519)
  char platform[16];        // (LR01)
  char project[16];         // (QUEH)
  int baud;                 // effective baud rate for rudics (1800)
  int blkSz;                // how many bytes to upload in a file block (256)
  int fileCnt;              // how many files to upload in a connection (2)
  int fileMaxKB;            // how many kilobytes max in a file (64K)
  int hdrResp;              // wait secs for ACK response to proj hdr (20)
  int hdrTry;               // header transmit retry (3)
  int landResp;             // wait secs for cmds/data string from land (20)
  int hupMs;                // ms +++ ms for HUP (2000)
  int redial;               // how many calls to make (5)
  int signalMin;            // min iri signal level to try transmit (2)
  int timeout;              // seconds for steps like signal, sats (60)
} IriInfo;

typedef struct IriData {
  GpsStats *stats1;         // we read stats, repeat
  GpsStats *stats2;         // ... until two match
  RTCTimer timer;           // elapsed usec timer for slower baud
  Serial port;
  uchar *block;             // offset into buf for file transfer
  uchar *buf;               // buffer for file transfer
  uchar projHdr[16];        // rudicsland
  int blkSz;                // size of *block - verify GspInfo.blockSz
  int log;                  // log of file transfers
  int sats;
  int signal;
  unsigned long usec;       // microsec delay calculated from rudBaud/sendSz
} IriData;

static int iriCRC(uchar *buf, int cnt);
static int iriDateTimeGet(GpsStats *stats);
static int iriSats(void);
static int iriSendSlow(uchar *c, int len);
static int iriSetTime(GpsStats *stats);
static void iriBufMalloc(void);

int iriDateTime(GpsStats *stats);
int iriDial(void);
int iriLandCmds(uchar *buff);
int iriLandResp(uchar *buff);
int iriLatLng(GpsStats *stats);
int iriProcessCmds(uchar *buff);
int iriProjHello(uchar *buff);
int iriPrompt(void);
int iriSendBlock(int bsiz, int bnum, int btot);
int iriSendFile(char *fname);
int iriSig(void);
int iriStart(void);
void iriHup(void);
void iriInit(void);
void iriStop(void);

#endif
