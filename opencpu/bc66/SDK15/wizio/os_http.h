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

#ifndef API_HTTP_H_
#define API_HTTP_H_

#include "os_ssl.h"

typedef enum
{
	HTTP_GET,
	HTTP_POST,
	HTTP_PUT,
	HTTP_DELETE,
	HTTP_HEAD
} HTTP_REQUEST_TYPE;

typedef enum {
    HTTP_ERROR_PARSE = -6,           /**< A URL parse error occurred. */
    HTTP_UNRESOLVED_DNS = -5,        /**< Could not resolve the hostname. */
    HTTP_ERROR_PRTCL = -4,           /**< A protocol error occurred. */
    HTTP_ERROR = -3,                 /**< An unknown error occurred. */
    HTTP_CLOSED = -2,                /**< Connection was closed by a remote host. */
    HTTP_ERROR_CONN = -1,            /**< Connection failed. */
    HTTP_OK = 0,                     /**< The operation was successful. */
    HTTP_RETRIEVE_MORE_DATA = 1      /**< More data needs to be retrieved. */
} HTTP_RESULT;

typedef struct
{
	int socket;
	int remote_port;
	int response_code;
	char *header;
	char *auth_user;
	char *auth_password;
	uint8_t is_http;
	int method;
	unsigned int timeout_in_sec;
	int start_pos;
	int total_len;
	int retry_cnt;
	const char *server_cert;
	const char *client_cert;
	const char *client_pk;
	int server_cert_len;
	int client_cert_len;
	int client_pk_len;
	void *ssl;
	uint8_t is_disable_request_header_range_flag;
	int reserved;
} httpclient_t;

typedef struct
{
	uint8_t is_more;
	uint8_t is_chunked;
	int retrieve_len;
	int response_content_len;
	int content_block_len;
	int post_buf_len;
	int response_buf_len;
	int header_buf_len;
	char *post_content_type;
	char *post_buf;
	char *response_buf;
	char *header_buf;
	int reserved;
} httpclient_data_t;

/* helper struct, do not use */
typedef struct {
    mbedtls_ssl_context ssl_ctx;        
    mbedtls_net_context net_ctx;        
    mbedtls_ssl_config ssl_conf;        
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_x509_crt_profile profile;
    mbedtls_x509_crt cacert;
    mbedtls_x509_crt clicert;
    mbedtls_pk_context pkey;
} httpclient_ssl_t;

#endif /* API_HTTP_H_ */
