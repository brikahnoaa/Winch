// mpc.h
#ifndef H_MPC
#define H_MPC

// PINS
#define MDM_PWR 21
#define ANT_PWR 22  // antenna module Power pin (1=ON, 0=OFF)
#define ANT_SEL 23  // set = antMod cf2, clear = a3la
#define PAM_34 24   // 24 cannot be on with 29
#define PAM_4 25    // 24&25 -> pam4 (sbe16)
#define HPSENS 26   // power for pressure sensor
#define PAM_TX 27   // com2,3: (pam_12)(pam_34)
#define PAM_RX 28   // selected by 24, 29
#define PAM_12 29   // 24 cannot be on with 29
#define PAM_2 30    // 29&30 -> wsp2
#define ANT_TX 31   // com1: ant mod, uMPC and A3LA
#define ANT_RX 32   // selected by 23
#define MDM_RX 33   // acoustic modem tpu14
#define MDM_TX 35   // tpu12
#define WISPR_PWR_ON 37
#define WISPR_PWR_OFF 42
// com0tx 43         // max 3222 RS<>/45
// com0rx 44         // max 3222 RS<>/46
// com0/tx 45
// com0/rx 46
// com4tx 47         // max 3222 RS<>/48
// com4rx 49         // max 3222 RS<>/50
// MDM_RX_TTL 48
// MDM_TX_TTL 50

typedef enum { null_pam=0, wsp1_pam, wsp2_pam, wsp3_pam, sbe16_pam } MpcPamType;

typedef struct MpcInfo {
  long diskFree;
  long diskSize;
  MpcPamType pamDev;             // wsp_1,_2,_3, sbe_16
  Serial pamPort;
} MpcInfo;

static void spur_ISR(void);
static void IRQ4_ISR(void);
static void IRQ5_ISR(void);

void mpcPamDev(MpcPamType pam);
void mpcPamPwr(MpcPamType pam, bool on);
void mpcPamPulse(int pin);
void mpcInit(void);
void mpcSleep(void);
void mpcStop(void);
Serial mpcPamPort(void);

#endif
