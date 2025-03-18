#include "custom_feature_def.h"
#include "ql_type.h"
#include "ql_stdlib.h"
#include "ql_uart.h"
#include "ril.h"
#include <stdint.h>
#include <stddef.h>

char DBG_BUFFER[256];
#define DBG_PORT UART_PORT0
#define DBG(FORMAT, ...)                                                  \
    {                                                                     \
        Ql_memset(DBG_BUFFER, 0, sizeof(DBG_BUFFER));                     \
        Ql_sprintf(DBG_BUFFER, FORMAT, ##__VA_ARGS__);                    \
        Ql_UART_Write(DBG_PORT, (u8 *)DBG_BUFFER, Ql_strlen(DBG_BUFFER)); \
    }

void uart_callback(Enum_SerialPort port, Enum_UARTEventType msg, bool level, void *customizedPara)
{
}

void proc_main_task(s32 taskId)
{
    s32 ret;
    ST_MSG msg;
    Ql_UART_Register(DBG_PORT, uart_callback, NULL);
    Ql_UART_Open(DBG_PORT, 115200, FC_NONE);
    DBG("[APP] Begin\n");
    while (1)
    {
        Ql_OS_GetMessage(&msg);
        switch (msg.message)
        {
        case MSG_ID_RIL_READY:
            Ql_RIL_Initialize();
            DBG("[APP] Ril Ready\n");
            break;
        }
    }
}