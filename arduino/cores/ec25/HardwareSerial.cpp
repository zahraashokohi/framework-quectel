/*
    Created on: 01.01.2019
    Author: Georgi Angelov
        http://www.wizio.eu/
        https://github.com/Wiz-IO    

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

// sudo chmod 666 /dev/ttyS0
// dmesg | grep tty

#include "HardwareSerial.h"
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions 	   	   */
#include <errno.h>   /* ERROR Number Definitions           */

#define SERIAL_DEBUG ::printf

HardwareSerial::HardwareSerial() // for console
{
    port_name = NULL;
    fd = -1;
    peeked = 0;
    pk = 0;
}

HardwareSerial::HardwareSerial(const char *name) // Serial(/dev/ttyUSB0")
{
    port_name = (char *)name;
    fd = -1;
    peeked = 0;
    pk = 0;
}

void HardwareSerial::setName(const char *name)
{
    port_name = (char *)name;
}

void HardwareSerial::begin(unsigned long brg)
{
    if (brg == 0 || port_name == NULL)
        return;
    switch (brg)
    {
    case 50:
        brg = B50;
        break;
    case 75:
        brg = B75;
        break;
    case 110:
        brg = B110;
        break;
    case 134:
        brg = B134;
        break;
    case 150:
        brg = B150;
        break;
    case 200:
        brg = B200;
        break;
    case 300:
        brg = B300;
        break;
    case 600:
        brg = B600;
        break;
    case 1200:
        brg = B1200;
        break;
    case 1800:
        brg = B1800;
        break;
    case 2400:
        brg = B2400;
        break;
    case 4800:
        brg = B4800;
        break;
    case 9600:
        brg = B9600;
        break;
    case 19200:
        brg = B19200;
        break;
    case 38400:
        brg = B38400;
        break;
    case 57600:
        brg = B57600;
        break;
    case 115200:
        brg = B115200;
        break;
    case 230400:
        brg = B230400;
        break;
    case 460800:
        brg = B460800;
        break;
    case 500000:
        brg = B500000;
        break;
    case 576000:
        brg = B576000;
        break;
    case 921600:
        brg = B921600;
        break;
    case 1000000:
        brg = B1000000;
        break;
    case 1152000:
        brg = B1152000;
        break;
    case 1500000:
        brg = B1500000;
        break;
    case 2000000:
        brg = B2000000;
        break;
    case 2500000:
        brg = B2500000;
        break;
    case 3000000:
        brg = B3000000;
        break;
    case 3500000:
        brg = B3500000;
        break;
    case 4000000:
        brg = B4000000;
        break;
    default:
        SERIAL_DEBUG("[ERROR] Serial invalid brg\n");
        return;
    }
    fd = open(port_name, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)
    {
        SERIAL_DEBUG("[ERROR] Serial open %s ( %d )\n", port_name, fd);
        return;
    }
    
    fcntl(fd, F_SETFL, FNDELAY); // read no delay

    /*---------- Setting the Attributes of the serial port using termios structure --------- */
    struct termios SerialPortSettings;                             /* Create the structure                          */
    tcgetattr(fd, &SerialPortSettings);                            /* Get the current attributes of the Serial port */
    cfsetispeed(&SerialPortSettings, brg);                         /* Set Read  Speed                        */
    cfsetospeed(&SerialPortSettings, brg);                         /* Set Write Speed                        */
    SerialPortSettings.c_cflag &= ~PARENB;                         /* Disables the Parity Enable bit(PARENB),So No Parity   */
    SerialPortSettings.c_cflag &= ~CSTOPB;                         /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
    SerialPortSettings.c_cflag &= ~CSIZE;                          /* Clears the mask for setting the data size             */
    SerialPortSettings.c_cflag |= CS8;                             /* Set the data bits = 8                                 */
    SerialPortSettings.c_cflag &= ~CRTSCTS;                        /* No Hardware flow Control                         */
    SerialPortSettings.c_cflag |= CREAD | CLOCAL;                  /* Enable receiver,Ignore Modem Control lines       */
    SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);         /* Disable XON/XOFF flow control both i/p and o/p */
    SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG); /* Non Cannonical mode                            */
    SerialPortSettings.c_oflag &= ~OPOST;                          /*No Output Processing*/
    if ((tcsetattr(fd, TCSANOW, &SerialPortSettings)) != 0)        /* Set the attributes to the termios structure*/
    {
        SERIAL_DEBUG("[ERROR] Serial setting attributes\n");
    }
    /* Flush Port, then applies attributes */
    tcflush(fd, TCIFLUSH);
    flush();
}

void HardwareSerial::end()
{
    if (fd == -1)
        return;
    flush();    
    close(fd);
    fd = -1;
}

int HardwareSerial::read(void)
{
    if (fd == -1)
        return 0;
    if (peeked)
    {
        peeked = 0;
        return pk;
    }
    char b;
    return ::read(fd, &b, 1) == 1 ? b : -1;
}

size_t HardwareSerial::write(uint8_t b)
{
    if (NULL == port_name)
    {
        ::printf("%c", b);
        return 1;
    }
    if (fd == -1)
        return 0;
    int w = ::write(fd, &b, sizeof(b));
    return w == 1 ? w : 0;
}

int HardwareSerial::available(void)
{
    if (fd == -1)
        return 0;
    int avail;
    int rc = ioctl(fd, FIONREAD, &avail);
    return rc ? 0 : avail;
}

int HardwareSerial::peek(void)
{
    if (fd == -1)
        return -1;
    if (!available())
        return -1;
    if (peeked)
    {
        return pk;
    }
    else
    {
        pk = read();
        if (pk != -1)
        {
            peeked = 1;
            return pk;
        }
    }
    return -1;
}

void HardwareSerial::flush(void)
{
    if (fd == -1)
        return;
    //ioctl(fd, TCFLSH, 0); // flush receive
    //ioctl(fd, TCFLSH, 1); // flush transmit
    ioctl(fd, TCFLSH, 2); // flush both
}
