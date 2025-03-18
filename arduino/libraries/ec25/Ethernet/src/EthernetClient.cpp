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

#include "EthernetClient.h"

#define DEBUG_CLIENT  
//Serial.printf

EthernetClient::EthernetClient() : _sock(-1)
{
	connect_true = false;
}

EthernetClient::EthernetClient(uint8_t sock) : _sock(sock)
{
	connect_true = true;
}

static int _connect(int sock, struct sockaddr_in *psin, unsigned int len, uint32_t timeout)
{
	int ret;
	DEBUG_CLIENT("[TCP] Connecting: %d.%d.%d.%d : %d\n",
			   (int)((psin->sin_addr.s_addr) & 0xFF),
			   (int)((psin->sin_addr.s_addr >> 8) & 0xFF),
			   (int)((psin->sin_addr.s_addr >> 16) & 0xFF),
			   (int)((psin->sin_addr.s_addr >> 24) & 0xFF),
			   (int)(int)ntohs(psin->sin_port));
	ret = connect(sock, (struct sockaddr *)psin, len);
	if (ret < 0)
	{
		DEBUG_CLIENT("[ERROR] TCP connect %d\n", ret);
		close(sock);
		return ret;
	}
	/* SET DEFAULT */
	int enable = 1;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
	setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&enable, sizeof(enable));
	setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (char *)&enable, sizeof(enable));
	DEBUG_CLIENT("[TCP] Connected\n");
	return ret;
}

int EthernetClient::connect(const char *host, uint16_t port)
{
	struct hostent *hp;
	int ret = 0;
	DEBUG_CLIENT("[TCP] Connecting: %s : %d\n", host, (int)port);
	if (host == NULL || _sock != -1)
	{
		DEBUG_CLIENT("[ERROR] TCP socket!\n");
		return 0;
	}
	_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_sock < 0)
	{
		DEBUG_CLIENT("[ERROR] TCP unable to open a TCP socket\n");
		return 0;
	}
	hp = gethostbyname(host);
	if (hp == NULL)
	{
		DEBUG_CLIENT("[ERROR] TCP gethostbyname %s fail\n", host);
		return 0;
	}
	struct sockaddr_in _sin;
	memcpy(&_sin.sin_addr, hp->h_addr, sizeof(_sin.sin_addr));
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(port);
	ret = _connect(_sock, &_sin, sizeof(_sin), 20000);
	if (ret < 0)
	{
		DEBUG_CLIENT("[ERROR] TCP unable to connect to target host %s\n", host);
		return 0;
	}
	if (ret == 0)
		connect_true = true;
	return 1;
}

int EthernetClient::connect(IPAddress ip, uint16_t port)
{
	int ret = 0;
	if (_sock != -1)
		return 0;
	_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_sock < 0)
	{
		DEBUG_CLIENT("[ERROR] TCP unable to open a TCP socket\n");
		return 0;
	}
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = (uint32_t)ip;
	sin.sin_port = htons(port);
	ret = _connect(_sock, &sin, sizeof(sin), 20000);
	if (ret < 0)
	{
		DEBUG_CLIENT("[ERROR] TCP unable to connect to target ip\n");
		return 0;
	}
	if (ret == 0)
		connect_true = true;
	return 1;
}

size_t EthernetClient::write(uint8_t b)
{
	return write(&b, 1);
}

size_t EthernetClient::write(const uint8_t *buf, size_t size)
{
	if (_sock < 0)
		return 0;
	if (send(_sock, (char *)buf, size, 0) < 0)
	{
		DEBUG_CLIENT("[ERROR] TCP write()\n");
		stop();
		size = 0;
	}
	return size;
}

int EthernetClient::read()
{
	char b = 0;
	int rc = recv(_sock, &b, 1, 0);
	if (rc > 0)
		return b;
	DEBUG_CLIENT("[ERROR] TCP read(byte) %d\n", rc);	
	return -1; // No data available
}

int EthernetClient::read(uint8_t *buf, size_t size)
{
	int rc = recv(_sock, (char *)buf, size, 0);
	if (rc > 0)
		return rc;
	DEBUG_CLIENT("[ERROR] TCP read(%d)\n", size);	
	return -1;
}

int EthernetClient::available()
{
	if (_sock < 0)
		return 0;
	u_long count;
	int rc = ioctl(_sock, FIONREAD, &count);
	if (rc < 0)
	{
		DEBUG_CLIENT("[ERROR] TCP available()\n");
		return 0;
	}
	return count;
}

int EthernetClient::peek()
{
	if (_sock < 0 || !available())
		return -1;
	char b;
	int rc = recv(_sock, &b, 1, MSG_PEEK);
	if (rc > 0)
		return b;
	return -1;
}

void EthernetClient::flush()
{
	if (_sock < 0)
		return;
	while (available())
		read();
}

void EthernetClient::stop()
{
	if (_sock < 0)
		return;
	connect_true = false;
	close(_sock);
	_sock = -1;
	//if (_pCloseServer != NULL)
}

uint8_t EthernetClient::connected()
{
	return connect_true == true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool EthernetClient::getNoDelay()
{
	int flag = 0;
	getOption(TCP_NODELAY, &flag);
	return flag;
}

int EthernetClient::setNoDelay(bool nodelay)
{
	u_long iMode = 1; //non-blocking mode is enabled > 0
	ioctl(_sock, FIONBIO, &iMode);

	int flag = nodelay;
	return setOption(TCP_NODELAY, &flag);
}

int EthernetClient::getOption(int option, int *value)
{
	socklen_t size = sizeof(int);
	int res = getsockopt(_sock, IPPROTO_TCP, option, (char *)value, &size);
	if (res < 0)
	{
		DEBUG_CLIENT("[ERROR] get IPPROTO_TCP\n");
	}
	return res;
}

int EthernetClient::setOption(int option, int *value)
{
	int res = setsockopt(_sock, IPPROTO_TCP, option, (char *)value, sizeof(int));
	if (res < 0)
	{
		DEBUG_CLIENT("[ERROR] set IPPROTO_TCP\n");
	}
	return res;
}

int EthernetClient::setSocketOption(int option, char *value, size_t len)
{
	int res = setsockopt(_sock, SOL_SOCKET, option, value, len);
	if (res < 0)
	{
		DEBUG_CLIENT("[ERROR] set SOL_SOCKET %X : %d\n", option, errno);
	}
	return res;
}

int EthernetClient::setTimeout(uint32_t seconds)
{
	Client::setTimeout(seconds * 1000);
	struct timeval tv;
	tv.tv_sec = seconds;
	tv.tv_usec = 0;
	if (setSocketOption(SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
	{
		DEBUG_CLIENT("[ERROR] set SO_RCVTIMEO\n");
		return -1;
	}
	return setSocketOption(SO_SNDTIMEO, (char *)&tv, sizeof(struct timeval));
}
