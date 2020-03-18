// ant.h
#ifndef H_ANT
#define H_ANT

typedef enum { null_dev, cf2_dev, a3la_dev } DevType;
typedef enum { null_ant, gps_ant, irid_ant } AntType;

typedef struct AntInfo {
  bool on;
  float subD;                 // subsurfaceD; ant.surfD + boy.ant2tip
  float surfD;                // surfaceDepth of buoy when ant is floating
  int log;
  AntType antenna;
  DevType dev;
  Serial port;
} AntInfo;


AntType antAntenna(void);
Serial antPort(void);
bool antSurf(void);
float antSurfD(void);
int antStart(void);
int antStop(void);
int antLogClose(void);
int antLogOpen(void);
void antDevPwr(char c, bool on);
void antDevice(DevType dev);
void antInit(void);
void antSwitch(AntType antenna);

#endif
