// ant.h

typedef struct AntInfo {
  bool on;
  float depth;
  float depth2;
  float surfD;                // depth of CTD when ant is floating
  float temp;
  char gpsLong[32];           // 123:45.6789W
  char gpsLat[32];            // 45:67.8900N
  Serial port;
} AntInfo;

bool antSurf(void);
float antDepth(void);
float antMoving(void);
float antTemp(void);
void antInit(void);
void antStop(void);
