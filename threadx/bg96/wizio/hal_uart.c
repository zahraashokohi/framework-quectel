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

#include <hal_uart.h>

#define UART_STACK_SIZE     (2048)
#define UART_RX_EVENT_MASK  (0x00000003)
#define UART_TX_EVENT_MASK  (0x80000000)

static void uart_transmit_isr(uint32_t num_bytes, void *user)
{
    if (user && num_bytes)
    {
        UART u = (UART)user;
        u->wr_size = num_bytes;
        EVENT_SEND(u->E_Signal, UART_TX_EVENT_MASK);
    }
}

static void uart_receive_isr(uint32_t num_bytes, void *user)
{
    if (user && num_bytes)
    {
        UART_BUF_T *p = (UART_BUF_T *)user;
        p->size = num_bytes;
        EVENT_SEND(p->event, p->index);
    }
}

static void uart_thread_entry(void *p)
{
    UART uart = (UART)p;
    while (uart)
    {
        DELAY(10);
        if (uart->handle) // wait open
        {
            ULONG signal = 0;
            EVENT_WAIT(uart->E_Signal, UART_RX_EVENT_MASK);
            signal &= 0xF; // ignore tx
            if (signal)
            {
                /* GET BUFFER & COPY TO RING */
                UART_BUF_T *p = &uart->uart_buffers[signal >> 1];
                if (0 == MUTEX_LOCK(uart->M_Ring))
                    uart->ring->write(uart->ring, p->data, p->size);
                MUTEX_UNLOCK(uart->M_Ring);
                p->size = 0;
                if (uart->handle)
                    qapi_UART_Receive(uart->handle, p->data, UART_BUFFER_SIZE, p);
            }
        }
    }
}

static int uart_stop_receive(UART uart) /* called from uart_stop_receive() */
{
    if (uart->Thread)
    {
        DELAY(10);
        tx_thread_terminate(uart->Thread);
        DELAY(10);
        tx_thread_delete(uart->Thread);
        uart->Thread = NULL;
        return 0;
    }
    return -1;
}

static int uart_start_receive(UART uart) /* called form uart_open() */
{
    if (uart->handle)
    {
        if (tx_thread_create(uart->Thread, "thread-uart",
                             uart_thread_entry, uart,
                             uart->Stack, UART_STACK_SIZE,
                             180, 180, TX_NO_TIME_SLICE, TX_AUTO_START))
        {
            //DEBUG_UART("[ERROR] uart_start_receive() thread\n");
            return -1;
        }
        for (int i = 0; i < UART_MAX_BUFFERS /* MAX 2 */; i++) /* SET QUEUE RX BUFFERS */
        {
            uart->uart_buffers[i].size = 0;
            uart->uart_buffers[i].index = 1 << i;         // mark buffers
            uart->uart_buffers[i].event = uart->E_Signal; // instance of event

            if (qapi_UART_Receive(uart->handle, (char *)uart->uart_buffers[i].data, UART_BUFFER_SIZE, &uart->uart_buffers[i]))
            {
                //DEBUG_UART("[ERROR] uart_start_receive() receive\n");
                return -1;
            }
        }
        return 0;
    }
    //DEBUG_UART("[ERROR] uart_start_receive() handle\n");
    return -1;
}

void uart_destroy(UART uart)
{
    if (uart)
    {
        uart_close(uart);
        if (uart->M_Receiver)
        {
            tx_mutex_delete(uart->M_Receiver);
            txm_module_object_deallocate(uart->M_Receiver);
        }
        if (uart->M_Transmiter)
        {
            tx_mutex_delete(uart->M_Transmiter);
            txm_module_object_deallocate(uart->M_Transmiter);
        }
        if (uart->M_Ring)
        {
            tx_mutex_delete(uart->M_Ring);
            txm_module_object_deallocate(uart->M_Ring);
        }
        if (uart->E_Signal)
        {
            tx_event_flags_delete(uart->E_Signal);
            txm_module_object_deallocate(uart->E_Signal);
        }
        if (uart->Thread)
            txm_module_object_deallocate(uart->Thread);
        if (uart->uart_buffers)
            free(uart->uart_buffers);
        if (uart->rx_buffer)
            free(uart->rx_buffer);
        if (uart->ring)
            free(uart->ring);
        if (uart->Stack)
            free(uart->Stack);
        memset(uart, 0, sizeof(UART_CONTEX_T)); 
        free(uart);
        uart =  NULL;
    }
}

UART uart_create(int port, void *config)
{
    UART_CONTEX_T *uart = (UART_CONTEX_T *)calloc(1, sizeof(UART_CONTEX_T));
    if (uart)
    {
        uart->port_id = (qapi_UART_Port_Id_e)port;
        if (config)
        {
            memcpy(&uart->cfg, config, sizeof(qapi_UART_Open_Config_t));
        }
        else
        {
            /* SET DEFAULT */
            uart->cfg.baud_Rate = 115200;
            uart->cfg.bits_Per_Char = QAPI_UART_8_BITS_PER_CHAR_E;
            uart->cfg.num_Stop_Bits = QAPI_UART_1_0_STOP_BITS_E;
            uart->cfg.parity_Mode = QAPI_UART_NO_PARITY_E; 
            uart->cfg.enable_Flow_Ctrl = QAPI_FCTL_OFF_E;
        }

        if (NULL == uart->cfg.rx_CB_ISR)
            uart->cfg.rx_CB_ISR = uart_receive_isr;
        if (NULL == uart->cfg.tx_CB_ISR)
            uart->cfg.tx_CB_ISR = uart_transmit_isr;

        assert(0 == txm_module_object_allocate(&uart->M_Ring, sizeof(TX_MUTEX)));
        assert(0 == txm_module_object_allocate(&uart->M_Receiver, sizeof(TX_MUTEX)));
        assert(0 == txm_module_object_allocate(&uart->M_Transmiter, sizeof(TX_MUTEX)));
        assert(0 == txm_module_object_allocate(&uart->E_Signal, sizeof(TX_EVENT_FLAGS_GROUP)));
        assert(0 == txm_module_object_allocate(&uart->Thread, sizeof(TX_THREAD)));
        assert(0 == tx_mutex_create(uart->M_Ring, "mutex-uart-ring", TX_INHERIT));
        assert(0 == tx_mutex_create(uart->M_Receiver, "mutex-uart-rx", TX_INHERIT));
        assert(0 == tx_mutex_create(uart->M_Transmiter, "mutext-uart-tx", TX_INHERIT));
        assert(0 == tx_event_flags_create(uart->E_Signal, "event-uart"));
        assert((uart->Stack = (UCHAR *)malloc(UART_STACK_SIZE)));
        assert((uart->uart_buffers = (UART_BUF_T *)calloc(1, sizeof(UART_BUF_T) * UART_MAX_BUFFERS)));
        assert((uart->rx_buffer = (char *)calloc(1, UART_RING_SIZE)));
        assert((uart->ring = (ring_buffer_t *)calloc(1, sizeof(ring_buffer_t))));
        ring_buffer_init(uart->ring, UART_RING_SIZE, uart->rx_buffer);
    }
    return uart;
}

int uart_close(UART uart)
{
    if (uart)
    {
        if (uart->handle)
        {
            //MUTEX_LOCK
            qapi_UART_Power_Off(uart->handle);
            qapi_UART_Close(uart->handle);
            uart->handle = NULL;
            uart_stop_receive(uart); // remove thread
            uart->ring->reset(uart->ring);
            //MUTEX_UNLOCK
            return 0;
        }
    }
    return -1;
}

int uart_open(UART uart)
{
    assert(uart);
    if (uart->handle)
        return -2; // already
    if (0 == qapi_UART_Open(&uart->handle, uart->port_id, &uart->cfg))
    {
        if (0 == qapi_UART_Power_On(uart->handle) && 0 == uart_start_receive(uart))
        {
            return 0;
        }
        else
        {
            if (uart->handle)
            {
                qapi_UART_Close(uart->handle);
                uart->handle = NULL;
            }
            
        }
    }
    return -1;
}

size_t uart_write(UART uart, const char *src, size_t size_to_write)
{
    int result = 0;
    if (uart)
    {
        if (uart->handle && src && size_to_write)
        {
            if (0 == MUTEX_LOCK(uart->M_Transmiter))
            {
                uart->wr_size = 0;
                if (qapi_UART_Transmit(uart->handle, src, size_to_write, uart))
                {
                    //DEBUG_UART("[ERROR] uart_write() transmit\n");
                    goto unlock;
                }
                /* WAIT READY */
                ULONG signal;
                if (0 == EVENT_WAIT(uart->E_Signal, UART_TX_EVENT_MASK))
                {
                    result = uart->wr_size;
                }
                else
                {
                    //DEBUG_UART("[ERROR] uart_write() event\n");
                }
            }
            else
            {
               //DEBUG_UART("[ERROR] uart_write() lock\n");
            }
        unlock:
            MUTEX_UNLOCK(uart->M_Transmiter);
        }
    }
    else
    {
        //DEBUG_UART("[ERROR] uart_write() uart\n");
    }
    return result;
}

size_t uart_read(UART uart, char *dst, size_t size)
{
    int result = 0;
    if (uart)
    {
        if (uart->handle && dst && size)
        {
            if (0 == MUTEX_LOCK(uart->M_Receiver))
            {
                if (0 == MUTEX_LOCK(uart->M_Ring))
                    result = uart->ring->read(uart->ring, dst, size);
                MUTEX_UNLOCK(uart->M_Ring);
            }
            MUTEX_UNLOCK(uart->M_Receiver);
        }
    }
    return result;
}

int uart_peek(UART uart)
{
    int c = -1;
    if (uart)
    {
        if (uart->handle)
        {
            if (0 == MUTEX_LOCK(uart->M_Ring))
                if (1 == uart->ring->peek(uart->ring, &c, 1))
                    c &= 0xFF;
            MUTEX_UNLOCK(uart->M_Ring);
        }
    }
    return c;
}

size_t uart_available(UART uart)
{
    size_t available = 0;
    if (uart)
    {
        if (uart->handle)
        {
            if (0 == MUTEX_LOCK(uart->M_Ring))
                available = UART_RING_SIZE - uart->ring->available(uart->ring);
            MUTEX_UNLOCK(uart->M_Ring);
        }
    }
    return available;
}

int uart_ioctl(UART uart, int command, void *param)
{
    if (uart)
        return (uart->handle) ? qapi_UART_Ioctl(uart->handle, (qapi_UART_Ioctl_Command_e)command, param) : -1;
    return -1;
}

