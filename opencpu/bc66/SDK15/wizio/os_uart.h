////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 Georgi Angelov
//      Simple UART driver, Arduino style, used for Serial class
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

#ifndef OS_UART_H_
#define OS_UART_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stddef.h>
#include <ql_uart.h>
#include "ring-buffer.h"

#ifndef SERIAL_BUFFER_SIZE
#define SERIAL_BUFFER_SIZE 1024
#endif

#define UART_MAX_PORTS 3

    typedef struct uart_ctx_s
    {
        int task;             // owner task
        Enum_SerialPort port; // fd
        ST_UARTDCB dcb;
        unsigned int M_Ring;
        ring_buffer_t ring;
        char ring_buffer[SERIAL_BUFFER_SIZE];

    } uart_ctx;
    typedef uart_ctx *UART;

    int uart_begin(UART, bool); /* private */

    void uart_destroy(UART uart);
    UART uart_create(int port);
    void uart_retarget(UART uart);
    int uart_open(UART uart, int brg, bool retarget);
    int uart_open_ex(UART uart, ST_UARTDCB *dcb);
    int uart_close(UART uart);
    size_t uart_read(UART uart, char *dst, size_t size);
    size_t uart_write(UART uart, const char *src, size_t size);
    size_t uart_available(UART uart);
    int uart_peek(UART uart);
    void uart_flush(UART uart);

    void uart_callback(Enum_SerialPort port, Enum_UARTEventType msg, bool level, void *customizedPara);

#ifdef __cplusplus
}
#endif
#endif