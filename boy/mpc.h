// mpc.h
#define MPC_H

#define WTMODE nsStdSmallBusAdj // choose: nsMotoSpecAdj or nsStdSmallBusAdj
#define SYSCLK 16000 // Clock speed: 2000 works 160-32000 kHz Default: 16000
#define COM1BAUD 9600

typedef enum { ant_dev, ctd_dev } DevType;

typedef struct MpcInfo {
  long diskFree;
  long diskSize;
  DevType device;             // sbe39|gpsirid
  Serial com1;
} MpcInfo;

static void IRQ4_ISR(void);
static void IRQ5_ISR(void);
static void spur_ISR(void);

Serial mpcCom1(void);
void mpcDevice(DevType dev);
void mpcInit(void);
void mpcSleep(void);
void mpcStop(void);
