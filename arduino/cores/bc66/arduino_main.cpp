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

#include <interface.h>
#include <os_uart.h>
#include <os_timer.h>
#include <os_eint.h>

extern void initVariant();
extern void setup();
extern void loop();

static void (*on_urc)(ST_MSG *msg) = NULL;
void setOnUrc(void (*onUrc)(ST_MSG *msg)) { on_urc = onUrc; }

static void *arduino_task_handle = NULL;

/// Main Task ID = 0
extern "C" void proc_main_task(int taskId)
{
    debug_init(UART_PORT0); // weak
    os_init();              // init API & CPP
    ST_MSG m;
    UART uart;
    EINT eint;
    TIMER timer;
    int owner_task = 0, res = -1000;
    while (true)
    {
        Ql_OS_GetMessage(&m);
        switch (m.message)
        {
        case MSG_ID_RIL_READY:
            Ql_RIL_Initialize();
            if (arduino_task_handle)
                Ql_OS_TaskResume(arduino_task_handle); // run arduino task
            continue;
        case MSG_ID_URC_INDICATION:         
            if (on_urc && 101 != m.param1)
                on_urc(&m);
            continue;
        } // SWITCH

        if (m.message < M_START || m.message >= M_END)
            continue;

        /*** SYSTEM CALLS ***/
        if (m.param1)
        {
            owner_task = *(int *)m.param1;
            uart = (UART)m.param1;
            eint = (EINT)m.param1;
            timer = (TIMER)m.param1;
            switch (m.message)
            {
                /*** UARTS ***/
            case M_UART_OPEN:
                res = uart_begin(uart, m.param2);
                break;
            case M_UART_CLOSE:
                Ql_UART_Close(uart->port);
                res = 0;
                break;

                /*** TIMERS ***/
            case M_TIMER_START:
                res = timer_begin(timer);
                break;
            case M_TIMER_STOP:
                res = Ql_Timer_Stop(timer->id);
                break;
            case M_TIMER_CLOSE:
                res = Ql_Timer_Delete(timer->id);
                break;

                /*** EINTS ***/
            case M_EINT_OPEN:
                res = eint_begin(eint);
                break;
            case M_EINT_CLOSE:
                res = Ql_EINT_Uninit(eint->pin);
                break;

            default:
                res = -1000;
                owner_task = 0;
                continue;
            } // SWITCH

            if (owner_task)
                Ql_OS_SendMessage(owner_task, m.message, res, 0); // send 'res' to owner task
        }

    } //WHILE
}

/// Arduino Task ID = 3
extern "C" void proc_arduino(int taskId)
{
    arduino_task_handle = Ql_OS_GetCurrentTaskHandle();
    Ql_OS_TaskSuspend(arduino_task_handle); // wait ril ready
    Ql_Sleep(1);                            // yield
    initVariant();
    setup();
    while (true)
    {
        loop();
        Ql_Sleep(1); // is 10ms
    }
}
