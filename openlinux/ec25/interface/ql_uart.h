/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Quectel Co., Ltd. 2016
*
*****************************************************************************/
#ifndef __QL_UART_H__
#define __QL_UART_H__
#include "ql_oe.h"

typedef enum {
    FC_NONE = 0,  // None Flow Control
    FC_RTSCTS,    // Hardware Flow Control (rtscts)
    FC_XONXOFF    // Software Flow Control (xon/xoff)
}Enum_FlowCtrl;

typedef enum {
    PB_NONE = 0,
    PB_ODD,
    PB_EVEN,
    PB_SPACE,
    PB_MARK
}Enum_ParityBits;

typedef struct {
    unsigned int        baudrate; 
    unsigned int       	dataBits;
    unsigned int       	stopBits;
    Enum_ParityBits     parity;
    Enum_FlowCtrl       flowCtrl;
}ST_UARTDCB;


int Ql_UART_Open(const char* port, unsigned int baudrate, Enum_FlowCtrl flowCtrl);
int Ql_UART_Read(int fd, char* buf, unsigned int buf_len);
int Ql_UART_Write(int fd, const char* buf, unsigned int buf_len);
int Ql_UART_SetDCB(int fd, ST_UARTDCB *dcb);
int Ql_UART_GetDCBConfig(int fd, ST_UARTDCB *dcb);
int Ql_UART_IoCtl(int fd, unsigned int cmd, void* pValue);
int Ql_UART_Close(int fd);

#endif //__QL_UART_H__
