////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 Georgi Angelov
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////

#ifndef INTERFACE_H_
#define INTERFACE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <os_wizio.h>
#include <os_dbg.h>
#include <os_api.h>
#include <os_ril.h>

#include <_ansi.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>

#include "ql_common.h"
#include "ql_type.h"
#include "ql_system.h"
#include "ql_memory.h"
#include "ql_stdlib.h"
#include "ql_trace.h"
#include "ql_error.h"
#include "ql_uart.h"
#include "ql_gpio.h"
#include "ql_adc.h"
#include "ql_pwm.h"
#include "ql_spi.h"
#include "ql_iic.h"
#include "ql_eint.h"
#include "ql_power.h"
#include "ql_rtc.h"
#include "ql_time.h"
#include "ql_timer.h"

#include "ril.h"
#include "ril_system.h"
#include "ril_util.h"

#define SYSCALL_TASK_ID 0 /* is main task */
#define ARDUINO_TASK_ID 3 /* arduino_task_id */

    void setOnUrc(void (*on_urc)(ST_MSG *msg));
    bool getVersion(char version[32]);
    void init_api(void);

    static inline unsigned int seconds(void) { return SECONDS(); }
    static inline unsigned int millis(void) { return MILLIS(); }
    unsigned int micros(void);
    static inline void delay(unsigned int ms) { DELAY(ms); }
    static inline void delayMicroseconds(unsigned int us) { DELAY_U(us); }

    char *itoa(int value, char *result, int base);
    char *utoa(unsigned int value, char *buffer, int radix);
    static inline char *ltoa(long value, char *result, int base) { return utoa(value, result, base); }
    static inline char *ultoa(unsigned long value, char *result, int base) { return utoa(value, result, base); }

    uint32_t clockCyclesPerMicrosecond(void);
    uint32_t clockCyclesToMicroseconds(uint32_t a);
    uint32_t microsecondsToClockCycles(uint32_t a);

    int isascii(int c);
    int toascii(int c);

#ifndef SERIAL_BUFFER_SIZE
#define SERIAL_BUFFER_SIZE 1024
#endif

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

#ifdef __cplusplus
} // extern "C"

#endif //__cplusplus

#endif /* INTERFACE_H_ */
