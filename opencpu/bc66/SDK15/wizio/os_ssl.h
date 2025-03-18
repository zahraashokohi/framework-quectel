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

#ifndef API_MBEDTLS_H
#define API_MBEDTLS_H
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

	typedef enum
	{
		MBEDTLS_MD_NONE = 0x0,
		MBEDTLS_MD_MD2 = 0x1,
		MBEDTLS_MD_MD4 = 0x2,
		MBEDTLS_MD_MD5 = 0x3,
		MBEDTLS_MD_SHA1 = 0x4,
		MBEDTLS_MD_SHA224 = 0x5,
		MBEDTLS_MD_SHA256 = 0x6,
		MBEDTLS_MD_SHA384 = 0x7,
		MBEDTLS_MD_SHA512 = 0x8,
		MBEDTLS_MD_RIPEMD160 = 0x9,
	} mbedtls_md_type_t;

	typedef enum
	{
		MBEDTLS_PK_NONE = 0x0,
		MBEDTLS_PK_RSA = 0x1,
		MBEDTLS_PK_ECKEY = 0x2,
		MBEDTLS_PK_ECKEY_DH = 0x3,
		MBEDTLS_PK_ECDSA = 0x4,
		MBEDTLS_PK_RSA_ALT = 0x5,
		MBEDTLS_PK_RSASSA_PSS = 0x6,
	} mbedtls_pk_type_t;

	typedef enum
	{
		MBEDTLS_PK_DEBUG_NONE = 0x0,
		MBEDTLS_PK_DEBUG_MPI = 0x1,
		MBEDTLS_PK_DEBUG_ECP = 0x2,
	} mbedtls_pk_debug_type;

	typedef enum
	{
		MBEDTLS_OPERATION_NONE = 0xFF,
		MBEDTLS_DECRYPT = 0x0,
		MBEDTLS_ENCRYPT = 0x1,
	} mbedtls_operation_t;

	typedef enum
	{
		MBEDTLS_CIPHER_NONE = 0x0,
		MBEDTLS_CIPHER_NULL = 0x1,
		MBEDTLS_CIPHER_AES_128_ECB = 0x2,
		MBEDTLS_CIPHER_AES_192_ECB = 0x3,
		MBEDTLS_CIPHER_AES_256_ECB = 0x4,
		MBEDTLS_CIPHER_AES_128_CBC = 0x5,
		MBEDTLS_CIPHER_AES_192_CBC = 0x6,
		MBEDTLS_CIPHER_AES_256_CBC = 0x7,
		MBEDTLS_CIPHER_AES_128_CFB128 = 0x8,
		MBEDTLS_CIPHER_AES_192_CFB128 = 0x9,
		MBEDTLS_CIPHER_AES_256_CFB128 = 0xA,
		MBEDTLS_CIPHER_AES_128_CTR = 0xB,
		MBEDTLS_CIPHER_AES_192_CTR = 0xC,
		MBEDTLS_CIPHER_AES_256_CTR = 0xD,
		MBEDTLS_CIPHER_AES_128_GCM = 0xE,
		MBEDTLS_CIPHER_AES_192_GCM = 0xF,
		MBEDTLS_CIPHER_AES_256_GCM = 0x10,
		MBEDTLS_CIPHER_CAMELLIA_128_ECB = 0x11,
		MBEDTLS_CIPHER_CAMELLIA_192_ECB = 0x12,
		MBEDTLS_CIPHER_CAMELLIA_256_ECB = 0x13,
		MBEDTLS_CIPHER_CAMELLIA_128_CBC = 0x14,
		MBEDTLS_CIPHER_CAMELLIA_192_CBC = 0x15,
		MBEDTLS_CIPHER_CAMELLIA_256_CBC = 0x16,
		MBEDTLS_CIPHER_CAMELLIA_128_CFB128 = 0x17,
		MBEDTLS_CIPHER_CAMELLIA_192_CFB128 = 0x18,
		MBEDTLS_CIPHER_CAMELLIA_256_CFB128 = 0x19,
		MBEDTLS_CIPHER_CAMELLIA_128_CTR = 0x1A,
		MBEDTLS_CIPHER_CAMELLIA_192_CTR = 0x1B,
		MBEDTLS_CIPHER_CAMELLIA_256_CTR = 0x1C,
		MBEDTLS_CIPHER_CAMELLIA_128_GCM = 0x1D,
		MBEDTLS_CIPHER_CAMELLIA_192_GCM = 0x1E,
		MBEDTLS_CIPHER_CAMELLIA_256_GCM = 0x1F,
		MBEDTLS_CIPHER_DES_ECB = 0x20,
		MBEDTLS_CIPHER_DES_CBC = 0x21,
		MBEDTLS_CIPHER_DES_EDE_ECB = 0x22,
		MBEDTLS_CIPHER_DES_EDE_CBC = 0x23,
		MBEDTLS_CIPHER_DES_EDE3_ECB = 0x24,
		MBEDTLS_CIPHER_DES_EDE3_CBC = 0x25,
		MBEDTLS_CIPHER_BLOWFISH_ECB = 0x26,
		MBEDTLS_CIPHER_BLOWFISH_CBC = 0x27,
		MBEDTLS_CIPHER_BLOWFISH_CFB64 = 0x28,
		MBEDTLS_CIPHER_BLOWFISH_CTR = 0x29,
		MBEDTLS_CIPHER_ARC4_128 = 0x2A,
		MBEDTLS_CIPHER_AES_128_CCM = 0x2B,
		MBEDTLS_CIPHER_AES_192_CCM = 0x2C,
		MBEDTLS_CIPHER_AES_256_CCM = 0x2D,
		MBEDTLS_CIPHER_CAMELLIA_128_CCM = 0x2E,
		MBEDTLS_CIPHER_CAMELLIA_192_CCM = 0x2F,
		MBEDTLS_CIPHER_CAMELLIA_256_CCM = 0x30,
	} mbedtls_cipher_type_t;

	typedef enum
	{
		MBEDTLS_KEY_EXCHANGE_NONE = 0x0,
		MBEDTLS_KEY_EXCHANGE_RSA = 0x1,
		MBEDTLS_KEY_EXCHANGE_DHE_RSA = 0x2,
		MBEDTLS_KEY_EXCHANGE_ECDHE_RSA = 0x3,
		MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA = 0x4,
		MBEDTLS_KEY_EXCHANGE_PSK = 0x5,
		MBEDTLS_KEY_EXCHANGE_DHE_PSK = 0x6,
		MBEDTLS_KEY_EXCHANGE_RSA_PSK = 0x7,
		MBEDTLS_KEY_EXCHANGE_ECDHE_PSK = 0x8,
		MBEDTLS_KEY_EXCHANGE_ECDH_RSA = 0x9,
		MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA = 0xA,
		MBEDTLS_KEY_EXCHANGE_ECJPAKE = 0xB,
	} mbedtls_key_exchange_type_t;

	typedef enum
	{
		MBEDTLS_MODE_NONE = 0x0,
		MBEDTLS_MODE_ECB = 0x1,
		MBEDTLS_MODE_CBC = 0x2,
		MBEDTLS_MODE_CFB = 0x3,
		MBEDTLS_MODE_OFB = 0x4,
		MBEDTLS_MODE_CTR = 0x5,
		MBEDTLS_MODE_GCM = 0x6,
		MBEDTLS_MODE_STREAM = 0x7,
		MBEDTLS_MODE_CCM = 0x8,
	} mbedtls_cipher_mode_t;

	typedef enum
	{
		MBEDTLS_CIPHER_ID_NONE = 0x0,
		MBEDTLS_CIPHER_ID_NULL = 0x1,
		MBEDTLS_CIPHER_ID_AES = 0x2,
		MBEDTLS_CIPHER_ID_DES = 0x3,
		MBEDTLS_CIPHER_ID_3DES = 0x4,
		MBEDTLS_CIPHER_ID_CAMELLIA = 0x5,
		MBEDTLS_CIPHER_ID_BLOWFISH = 0x6,
		MBEDTLS_CIPHER_ID_ARC4 = 0x7,
	} mbedtls_cipher_id_t;

	typedef enum
	{
		MBEDTLS_PADDING_PKCS7 = 0x0,
		MBEDTLS_PADDING_ONE_AND_ZEROS = 0x1,
		MBEDTLS_PADDING_ZEROS_AND_LEN = 0x2,
		MBEDTLS_PADDING_ZEROS = 0x3,
		MBEDTLS_PADDING_NONE = 0x4,
	} mbedtls_cipher_padding_t;

	typedef enum
	{
		MBEDTLS_KEY_LENGTH_NONE = 0x0,
		MBEDTLS_KEY_LENGTH_DES = 0x40,
		MBEDTLS_KEY_LENGTH_DES_EDE = 0x80,
		MBEDTLS_KEY_LENGTH_DES_EDE3 = 0xC0,
	} mbedtls_key_length;

	typedef enum
	{
		MBEDTLS_SSL_HELLO_REQUEST = 0x0,
		MBEDTLS_SSL_CLIENT_HELLO = 0x1,
		MBEDTLS_SSL_SERVER_HELLO = 0x2,
		MBEDTLS_SSL_SERVER_CERTIFICATE = 0x3,
		MBEDTLS_SSL_SERVER_KEY_EXCHANGE = 0x4,
		MBEDTLS_SSL_CERTIFICATE_REQUEST = 0x5,
		MBEDTLS_SSL_SERVER_HELLO_DONE = 0x6,
		MBEDTLS_SSL_CLIENT_CERTIFICATE = 0x7,
		MBEDTLS_SSL_CLIENT_KEY_EXCHANGE = 0x8,
		MBEDTLS_SSL_CERTIFICATE_VERIFY = 0x9,
		MBEDTLS_SSL_CLIENT_CHANGE_CIPHER_SPEC = 0xA,
		MBEDTLS_SSL_CLIENT_FINISHED = 0xB,
		MBEDTLS_SSL_SERVER_CHANGE_CIPHER_SPEC = 0xC,
		MBEDTLS_SSL_SERVER_FINISHED = 0xD,
		MBEDTLS_SSL_FLUSH_BUFFERS = 0xE,
		MBEDTLS_SSL_HANDSHAKE_WRAPUP = 0xF,
		MBEDTLS_SSL_HANDSHAKE_OVER = 0x10,
		MBEDTLS_SSL_SERVER_NEW_SESSION_TICKET = 0x11,
		MBEDTLS_SSL_SERVER_HELLO_VERIFY_REQUEST_SENT = 0x12,
	} mbedtls_ssl_states;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	typedef struct mbedtls_ssl_session mbedtls_ssl_session;
	typedef struct mbedtls_ssl_context mbedtls_ssl_context;
	typedef struct mbedtls_ssl_config mbedtls_ssl_config;

	typedef int mbedtls_ssl_send_t(void *, const unsigned char *, size_t);
	typedef int mbedtls_ssl_recv_t(void *, unsigned char *, size_t);
	typedef int mbedtls_ssl_recv_timeout_t(void *, unsigned char *, size_t, uint32_t);
	typedef void mbedtls_ssl_set_timer_t(void *, uint32_t, uint32_t);
	typedef int mbedtls_ssl_get_timer_t(void *);

	typedef struct
	{
		int tag;
		size_t len;
		unsigned char *p;
	} mbedtls_asn1_buf;

	typedef struct mbedtls_asn1_sequence
	{
		mbedtls_asn1_buf buf;
		struct mbedtls_asn1_sequence *next;
	} mbedtls_asn1_sequence;

	typedef struct mbedtls_asn1_named_data
	{
		mbedtls_asn1_buf oid;
		mbedtls_asn1_buf val;
		struct mbedtls_asn1_named_data *next;
		unsigned char next_merged;
	} mbedtls_asn1_named_data __attribute__((aligned(4)));

	typedef struct
	{
		int code;
		const unsigned char *string;
	} x509_crt_verify_string;

	typedef struct mbedtls_x509_time
	{
		int year;
		int mon;
		int day;
		int hour;
		int min;
		int sec;
	} mbedtls_x509_time;

	typedef mbedtls_asn1_buf mbedtls_x509_buf;
	typedef mbedtls_asn1_named_data mbedtls_x509_name;
	typedef mbedtls_asn1_sequence mbedtls_x509_sequence;

	typedef struct
	{
		int private_context[0x0030];
	} mbedtls_pk_info_t;

	typedef struct
	{
		int private_context[0x0008];
	} mbedtls_pk_context;

	typedef struct mbedtls_x509_crl
	{
		mbedtls_x509_buf raw;
		int private_context[0x00F4];
	} mbedtls_x509_crl;

	typedef struct mbedtls_x509_crt
	{
		mbedtls_x509_buf raw;
		int private_context[0x0134];
	} mbedtls_x509_crt;

	typedef struct
	{
		int private_context[0x0010];
	} mbedtls_x509_crt_profile;

	struct mbedtls_ssl_session
	{
		int private_context[0x0064];
	};

	struct mbedtls_ssl_context /* size = 0xC0 */
	{
		mbedtls_ssl_config *conf; // need conf->read_timeout
		int state;
		int major_ver;
		int minor_ver;
		mbedtls_ssl_send_t *f_send;
		mbedtls_ssl_recv_t *f_recv;
		mbedtls_ssl_recv_timeout_t *f_recv_timeout;
		void *p_bio;
		mbedtls_ssl_session *session_in;
		mbedtls_ssl_session *session_out;
		mbedtls_ssl_session *session; // need this for verify ... session->peer_cert
		mbedtls_ssl_session *session_negotiate;
		void *handshake;
		void *transform_in;
		void *transform_out;
		void *transform;
		void *transform_negotiate;
		void *p_timer;
		mbedtls_ssl_set_timer_t *f_set_timer;
		mbedtls_ssl_get_timer_t *f_get_timer;
		unsigned char *in_buf;
		unsigned char *in_ctr;
		unsigned char *in_hdr;
		unsigned char *in_len;
		unsigned char *in_iv;
		unsigned char *in_msg;
		unsigned char *in_offt;
		int in_msgtype;
		size_t in_msglen; // need this for available bytes
		size_t in_left;
		uint16_t in_epoch;
		size_t next_record_offset;
		size_t in_hslen;
		int nb_zero;
		int record_read;
		unsigned char *out_buf;
		unsigned char *out_ctr;
		unsigned char *out_hdr;
		unsigned char *out_len;
		unsigned char *out_iv;
		unsigned char *out_msg;
		int out_msgtype;
		size_t out_msglen;
		int out_left;
		int client_auth;
		unsigned char *hostname;
		const unsigned char *alpn_chosen;
		int secure_renegotiation;
		int dummy[0x0040];
	};

	struct mbedtls_ssl_config /* size 0x80 */
	{
		int private_context[0x0080];
	};

	typedef struct /* size 0x278 */
	{
		int private_context[0x0278];
	} mbedtls_entropy_context;

	typedef struct /* size 0x148 */
	{
		int private_context[0x0148];
	} mbedtls_ctr_drbg_context;

	typedef struct
	{
		int fd;
	} mbedtls_net_context;

	/************************************************************************************************/

	typedef struct
	{
		uint32_t total[2];
		uint32_t state[4];
		unsigned char buffer[64];
	} mbedtls_md5_context;

	typedef struct
	{
		uint32_t total[2];
		uint32_t state[5];
		unsigned char buffer[64];
	} mbedtls_sha1_context;

	typedef struct
	{
		uint32_t total[2];
		uint32_t state[8];
		unsigned char buffer[64];
		int is224;
	} mbedtls_sha256_context;

	typedef struct
	{
		int nr;
		uint32_t *rk;
		uint32_t buf[68];
	} mbedtls_aes_context;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MBEDTLS_OID_SIZE(x) (sizeof(x) - 1)
#define MBEDTLS_OID_CMP(oid_str, oid_buf)             \
	((MBEDTLS_OID_SIZE(oid_str) != (oid_buf)->len) || \
	 memcmp((oid_str), (oid_buf)->p, (oid_buf)->len) != 0)

#define MBEDTLS_OID_ISO_MEMBER_BODIES "\x2a"  /* {iso(1) member-body(2)} */
#define MBEDTLS_OID_ISO_IDENTIFIED_ORG "\x2b" /* {iso(1) identified-organization(3)} */
#define MBEDTLS_OID_ISO_CCITT_DS "\x55"		  /* {joint-iso-ccitt(2) ds(5)} */
#define MBEDTLS_OID_ISO_ITU_COUNTRY "\x60"	/* {joint-iso-itu-t(2) country(16)} */

#define MBEDTLS_OID_AT MBEDTLS_OID_ISO_CCITT_DS "\x04" /**< id-at OBJECT IDENTIFIER ::= {joint-iso-ccitt(2) ds(5) 4} */
#define MBEDTLS_OID_AT_CN MBEDTLS_OID_AT "\x03"		   /**< id-at-commonName AttributeType:= {id-at 3} */

/*
 * SSL Error codes
 */
#define MBEDTLS_ERR_SSL_FEATURE_UNAVAILABLE -0x7080			  /**< The requested feature is not available. */
#define MBEDTLS_ERR_SSL_BAD_INPUT_DATA -0x7100				  /**< Bad input parameters to function. */
#define MBEDTLS_ERR_SSL_INVALID_MAC -0x7180					  /**< Verification of the message MAC failed. */
#define MBEDTLS_ERR_SSL_INVALID_RECORD -0x7200				  /**< An invalid SSL record was received. */
#define MBEDTLS_ERR_SSL_CONN_EOF -0x7280					  /**< The connection indicated an EOF. */
#define MBEDTLS_ERR_SSL_UNKNOWN_CIPHER -0x7300				  /**< An unknown cipher was received. */
#define MBEDTLS_ERR_SSL_NO_CIPHER_CHOSEN -0x7380			  /**< The server has no ciphersuites in common with the client. */
#define MBEDTLS_ERR_SSL_NO_RNG -0x7400						  /**< No RNG was provided to the SSL module. */
#define MBEDTLS_ERR_SSL_NO_CLIENT_CERTIFICATE -0x7480		  /**< No client certification received from the client, but required by the authentication mode. */
#define MBEDTLS_ERR_SSL_CERTIFICATE_TOO_LARGE -0x7500		  /**< Our own certificate(s) is/are too large to send in an SSL message. */
#define MBEDTLS_ERR_SSL_CERTIFICATE_REQUIRED -0x7580		  /**< The own certificate is not set, but needed by the server. */
#define MBEDTLS_ERR_SSL_PRIVATE_KEY_REQUIRED -0x7600		  /**< The own private_context key or pre-shared key is not set, but needed. */
#define MBEDTLS_ERR_SSL_CA_CHAIN_REQUIRED -0x7680			  /**< No CA Chain is set, but required to operate. */
#define MBEDTLS_ERR_SSL_UNEXPECTED_MESSAGE -0x7700			  /**< An unexpected message was received from our peer. */
#define MBEDTLS_ERR_SSL_FATAL_ALERT_MESSAGE -0x7780			  /**< A fatal alert message was received from our peer. */
#define MBEDTLS_ERR_SSL_PEER_VERIFY_FAILED -0x7800			  /**< Verification of our peer failed. */
#define MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY -0x7880			  /**< The peer notified us that the connection is going to be closed. */
#define MBEDTLS_ERR_SSL_BAD_HS_CLIENT_HELLO -0x7900			  /**< Processing of the ClientHello handshake message failed. */
#define MBEDTLS_ERR_SSL_BAD_HS_SERVER_HELLO -0x7980			  /**< Processing of the ServerHello handshake message failed. */
#define MBEDTLS_ERR_SSL_BAD_HS_CERTIFICATE -0x7A00			  /**< Processing of the Certificate handshake message failed. */
#define MBEDTLS_ERR_SSL_BAD_HS_CERTIFICATE_REQUEST -0x7A80	/**< Processing of the CertificateRequest handshake message failed. */
#define MBEDTLS_ERR_SSL_BAD_HS_SERVER_KEY_EXCHANGE -0x7B00	/**< Processing of the ServerKeyExchange handshake message failed. */
#define MBEDTLS_ERR_SSL_BAD_HS_SERVER_HELLO_DONE -0x7B80	  /**< Processing of the ServerHelloDone handshake message failed. */
#define MBEDTLS_ERR_SSL_BAD_HS_CLIENT_KEY_EXCHANGE -0x7C00	/**< Processing of the ClientKeyExchange handshake message failed. */
#define MBEDTLS_ERR_SSL_BAD_HS_CLIENT_KEY_EXCHANGE_RP -0x7C80 /**< Processing of the ClientKeyExchange handshake message failed in DHM / ECDH Read Public. */
#define MBEDTLS_ERR_SSL_BAD_HS_CLIENT_KEY_EXCHANGE_CS -0x7D00 /**< Processing of the ClientKeyExchange handshake message failed in DHM / ECDH Calculate Secret. */
#define MBEDTLS_ERR_SSL_BAD_HS_CERTIFICATE_VERIFY -0x7D80	 /**< Processing of the CertificateVerify handshake message failed. */
#define MBEDTLS_ERR_SSL_BAD_HS_CHANGE_CIPHER_SPEC -0x7E00	 /**< Processing of the ChangeCipherSpec handshake message failed. */
#define MBEDTLS_ERR_SSL_BAD_HS_FINISHED -0x7E80				  /**< Processing of the Finished handshake message failed. */
#define MBEDTLS_ERR_SSL_ALLOC_FAILED -0x7F00				  /**< Memory allocation failed */
#define MBEDTLS_ERR_SSL_HW_ACCEL_FAILED -0x7F80				  /**< Hardware acceleration function returned with error */
#define MBEDTLS_ERR_SSL_HW_ACCEL_FALLTHROUGH -0x6F80		  /**< Hardware acceleration function skipped / left alone data */
#define MBEDTLS_ERR_SSL_COMPRESSION_FAILED -0x6F00			  /**< Processing of the compression / decompression failed */
#define MBEDTLS_ERR_SSL_BAD_HS_PROTOCOL_VERSION -0x6E80		  /**< Handshake protocol not within min/max boundaries */
#define MBEDTLS_ERR_SSL_BAD_HS_NEW_SESSION_TICKET -0x6E00	 /**< Processing of the NewSessionTicket handshake message failed. */
#define MBEDTLS_ERR_SSL_SESSION_TICKET_EXPIRED -0x6D80		  /**< Session ticket has expired. */
#define MBEDTLS_ERR_SSL_PK_TYPE_MISMATCH -0x6D00			  /**< Public key type mismatch (eg, asked for RSA key exchange and presented EC key) */
#define MBEDTLS_ERR_SSL_UNKNOWN_IDENTITY -0x6C80			  /**< Unknown identity received (eg, PSK identity) */
#define MBEDTLS_ERR_SSL_INTERNAL_ERROR -0x6C00				  /**< Internal error (eg, unexpected failure in lower-level module) */
#define MBEDTLS_ERR_SSL_COUNTER_WRAPPING -0x6B80			  /**< A counter would wrap (eg, too many messages exchanged). */
#define MBEDTLS_ERR_SSL_WAITING_SERVER_HELLO_RENEGO -0x6B00   /**< Unexpected message at ServerHello in renegotiation. */
#define MBEDTLS_ERR_SSL_HELLO_VERIFY_REQUIRED -0x6A80		  /**< DTLS client must retry for hello verification */
#define MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL -0x6A00			  /**< A buffer is too small to receive or write a message */
#define MBEDTLS_ERR_SSL_NO_USABLE_CIPHERSUITE -0x6980		  /**< None of the common ciphersuites is usable (eg, no suitable certificate, see debug messages). */
#define MBEDTLS_ERR_SSL_WANT_READ -0x6900					  /**< Connection requires a read call. */
#define MBEDTLS_ERR_SSL_WANT_WRITE -0x6880					  /**< Connection requires a write call. */
#define MBEDTLS_ERR_SSL_TIMEOUT -0x6800						  /**< The operation timed out. */
#define MBEDTLS_ERR_SSL_CLIENT_RECONNECT -0x6780			  /**< The client initiated a reconnect from the same port. */
#define MBEDTLS_ERR_SSL_UNEXPECTED_RECORD -0x6700			  /**< Record header looks valid but is not expected. */
#define MBEDTLS_ERR_SSL_NON_FATAL -0x6680					  /**< The alert message received indicates a non-fatal error. */
#define MBEDTLS_ERR_SSL_INVALID_VERIFY_HASH -0x6600			  /**< Couldn't set the hash for verifying CertificateVerify */

/*
 * Various constants
 */
#define MBEDTLS_SSL_MAJOR_VERSION_3 3
#define MBEDTLS_SSL_MINOR_VERSION_0 0 /*!< SSL v3.0 */
#define MBEDTLS_SSL_MINOR_VERSION_1 1 /*!< TLS v1.0 */
#define MBEDTLS_SSL_MINOR_VERSION_2 2 /*!< TLS v1.1 */
#define MBEDTLS_SSL_MINOR_VERSION_3 3 /*!< TLS v1.2 */

#define MBEDTLS_SSL_TRANSPORT_STREAM 0   /*!< TLS      */
#define MBEDTLS_SSL_TRANSPORT_DATAGRAM 1 /*!< DTLS     */

#define MBEDTLS_SSL_MAX_HOST_NAME_LEN 255 /*!< Maximum host name defined in RFC 1035 */

/* RFC 6066 section 4, see also mfl_code_to_length in ssl_tls.c
 * NONE must be zero so that memset()ing structure to zero works */
#define MBEDTLS_SSL_MAX_FRAG_LEN_NONE 0	/*!< don't use this extension   */
#define MBEDTLS_SSL_MAX_FRAG_LEN_512 1	 /*!< MaxFragmentLength 2^9      */
#define MBEDTLS_SSL_MAX_FRAG_LEN_1024 2	/*!< MaxFragmentLength 2^10     */
#define MBEDTLS_SSL_MAX_FRAG_LEN_2048 3	/*!< MaxFragmentLength 2^11     */
#define MBEDTLS_SSL_MAX_FRAG_LEN_4096 4	/*!< MaxFragmentLength 2^12     */
#define MBEDTLS_SSL_MAX_FRAG_LEN_INVALID 5 /*!< first invalid value        */

#define MBEDTLS_SSL_IS_CLIENT 0
#define MBEDTLS_SSL_IS_SERVER 1

#define MBEDTLS_SSL_IS_NOT_FALLBACK 0
#define MBEDTLS_SSL_IS_FALLBACK 1

#define MBEDTLS_SSL_EXTENDED_MS_DISABLED 0
#define MBEDTLS_SSL_EXTENDED_MS_ENABLED 1

#define MBEDTLS_SSL_ETM_DISABLED 0
#define MBEDTLS_SSL_ETM_ENABLED 1

#define MBEDTLS_SSL_COMPRESS_NULL 0
#define MBEDTLS_SSL_COMPRESS_DEFLATE 1

#define MBEDTLS_SSL_VERIFY_NONE 0
#define MBEDTLS_SSL_VERIFY_OPTIONAL 1
#define MBEDTLS_SSL_VERIFY_REQUIRED 2
#define MBEDTLS_SSL_VERIFY_UNSET 3 /* Used only for sni_authmode */

#define MBEDTLS_SSL_LEGACY_RENEGOTIATION 0
#define MBEDTLS_SSL_SECURE_RENEGOTIATION 1

#define MBEDTLS_SSL_RENEGOTIATION_DISABLED 0
#define MBEDTLS_SSL_RENEGOTIATION_ENABLED 1

#define MBEDTLS_SSL_ANTI_REPLAY_DISABLED 0
#define MBEDTLS_SSL_ANTI_REPLAY_ENABLED 1

#define MBEDTLS_SSL_RENEGOTIATION_NOT_ENFORCED -1
#define MBEDTLS_SSL_RENEGO_MAX_RECORDS_DEFAULT 16

#define MBEDTLS_SSL_LEGACY_NO_RENEGOTIATION 0
#define MBEDTLS_SSL_LEGACY_ALLOW_RENEGOTIATION 1
#define MBEDTLS_SSL_LEGACY_BREAK_HANDSHAKE 2

#define MBEDTLS_SSL_TRUNC_HMAC_DISABLED 0
#define MBEDTLS_SSL_TRUNC_HMAC_ENABLED 1
#define MBEDTLS_SSL_TRUNCATED_HMAC_LEN 10 /* 80 bits, rfc 6066 section 7 */

#define MBEDTLS_SSL_SESSION_TICKETS_DISABLED 0
#define MBEDTLS_SSL_SESSION_TICKETS_ENABLED 1

#define MBEDTLS_SSL_CBC_RECORD_SPLITTING_DISABLED 0
#define MBEDTLS_SSL_CBC_RECORD_SPLITTING_ENABLED 1

#define MBEDTLS_SSL_ARC4_ENABLED 0
#define MBEDTLS_SSL_ARC4_DISABLED 1

#define MBEDTLS_SSL_PRESET_DEFAULT 0
#define MBEDTLS_SSL_PRESET_SUITEB 2

	/////////////////////////////////////////////////////////////////////////////////////////

#define MBEDTLS_SSL_MAX_CONTENT_LEN (16 * 1024)
#define MBEDTLS_PSK_MAX_LEN 32

#ifdef __cplusplus
}
#endif
#endif
