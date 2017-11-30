// ant.h

// ant
typedef struct AntennaData {
  bool off;
  bool surfaced;        // Set to true when Antenna is floating
  float ctdPos;         // depth of CTD when floating
  char gpsLong[20];     // 123:45.6789 W
  char gpsLat[20];      // 45:67.8900 N
  TUPort *port;
} AntennaData;
extern AntennaData ant;

void DevSelect(int);
int AntMode(char);
