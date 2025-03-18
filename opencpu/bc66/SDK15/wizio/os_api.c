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

#include "os_api.h"

const char *firmware = "unknown";

volatile api_t *API = NULL;

volatile api_t api_table[4] = {
    {
        "BC66NBR01A10", ///////////////////////////////////////////////////////
        0x0813800D,     // firmware string
        // HAL
        0x080852E8 + 1, // hal_sleep_manager_register_suspend_callback
        0x0807A9B0 + 1, // hal_flash_erase
        0x0807A9FC + 1, // hal_flash_write
        // TASK
        0x08074428 + 1, // xTaskGenericCreate
        0x08074608 + 1, // vTaskDelete
        0x08076194 + 1, // xTimerCreate
        0x080761E4 + 1, // xTimerGenericCommand
        // SOCKETS
        0x0808A658 + 1, // socket
        0x0808A240 + 1, // connect
        0x08089D18 + 1, // close
        0x0808AA78 + 1, // shutdown
        0x0808A1D6 + 1, // bind
        0x0808A16C + 1, // accept
        0x0808A2AA + 1, // listen
        0x0808A720 + 1, // select
        0x0808A30A + 1, // recvfrom
        0x0808A4A8 + 1, // send
        0x0808A3A2 + 1, // sendto
        0x0808AB50 + 1, // setsockopt
        0,              // getsockopt NOT EXIST, reserved
        0x0808AC1C + 1, // fcntl
        0x0808919C + 1, // getaddrinfo
        0x0808918C + 1, // freeaddrinfo
        // HTTP CLIENT
        0x080874F8 + 1, // httpclient_common
        0x08086B74 + 1, // httpclient_connect
        0x0808745C + 1, // httpclient_close
        // MQTT CLIENT
        0x080C1A28 + 1, // MQTT NewNetwork
        0x080C1A48 + 1, // MQTT ConnectNetwork
        0x080C0C58 + 1, // MQTTClient
        0x080C1278 + 1, // MQTTConnect
        0x080C16A0 + 1, // MQTTDisconnect
        0x080C135C + 1, // MQTTSubscribe
        0x080C1484 + 1, // MQTTUnsubscribe
        0x080C1560 + 1, // MQTTPublish
        0x080C1184 + 1, // MQTTYield
        // SSL
        0x080AB230 + 1, // mbedtls_ssl_read
        0x080AA2C4 + 1, // mbedtls_ssl_write
        0x080A8600 + 1, // mbedtls_ssl_conf_read_timeout
        0x080A85F4 + 1  // mbedtls_ssl_set_bio
    },                  /////////////////////////////////////// BC66NBR01A10 //
    {
        "BC66NBR01A07", ///////////////////////////////////////////////////////
        0x0812A0F1,     // firmware string
        // HAL
        0x08079144 + 1, // hal_sleep_manager_register_suspend_callback
        0x0806E9EC + 1, // hal_flash_erase
        0x0806EA38 + 1, // hal_flash_write
        // TASK
        0x08068604 + 1, // xTaskGenericCreate
        0x080687E4 + 1, // vTaskDelete
        0x0806A230 + 1, // xTimerCreate
        0x0806A280 + 1, // xTimerGenericCommand
        // SOCKETS
        0x0807E60D,     // socket
        0x0807E1F5,     // connect
        0x0807DCCC + 1, // close
        0x0807EA2C + 1, // shutdown
        0x0807E18B,     // bind
        0x0807E121,     // accept
        0x0807E25F,     // listen
        0x0807E6D5,     // select
        0x0807E2BF,     // recvfrom
        0x0807E45D,     // send
        0x0807E357,     // sendto
        0x0807EBF9,     // setsockopt
        0x0807EB0D,     // getsockopt
        0x0807ECC5,     // fcntl
        0x0807CE75,     // getaddrinfo
        0x0807CE65,     // freeaddrinfo
        // HTTP CLIENT
        0x0807B1D4 + 1, // httpclient_common
        0x0807A850 + 1, // httpclient_connect
        0x0807B138 + 1, // httpclient_close
        // MQTT CLIENT
        0x080B5238 + 1, // MQTT NewNetwork
        0x080B5258 + 1, // MQTT ConnectNetwork
        0x080B4468 + 1, // MQTTClient
        0x080B4A88 + 1, // MQTTConnect
        0x080B4EB0 + 1, // MQTTDisconnect
        0x080B4B6C + 1, // MQTTSubscribe
        0x080B4C94 + 1, // MQTTUnsubscribe
        0x080B4D70 + 1, // MQTTPublish
        0x080B4994 + 1, // MQTTYield
        // SSL
        0x0809EEA4 + 1, // mbedtls_ssl_read
        0x0809DF38 + 1, // mbedtls_ssl_write
        0x0809C318 + 1, // mbedtls_ssl_conf_read_timeout
        0x0809C30C + 1, // mbedtls_ssl_set_bio
    }                   /////////////////////////////////////// BC66NBR01A07 //
};

/* check suported versions */
void os_api_setup(void)
{
    const char *name;
    const char *version;
    for (int i = 0; i < 2; i++) // only two firmwares
    {
        name = api_table[i].name;
        version = api_table[i].version;
        if (version)
        {
            if (0 == strcmp(name, version))
            {
                API = &api_table[i];
                firmware = name;
            }
        }
    }
}

/* TASK */
BaseType_t os_task_create_ex(TaskFunction_t pxTaskCode,
                             const char *const pcName,
                             const uint16_t usStackDepth, /* words */
                             void *const pvParameters,
                             UBaseType_t uxPriority,
                             TaskHandle_t *const pxCreatedTask,
                             StackType_t *const puxStackBuffer,
                             const void *const xRegions)
{
    return API->xTaskGenericCreate(pxTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask, puxStackBuffer, 0);
}

TaskHandle_t os_task_create(void (*fun)(void *), void *const arg, const char *const name, const uint16_t stack)
{
    TaskHandle_t handle = NULL;
    if (1 != API->xTaskGenericCreate(fun, name, stack, arg, DEFAUT_PRIO, &handle, 0, 0))
        return NULL;
    return handle;
}

/* SOCKETS */
#pragma GCC push_options
#pragma GCC optimize("O0")

int socket(int domain, int type, int protocol) { return API->socket(domain, type, protocol); }
int connect(int fd, const struct sockaddr *n, socklen_t s) { return API->connect(fd, n, s); }
int closesocket(int fd) { return API->closesocket(fd); } /* don`t use close(fd) */
int shutdown(int fd, int h) { return API->shutdown(fd, h); }
int bind(int fd, const struct sockaddr *a, socklen_t s) { return API->bind(fd, a, s); }
int accept(int fd, struct sockaddr *a, socklen_t *s) { return API->accept(fd, a, s); }
int listen(int fd, int b) { return API->listen(fd, b); }
int send(int fd, const void *m, size_t s, int f) { return API->send(fd, m, s, f); }
int sendto(int fd, const void *m, size_t s, int f, const struct sockaddr *to, socklen_t len) { return API->sendto(fd, m, s, f, to, len); }
int recvfrom(int fd, void *m, size_t s, int f, struct sockaddr *from, socklen_t *len) { return API->recvfrom(fd, m, s, f, from, len); }
int recv(int fd, void *m, size_t s, int f) { return API->recvfrom(s, m, s, f, NULL, NULL); }
int getaddrinfo(const char *n, const char *s, const struct addrinfo *h, struct addrinfo **r) { return API->getaddrinfo(n, s, h, r); }
void freeaddrinfo(struct addrinfo *a) { API->freeaddrinfo(a); }
int select(int m, fd_set *r, fd_set *w, fd_set *e, struct timeval *t) { return API->select(m, r, w, e, t); }
int setsockopt(int s, int lev, int name, const void *val, socklen_t len) { return API->setsockopt(s, lev, name, val, len); }
int getsockopt(int s, int lev, int name, void *val, socklen_t *len)
{ // getsockopt not exist in some versions, reserved
    if (API->getsockopt)
        return API->getsockopt(s, lev, name, val, len);
    return -1;
}
int fcntl(int fd, int c, ...)
{
    va_list vl;
    va_start(vl, c);
    int v = va_arg(vl, int);
    return API->fcntl(fd, c, v);
}

#pragma GCC pop_options

int ip4addr_aton(const char *cp, ip4_addr_t *addr)
{ //return 1 if cp could be converted to addr, 0 on failure
    int res = false, d[4];
    if (cp && addr)
    {
        if (4 == sscanf(cp, "%d.%d.%d.%d", &d[3], &d[2], &d[1], &d[0]))
        {
            addr->addr = (d[0] << 24) | (d[1] << 16) | (d[2] << 8) | d[3];
            res = true;
        }
        else
        {
            addr->addr = 0;
        }
    }
    return res;
}

u32_t ipaddr_addr(const char *cp)
{
    u32_t result;
    ip4_addr_t val;
    if (ip4addr_aton(cp, &val))
        result = val.addr;
    else
        result = -1;
    return result;
}

bool os_resolve_address(const char *server, struct sockaddr_in *addr)
{
    int res = -1;
    if (server && addr)
    {
        memset(addr, 0, sizeof(struct sockaddr_in));
        uint32_t val = ipaddr_addr(server);
        if (val != 0xFFFFFFFF)
        {
            addr->sin_family = 2;
            addr->sin_addr.s_addr = val;
            res = 1;
        }
        else
        {
            struct addrinfo *ai;
            struct addrinfo hints;
            memset(&hints, 0, sizeof(hints));
            hints.ai_socktype = 1;
            hints.ai_protocol = 6;
            if (0 == (res = API->getaddrinfo(server, 0, &hints, &ai)))
            {
                while (ai)
                {
                    if (ai->ai_family == 2)
                    {
                        res = ai->ai_addrlen;
                        if (res < 0)
                            break;
                        addr->sin_family = 2;
                        uint32_t *p = (uint32_t *)&ai->ai_addr->sa_data[2];
                        addr->sin_addr.s_addr = *p;
                    }
                    ai = ai->ai_next;
                }
                API->freeaddrinfo(ai);
            }
            else
                res = -1;
        }
    }
    return res > 0;
}

uint32_t getHostByName(const char *host)
{
    struct sockaddr_in a;
    return os_resolve_address(host, &a) ? a.sin_addr.s_addr : 0;
}
