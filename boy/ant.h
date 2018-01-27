// ant.h

// ant
typedef struct AntennaData {
  bool on;
  float depth;
  float surfaceD;        // depth of CTD when ant is floating
  char gpsLong[20];     // 123:45.6789 W
  char gpsLat[20];      // 45:67.8900 N
} AntennaData;
extern AntennaData ant;

void antOpen();
void antClose();
float antDepth();
void antInit();
