/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Quectel Co., Ltd. 2016
*
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   ql_uart.c
 *
 * Project:
 * --------
 *   OpenLinux
 *
 * Description:
 * ------------
 *   API implementation for UART driver.
 *
 * Author:
 * ------------
 * Stanley YONG
 * ------------
 *
 *============================================================================
 *             HISTORY
 *----------------------------------------------------------------------------
 * WHO            WHEN                WHAT
 *----------------------------------------------------------------------------
 * Stanley.YONG   18/07/2016		  Create
 * Stanley.YONG   03/09/2016		  Support more baudrate.
 ****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "ql_uart.h"


static int buildBaudrate(unsigned int baudrate, struct termios* term);
static int getBaudrate(struct termios* term);
static int buildDataBit(unsigned int dataBit, struct termios* term);
static int buildStopBit(unsigned int stop_bit, struct termios* term);
static int buildParity(Enum_ParityBits parity, struct termios* term);
static int buildFlowControl(Enum_FlowCtrl fc, struct termios* term);

int Ql_UART_Open(const char* port, unsigned int baudrate, Enum_FlowCtrl flowCtrl)
{
    int fd;
    struct termios term;
    
    fd = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);
    printf("open -%s, %s\n", port, strerror(errno));
    
    bzero(&term, sizeof(term));
    cfmakeraw(&term);
    term.c_cflag |= CREAD;
    tcgetattr(fd, &term);
    
    buildBaudrate(baudrate, &term);
    buildDataBit(8, &term);
    buildStopBit(1, &term);
    buildParity(PB_NONE, &term);
    buildFlowControl(flowCtrl, &term);

    term.c_iflag &= ~ICRNL;
    term.c_iflag &= ~INLCR;
    term.c_iflag |= IGNBRK;

    term.c_oflag &= ~OCRNL;
    term.c_oflag &= ~ONLCR;
    term.c_oflag &= ~OPOST;

    term.c_lflag &= ~ICANON;
    term.c_lflag &= ~ISIG;
    term.c_lflag &= ~IEXTEN;
    term.c_lflag &= ~(ECHO|ECHOE|ECHOK|ECHONL|ECHOCTL|ECHOPRT|ECHOKE);

    tcsetattr(fd, TCSANOW, &term);
    tcflush(fd, TCIOFLUSH);
	
	return fd;
}

int Ql_UART_SetDCB(int fd, ST_UARTDCB *dcb)
{
	return 0;
}
int Ql_UART_GetDCBConfig(int fd, ST_UARTDCB *dcb)
{
	return 0;
}

int Ql_UART_Write(int fd, const char* buf, unsigned int buf_len)
{
#define QL_UART_WRITE_MAX_SIZE 64
    size_t size;
    size_t size_to_wr;
    ssize_t size_written;

    for(size = 0; size < buf_len;)
	{
        size_to_wr = buf_len - size;
        if( size_to_wr > QL_UART_WRITE_MAX_SIZE)
            size_to_wr = QL_UART_WRITE_MAX_SIZE;

        size_written = write(fd, &buf[size], size_to_wr);

        if (size_written==-1) {
            printf("Cannot write on uart: %s",strerror(errno));
            return 0;
        }

        printf("Uart Write: %zd", size_written);
        size += size_written;

        if(size_written != size_to_wr)
            break;
    }

    return size;
}

int Ql_UART_Read (int fd, char* buf, unsigned int buf_len)
{
    return read(fd, buf, buf_len);
}

int Ql_UART_IoCtl(int fd, unsigned int cmd, void* pValue)
{
    return ioctl(fd, cmd, pValue);
}

int Ql_UART_Close(int fd)
{
    return close(fd);
}


static int buildParity(Enum_ParityBits parity, struct termios* term)
{
	if (PB_NONE == parity)
	{
		term->c_cflag &= ~PARENB;
	}
	else if (PB_ODD == parity)
	{
		term->c_cflag |= (PARENB | PARODD);
	}
	else if (PB_EVEN == parity)
	{
		term->c_cflag &= ~PARODD;
		term->c_cflag |= PARENB;
	}
	else {
		return -1;
	}
	
	return 0;
}

static int buildFlowControl(Enum_FlowCtrl fc, struct termios* term)
{
	if (FC_NONE == fc)
	{
		term->c_cflag |= CRTSCTS;
		term->c_iflag &= ~(IXON | IXOFF);
	}
	else if (FC_RTSCTS == fc)
	{
		term->c_cflag &= ~CRTSCTS;
		term->c_iflag |= (IXON | IXOFF);
	}
	else if (FC_XONXOFF == fc)
	{
		term->c_cflag &= ~CRTSCTS;
		term->c_iflag &= ~(IXON | IXOFF);
	}else{
		return -1;
	}
	
	return 0;
}

static int buildDataBit(unsigned int dataBit, struct termios* term)
{
	term->c_cflag &= ~CSIZE;
	
	switch (dataBit)
	{
	case 5:
		term->c_cflag |= CS5;
		break;
	case 6:
		term->c_cflag |= CS6;
		break;
	case 7:
		term->c_cflag |= CS7;
		break;
	case 8:
		term->c_cflag |= CS8;
		break;
	default:
		return -1;
	}
	return 0;
}

static int buildStopBit(unsigned int stop_bit, struct termios* term)
{
	if (stop_bit == 1)
	{
		term->c_cflag &= ~CSTOPB;
	}
	else if (stop_bit == 2)
	{
		term->c_cflag |= CSTOPB;
	} else {
		return -1;
	}
	
	return 0;
}

static int buildBaudrate(unsigned int baudrate, struct termios* term)
{
  switch (baudrate)
  {
    case 300:
    {
        cfsetspeed(term, B300);
        break;
    }
    case 600:
    {
        cfsetspeed(term, B600);
        break;
    }
    case 1200:
    {
        cfsetspeed(term, B1200);
        break;
    }
    case 2400:
    {
        cfsetspeed(term, B2400);
        break;
    }
    case 4800:
    {
        cfsetspeed(term, B4800);
        break;
    }
    case 9600:
    {
        cfsetspeed(term, B9600);
        break;
    }
    case 19200:
    {
        cfsetspeed(term, B19200);
        break;
    }
    case 38400:
    {
        cfsetspeed(term, B38400);
        break;
    }
    case 57600:
    {
        cfsetspeed(term, B57600);
        break;
    }
    case 115200:
    {
        cfsetspeed(term, B115200);
        break;
    }
    case 230400:
    {
        cfsetspeed(term, B230400);
        break;
    }
    case 460800:
    {
        cfsetspeed(term, B460800);
        break;
    }
    case 921600:
    {
        cfsetspeed(term, B921600);
        break;
    }
    default:
        printf("SERIAL: unsupported baudrate: %d\n", baudrate);
        return -1;
  }
  return 0;
}

static int getBaudrate(struct termios* term)
{
	speed_t s = cfgetispeed(term);
	if (s != cfgetospeed(term))
	printf("SERIAL: Input and Output speed are different. Check the serial configuration !\n");
	
	switch (s)
	{
	case B300:
		s = 300;
		break;
	case B600:
		s = 600;
		break;
	case B1200:
		s = 1200;
		break;
	case B2400:
		s = 2400;
		break;
	case B4800:
		s = 4800;
		break;
	case B9600:
	    s = 9600;
	    break;
	case B19200:
	    s = 19200;
	    break;
	case B38400:
	    s = 38400;
	    break;
	case B57600:
	    s = 57600;
	    break;
	case B115200:
	    s = 115200;
	    break;
	case B230400:
		s = 230400;
		break;
	case B460800:
		s = 460800;
		break;
	case B921600:
		s = 921600;
		break;
	default:
	    printf("SERIAL: Unknown speed setting: %d!\n", s);
	    s = 0;
	    break;
	}
	return s;
}
