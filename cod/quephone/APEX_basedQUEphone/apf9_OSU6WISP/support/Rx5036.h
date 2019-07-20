#ifndef RX5036_H
#define RX5036_H

#include <stdio.h>
#include <serial.h>

/* prototypes for external functions */
long int Rx5036(const struct SerialPort *port,FILE *dest);

extern int CrcMode;
extern int BinMode;

#endif /* RX5036_H */
