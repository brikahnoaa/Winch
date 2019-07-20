#ifndef TX_H
#define TX_H (0x1000U)

#include <stdio.h>
#include <serial.h>

/* prototypes for external functions */
long int Tx(const struct SerialPort *port,FILE *source);

#endif /* TX_H */
