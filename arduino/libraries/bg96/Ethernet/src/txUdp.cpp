/*
  txUdp.cpp: Library to send/receive UDP packets.

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

#include "txUdp.h"
#include "txDns.h"

#define DEBUG_UDP DBG

#undef write
#undef read

txUDP::txUDP()
    : m_socket(-1),
      server_port(0),
      remote_port(0),
      tx_buffer(0),
      tx_buffer_len(0),
      rx_buffer(0)
{
}

txUDP::~txUDP()
{
    stop();
}

uint8_t txUDP::begin(IPAddress address, uint16_t port)
{
    int r;
    stop();
    server_port = port;
    tx_buffer = new char[1460];
    if (!tx_buffer)
    {
        DEBUG_UDP("[ERROR] UDP create tx buffer");
        return 0;
    }
    if ((m_socket = qapi_socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        DEBUG_UDP("[ERROR] UDP create socket: %d", m_socket);
        return 0;
    }
    struct sockaddr_in addr;
    memset((char *)&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(server_port);
    addr.sin_addr.s_addr = (uint32_t)address;
    if ((r = qapi_bind(m_socket, (struct sockaddr *)&addr, sizeof(addr))))
    {
        DEBUG_UDP("[ERROR] UDP bind socket: %d", r);
        stop();
        return 0;
    }
    if ((r = setsockopt(SOL_SOCKET, SO_NBIO, NULL, 0)))
    {
        DEBUG_UDP("[ERROR] UDP setsockopt NB: %d", r);
        stop();
        return 0;
    }
    return 1;
}

uint8_t txUDP::begin(uint16_t p)
{
    return begin(IPAddress((uint32_t)0), p); //INADDR_ANY
}

void txUDP::stop()
{
    if (tx_buffer)
    {
        delete[] tx_buffer;
        tx_buffer = NULL;
    }
    tx_buffer_len = 0;
    if (rx_buffer)
    {
        cbuf *b = rx_buffer;
        rx_buffer = NULL;
        delete b;
    }
    if (m_socket == -1)
        return;
    if (multicast_ip != 0)
    {
    }
    qapi_socketclose(m_socket);
    m_socket = -1;
}

int txUDP::beginPacket()
{
    if (!remote_port)
        return 0;
    if (!tx_buffer) 
    {
        tx_buffer = new char[1460];
        if (!tx_buffer)
        {
            DEBUG_UDP("[ERROR] UDP create tx buffer");
            return 0;
        }
    }
    tx_buffer_len = 0;
    if (m_socket != -1) 
        return 1;
    if ((m_socket = qapi_socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        DEBUG_UDP("[ERROR] UDP create socket: %d", m_socket);
        return 0;
    }
    int r;
    if ((r = setsockopt(SOL_SOCKET, SO_NBIO, NULL, 0)))
    {
        DEBUG_UDP("[ERROR] UDP setsockopt NB: %d", r);
        stop();
        return 0;
    }
    return 1;
}

int txUDP::beginPacket(IPAddress ip, uint16_t port)
{
    remote_ip = ip;
    remote_port = port;
    return beginPacket();
}

int txUDP::beginPacket(const char *host, uint16_t port)
{
    struct ip46addr addr;
    if (Dns::query(host, &addr))
    {
        IPAddress ip = (uint32_t)addr.a.addr4;
        return beginPacket(ip, port);
    }
    DEBUG_UDP("[ERROR] UDP get host from dns");
    return 0;
}

int txUDP::endPacket()
{
    struct sockaddr_in recipient;
    recipient.sin_addr.s_addr = (uint32_t)remote_ip;
    recipient.sin_family = AF_INET;
    recipient.sin_port = htons(remote_port);
    int sent = qapi_sendto(m_socket, tx_buffer, tx_buffer_len, 0, &recipient, sizeof(recipient));
    if (sent < 0)
    {
        DEBUG_UDP("[ERROR] UDP send data: %d", sent);
        return 0;
    }
    return 1;
}

size_t txUDP::write(uint8_t data)
{
    if (tx_buffer_len == 1460)
    {
        endPacket();
        tx_buffer_len = 0;
    }
    tx_buffer[tx_buffer_len++] = data;
    return 1;
}

size_t txUDP::write(const uint8_t *buffer, size_t size)
{
    size_t i;
    for (i = 0; i < size; i++)
        write(buffer[i]);
    return i;
}

int txUDP::parsePacket()
{
    if (rx_buffer)
        return 0;
    struct sockaddr_in si_other;
    int slen = sizeof(si_other), len;
    char *buf = new char[1460];
    if (!buf)
        return 0;
    if ((len = qapi_recvfrom(m_socket, buf, 1460, MSG_DONTWAIT, &si_other, &slen)) == -1)
    {
        delete[] buf;
        if (len == EWOULDBLOCK)
        {
            return 0;
        }
        DEBUG_UDP("[ERROR] UDP receive data: %d", len);
        return 0;
    }
    remote_ip = IPAddress(si_other.sin_addr.s_addr);
    remote_port = ntohs(si_other.sin_port);
    if (len > 0)
    {
        rx_buffer = new cbuf(len);
        rx_buffer->write(buf, len);
    }
    delete[] buf;
    return len;
}

int txUDP::available()
{
    if (!rx_buffer)
        return 0;
    return rx_buffer->available();
}

int txUDP::read()
{
    if (!rx_buffer)
        return -1;
    int out = rx_buffer->read();
    if (!rx_buffer->available())
    {
        cbuf *b = rx_buffer;
        rx_buffer = 0;
        delete b;
    }
    return out;
}

int txUDP::read(unsigned char *buffer, size_t len)
{
    return read((char *)buffer, len);
}

int txUDP::read(char *buffer, size_t len)
{
    if (!rx_buffer)
        return 0;
    int out = rx_buffer->read(buffer, len);
    if (!rx_buffer->available())
    {
        cbuf *b = rx_buffer;
        rx_buffer = 0;
        delete b;
    }
    return out;
}

int txUDP::peek()
{
    if (!rx_buffer)
        return -1;
    return rx_buffer->peek();
}

void txUDP::flush()
{
    if (!rx_buffer)
        return;
    cbuf *b = rx_buffer;
    rx_buffer = 0;
    delete b;
}

IPAddress txUDP::remoteIP()
{
    return remote_ip;
}

uint16_t txUDP::remotePort()
{
    return remote_port;
}