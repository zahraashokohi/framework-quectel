/*
    Created on: 01.01.2019
    Author: Georgi Angelov

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA   
 */

#include "dbg.h"
#include <qapi_timer.h>

static qapi_UART_Handle_t debugPort = NULL;

void debug_enable(void *uartPort)
{
    debugPort = (qapi_UART_Handle_t *)uartPort;
}

void debug_disable(void)
{
    debugPort = NULL;
}

#ifdef ENABLE_DEBUG

#define DEBUG_PORT QAPI_UART_PORT_002_E

qapi_UART_Handle_t DEBUG_UART;

static void dbg_uart_rx_cb(uint32_t num_bytes, void *cb_data) {}
static void dbg_uart_tx_cb(uint32_t num_bytes, void *cb_data) {}

void dbg_init(void)
{
    qapi_UART_Open_Config_t cfg;
    memset(&cfg, 0, sizeof(qapi_UART_Open_Config_t));
    cfg.baud_Rate = 115200;
    cfg.bits_Per_Char = QAPI_UART_8_BITS_PER_CHAR_E;
    cfg.num_Stop_Bits = QAPI_UART_1_0_STOP_BITS_E;
    cfg.rx_CB_ISR = (qapi_UART_Callback_Fn_t)&dbg_uart_rx_cb;
    cfg.tx_CB_ISR = (qapi_UART_Callback_Fn_t)&dbg_uart_tx_cb;
    qapi_UART_Open(&DEBUG_UART, DEBUG_PORT, &cfg);
    qapi_UART_Power_On(DEBUG_UART);
    debug_enable(DEBUG_UART);
}

static char dbgBuffer[1024];
int log_printf(const char *frm, ...)
{
    if (NULL == frm || NULL == debugPort)
        return 0;
    va_list arg;
    va_start(arg, frm);
    int n = vsnprintf(dbgBuffer, sizeof(dbgBuffer), frm, arg);
    va_end(arg);
    if (n > 0)
    {
        n = qapi_UART_Transmit(debugPort, dbgBuffer, n, NULL);
        qapi_Timer_Sleep(10, QAPI_TIMER_UNIT_MSEC, 1);
    }
    return n;
}

static char bufferDump[1024];
void log_buf(const char *text, const unsigned char *buf, unsigned int len)
{
    if (!text || !buf || !len)
        return;
    char txt[17];
    unsigned int i, idx = 0;
    snprintf(bufferDump + idx, sizeof(bufferDump) - idx, "[DMP] %s 0x%08X (%u bytes)\n", text, (int)buf, (unsigned int)len);
    log_printf(bufferDump);
    idx = 0;
    memset(txt, 0, sizeof(txt));
    for (i = 0; i < len; i++)
    {
        if (i >= 4096)
            break;
        if (i % 16 == 0)
        {
            if (i > 0)
            {
                snprintf(bufferDump + idx, sizeof(bufferDump) - idx, "  %s\n", txt);
                log_printf(bufferDump);
                idx = 0;
                memset(txt, 0, sizeof(txt));
            }
            idx += snprintf(bufferDump + idx, sizeof(bufferDump) - idx, "%04X: ", (unsigned int)i);
        }
        idx += snprintf(bufferDump + idx, sizeof(bufferDump) - idx, " %02X", (unsigned int)buf[i]);
        txt[i % 16] = (buf[i] > 31 && buf[i] < 127) ? buf[i] : '.';
    }
    if (len > 0)
    {
        for (/* i = i */; i % 16 != 0; i++)
            idx += snprintf(bufferDump + idx, sizeof(bufferDump) - idx, "   ");
        snprintf(bufferDump + idx, sizeof(bufferDump) - idx, "  %s\n", txt);
        log_printf(bufferDump);
    }
}

#endif