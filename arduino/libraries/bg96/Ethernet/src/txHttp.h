/*
 * Http.h
 *
 *  Created on: 26.06.2018
 *      Author: georgi.angelov
 */

#ifndef HTTPC_H_
#define HTTPC_H_

#include <interface.h>

typedef enum
{
    HTTP_GET = QAPI_NET_HTTP_CLIENT_GET_E,
    HTTP_POST = QAPI_NET_HTTP_CLIENT_POST_E,
    HTTP_PUT = QAPI_NET_HTTP_CLIENT_PUT_E,
    HTTP_PATCH = QAPI_NET_HTTP_CLIENT_PATCH_E,
    HTTP_HEAD = QAPI_NET_HTTP_CLIENT_HEAD_E,
    HTTP_CONNECT = QAPI_NET_HTTP_CLIENT_CONNECT_E
} HTTP_Method_e;

class Http
{
private:
    bool started = false;
    qapi_Net_HTTPc_handle_t handle = NULL;
    qapi_Net_SSL_Obj_Hdl_t ssl_handle = 0;
    
    static void callback(void *user, int32 state, void *data)
    {
        if (user)
        {
            Http *THIS = (Http *)user;
            if (THIS->onRequest)
                THIS->onRequest(*THIS, state, data);
        }
    }

public:
    void (*onRequest)(Http &H, int32 state, void *data) = NULL;

    Http()
    {
        httpc_Max_Body_Length = 1024;
        httpc_Max_Header_Length = 1024;
    }

    ~Http()
    {
        disconnect();
        destroy();
    }

    operator bool() { return handle != NULL; }

    uint32_t httpc_Max_Body_Length;
    uint32_t httpc_Max_Header_Length;

    void begin(void)
    { // Starts or Restarts
        started = qapi_Net_HTTPc_Start() == 0;
    }

    void end(void)
    {
        disconnect();
        destroy();
        if (started)
            qapi_Net_HTTPc_Stop();
    }

    int destroy()
    {
        int err = 0;
        if (handle)
        {
            err = qapi_Net_HTTPc_Free_sess(handle);
            handle = NULL;
        }
        return err;
    }

    int disconnect()
    {
        if (handle)
            return qapi_Net_HTTPc_Disconnect(handle);
        return -1;
    }

    int clearHeader()
    {
        if (!handle)
            return -1;
        return qapi_Net_HTTPc_Clear_Header(handle);
    }

    int create(uint32_t timeout)
    {
        if (!timeout)
            timeout = 5000;
        handle = qapi_Net_HTTPc_New_sess(timeout, ssl_handle, callback, this, 1024, 1024);
        return handle == NULL;
    }

    int create_ex(uint32_t timeout, qapi_HTTPc_CB_t cb, void *user)
    {
        if (!timeout)
            timeout = 5000;
        handle = qapi_Net_HTTPc_New_sess(timeout, ssl_handle, cb, user, httpc_Max_Body_Length, httpc_Max_Header_Length);
        return handle == NULL;
    }

    int addBody(char *body, uint32_t size)
    {
        if (!handle || !body || !size)
            return -1;
        return qapi_Net_HTTPc_Set_Body(handle, (const char *)body, size);
    }

    int addParam(char *key, char *value)
    {
        if (!handle || !key || !value)
            return -1;
        return qapi_Net_HTTPc_Set_Param(handle, (const char *)key, (const char *)value);
    }

    int addHeader(char *key, char *value)
    {
        if (!handle || !key || !value)
            return -1;
        return qapi_Net_HTTPc_Add_Header_Field(handle, (const char *)key, (const char *)value);
    }

    int connect(char *url, uint16_t port)
    {
        if (!handle || !url || !port)
            return -1;
        return qapi_Net_HTTPc_Connect(handle, url, port);
    }

    int request(HTTP_Method_e method, char *url)
    {
        if (!handle)
            return -1;
        return qapi_Net_HTTPc_Request(handle, method, url);
    }

    int http_config(qapi_Net_HTTPc_Config_t *cfg)
    {
        if (!handle || !cfg)
            return -1;
        return qapi_Net_HTTPc_Configure(handle, cfg);
    }

    int proxy_config(char *url, uint16_t port, uint8_t secure_proxy)
    {
        if (!handle || !url || !port)
            return -1;
        return qapi_Net_HTTPc_Proxy_Connect(handle, (const char *)url, port, secure_proxy);
    }

    int ssl_config(qapi_Net_SSL_Config_t *ssl)
    {
        if (!handle || !ssl)
            return -1;
        return qapi_Net_HTTPc_Configure_SSL(handle, ssl);
    }
};

#endif /* HTTPC_H_ */
