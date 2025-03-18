////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 Georgi Angelov
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
////////////////////////////////////////////////////////////////////////////
#ifndef _LUDP_H_
#define _LUDP_H_

#include <Arduino.h>
#include <Udp.h>
#include <cbuf.h>

#define DEBUG_UDP ::printf

#undef write
#undef read

#define UDP_BUF_SIZE 1460

class LUDP : public UDP
{
private:
    int udp_server;
    IPAddress remote_ip;
    uint16_t server_port;
    uint16_t remote_port;
    char *tx_buffer;
    size_t tx_buffer_len;
    ring_buffer_t ring;
    char ring_buffer[UDP_BUF_SIZE];

public:
    LUDP() : udp_server(-1),
             server_port(0),
             remote_port(0),
             tx_buffer(0),
             tx_buffer_len(0)
    {
        ring_buffer_init(&ring, sizeof(ring_buffer), ring_buffer);
        ring.reset(&ring);
    }
    ~LUDP() { stop(); }

    uint8_t begin(IPAddress address, uint16_t port)
    {
        stop();
        server_port = port;
        tx_buffer = new char[UDP_BUF_SIZE];
        if (!tx_buffer)
        {
            DEBUG_UDP("could not create tx buffer\n");
            return 0;
        }
        udp_server = ::socket(AF_INET, SOCK_DGRAM, 0);
        if (-1 == udp_server)
        {
            DEBUG_UDP("could not create socket\n");
            return 0;
        }
        struct sockaddr_in addr;
        memset((char *)&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(server_port);
        addr.sin_addr.s_addr = (in_addr_t)address;
        if (::bind(udp_server, (struct sockaddr *)&addr, sizeof(addr)) == -1)
        {
            DEBUG_UDP("could not bind socket\n");
            stop();
            return 0;
        }
        ::fcntl(udp_server, F_SETFL, O_NONBLOCK);
        return 1;
    }

    uint8_t begin(uint16_t p)
    {
        return begin(IPAddress(INADDR_ANY), p);
    }

    void stop()
    {
        if (-1 != udp_server)
        {
            ::closesocket(udp_server);
            udp_server = -1;
        }
        ring.reset(&ring);
        delete[] tx_buffer;
        tx_buffer = NULL;
        tx_buffer_len = 0;
    }

    int beginPacket()
    {
        stop();
        if (!remote_port)
            return 0;
        // allocate tx_buffer if is necessary
        if (!tx_buffer)
        {
            tx_buffer = new char[UDP_BUF_SIZE];
            if (!tx_buffer)
            {
                DEBUG_UDP("[ERROR] UDP buffer");
                return 0;
            }
        }
        tx_buffer_len = 0;
        // check whereas socket is already open
        if (udp_server != -1)
            return 1;
        if ((udp_server = ::socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        {
            DEBUG_UDP("[ERROR] UDP socket");
            return 0;
        }
        ::fcntl(udp_server, F_SETFL, O_NONBLOCK);
        return 1;
    }

    int beginPacket(IPAddress ip, uint16_t port)
    {
        remote_ip = ip;
        remote_port = port;
        return beginPacket();
    }

    int beginPacket(const char *host, uint16_t port)
    {
        IPAddress ip;
        struct sockaddr_in a;
        if (0 == os_resolve_address(host, &a))
        {
            DEBUG_UDP("[ERROR] UDP resolve\n");
            return 0;
        }
        ip = a.sin_addr.s_addr;
        return beginPacket(ip, port);
    }

    int endPacket()
    {
        struct sockaddr_in recipient;
        recipient.sin_addr.s_addr = (uint32_t)remote_ip;
        recipient.sin_family = AF_INET;
        recipient.sin_port = HTONS(remote_port);
        int sent = ::sendto(udp_server, tx_buffer, tx_buffer_len, 0, (struct sockaddr *)&recipient, sizeof(recipient));
        if (sent < 0)
        {
            DEBUG_UDP("[ERROR] UDP send\n");
            return 0;
        }
        return 1;
    }

    size_t write(uint8_t data)
    {
        if (tx_buffer_len == UDP_BUF_SIZE)
        {
            endPacket();
            tx_buffer_len = 0;
        }
        tx_buffer[tx_buffer_len++] = data;
        return 1;
    }

    size_t write(const uint8_t *buffer, size_t size)
    {
        size_t i;
        for (i = 0; i < size; i++)
            write(buffer[i]);
        return i;
    }

    size_t write(const char *str) { return write((const uint8_t *)str, strlen(str)); }

    int parsePacket()
    {
        struct sockaddr_in si_other;
        int slen = sizeof(si_other), len;
        char *buffer = new char[UDP_BUF_SIZE];
        if (!buffer)
            return 0;
        len = ::recvfrom(udp_server, buffer, UDP_BUF_SIZE, MSG_DONTWAIT, (struct sockaddr *)&si_other, (socklen_t *)&slen);
        if (-1 == len)
        {
            delete[] buffer;
            return 0;
        }
        remote_ip = IPAddress(si_other.sin_addr.s_addr);
        remote_port = HTONS(si_other.sin_port);
        if (len > 0)
            ring.write(&ring, buffer, len);
        delete[] buffer;
        return len;
    }

    int read(char *buffer, size_t len) { return ring.read(&ring, buffer, len); }
    int read()
    {
        uint8_t byte;
        return 1 == read(&byte, 1) ? byte : -1;
    }
    int read(unsigned char *buffer, size_t len) { return read((char *)buffer, len); }

    int available() { return sizeof(ring_buffer) - ring.available(&ring); }

    int peek()
    {
        int res = -1;
        if (available())
        {
            if (1 == ring.peek(&ring, &res, 1))
                res &= 0xFF;
            else
                res = -1;
        }
        return res;
    }

    void flush()
    {
        while (available())
            read();
        ring.reset(&ring);
    }

    IPAddress remoteIP() { return remote_ip; }
    uint16_t remotePort() { return remote_port; }
};

#endif /* _LUDP_H_ */
