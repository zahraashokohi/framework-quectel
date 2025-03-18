////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 Georgi Angelov
//  * Missing functions *
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

#ifndef OS_API_H_
#define OS_API_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "sys/socket.h"
#include "sys/select.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>

#include <custom_feature_def.h>
#include <ql_type.h>
#include <ql_system.h>
#include <ql_stdlib.h>
#include <ql_power.h>
#include <ql_uart.h>
#include <ql_timer.h>
#include <ql_rtc.h>
#include <ril.h>

#include "os_uart.h"
#include "os_dbg.h"
#include "os_task.h"
#include "os_ssl.h"
#include "os_http.h"
#include "os_mqtt.h"
#include "os_ril.h"

  void os_api_setup(void);

  extern const char *firmware;
  static inline const char *os_get_firmware(void) { return firmware; }

  /*** API STRUCT ***/

  typedef void (*sleep_callback_t)(void *, uint32_t);

  typedef struct api_s
  {
    const char *name;    // test string
    const char *version; // address
    //////////////////////////////////////////////////////////////////////////////
    void (*hal_sleep_manager_register_suspend_callback)(sleep_callback_t, void *, uint32_t); // before sleep
    int (*hal_flash_erase)(uint32_t start_address, int block);                               // filesystem
    int (*hal_flash_write)(uint32_t address, const uint8_t *data, uint32_t length);          // filesystem
    //////////////////////////////////////////////////////////////////////////////
    int (*xTaskGenericCreate)(void (*)(void *), const char *const, const uint16_t, void *const, unsigned int, int, int, int); // pthread
    void (*vTaskDelete)(void *);                                                                                              // pthread
    void *(*xTimerCreate)(const char *const, const uint32_t, const uint32_t, void *const, void (*)(void *));
    int (*xTimerGenericCommand)(void *, const int, const uint32_t, int *const, const uint32_t);
    //////////////////////////////////////////////////////////////////////////////
    int (*socket)(int, int, int);
    int (*connect)(int, const struct sockaddr *, socklen_t);
    int (*closesocket)(int);
    int (*shutdown)(int, int);
    int (*bind)(int, const struct sockaddr *, socklen_t);
    int (*accept)(int, struct sockaddr *, socklen_t *);
    int (*listen)(int, int);
    int (*select)(int, fd_set *, fd_set *, fd_set *, struct timeval *);
    int (*recvfrom)(int, void *, size_t, int, struct sockaddr *, socklen_t *);
    int (*send)(int, const void *, size_t, int);
    int (*sendto)(int, const void *, size_t, int, const struct sockaddr *, socklen_t);
    int (*setsockopt)(int, int, int, const void *, socklen_t);
    int (*getsockopt)(int, int, int, void *, socklen_t *); // getsockopt not exist in some versions, reserved
    int (*fcntl)(int, int, int);
    int (*getaddrinfo)(const char *, const char *, const struct addrinfo *, struct addrinfo **);
    void (*freeaddrinfo)(struct addrinfo *);
    //////////////////////////////////////////////////////////////////////////////
    int (*httpclient_common)(httpclient_t *, char *, int, httpclient_data_t *); // get, post, etc
    int (*httpclient_connect)(httpclient_t *, char *);                          // need only for ssl
    void (*httpclient_close)(httpclient_t *);                                   // need only for ssl
    //////////////////////////////////////////////////////////////////////////////
    void (*MQTTNewNetwork)(MqttNetwork *n);
    int (*MQTTConnectNetwork)(MqttNetwork *n, char *addr, char *port);
    void (*MQTTClient)(MqttClient *c, MqttNetwork *MqttNetwork, unsigned int command_timeout_ms, uint8_t *buf, size_t buf_size, uint8_t *readbuf, size_t readbuf_size);
    int (*MQTTConnect)(MqttClient *c, MQTTPacket_connectData *options);
    int (*MQTTDisconnect)(MqttClient *c);
    int (*MQTTSubscribe)(MqttClient *c, const char *topicFilter, QoS qos, messageHandler messageHandler);
    int (*MQTTUnsubscribe)(MqttClient *c, const char *topicFilter);
    int (*MQTTPublish)(MqttClient *c, const char *topicName, MQTTMessage *message);
    int (*MQTTYield)(MqttClient *c, int timeout_ms);
    //////////////////////////////////////////////////////////////////////////////
    int (*mbedtls_ssl_read)(mbedtls_ssl_context *ssl, unsigned char *, size_t);
    int (*mbedtls_ssl_write)(mbedtls_ssl_context *ssl, const unsigned char *, size_t);
    void (*mbedtls_ssl_conf_read_timeout)(mbedtls_ssl_config *, uint32_t);
    void (*mbedtls_ssl_set_bio)(mbedtls_ssl_context *ssl, void *p_bio, mbedtls_ssl_send_t *f_send, mbedtls_ssl_recv_t *f_recv, mbedtls_ssl_recv_timeout_t *f_recv_timeout);
    //////////////////////////////////////////////////////////////////////////////
  } api_t;

  extern volatile api_t *API;

  /*** IMPLEMENTATION ***/

#define SI static inline

  /* HALL */
  SI void os_reset(void) { Ql_Reset(0); }
  SI void os_power_off(void) { Ql_PowerDown(0); }
  SI const uint32_t *os_get_uid(void) { return (const uint32_t *)0xA20A0610; /* uint32_t uid[4] */ }                                        /* device UID */
  SI void os_before_sleep(void (*cb)(void *user, uint32_t mode), void *d) { API->hal_sleep_manager_register_suspend_callback(cb, d, 0xC); } /* register callback before deep-sleep */
  SI void os_enable_sleep(void) { Ql_SleepEnable(); }                                                                                       /* enable deep-sleep mode */
  SI void os_disable_sleep(void) { Ql_SleepDisable(); }                                                                                     /* disable deep-sleep mode */

/* TASK */
#define DEFAUT_PRIO 4
#define DEFAULT_STACK_SIZE 1024 /* bytes */
  void *os_task_create(void (*func)(void *), void *const arg, const char *const name, const uint16_t stack);
  BaseType_t os_task_create_ex(TaskFunction_t pxTaskCode, const char *const pcName, const uint16_t usStackDepth,
                               void *const pvParameters, UBaseType_t uxPriority, TaskHandle_t *const pxCreatedTask,
                               StackType_t *const puxStackBuffer, const void *const xRegions);
  SI void os_task_delete(void *h) { API->vTaskDelete(h); }

  /* TIMER follow FreeRTOS */
  SI void *os_timer_create(const char *const n, const uint32_t p, const uint32_t r, void *const i, void (*cb)(void *xTimer)) { return API->xTimerCreate(n, p, r, i, cb); }
  SI int os_timer_generic_command(void *t, const int i, const uint32_t v, int *const h, const uint32_t w) { return API->xTimerGenericCommand(t, i, v, h, w); } /* used from macros */

  /*  HTTP CLIENT */
  SI int os_http_execute(httpclient_t *c, char *u, int m, httpclient_data_t *d) { return API->httpclient_common(c, u, m, d); } /* complex function get, post... etc */
  SI int os_http_connect(httpclient_t *c, char *u) { return API->httpclient_connect(c, u); }                                   /* only for ssl */
  SI void os_http_close(httpclient_t *c) { API->httpclient_close(c); }                                                         /* only for ssl */

  /* MQTT CLIENT follow Eclipse client */
  SI void os_mqtt_NewNetwork(MqttNetwork *n) { API->MQTTNewNetwork(n); }
  SI int os_mqtt_ConnectNetwork(MqttNetwork *n, char *a, char *p) { return API->MQTTConnectNetwork(n, a, p); }
  SI void os_mqtt_Client(MqttClient *c, MqttNetwork *n, unsigned int t, uint8_t *tx, size_t tx_size, uint8_t *rx_buf, size_t rx_size) { API->MQTTClient(c, n, t, tx, tx_size, rx_buf, rx_size); }
  SI int os_mqtt_Connect(MqttClient *c, MQTTPacket_connectData *o) { return API->MQTTConnect(c, o); }
  SI int os_mqtt_Disconnect(MqttClient *c) { return API->MQTTDisconnect(c); }
  SI int os_mqtt_Subscribe(MqttClient *c, const char *t, QoS q, messageHandler h) { return API->MQTTSubscribe(c, t, q, h); }
  SI int os_mqtt_Unsubscribe(MqttClient *c, const char *t) { return API->MQTTUnsubscribe(c, t); }
  SI int os_mqtt_Publish(MqttClient *c, const char *t, MQTTMessage *m) { return API->MQTTPublish(c, t, m); }
  SI int os_mqtt_Yield(MqttClient *c, int t) { return API->MQTTYield(c, t); }

  /* SSL follow mbedtls, ssl/tls is too big for here, this functions need only for mqtt */
  SI int os_ssl_read(mbedtls_ssl_context *s, unsigned char *b, size_t l) { return API->mbedtls_ssl_read(s, b, l); }
  SI int os_ssl_write(mbedtls_ssl_context *s, const unsigned char *b, size_t l) { return API->mbedtls_ssl_write(s, b, l); }
  SI void os_ssl_conf_read_timeout(mbedtls_ssl_config *c, uint32_t t) { API->mbedtls_ssl_conf_read_timeout(c, t); };
  SI void os_ssl_set_bio(mbedtls_ssl_context *s, void *p, mbedtls_ssl_send_t *f_send, mbedtls_ssl_recv_t *f_recv, mbedtls_ssl_recv_timeout_t *f_recv_timeout) { API->mbedtls_ssl_set_bio(s, p, f_send, f_recv, f_recv_timeout); }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  bool os_resolve_address(const char *server, struct sockaddr_in *addr);
  uint32_t getHostByName(const char *host);

  u32 Ql_GenHash(char *strSrc, u32 len);
  SI unsigned int os_hash(const char *s) { return Ql_GenHash((char *)s, Ql_strlen(s)); }

#ifdef __cplusplus
}
#endif
#endif