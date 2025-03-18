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

#include "os_wizio.h"
#include "os_uart.h"
#include "os_dbg.h"
#include <stdio.h>
#include <stdlib.h>
#include <reent.h>
#include <unistd.h>

void uart_callback(Enum_SerialPort port, Enum_UARTEventType event, bool level, void *user)
{
    uint8_t b;
    UART uart = user;
    if (EVENT_UART_READY_TO_READ == event && uart)
    {
        if (MUTEX_LOCK(uart->M_Ring))
        {
            while (1 == Ql_UART_Read(uart->port, &b, 1))
                if (uart->ring.write(&uart->ring, &b, 1) < 0)
                    break;
            MUTEX_UNLOCK(uart->M_Ring);
        }
    }
}

UART uart_create(int port)
{
    UART uart = calloc(1, sizeof(uart_ctx));
    if (uart)
    {
        uart->dcb.flowCtrl = FC_NONE;
        uart->port = port;
        uart->M_Ring = Ql_OS_CreateMutex();
        ring_buffer_init(&uart->ring, SERIAL_BUFFER_SIZE, uart->ring_buffer);
        uart->ring.reset(&uart->ring);
    }
    return uart;
}

void uart_destroy(UART uart)
{
    uart_close(uart);
    free(uart);
}

/* private */
int uart_begin(UART u, bool ex)
{
    int res = -1;
    if (u)
    {
        res = Ql_UART_Register(u->port, uart_callback, u);
        if (0 == res)
        {
            if (ex)
                res = Ql_UART_OpenEx(u->port, &u->dcb);
            else
                res = Ql_UART_Open(u->port, u->dcb.baudrate, u->dcb.flowCtrl);
        }
    }
    return res;
}

int uart_open(UART uart, int brg, bool retarget)
{
    int res = -1;
    if (uart)
    {
        uart->dcb.baudrate = brg;
        uart->task = Ql_OS_GetActiveTaskId();
#ifdef ARDUINO
        res = SYSCALL(M_UART_OPEN, uart, false);
#else
        res = uart_begin(uart, false);
#endif
        if (0 == res && retarget)
            uart_retarget(uart);
    }
    return res;
}

int uart_open_ex(UART uart, ST_UARTDCB *dcb)
{
    int res = -1;
    if (uart && dcb)
    {
        uart->task = Ql_OS_GetActiveTaskId();
#ifdef ARDUINO
        res = SYSCALL(M_UART_OPEN, uart, true);
#else
        res = uart_begin(uart, true);
#endif
    }
    return res;
}

int uart_close(UART uart)
{
    int res = -1;
    if (uart)
    {
#ifdef ARDUINO
        res = SYSCALL(M_UART_CLOSE, uart, 0);
#else
        Ql_UART_Close(uart->port);
        res = 0;
#endif
    }
    return res;
}

size_t uart_write(UART uart, const char *src, size_t size)
{
    int res = 0;
    if (uart && src && size)
        res = Ql_UART_Write(uart->port, (u8 *)src, size);
    return res;
}
int uart_write_r(struct _reent *ignore, _PTR ctx, const char *buf, int len)
{
    return (ctx) ? uart_write((UART)ctx, buf, len) : 0;
}

size_t uart_read(UART uart, char *dst, size_t size)
{
    int result = 0;
    if (uart)
    {
        if (dst && size)
        {
            if (MUTEX_LOCK(uart->M_Ring))
            {
                result = uart->ring.read(&uart->ring, dst, size);
                MUTEX_UNLOCK(uart->M_Ring);
            }
        }
    }
    return result;
}
int uart_read_r(struct _reent *ignore, _PTR ctx, char *buf, int len)
{
    return (ctx) ? uart_read((UART)ctx, buf, len) : 0;
}

size_t uart_available(UART uart)
{
    size_t available = 0;
    if (uart)
    {
        if (MUTEX_LOCK(uart->M_Ring))
        {
            available = SERIAL_BUFFER_SIZE - uart->ring.available(&uart->ring);
            MUTEX_UNLOCK(uart->M_Ring);
        }
    }
    return available;
}

int uart_peek(UART uart)
{
    int c = -1;
    if (uart)
    {
        if (MUTEX_LOCK(uart->M_Ring))
        {
            if (1 == uart->ring.peek(&uart->ring, &c, 1))
                c &= 0xFF;
            MUTEX_UNLOCK(uart->M_Ring);
        }
    }
    return c;
}
void uart_flush(UART uart)
{
    if (uart)
        uart->ring.reset(&uart->ring);
}

extern void __sinit(struct _reent *s);
void uart_retarget(UART uart)
{
    __sinit(_impure_ptr);
    if (uart)
    {
        /* STDOUT */
        stdout->_cookie = uart;
        stdout->_write = uart_write_r;
        stdin->_read = 0;
        setvbuf(stdout, NULL, _IONBF, 0);

        /* STDERR */
        stderr->_cookie = uart;
        stderr->_write = uart_write_r;
        stdin->_read = 0;
        setvbuf(stderr, NULL, _IONBF, 0);

        /* STDIN */
        stdin->_cookie = uart;
        stdin->_read = uart_read_r;
        stderr->_write = 0;
        setvbuf(stdin, NULL, _IONBF, 0);

        //printf("[SERIAL-%d] READY\n", uart->port);
    }
}