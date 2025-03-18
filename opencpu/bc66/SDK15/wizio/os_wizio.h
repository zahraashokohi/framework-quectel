////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 Georgi Angelov
//      Basic library and helpers for Arduino
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

#ifndef OS_WIZIO_H_
#define OS_WIZIO_H_
#ifdef __cplusplus
extern "C"
{
#endif

#define _POSIX_THREADS 1
#define _POSIX_THREAD_PRIO_PROTECT 1
#include <errno.h>
#include <assert.h>
#include "os_api.h"

#define TICK_MS (10L)
#define DELAY(T) Ql_Sleep(T)
#define DELAY_U(T) Ql_Delay_us(T)
#define DELAY_M(T) Ql_Delay_ms(T)
#define SLEEP(T) Ql_Sleep(T * 1000)
#define HAL_MILLIS() (1000L * *((volatile uint32_t *)0xA2130118) >> 15) /* rollover at 0x20000 ms */
#define MILLIS() (unsigned int)(Ql_OS_GetTaskTickCount() * TICK_MS)
#define SECONDS() (MILLIS() / 1000)

#define MUTEX_LOCK(M) Ql_OS_TakeMutex(M, -1)
#define MUTEX_UNLOCK(M) Ql_OS_GiveMutex(M)
#define EVENT_WAIT(E, M) Ql_OS_WaitEvent(E, M, -1)
#define EVENT_SEND(E, M) Ql_OS_SetEvent(E, M)

    typedef enum
    {
        M_START = MSG_ID_USER_START + 10,
        M_UART_OPEN,
        M_UART_CLOSE,
        M_TIMER_START,
        M_TIMER_STOP,
        M_TIMER_CLOSE,
        M_EINT_OPEN,
        M_EINT_CLOSE,
        M_END,
    } E_SYSCALL;
    int SYSCALL(int M, int A, int B); /* for arduino */

    void proc_main_task(int taskId);
    void proc_arduino(int taskId);
    void os_init(void);
    void os_api_setup(void);
    time_t now(void); /* get rtc in time_t format */

#ifdef __cplusplus
}
#endif
#endif
