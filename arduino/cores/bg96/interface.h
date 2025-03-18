#ifndef INTERFACE_H_
#define INTERFACE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <scanf.h>
#include <snprintf.h>

#include "dbg.h"
#include <qurt_timetick.h>
#include <qapi_types.h>
#include <qapi.h>
#include <qapi_status.h>
#include <qapi_uart.h>
#include <qapi_timer.h>
#include <qapi_device_info.h>
#include <qapi_dss.h>
#include <qapi_ns_utils.h>
#include <qapi_socket.h>
#include <qapi_dnsc.h>
#include <qapi_mqtt.h>
#include <qapi_httpc.h>
#include <qapi_adc.h>
#include <qapi_tlmm.h>
#include <qapi_i2c_master.h>
#include <qapi_spi_master.h>
#include <qapi_psm.h>
#include <qapi_fs_types.h>
#include <qapi_fs.h>

#define PRIO (180) // from preamble
#define TICKS_PER_MSEC (10)
#define DEFAUT_STACK_SIZE (4096)

    extern TX_BYTE_POOL *heap;

    unsigned int seconds(void);
    unsigned int millis(void);
    unsigned int micros(void);
    void delay(unsigned int);
    void delayMicroseconds(unsigned int us);

    int isascii(int c);
    int toascii(int c);
    long atol(const char *s);
    char *itoa(int value, char *result, int base);
    char *ltoa(long value, char *result, int base);
    char *utoa(unsigned value, char *result, int base);
    char *ultoa(unsigned long value, char *result, int base);

    uint32_t clockCyclesPerMicrosecond(void);
    uint32_t clockCyclesToMicroseconds(uint32_t a);
    uint32_t microsecondsToClockCycles(uint32_t a);

    int analogRead(uint8_t pin);
    void analogWrite(uint8_t pin, int val);      

#ifndef SERIAL_BUFFER_SIZE
#define SERIAL_BUFFER_SIZE 1024
#endif

#undef  assert
#define assert(__e)         ((__e) ? (void)0 : abort())

#define DELAY(T) 			qapi_Timer_Sleep(T, QAPI_TIMER_UNIT_MSEC, 1)

#define MUTEX_LOCK(M) 		tx_mutex_get(M, TX_WAIT_FOREVER)
#define MUTEX_UNLOCK(M) 	tx_mutex_put(M)

#define EVENT_WAIT(E, M) 	tx_event_flags_get(E, M, TX_OR_CLEAR, &signal, TX_WAIT_FOREVER)
#define EVENT_SEND(E, M) 	tx_event_flags_set(E, M, TX_OR);

#define ENTER_CRITICAL()  \
    do                    \
    {                     \
        __asm("cpsid i"); \
    } while (0)

#define EXIT_CRITICAL()   \
    do                    \
    {                     \
        __asm("cpsie i"); \
    } while (0)

    // for SPI
    __attribute__((always_inline)) static inline uint32_t __RBIT(uint32_t value)
    {
        uint32_t result;
        __asm volatile("rbit %0, %1"
                       : "=r"(result)
                       : "r"(value));
        return (result);
    }

    __attribute__((always_inline)) static inline uint32_t __REV(uint32_t value)
    {
        uint32_t result;
        __asm volatile("rev %0, %1"
                       : "=r"(result)
                       : "r"(value));
        return (result);
    }

    __attribute__((always_inline)) static inline uint32_t __REV16(uint32_t value)
    {
        uint32_t result;
        __asm volatile("rev16 %0, %1"
                       : "=r"(result)
                       : "r"(value));
        return (result);
    }

#define _htons(x) ((unsigned short)((((unsigned short)(x) & 0x00ff) << 8) | (((unsigned short)(x) & 0xff00) >> 8)))
#define _ntohs(x) ((unsigned short)((((unsigned short)(x) & 0x00ff) << 8) | (((unsigned short)(x) & 0xff00) >> 8)))

void replaceChar(char *str, char _old, char _new);
int strStartsWith(const char *line, const char *prefix);
int inet_ntoa(const unsigned int inaddr, unsigned char *buf, unsigned int len);

#ifdef __cplusplus
} // close extern "C"
#endif
#endif /* INTERFACE_H_ */