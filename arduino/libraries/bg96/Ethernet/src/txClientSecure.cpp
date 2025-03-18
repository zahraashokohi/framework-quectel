/*
  txClientSecure.cpp - Client Secure class
  https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFiClientSecure/src
*/

#include "Arduino.h"

#undef connect
#undef write
#undef read

#include "txClientSecure.h"

bool verify_ssl_fingerprint(ssl_context *ssl_client, const char *fp, const char *domain_name) { return false; }
bool verify_ssl_dn(ssl_context *ssl_client, const char *domain_name) { return false; }
int start_ssl_client(ssl_context *ssl_client, const char *host, uint32_t port, int timeout,
                     const char *rootCABuff, const char *cli_cert, const char *cli_key,
                     const char *pskIdent, const char *psKey) {}
void stop_ssl_socket(ssl_context *ssl_client, const char *rootCABuff, const char *cli_cert, const char *cli_key) {}

int data_to_read(ssl_context *ssl_client) {}

int get_ssl_receive(ssl_context *ctx, uint8_t *data, int len)
{
    fd_set rset;
    size_t rxLen = 0;
    int ret;
    while (len > 0)
    {
        /* This read will timeout after IOT_SSL_READ_TIMEOUT if there's no data to be read	*/
        FD_ZERO(&rset);
        FD_SET(ctx->socket, &rset);
        ret = qapi_select(&rset, NULL, NULL, 1000);
        if (ret > 0)
        {
            ret = qapi_Net_SSL_Read(ctx->hConn, data, len);
            if (ret > 0)
            {
                rxLen += ret;
                data += ret;
                len -= ret;
            }
            else if (ret != QAPI_OK)
            {
                DBG("[ERROR] qapi_Net_SSL_Read() %d\n", ret);
                return ret;
            }
        }
    }
    if (len == 0 || (rxLen > 0 && ret == 0))
        return rxLen;
    if (rxLen == 0)
        return 0; //NETWORK_SSL_NOTHING_TO_READ;
    return -1;    //NETWORK_SSL_READ_TIMEOUT_ERROR;
}

int send_ssl_data(ssl_context *ctx, const uint8_t *data, uint16_t len)
{
    size_t written_so_far;
    int frags, ret = 0;
    bool isErrorFlag = false;
    for (written_so_far = 0, frags = 0; written_so_far < len; written_so_far += ret, frags++)
    {
        while ((ret = qapi_Net_SSL_Write(ctx->hConn, data + written_so_far, len - written_so_far)) <= 0)
        {
            if (ret != QAPI_OK)
            {
                DBG("[ERROR] qapi_Net_SSL_Write() %d", ret);
                isErrorFlag = true;
                break;
            }
        }
        if (isErrorFlag)
            break;
    }
    if (isErrorFlag)
        return ret;
    if (written_so_far != len)
        return -1;
    return written_so_far;
}

int ssl_disconnect(ssl_context *ctx)
{
    if (ctx->hConn != QAPI_NET_SSL_INVALID_HANDLE)
    {
        qapi_Net_SSL_Shutdown(ctx->hConn);
        ctx->hConn = QAPI_NET_SSL_INVALID_HANDLE;
    }
    if (ctx->socket != -1)
    {
        qapi_socketclose(ctx->socket);
        ctx->socket = -1;
    }
    DBG("[TLS] ssl_disconnect()\n");
    return 0;
}

int ssl_destroy(ssl_context *ctx)
{
    if (ctx->hCtxt != QAPI_NET_SSL_INVALID_HANDLE)
    {
        qapi_Net_SSL_Obj_Free(ctx->hCtxt);
        ctx->hCtxt = QAPI_NET_SSL_INVALID_HANDLE;
    }
    DBG("[TLS] ssl_destroy()\n");
}

void ssl_init(ssl_context *ssl_client)
{
    DBG("[TLS] ssl_init()\n");
}

//////////////////////////////////////////////////////////////////////////////////

txClientSecure::txClientSecure()
{
    _connected = false;
    sslclient = new ssl_context;
    ssl_init(sslclient);
    sslclient->socket = -1;
    sslclient->handshake_timeout = 120000;
    _CA_cert = NULL;
    _cert = NULL;
    _private_key = NULL;
    _pskIdent = NULL;
    _psKey = NULL;
    next = NULL;
}

txClientSecure::txClientSecure(int sock)
{
    _connected = false;
    _timeout = 0;
    sslclient = new ssl_context;
    ssl_init(sslclient);
    sslclient->socket = sock;
    sslclient->handshake_timeout = 120000;
    if (sock >= 0)
        _connected = true;
    _CA_cert = NULL;
    _cert = NULL;
    _private_key = NULL;
    _pskIdent = NULL;
    _psKey = NULL;
    next = NULL;
}

txClientSecure::~txClientSecure()
{
    stop();
    delete sslclient;
}

txClientSecure &txClientSecure::operator=(const txClientSecure &other)
{
    stop();
    sslclient->socket = other.sslclient->socket;
    _connected = other._connected;
    return *this;
}

void txClientSecure::stop()
{
    if (sslclient->socket >= 0)
    {
        ssl_disconnect(sslclient);
        sslclient->socket = -1;
        _connected = false;
        _peek = -1;
    }
    ssl_destroy(sslclient);
}

int txClientSecure::connect(IPAddress ip, uint16_t port)
{
    if (_pskIdent && _psKey)
        return connect(ip, port, _pskIdent, _psKey);
    return connect(ip, port, _CA_cert, _cert, _private_key);
}

int txClientSecure::connect(IPAddress ip, uint16_t port, int32_t timeout)
{
    _timeout = timeout;
    return connect(ip, port);
}

int txClientSecure::connect(const char *host, uint16_t port)
{
    if (_pskIdent && _psKey)
        return connect(host, port, _pskIdent, _psKey);
    return connect(host, port, _CA_cert, _cert, _private_key);
}

int txClientSecure::connect(const char *host, uint16_t port, int32_t timeout)
{
    _timeout = timeout;
    return connect(host, port);
}

int txClientSecure::connect(IPAddress ip, uint16_t port, const char *_CA_cert, const char *_cert, const char *_private_key)
{
    return connect(ip.toString().c_str(), port, _CA_cert, _cert, _private_key);
}

int txClientSecure::connect(const char *host, uint16_t port, const char *_CA_cert, const char *_cert, const char *_private_key)
{
    if (_timeout > 0)
    {
        sslclient->handshake_timeout = _timeout;
    }
    int ret = start_ssl_client(sslclient, host, port, _timeout, _CA_cert, _cert, _private_key, NULL, NULL);
    _lastError = ret;
    if (ret < 0)
    {
        //log_e("start_ssl_client: %d", ret);
        stop();
        return 0;
    }
    _connected = true;
    return 1;
}

int txClientSecure::connect(IPAddress ip, uint16_t port, const char *pskIdent, const char *psKey)
{
    return connect(ip.toString().c_str(), port, _pskIdent, _psKey);
}

int txClientSecure::connect(const char *host, uint16_t port, const char *pskIdent, const char *psKey)
{
    //log_v("start_ssl_client with PSK");
    if (_timeout > 0)
        sslclient->handshake_timeout = _timeout;
    int ret = start_ssl_client(sslclient, host, port, _timeout, NULL, NULL, NULL, _pskIdent, _psKey);
    _lastError = ret;
    if (ret < 0)
    {
        //log_e("start_ssl_client: %d", ret);
        stop();
        return 0;
    }
    _connected = true;
    return 1;
}

int txClientSecure::peek()
{
    if (_peek >= 0)
        return _peek;
    _peek = timedRead();
    return _peek;
}

size_t txClientSecure::write(uint8_t data)
{
    return write(&data, 1);
}

int txClientSecure::read()
{
    uint8_t data = -1;
    int res = read(&data, 1);
    if (res < 0)
        return res;
    return data;
}

size_t txClientSecure::write(const uint8_t *buf, size_t size)
{
    if (!_connected)
        return 0;
    int res = send_ssl_data(sslclient, buf, size);
    if (res < 0)
    {
        stop();
        res = 0;
    }
    return res;
}

int txClientSecure::read(uint8_t *buf, size_t size)
{
    int peeked = 0;
    int avail = available();
    if ((!buf && size) || avail <= 0)
        return -1;
    if (!size)
        return 0;
    if (_peek >= 0)
    {
        buf[0] = _peek;
        _peek = -1;
        size--;
        avail--;
        if (!size || !avail)
            return 1;
        buf++;
        peeked = 1;
    }
    int res = get_ssl_receive(sslclient, buf, size);
    if (res < 0)
    {
        stop();
        return peeked ? peeked : res;
    }
    return res + peeked;
}

int txClientSecure::available()
{
    int peeked = (_peek >= 0);
    if (!_connected)
        return peeked;
    int res = data_to_read(sslclient);
    if (res < 0)
    {
        stop();
        return peeked ? peeked : res;
    }
    return res + peeked;
}

uint8_t txClientSecure::connected()
{
    uint8_t dummy = 0;
    read(&dummy, 0);
    return _connected;
}

void txClientSecure::setCACert(const char *rootCA)
{
    _CA_cert = rootCA;
}

void txClientSecure::setCertificate(const char *client_ca)
{
    _cert = client_ca;
}

void txClientSecure::setPrivateKey(const char *private_key)
{
    _private_key = private_key;
}

void txClientSecure::setPreSharedKey(const char *pskIdent, const char *psKey)
{
    _pskIdent = pskIdent;
    _psKey = psKey;
}

bool txClientSecure::verify(const char *fp, const char *domain_name)
{
    if (!sslclient)
        return false;
    return verify_ssl_fingerprint(sslclient, fp, domain_name);
}

char *txClientSecure::_streamLoad(Stream &stream, size_t size)
{
    static char *dest = nullptr;
    if (dest)
        free(dest);
    dest = (char *)malloc(size);
    if (!dest)
        return nullptr;
    if (size != stream.readBytes(dest, size))
    {
        free(dest);
        dest = nullptr;
    }
    return dest;
}

bool txClientSecure::loadCACert(Stream &stream, size_t size)
{
    char *dest = _streamLoad(stream, size);
    bool ret = false;
    if (dest)
    {
        setCACert(dest);
        ret = true;
    }
    return ret;
}

bool txClientSecure::loadCertificate(Stream &stream, size_t size)
{
    char *dest = _streamLoad(stream, size);
    bool ret = false;
    if (dest)
    {
        setCertificate(dest);
        ret = true;
    }
    return ret;
}

bool txClientSecure::loadPrivateKey(Stream &stream, size_t size)
{
    char *dest = _streamLoad(stream, size);
    bool ret = false;
    if (dest)
    {
        setPrivateKey(dest);
        ret = true;
    }
    return ret;
}

int txClientSecure::lastError(char *buf, const size_t size)
{
    if (!_lastError)
        return 0;
    char error_buf[100];
    //mbedtls_strerror(_lastError, error_buf, 100);
    snprintf(buf, size, "%s", error_buf);
    return _lastError;
}

void txClientSecure::setHandshakeTimeout(unsigned long handshake_timeout)
{
    sslclient->handshake_timeout = handshake_timeout * 1000;
}