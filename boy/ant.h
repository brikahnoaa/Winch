// ant.h
#ifndef H_ANT
#define H_ANT

typedef enum { null_dev, cf2_dev, a3la_dev } DevType;
typedef enum { null_ant, gps_ant, irid_ant } AntType;

typedef struct AntInfo {
  bool on;
  bool surf;                  // on surface
  char *me;
  float subD;                 // subsurfaceD; ant.surfD + boy.ant2tip
  float surfD;                // surfaceDepth of buoy when ant is floating
  int log;
  AntType antenna;
  DevType dev;
  Serial port;
} AntInfo;

float antSurfD(void);
int antLogClose(void);
int antLogOpen(void);
int antStart(void);
int antStop(void);
void antAntenna(AntType *antenna);
void antDevPwr(char c, bool on);
void antDevice(DevType dev);
void antInit(void);
void antPort(Serial *port);
void antSwitch(AntType antenna);

#endif
