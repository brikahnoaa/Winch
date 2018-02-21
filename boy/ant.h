// ant.h

typedef enum { 
  null_mod, idle_mod, td_mod, gps_mod, call_mod, stop_mod 
} ModType;

typedef struct AntInfo {
  bool on;
  float depth;
  float surfD;       // depth of CTD when ant is floating
  char gpsLong[32];     // 123:45.6789 W
  char gpsLat[32];      // 45:67.8900 N
  Serial port;          // same as mpc.port, ctd.port
} AntInfo;

void antInit(void);
bool antDropping(void);
bool antRising(void);
bool antSteady(void);
bool antSurf(void);
ModType antMode(ModType mod);
float antDepth(void);
void antStop(void);
