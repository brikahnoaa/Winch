#ifndef RX_H
#define RX_H (0x1000U)

#include <stdio.h>
#include <serial.h>

/* prototypes for external functions */
long int Rx(const struct SerialPort *port,FILE *dest);

extern int CrcMode;
extern int BinMode;

#endif /* RX_H */
