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

#ifndef LHTTP_H_
#define LHTTP_H_

#include <Arduino.h>
#include <os_http.h>
#include <os_ssl.h>

class LHTTP
{
#define DEFAUT_LEN 512
private:
    httpclient_t client;
    httpclient_data_t data;

    unsigned int limit_len(int len)
    {
        if (len > (4 * 1024))
            len = (4 * 1024);
        if (len < 64)
            len = 64;
        return len;
    }

    void clean_buffers()
    {
        if (data.response_buf && data.response_buf_len)
            memset(data.response_buf, 0, data.response_buf_len);

        if (data.header_buf && data.header_buf_len)
            memset(data.header_buf, 0, data.header_buf_len);
    }

    bool new_response_size(int response_buf_len)
    {
        response_buf_len = limit_len(response_buf_len);
        if (data.response_buf_len != response_buf_len)
        {
            if (NULL == (data.response_buf = (char *)realloc(data.response_buf, response_buf_len)))
                return false;
            else
                data.response_buf_len = response_buf_len;
        }
        clean_buffers();
        return true;
    }

public:
    LHTTP(int bufers_size = DEFAUT_LEN)
    {
        clean(bufers_size);
    }

    ~LHTTP()
    {
        free(data.response_buf);
        free(data.header_buf);
    }

    void clean(int bufers_size = DEFAUT_LEN)
    {
        free(data.response_buf);
        free(data.header_buf);

        memset(&client, 0, sizeof(httpclient_t));
        memset(&data, 0, sizeof(httpclient_data_t));
        client.timeout_in_sec = 60;
        client.socket = -1;

        if (bufers_size)
        {
            bufers_size = limit_len(bufers_size);
            data.header_buf = (char *)malloc(bufers_size);
            data.header_buf_len = bufers_size;
            if (NULL == data.header_buf)
                abort();

            data.response_buf = (char *)malloc(bufers_size);
            data.response_buf_len = bufers_size;
            if (NULL == data.response_buf)
                abort();
        }
    }

    void set_certificates(const char *ca, const char *cert, const char *client_pk)
    {
        client.server_cert = ca;
        client.server_cert_len = (ca) ? strlen(ca) + 1 : 0;
        client.client_cert = cert;
        client.client_cert_len = (cert) ? strlen(cert) + 1 : 0;
        client.client_pk = client_pk;
        client.client_pk_len = (client_pk) ? strlen(client_pk) + 1 : 0;
    }

    void set_timeout(unsigned int timeout_in_sec) { client.timeout_in_sec = timeout_in_sec; }
    void set_retry_cnt(unsigned int retry_cnt) { client.timeout_in_sec = retry_cnt & 3; }

    const char *get_response() { return data.response_buf; }
    const char *get_header() { return data.header_buf; }

    /* return -error or response code */
    int get(const char *url, int response_buf_len = DEFAUT_LEN)
    {
        int res = -1;
        if (url && response_buf_len)
        {
            if (new_response_size(response_buf_len))
            {
                data.response_content_len = -1;
                data.post_buf = NULL;
                data.post_buf_len = 0;
                res = os_http_execute(&client, (char *)url, HTTP_GET, &data);
                if (0 == res)
                    return client.response_code;
            }
        }
        return res;
    }

    /* return -error or response code */
    int post(const char *url, const char *buffer, size_t size, int response_buf_len = DEFAUT_LEN)
    {
        int res = -1;
        if (url && buffer && size && response_buf_len)
        {
            if (new_response_size(response_buf_len))
            {
                data.response_content_len = -1;
                data.post_buf = (char *)buffer;
                data.post_buf_len = size;
                res = os_http_execute(&client, (char *)url, HTTP_POST, &data);
                if (0 == res)
                    return client.response_code;
            }
        }
        return res;
    }

    /*** ONLY FOR SSL ***/

    int socket()
    {
        return client.socket;
    }

    mbedtls_ssl_context *context()
    {
        return &((httpclient_ssl_t *)client.ssl)->ssl_ctx;
    }

    int disconnect()
    {
        if (!client.is_http && client.socket >= 0)
        {
            os_http_close(&client);
            client.socket = -1;
        }
        return -1; 
    }

    int handshake(const char *url, // "https://host_name.com:8883/"
                  const char *server_ca,
                  const char *client_cert,
                  const char *client_privatekey,
                  unsigned int timeout_in_seconds = 60 //
    )
    {
        int res = -1;
        if (url && client.socket < 0 && 0 == memcmp("https://", url, 8))
        {
            memset(&client, 0, sizeof(httpclient_t));
            client.timeout_in_sec = timeout_in_seconds; // handshake timeout
            client.server_cert = server_ca;
            client.client_cert = client_cert;
            client.client_pk = client_privatekey;
            if (server_ca)
                client.server_cert_len = strlen(server_ca) + 1;
            if (client_cert && client_privatekey)
            {
                client.client_cert_len = strlen(client_cert) + 1;
                client.client_pk_len = strlen(client_privatekey) + 1;
            }

            if (0 == (res = os_http_connect(&client, (char *)url)))
                return client.socket;
            disconnect();
        }
        return res;
    }

};

#endif