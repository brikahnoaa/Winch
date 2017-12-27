// ant.h

// ant
typedef struct AntennaData {
  bool on;
  bool surfaced;        // Set to true when Antenna is floating
  float ctdPos;         // depth of CTD when floating
  float depth;
  char gpsLong[20];     // 123:45.6789 W
  char gpsLat[20];      // 45:67.8900 N
  Serial port;
} AntennaData;
extern AntennaData ant;

void antOpen();
void antClose();
float antDepth();
