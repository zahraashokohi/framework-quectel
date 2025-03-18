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

#ifndef API_MQTT_H 
#define API_MQTT_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "os_ssl.h"

#define MQTTString_initializer \
    {                          \
        NULL, { 0, NULL }      \
    }

#define MQTTPacket_willOptions_initializer                                  \
    {                                                                       \
        {'M', 'Q', 'T', 'W'}, 0, {NULL, {0, NULL}}, {NULL, {0, NULL}}, 0, 0 \
    }
#define MQTTPacket_connectData_initializer                         \
    {                                                              \
        {'M', 'Q', 'T', 'C'}, 0, 4, {NULL, {0, NULL}}, 60, 1, 0,   \
            MQTTPacket_willOptions_initializer, {NULL, {0, NULL}}, \
        {                                                          \
            NULL, { 0, NULL }                                      \
        }                                                          \
    }

    typedef enum
    {
        QOS0 = 0x0,
        QOS1 = 0x1,
        QOS2 = 0x2,
    } QoS;

    typedef struct
    {
        int len;
        uint8_t *data;
    } MQTTLenString;

    typedef struct
    {
        char *cstring;
        MQTTLenString lenstring;
    } MQTTString;

    typedef struct
    {
        QoS qos;
        uint8_t retained;
        uint8_t dup;
        uint16_t id;
        void *payload;
        size_t payloadlen;
    } MQTTMessage;

    typedef struct
    {
        MQTTMessage *message;
        MQTTString *topicName;
    } MessageData;

    typedef struct
    {
        const uint8_t *topicFilter;
        void (*fp)(MessageData *);
    } MessageHandlers;

    typedef void (*messageHandler)(MessageData *);

    typedef struct MqttNetwork
    {
        int my_socket;
        int (*mqttread)(struct MqttNetwork *, uint8_t *, int, int);
        int (*mqttwrite)(struct MqttNetwork *, uint8_t *, int, int);
        void (*disconnect)(struct MqttNetwork *);
        void (*on_disconnect_callback)(struct MqttNetwork *);
        mbedtls_ssl_context ssl;
        mbedtls_net_context fd;
        mbedtls_ssl_config conf;
        mbedtls_x509_crt cacertl;
        mbedtls_x509_crt clicert;
        mbedtls_pk_context pkey;
    } MqttNetwork;

    typedef struct
    {
        unsigned int systick_period;
        unsigned int end_time;
    } MqttTimer;

    typedef struct
    {
        unsigned int next_packetid;
        unsigned int command_timeout_ms;
        size_t buf_size;
        size_t readbuf_size;
        uint8_t *buf;
        uint8_t *readbuf;
        unsigned int keepAliveInterval;
        uint8_t ping_outstanding;
        int isconnected;
        int isvalid;
        MessageHandlers messageHandlers[5];
        void (*defaultMessageHandler)(MessageData *);
        MqttNetwork *ipstack;
        MqttTimer ping_timer;
    } MqttClient;

    typedef struct
    {
        uint8_t struct_id[4];
        int struct_version;
        MQTTString topicName;
        MQTTString message;
        uint8_t retained;
        uint8_t qos;
    } MQTTPacket_willOptions __attribute__((aligned(4)));

    typedef struct
    {
        uint8_t struct_id[4];
        int struct_version;
        uint8_t MQTTVersion;
        MQTTString clientID;
        uint16_t keepAliveInterval;
        uint8_t cleansession;
        uint8_t willFlag;
        MQTTPacket_willOptions will;
        MQTTString username;
        MQTTString password;
    } MQTTPacket_connectData;

#ifdef __cplusplus
}
#endif
#endif