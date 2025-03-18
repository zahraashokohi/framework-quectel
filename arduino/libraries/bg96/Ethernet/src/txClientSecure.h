/*
  txClientSecure.h - Base class that provides Client SSL
*/

#ifndef txClientSecure_h
#define txClientSecure_h
#include <Arduino.h>
#include "IPAddress.h"
#include "txClient.h"

typedef struct ssl_context
{
    qapi_Net_SSL_Obj_Hdl_t hCtxt;
    qapi_Net_SSL_Con_Hdl_t hConn;
    qapi_Net_SSL_Config_t sConf;
    qapi_Net_SSL_Role_t eRole;
    int socket;
    bool connectFailure;

    unsigned long handshake_timeout;
} ssl_context;

class txClientSecure : public txClient
{
protected:
    ssl_context *sslclient;

    int _lastError = 0;
    int _peek = -1;
    int _timeout = 0;
    const char *_CA_cert;
    const char *_cert;
    const char *_private_key;
    const char *_pskIdent; // identity for PSK cipher suites
    const char *_psKey;    // key in hex for PSK cipher suites

public:
    txClientSecure *next;
    txClientSecure();
    txClientSecure(int socket);
    ~txClientSecure();
    int connect(IPAddress ip, uint16_t port);
    int connect(IPAddress ip, uint16_t port, int32_t timeout);
    int connect(const char *host, uint16_t port);
    int connect(const char *host, uint16_t port, int32_t timeout);
    int connect(IPAddress ip, uint16_t port, const char *rootCABuff, const char *cli_cert, const char *cli_key);
    int connect(const char *host, uint16_t port, const char *rootCABuff, const char *cli_cert, const char *cli_key);
    int connect(IPAddress ip, uint16_t port, const char *pskIdent, const char *psKey);
    int connect(const char *host, uint16_t port, const char *pskIdent, const char *psKey);
    int peek();
    size_t write(uint8_t data);
    size_t write(const uint8_t *buf, size_t size);
    int available();
    int read();
    int read(uint8_t *buf, size_t size);
    void flush() {}
    void stop();
    uint8_t connected();
    int lastError(char *buf, const size_t size);
    void setPreSharedKey(const char *pskIdent, const char *psKey); // psKey in Hex
    void setCACert(const char *rootCA);
    void setCertificate(const char *client_ca);
    void setPrivateKey(const char *private_key);
    bool loadCACert(Stream &stream, size_t size);
    bool loadCertificate(Stream &stream, size_t size);
    bool loadPrivateKey(Stream &stream, size_t size);
    bool verify(const char *fingerprint, const char *domain_name);
    void setHandshakeTimeout(unsigned long handshake_timeout);

    operator bool()
    {
        return connected();
    }

    txClientSecure &operator=(const txClientSecure &other);

    bool operator==(const bool value)
    {
        return bool() == value;
    }

    bool operator!=(const bool value)
    {
        return bool() != value;
    }

    bool operator==(const txClientSecure &);

    bool operator!=(const txClientSecure &rhs)
    {
        return !this->operator==(rhs);
    };

    int socket()
    {
        //return sslclient->socket = -1;
    }

private:
    char *_streamLoad(Stream &stream, size_t size);

    //friend class txServer;
    using Print::write;
};

#endif /* _txCLIENT_H_ */