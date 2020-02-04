#ifndef CHAT_H
#define CHAT_H (0x2000U)

#include <time.h>
#include <serial.h>

/* function prototype */
int chat(const struct SerialPort *port, const char *cmd, const char *expect, time_t sec, const char *trm);
int uchat(const struct SerialPort *port, const unsigned char *cmd, const unsigned char *expect, time_t sec, const unsigned char *trm);

#endif /* CHAT_H */
