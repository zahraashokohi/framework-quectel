#ifndef INTERFACE_H_
#define INTERFACE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <ctype.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>

    unsigned int seconds(void);
    unsigned int millis(void);
    unsigned int micros(void);
    void delay(unsigned int);
    void delayMicroseconds(unsigned int us);
    uint32_t utc(void);

    char *utoa(unsigned value, char *result, int base);
    char *ultoa(unsigned long value, char *result, int base);
    char *itoa(int value, char *result, int base);
    char *ltoa(long value, char *result, int base);

    uint32_t clockCyclesPerMicrosecond(void);
    uint32_t clockCyclesToMicroseconds(uint32_t a);
    uint32_t microsecondsToClockCycles(uint32_t a);

    int analogRead(uint8_t pin);
    void analogWrite(uint8_t pin, int val);  

#ifndef SERIAL_BUFFER_SIZE
#define SERIAL_BUFFER_SIZE 1024
#endif



#ifdef __cplusplus
} // close extern "C"
#endif
#endif /* INTERFACE_H_ */