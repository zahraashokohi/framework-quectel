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

#ifndef LClient_H
#define LClient_H

#include <Arduino.h>
#include <Client.h>
#include <IPAddress.h>
#include "sys/socket.h"
#include "sys/select.h"

#define DEBUG_TCP ::printf

#undef htons

class LClient : public Client
{
#define RX_BUFFER_SIZE 256
public:
    LClient() { ctor(); }

    LClient(int sock)
    {
        ctor();
        sockfd = sock;
        externalSocket = true;
    }

    ~LClient()
    {
        if (!externalSocket)
            stop();
    }

    virtual int connect(const char *host, uint16_t port)
    {
        struct sockaddr_in a;
        if (os_resolve_address(host, &a))
        {
            IPAddress ip = (uint32_t)a.sin_addr.s_addr;
            //Serial.println(ip);
            return connect(ip, port);
        }
        DEBUG_TCP("[ERROR] resolve %s\n", host);
        return false;
    }

    virtual int connect(IPAddress ip, uint16_t port)
    {
        if (sockfd >= 0)
            stop();
        int res;
        sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
        externalSocket = true;
        if (sockfd < 0)
        {
            DEBUG_TCP("[ERROR] create socket\n");
            return false; // error
        }

        /* nonblocking */
        res = ::fcntl(sockfd, F_GETFL, 0);
        if (res < 0)
        {
            DEBUG_TCP("[ERROR] F_GETFL\n");
            stop();
            return false;
        }
        ::fcntl(sockfd, F_SETFL, res | O_NONBLOCK);
        if (res < 0)
        {
            DEBUG_TCP("[ERROR] F_SETFL\n");
            stop();
            return false;
        }

        struct sockaddr_in a;
        a.sin_addr.s_addr = (uint32_t)ip;
        a.sin_family = AF_INET;
        a.sin_port = HTONS(port);

        if ((res = ::connect(sockfd, (struct sockaddr *)&a, sizeof(a))))
        {
            if (res == -7) // ERR_WOULDBLOCK
            {
                fd_set st;
                FD_ZERO(&st);
                FD_SET(sockfd, &st);
                struct timeval tv;
                tv.tv_sec = timeout / 1000;
                tv.tv_usec = 0;
                if (::select(sockfd + 1, &st, &st, &st, &tv) > 0)
                    _connected = true;
            }
            else
            {
                stop();
                DEBUG_TCP("[ERROR] TCP connect = %d\n", res);
            }
        }
        else
            _connected = true;
        return _connected;
    }

    virtual size_t write(const uint8_t *buffer, size_t size)
    {
        if (_connected && buffer && size)
            return ::send(sockfd, buffer, size, MSG_DONTWAIT);
        stop();
        return 0;
    }
    virtual size_t write(uint8_t b) { return write(&b, 1); }

    virtual int available()
    {
        size_t available = 0;
        if (_connected)
        {
            available = do_read();
        }
        else
        {
            stop();
        }
        return available;
    }

    virtual int read(uint8_t *buf, size_t size)
    {
        int res = -1;
        if (_connected && buf && size)
        {
            if ((res = available()))
            {
                if (MUTEX_LOCK(M_Ring))
                {
                    res = ring.read(&ring, buf, size);
                    MUTEX_UNLOCK(M_Ring);
                }
            }
        }
        else
            stop();
        return res;
    }

    virtual int read()
    {
        uint8_t byte;
        return 1 == read(&byte, 1) ? byte : -1;
    }

    virtual int peek()
    {
        int res = -1;
        if (available())
        {
            if (MUTEX_LOCK(M_Ring))
            {
                if (1 == ring.peek(&ring, &res, 1))
                    res &= 0xFF;
                else
                    res = -1;
                MUTEX_UNLOCK(M_Ring);
            }
        }
        return res;
    }

    virtual void flush()
    {
        while (available())
            read();
        if (MUTEX_LOCK(M_Ring))
        {
            ring.reset(&ring);
            MUTEX_UNLOCK(M_Ring);
        }
    }

    virtual void stop()
    {
        if (sockfd >= 0)
        {
            ::closesocket(sockfd);
            sockfd = -1;
        }
        if (MUTEX_LOCK(M_Ring))
        {
            ring.reset(&ring);
            MUTEX_UNLOCK(M_Ring);
        }
        _connected = false;
    }

    void set_timeout(int32_t ms) { timeout = ms; }

    virtual uint8_t connected() { return _connected; }

    virtual operator bool() { return sockfd >= 0; }
    virtual bool operator==(const bool value) { return bool() == value; }
    virtual bool operator!=(const bool value) { return bool() != value; }
    virtual bool operator==(const LClient &r) { return sockfd == r.sockfd && bool(); }
    virtual bool operator!=(const LClient &r) { return !this->operator==(r); };

    friend class LServer;
    using Print::write;

protected:
    bool _connected;

private:
    int sockfd;
    unsigned int timeout;
    bool externalSocket;

    unsigned int M_Ring;
    ring_buffer_t ring;
    char ring_buffer[RX_BUFFER_SIZE];

    void ctor(void)
    {
        sockfd = -1;
        externalSocket = _connected = false;
        set_timeout(60000);
        M_Ring = Ql_OS_CreateMutex();
        ring_buffer_init(&ring, sizeof(ring_buffer), ring_buffer);
        ring.reset(&ring);
    }

    size_t do_read()
    {
        size_t available = 0;
        int rd;
        if (MUTEX_LOCK(M_Ring))
        {
            int free_bytes = ring.available(&ring);
            char buffer[free_bytes];
            rd = ::recvfrom(sockfd, buffer, free_bytes, MSG_DONTWAIT, 0, 0);
            if (rd > 0)
                ring.write(&ring, buffer, rd);
            available = sizeof(ring_buffer) - ring.available(&ring);
            MUTEX_UNLOCK(M_Ring);
        }
        return available;
    }
};

#endif