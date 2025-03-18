/*
    ThreadX BG96 - MAIN
        Created on: 01.01.2019
        Author: Georgi Angelov  
        http://www.wizio.eu/
        https://github.com/Wiz-IO/platform-quectel        
 */

#include <_ansi.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include <qapi_types.h>
#include <qapi.h>
#include <qapi_status.h>
#include <qapi_uart.h>
#include <qapi_timer.h>
#include <qapi_device_info.h>
#include <qurt_timetick.h>
#include <quectel_utils.h>
#include <quectel_uart_apis.h>

void *_sbrk(intptr_t increment) { return (void *)-1; }

void abort(void)
{
    while (1) // dont block app
        qapi_Timer_Sleep(100, QAPI_TIMER_UNIT_SEC, 1); 
}

TX_BYTE_POOL *heap;
static char heap_buffer[HEAP]; // build_flags = -D HEAP=xxx
void heap_init(void)
{
    if (txm_module_object_allocate(&heap, sizeof(TX_BYTE_POOL)))
        abort();
    if (tx_byte_pool_create(heap, "heap_byte_pool", heap_buffer, HEAP))
        abort();
}

static char *rx_buff = NULL;
static char *tx_buff = NULL;
static QT_UART_CONF_PARA uart_conf;

int quectel_dbg_uart_init(qapi_UART_Port_Id_e port_id)
{
    if (tx_byte_allocate(heap, (VOID *)&rx_buff, 4 * 1024, TX_NO_WAIT))
        abort();
    if (tx_byte_allocate(heap, (VOID *)&tx_buff, 4 * 1024, TX_NO_WAIT))
        abort();
    uart_conf.hdlr = NULL;
    uart_conf.port_id = port_id;
    uart_conf.tx_buff = tx_buff;
    uart_conf.tx_len = sizeof(tx_buff);
    uart_conf.rx_buff = rx_buff;
    uart_conf.rx_len = sizeof(rx_buff);
    uart_conf.baudrate = 115200;
    uart_init(&uart_conf); // open uart
    return TX_SUCCESS;
}

__attribute__((section(".library"))) int TXM_MODULE_THREAD_ENTRY(void)
{
    heap_init();
    if (quectel_dbg_uart_init(QAPI_UART_PORT_002_E)) // QAPI_UART_PORT_002_E ---> EVB COM2(DEBUG)
        abort();
    qt_uart_dbg(uart_conf.hdlr, "\nThreadX BG96 Hello World");
    while (1)
    {
        qt_uart_dbg(uart_conf.hdlr, "LOOP");
        tx_thread_sleep(500); // only here or qapi_Timer_Sleep()
    }
}
