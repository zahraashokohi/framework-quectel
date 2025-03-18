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

#ifndef txClient_h
#define txClient_h

#include <interface.h>
#include "Client.h"
#include "IPAddress.h"

class txClient : public Client
{
#define htons(s) ((((s) >> 8) & 0xff) | (((s) << 8) & 0xff00))
public:
  txClient();
  txClient(int sock);
  ~txClient();

  virtual int connect(IPAddress ip, uint16_t port);
  virtual int connect(const char *host, uint16_t port);
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  virtual int available();
  virtual int read();
  virtual int read(uint8_t *buf, size_t size);
  virtual int peek();
  virtual void flush() {}
  virtual void stop();
  virtual uint8_t connected();
  virtual operator bool();
  void setTimeout(int32_t ms) { m_timeout = ms; }

  friend class txServer;
  using Print::write;

protected:
  bool _connected;

private:
  int m_socket;
  bool m_externalSocket;
  int32_t m_timeout;

  uint32_t errno()
  {
    return qapi_errno(m_socket);
  }

  int getsockopt(int32_t level, int32_t optname, void *optval, int32_t optlen)
  {
    return qapi_getsockopt(m_socket, level, optname, optval, optlen);
  }

  int setsockopt(int32_t level, int32_t optname, void *optval, int32_t optlen)
  {
    return qapi_setsockopt(m_socket, level, optname, optval, optlen);
  }

  int select(fd_set *rd, fd_set *wr, fd_set *ex, int32_t timeout_ms)
  {
    return qapi_select(rd, wr, ex, timeout_ms);
  }
};

#endif