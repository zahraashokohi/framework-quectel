////////////////////////////////////////////////////////////////////////////////////////
//
// UART Copyright 2020 Georgi Angelov
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
////////////////////////////////////////////////////////////////////////////////////////

#ifndef HAL_UART_H_
#define HAL_UART_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <txm_module.h>
#include <qapi.h>
#include <qapi_uart.h>
#include <qapi_timer.h>
#include "interface.h"
#include "ring-buffer.h"

#define UART_BUFFER_SIZE    (1024)
#define UART_RING_SIZE      (4096)
#define UART_MAX_BUFFERS    (2) /* MAX 2 DONT TOUCH */

    typedef struct UART_BUF_S
    {
        int index;
        TX_EVENT_FLAGS_GROUP *event;
        int32_t size;
        uint8_t data[UART_BUFFER_SIZE];
    } UART_BUF_T;

    typedef struct UART_CONTEX_S
    {
        qapi_UART_Port_Id_e port_id;
        qapi_UART_Open_Config_t cfg;
        qapi_UART_Handle_t handle;
        TX_MUTEX *M_Transmiter, *M_Receiver, *M_Ring;
        TX_EVENT_FLAGS_GROUP *E_Signal;
        UCHAR *Stack;
        TX_THREAD *Thread;
        size_t wr_size;
        UART_BUF_T *uart_buffers; // [UART_MAX_BUFFERS];
        char *rx_buffer;          // [UART_RING_SIZE];
        ring_buffer_t *ring;
    } UART_CONTEX_T;
    typedef UART_CONTEX_T *UART;

    UART uart_create(int port, void *config);
    void uart_destroy(UART uart);
    
    int uart_open(UART uart);
    int uart_close(UART uart);

    size_t uart_write(UART uart, const char *buf, size_t size);
    inline int uart_write_r(struct _reent *r, _PTR ctx, const char *buf, int len) { return uart_write((UART)ctx, buf, len); };

    size_t uart_read(UART uart, char *dst, size_t size_to_read);
    inline int uart_read_r(struct _reent *r, _PTR ctx, char *buf, int len) { return uart_read((UART)ctx, buf, len); };

    int uart_ioctl(UART uart, int command, void *param);

    inline int uart_getchar(UART uart)
    {
        char c;
        return uart_read(uart, &c, 1) ? c : -1;
    }

    inline size_t uart_putchar(UART uart, char c) { return uart_write(uart, &c, 1); }

    int uart_peek(UART uart);

    size_t uart_available(UART uart);

#ifdef __cplusplus
}
#endif
#endif /* HAL_UART_H_ */