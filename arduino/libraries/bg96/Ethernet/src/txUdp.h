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

#ifndef _NBUDP_H_
#define _NBUDP_H_

#include <interface.h>
#include <Udp.h>
#include <cbuf.h>

class txUDP : public UDP
{
#define htons(s) ((((s) >> 8) & 0xff) | (((s) << 8) & 0xff00))
#define ntohs(s) (((s & 0xff) << 8) | (s >> 8))
private:
  int m_socket;
  IPAddress multicast_ip;
  IPAddress remote_ip;
  uint16_t server_port;
  uint16_t remote_port;
  char *tx_buffer;
  size_t tx_buffer_len;
  cbuf *rx_buffer;

public:
  txUDP();
  ~txUDP();
  uint8_t begin(IPAddress a, uint16_t p);
  uint8_t begin(uint16_t p);
  void stop();
  int beginPacket();
  int beginPacket(IPAddress ip, uint16_t port);
  int beginPacket(const char *host, uint16_t port);
  int endPacket();
  size_t write(uint8_t);
  size_t write(const uint8_t *buffer, size_t size);
  int parsePacket();
  int available();
  int read();
  int read(unsigned char *buffer, size_t len);
  int read(char *buffer, size_t len);
  int peek();
  void flush();
  IPAddress remoteIP();
  uint16_t remotePort();

  int getsockopt(int32_t level, int32_t optname, void *optval, int32_t optlen)
  {
    return qapi_getsockopt(m_socket, level, optname, optval, optlen);
  }

  int setsockopt(int32_t level, int32_t optname, void *optval, int32_t optlen)
  {
    return qapi_setsockopt(m_socket, level, optname, optval, optlen);
  }
};

#endif /* _NBUDP_H_ */
