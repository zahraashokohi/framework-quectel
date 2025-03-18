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

#include <os_api.h>
#include <WString.h>
#include <IPAddress.h>

#define DEBUG_MQTT
//::printf

class LMQTT
{
private:
    size_t _sz;
    uint8_t *_tx;
    uint8_t *_rx;
    unsigned int _cmd_timeout;

    MqttNetwork NT;
    MqttClient CL;
    MQTTPacket_connectData CD;

public:
    void (*onDisconnect)(void);

    ~LMQTT()
    {
        disconnect();
        free(_tx);
        free(_rx);
    }

    LMQTT(size_t size = 256)
    {
        if (size > 1024)
            size = 1024;
        if (size < 64)
            size = 64;
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
        NT.on_disconnect_callback = (void (*)(struct MqttNetwork *))onDisconnect; // ignore net, called from mqtt_read
        os_mqtt_NewNetwork(&NT);
    }

    void disconnect()
    {
        if (CL.isconnected)
        {
            DEBUG_MQTT("[MQTT] MQTTDisconnect\n");
            os_mqtt_Disconnect(&CL);
            NT.disconnect(&NT);
        }
    }

    bool connect(IPAddress ip, unsigned int port)
    {
        char host[64];
        if (4 != sprintf(host, "%d.%d.%d.%d", ip[3], ip[2], ip[1], ip[0]))
            return -1;
        char prt[16];
        if (1 != sprintf(prt, "%u", port))
            return -1;
        return connect((const char *)host, (const char *)prt);
    }

    bool connect(const char *host, const char *port)
    {
        int res = -1;
        if (host && port)
        {
            if (NULL == CD.clientID.cstring)
            {
                char imei[16], id[64];
                RIL_GetIMEI(imei);
                snprintf(id, 64, "QUECTEL-BC66-%s", imei);
                CD.clientID.cstring = id;
            }

            res = os_mqtt_ConnectNetwork(&NT, (char *)host, (char *)port);
            DEBUG_MQTT("[MQTT] Network: %d\n", res);
            if (0 == res)
            {
                os_mqtt_Client(&CL, &NT, _cmd_timeout, _tx, _sz, _rx, _sz);
                res = os_mqtt_Connect(&CL, &CD);
            }
        }
        DEBUG_MQTT("[MQTT] Connect: %d\n", res);
        return CL.isconnected;
    }

    #include "mqtt_common"
};