/*
  txClient.cpp - Library
  Copyright (c) 2011-2014 Arduino.  All right reserved.

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

#define DEBUG_TCP
//DBG

#include "txClient.h"
#include "txDns.h"

txClient::txClient() : m_socket(-1),
                       m_externalSocket(false),
                       m_timeout(10000),
                       _connected(false)
{
}

txClient::txClient(int sock) : m_socket(sock),
                               m_externalSocket(true),
                               m_timeout(10000),
                               _connected(false)
{
    // accepting an externally allocated socket
}

txClient::~txClient()
{
    if (!m_externalSocket)
        stop();
}

int txClient::connect(const char *host, uint16_t port)
{
    struct ip46addr addr;
    if (Dns::query(host, &addr))
    {
        IPAddress ip = (uint32_t)addr.a.addr4; 
        return connect(ip, port);
    }
    return 0;
}

int txClient::connect(IPAddress ip, uint16_t port)
{
    if (m_socket)
        stop();
    int r;
    m_socket = qapi_socket(AF_INET, SOCK_STREAM, 0);
    m_externalSocket = true;
    if (-1 == m_socket)
    {
        DEBUG_TCP("[ERROR] TCP socket\n");
        return 0;
    }
    struct sockaddr_in a;
    a.sin_addr.s_addr = (uint32_t)ip;
    a.sin_family = AF_INET;
    a.sin_port = htons(port);
    r = qapi_connect(m_socket, (struct sockaddr *)&a, sizeof(a));
    if (r)
    {
    ERROR:
        qapi_socketclose(m_socket);
        m_socket = -1;
        DEBUG_TCP("[ERROR] TCP connect: %d\n", r);
        return 0;
    }
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(m_socket, &fdset);
    if ((r = select(&fdset, &fdset, &fdset, m_timeout)) < 1)
    {
        r = ETIMEDOUT;
        DEBUG_TCP("[ERROR] TCP timeout\n");
        goto ERROR;
    }
    _connected = true;
    return _connected;
}

size_t txClient::write(uint8_t b)
{
    if (m_socket == -1)
        return 0;
    return qapi_send(m_socket, &b, 1, 0) == 1 ? 1 : 0;
}

size_t txClient::write(const uint8_t *buf, size_t size)
{
    if (-1 == m_socket || NULL == buf || 0 == size)
        return 0;
    int ret = qapi_send(m_socket, buf, size, 0);
    return ret > 0 ? ret : 0;
}

int txClient::available()
{
    if (m_socket < 0)
        return 0;
    int val, r;
    return getsockopt(SOL_SOCKET, SO_RXDATA, &val, sizeof(val)) ? 0 : val;
}

int txClient::read()
{
    if (m_socket == -1)
        return -1;
    uint8_t b;
    return 1 == qapi_recv(m_socket, &b, 1, 0) ? b : -1;
}

int txClient::read(uint8_t *buf, size_t size)
{
    if (-1 == m_socket || NULL == buf || 0 == size)
        return 0;
    int r;
    r = qapi_recv(m_socket, buf, size, 0);
    return r > 0 ? r : -1;
}

int txClient::peek()
{
    if (m_socket == -1)
        return -1;
    uint8_t b;
    int ret = qapi_recv(m_socket, &b, 1, MSG_PEEK | MSG_DONTWAIT);
    return ret == 1 ? b : -1;
}

void txClient::stop()
{
    if (m_socket == -1)
        return;
    qapi_socketclose(m_socket);
    m_socket = -1;
    _connected = false;
}

uint8_t txClient::connected()
{
    return _connected;
#if 0    
    if (-1 == m_socket)
        return 0;
    int error_code = 0;
    int r = getsockopt(SOL_SOCKET, SO_ERROR, &error_code, sizeof(error_code));
    if (r || (error_code == ENOTCONN) || (error_code == ENOTSOCK) || (error_code == ECONNRESET) || (error_code == ETIMEDOUT))
        return 0;
    else
        return 1;
#endif
}

txClient::operator bool()
{
    return connected();
}