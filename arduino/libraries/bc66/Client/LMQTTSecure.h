//#pragma GCC error "LMQTTSecure is not ready yet"

#include <LHTTP.h>

class LMQTTSecure : private LHTTP
{

private:
    size_t _sz;
    uint8_t *_tx;
    uint8_t *_rx;
    unsigned int _cmd_timeout;

    MqttNetwork NT;
    MqttClient CL;
    MQTTPacket_connectData CD;

    static int mqtt_ssl_read(struct MqttNetwork *net, uint8_t *buf, int len, int timeout)
    { //// we not use original net.ssl
        LMQTTSecure *THIS = (LMQTTSecure *)net->ssl.conf;
        mbedtls_ssl_context *ctx = (mbedtls_ssl_context *)THIS->context();
        int res = -1;
        if (ctx && buf)
        {
            os_ssl_conf_read_timeout(ctx->conf, timeout);
            for (int i = 0;; i += res)
            {
                if (i >= len)
                    return i;
                res = os_ssl_read(ctx, buf + i, len - i);
                if (res <= 0)
                    break;
            }
        }
        return res;
    }

    static int mqtt_ssl_write(struct MqttNetwork *net, uint8_t *buf, int len, int timeout)
    { //// we not use original net.ssl
        LMQTTSecure *THIS = (LMQTTSecure *)net->ssl.conf;
        mbedtls_ssl_context *ctx = (mbedtls_ssl_context *)THIS->context();
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

    static void mqtt_ssl_disconnect(struct MqttNetwork *net)
    { //// we not use original net.ssl
        LMQTTSecure *THIS = (LMQTTSecure *)net->ssl.conf;
        THIS->disconnect();
    }

public:
    void (*onDisconnect)(void);

    ~LMQTTSecure()
    {
        this->disconnect();
        free(_tx);
        free(_rx);
    }

    LMQTTSecure(size_t size = 256)
    {
        if (size > 1024)
            size = 1024;
        if (size < 256)
            size = 256;
        onDisconnect = NULL;
        _cmd_timeout = 10000;
        _sz = size;
        _tx = (uint8_t *)calloc(1, size);
        _rx = (uint8_t *)calloc(1, size);
        memset(&NT, 0, sizeof(MqttNetwork));
        memset(&CL, 0, sizeof(MqttClient));
        memset(&CD, 0, sizeof(MQTTPacket_connectData));
        CD.MQTTVersion = 4;
        CD.struct_id[0] = 'M';
        CD.struct_id[1] = 'Q';
        CD.struct_id[2] = 'T';
        CD.struct_id[3] = 'C';
        CD.keepAliveInterval = 60;
        CD.cleansession = 1;
        NT.disconnect = mqtt_ssl_disconnect;
        NT.mqttread = mqtt_ssl_read;
        NT.mqttwrite = mqtt_ssl_write;
        NT.on_disconnect_callback = (void (*)(struct MqttNetwork *))onDisconnect; // ignore net, called from mqtt_read
    }

    void disconnect()
    {
        if (CL.isconnected)
        {
            os_mqtt_Disconnect(&CL);
            LHTTP::disconnect();
        }
        CL.isconnected = 0;
    }

    bool connect(const char *url, const char *ca, const char *client_cert, const char *private_key, unsigned int handshake_timeout_in_seconds = 60)
    {
        int res = -1;

        if (!CL.isconnected)
        {
            check_client_id();
            NT.my_socket = LHTTP::handshake(url, ca, client_cert, private_key, handshake_timeout_in_seconds);
            if (NT.my_socket >= 0)
            {
                NT.ssl.conf = (mbedtls_ssl_config *)this; //// we not use original mqtt.ssl... for now
                os_mqtt_Client(&CL, &NT, _cmd_timeout, _tx, _sz, _rx, _sz);
                res = os_mqtt_Connect(&CL, &CD);
            }
            if (res < 0)
                NT.my_socket = LHTTP::disconnect();
        }
        return 0 == res;
    }

#include "mqtt_common"
};
