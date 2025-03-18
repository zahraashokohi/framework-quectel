//#pragma GCC error "LClientSecure is not ready yet"

#include <Arduino.h>
#include <IPAddress.h>
#include "LHTTP.h"
#include "LClient.h"
#include <os_ssl.h>

class LClientSecure : public LClient, private LHTTP
{
private:
    unsigned int M_Ring;
    ring_buffer_t ring;
    char ring_buffer[64];
    unsigned int _timeout; // read
    const char *_ca;
    const char *_client_cert;
    const char *_private_key;
    int _socket;

    int _ssl_read(uint8_t *buf, int len)
    {
        mbedtls_ssl_context *ctx = (mbedtls_ssl_context *)LHTTP::context();
        int res = -1;
        if (ctx && buf)
        {
            os_ssl_conf_read_timeout(ctx->conf, _timeout);
            res = os_ssl_read(ctx, buf, len);
        }
        return res;
    }

    int _ssl_write(uint8_t *buf, int len)
    {
        mbedtls_ssl_context *ctx = (mbedtls_ssl_context *)LHTTP::context();
        int res = -1;
        if (ctx && buf)
        {
            for (int i = 0;; i += res)
            {
                if (i >= len)
                    return i;
                res = os_ssl_write(ctx, buf + i, len - i);
                if (res <= 0)
                    break;
            }
        }
        return res;
    }

    size_t _read()
    {
        size_t available = 0;
        int rd;
        if (MUTEX_LOCK(M_Ring))
        {
            int free_bytes = ring.available(&ring);
            uint8_t buf[free_bytes];
            rd = _ssl_read(buf, free_bytes);
            if (rd > 0)
                ring.write(&ring, buf, rd);
            available = sizeof(ring_buffer) - ring.available(&ring);
            MUTEX_UNLOCK(M_Ring);
        }
        return available;
    }

    void _ctor()
    {
        M_Ring = Ql_OS_CreateMutex();
        ring_buffer_init(&ring, sizeof(ring_buffer), ring_buffer);
        ring.reset(&ring);
        _timeout = 10;
        _ca = NULL;
        _client_cert = NULL;
        _private_key = NULL;
        _socket = -1;
    }

public:
    LClientSecure() { _ctor(); }

    ~LClientSecure() { stop(); }

    void stop()
    {
        LHTTP::disconnect();
        _socket = -1;
    }

    uint8_t connected() { return _socket >= 0; }

    int connect(const char *host, uint16_t port, int32_t handshake_timeout_in_seconds = 60)
    {
        if (NULL == host)
            return false;
        stop();
        char url[256];
        snprintf(url, 256, "https://%s:%d/", host, (int)port);
        _socket = LHTTP::handshake(url, _ca, _client_cert, _private_key, handshake_timeout_in_seconds);
        return _socket >= 0;
    }

    int connect(IPAddress ip, uint16_t port) { return connect(ip.toString().c_str(), port); }

    size_t write(const uint8_t *buffer, size_t size)
    {
        if (connected() && buffer && size)
            return _ssl_write((uint8_t *)buffer, size);
        stop();
        return 0;
    }
    size_t write(uint8_t b) { return write(&b, 1); }

    int read(uint8_t *buf, size_t size)
    {
        int res = -1;
        if (connected() && buf && size)
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
    int read()
    {
        uint8_t byte;
        return 1 == read(&byte, 1) ? byte : -1;
    }

    int available()
    {
        size_t available = 0;
        if (connected())
        {
            available = _read();
        }
        else
        {
            stop();
        }
        return available;
    }

    int peek()
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

    void flush()
    {
        while (available())
            read();
        if (MUTEX_LOCK(M_Ring))
        {
            ring.reset(&ring);
            MUTEX_UNLOCK(M_Ring);
        }
    }

    void set_timeout(unsigned int timeout) { _timeout = timeout; }
    
    void set_ca_certificate(const char *ca) { _ca = ca; }

    void set_client_certificate(const char *cert, const char *key)
    {
        _client_cert = cert;
        _private_key = key;
    }

    operator bool() { return connected(); }
    LClientSecure &operator=(const LClientSecure &other);
    bool operator==(const bool value) { return bool() == value; }
    bool operator!=(const bool value) { return bool() != value; }
    bool operator==(const LClientSecure &);
    bool operator!=(const LClientSecure &rhs) { return !this->operator==(rhs); }
    int socket() { return _socket; }
    int lastError() { return 0; }
};