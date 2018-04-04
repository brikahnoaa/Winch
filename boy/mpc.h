// mpc.h
#define MPC_H

#define WTMODE nsStdSmallBusAdj // choose: nsMotoSpecAdj or nsStdSmallBusAdj
#define SYSCLK 16000 // Clock speed: 2000 works 160-32000 kHz Default: 16000
#define PAM_BAUD 9600

typedef enum { non_pam, wsp_pam, sbe_pam } PamType;

typedef struct MpcInfo {
  long diskFree;
  long diskSize;
  PamType pam;             // wsp_12, sbe_16
  Serial port;
} MpcInfo;

static void IRQ4_ISR(void);
static void IRQ5_ISR(void);
static void spur_ISR(void);

Serial mpcPort(void);
void mpcPam(PamType pam);
void mpcInit(void);
void mpcSleep(void);
void mpcStop(void);
