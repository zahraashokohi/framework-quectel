/*
 * MqttClient.h
 *
 *  Created on: 22.06.2018
 *      Author: georgi.angelov
 */

#ifndef MQTTCLIENT_H_
#define MQTTCLIENT_H_

#include <interface.h>
#include "txSSL_Store.h"

#define DEBUG_MQTT DBG

#define SSL_CA_LIST_BIN "CA_LIST.BIN"
#define SSL_CERT_KEY_BIN "CERT_KEY.BIN"

class MqttClient
{
private:
    qapi_Net_MQTT_Hndl_t handle;
    bool conn;

public:
    qapi_Net_MQTT_Config_t config;
    qapi_Net_MQTT_Connect_CB_t onConnect;
    qapi_Net_MQTT_Subscribe_CB_t onSubscribe;
    qapi_Net_MQTT_Publish_CB_t onPublish;
    qapi_Net_MQTT_Message_CB_t onMessage;
    MqttClient()
    {
        handle = NULL;
        onConnect = NULL;
        onSubscribe = NULL;
        onPublish = NULL;
        onMessage = NULL;
        conn = false;
        qapi_Net_MQTT_New(&handle);
        memset(&config, 0, sizeof(qapi_Net_MQTT_Config_t));
        config.keepalive_duration = 120;
        config.clean_session = true;
    }

    ~MqttClient()
    {
        disconnect();
        int rc = 0;
        if (handle)
            rc = qapi_Net_MQTT_Destroy(handle);
        handle = NULL;
    }

    bool disconnect()
    {
        int rc = 0;
        if (handle && conn)
            rc = qapi_Net_MQTT_Disconnect(handle);
        conn = rc == 0;
        return conn;
    }

    void client(const char *id)
    {
        memset((char *)config.client_id, 0, QAPI_NET_MQTT_MAX_CLIENT_ID_LEN);
        config.client_id_len = 0;
        if (id)
        {
            strncpy((char *)config.client_id, (char *)id, QAPI_NET_MQTT_MAX_CLIENT_ID_LEN);
            config.client_id_len = strlen((char *)config.client_id);
        }
    }

    bool client(String &str)
    {
        client(str.c_str());
    }

    void server(uint32_t ip, uint16_t port = 1883)
    {
        struct sockaddr_in *sin4;
        sin4 = (struct sockaddr_in *)&config.remote;
        sin4->sin_family = AF_INET;
        sin4->sin_port = _htons(port); // server port 8883
        sin4->sin_addr.s_addr = ip;    // server IP, host resolved
        sin4 = (struct sockaddr_in *)&config.local;
        sin4->sin_family = AF_INET;
        sin4->sin_port = _htons(port);
        sin4->sin_addr.s_addr = 0;
    }

    bool connect()
    {
        int rc;
        if (handle && conn == false)
        {
            if (onConnect)
                if ((rc = qapi_Net_MQTT_Set_Connect_Callback(handle, onConnect)))
                    return false;
            if (onSubscribe)
                if ((rc = qapi_Net_MQTT_Set_Subscribe_Callback(handle, onSubscribe)))
                    return false;
            if (onMessage)
                if ((rc = qapi_Net_MQTT_Set_Message_Callback(handle, onMessage)))
                    return false;
            if (onPublish)
                if ((rc = qapi_Net_MQTT_Set_Publish_Callback(handle, onPublish)))
                    return false;
            int rc = qapi_Net_MQTT_Connect(handle, &config);
            if (rc)
            {
                DEBUG_MQTT("[ERROR] qapi_Net_MQTT_Connect <%X> %d\n", handle, rc);
            }
            conn = rc == 0;
        }
        return conn;
    }

    bool connected()
    {
        return conn;
    }

    bool sub(const char *topic, int qos = 0)
    {
        if (false == conn)
            return false;
        if (!topic || !handle)
            return false;
        return qapi_Net_MQTT_Subscribe(handle, (uint8_t *)topic, qos) == 0;
    }

    bool pub(char *topic, char *msg, size_t size, int qos = 0, int retain = 0)
    {
        if (false == conn)
            return false;
        if (!topic || !msg || !size || !handle)
            return false;
        return qapi_Net_MQTT_Publish(handle, (uint8_t *)topic, (uint8_t *)msg, size, qos, retain) == 0;
    }

    int read_file(const char *name, void **buffer_ptr, size_t *buffer_size)
    {
        int bytes_read;
        int efs_fd = -1;
        struct qapi_FS_Stat_Type_s stat;
        uint8_t *file_buf = NULL;
        stat.st_size = 0;
        if ((!name) || (!buffer_ptr) || (!buffer_size))
        {
            DEBUG_MQTT("[EFS] Reading SSL from EFS file failed!\n");
            return QAPI_ERROR;
        }
        if (qapi_FS_Open(name, QAPI_FS_O_RDONLY_E, &efs_fd) < 0)
        {
            DEBUG_MQTT("[EFS] Opening EFS EFS file %s failed\n", name);
            return QAPI_ERROR;
        }
        if (qapi_FS_Stat_With_Handle(efs_fd, &stat) < 0)
        {
            DEBUG_MQTT("[EFS] Reading EFS file %s failed\n", name);
            return QAPI_ERROR;
        }
        DEBUG_MQTT("[EFS] Reading EFS file size %d \n", stat.st_size);
        file_buf = (uint8_t *)malloc(stat.st_size);
        if (file_buf == NULL)
        {
            DEBUG_MQTT("[EFS] SSL_cert_store: QAPI_ERR_NO_MEMORY \n");
            return QAPI_ERR_NO_MEMORY;
        }
        qapi_FS_Read(efs_fd, file_buf, stat.st_size, &bytes_read);
        if ((bytes_read < 0) || (bytes_read != stat.st_size))
        {
            free(file_buf);
            DEBUG_MQTT("[EFS] SSL_cert_store: Reading EFS file error\n");
            return QAPI_ERROR;
        }
        *buffer_ptr = file_buf;
        *buffer_size = stat.st_size;
        qapi_FS_Close(efs_fd);
        return QAPI_OK;
    }

    void secured()
    {
        config.ssl_cfg.protocol = QAPI_NET_SSL_PROTOCOL_TLS_1_2;
        config.ssl_cfg.max_Frag_Len = 4096; // 4096 * 4;
        config.ssl_cfg.max_Frag_Len_Neg_Disable = 0;

#if 1 // AWS        
        config.ssl_cfg.cipher[0] = QAPI_NET_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384; 
        config.ssl_cfg.cipher[1] = QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384;
        config.ssl_cfg.cipher[2] = QAPI_NET_TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256;
        config.ssl_cfg.cipher[3] = QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256;
        config.ssl_cfg.cipher[4] = QAPI_NET_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA;
        config.ssl_cfg.cipher[5] = QAPI_NET_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA;
        config.ssl_cfg.cipher[6] = QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256; // FOR GOOGLE IoT Core
        config.ssl_cfg.cipher[7] = QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384;
#endif 

        config.ca_list = "empty"; // must exist string
        config.cert = "empty";    // must exist string
    }

    bool ca_root(const char *efs_file)
    {
        int rc;
        qapi_Net_SSL_Cert_Info_t cert_info;
        if (efs_file && config.ca_list)
        {
            qapi_NET_SSL_CA_Info_t calist_info[QAPI_NET_SSL_MAX_CA_LIST];
            rc = read_file(efs_file, (void **)&calist_info[0].ca_Buf, (size_t *)&calist_info[0].ca_Size);
            if (rc)
            {
                DEBUG_MQTT("[WARN] ca root\n");
                return false;
            }
            cert_info.info.ca_List.ca_Info[0] = &calist_info[0];
            cert_info.info.ca_List.ca_Cnt = 1;
            cert_info.cert_Type = QAPI_NET_SSL_CA_LIST_E;
            rc = qapi_Net_SSL_Cert_delete(config.ca_list, QAPI_NET_SSL_CA_LIST_E);
            if (rc)
            {
                DEBUG_MQTT("[WARN] qapi_Net_SSL_Cert_delete( %s ) %d\n", config.cert, rc);
            }
            rc = qapi_Net_SSL_Cert_Convert_And_Store(&cert_info, config.ca_list);
            if (rc)
            {
                DEBUG_MQTT("[ERROR] qapi_Net_SSL_Cert_Convert_And_Store( %s ) %d\n", config.ca_list, rc);
            }
            else
            {
                DEBUG_MQTT("[TLS] CA_ROOT Stored: %s\n", config.ca_list);
                qapi_FS_Truncate(efs_file, 0);
                qapi_FS_Unlink(efs_file);
            }
            return rc == 0;
        }
    }

    bool certificate(const char *cert, const char *key = NULL)
    {
        int rc;
        qapi_Net_SSL_Cert_Info_t cert_info;
        if (key == NULL && cert && config.ca_list)
        {
            qapi_NET_SSL_CA_Info_t calist_info[QAPI_NET_SSL_MAX_CA_LIST];
            rc = read_file(cert, (void **)&calist_info[0].ca_Buf, (size_t *)&calist_info[0].ca_Size);
            if (rc)
            {
                DEBUG_MQTT("[WARN] ca list\n");
                return false;
            }
            cert_info.info.ca_List.ca_Info[0] = &calist_info[0];
            cert_info.info.ca_List.ca_Cnt = 1;
            cert_info.cert_Type = QAPI_NET_SSL_CA_LIST_E;
            rc = qapi_Net_SSL_Cert_delete(config.ca_list, QAPI_NET_SSL_CA_LIST_E);
            if (rc)
            {
                DEBUG_MQTT("[WARN] qapi_Net_SSL_Cert_delete( %s ) %d\n", config.cert, rc);
            }
            rc = qapi_Net_SSL_Cert_Convert_And_Store(&cert_info, config.ca_list);
            if (rc)
            {
                DEBUG_MQTT("[ERROR] qapi_Net_SSL_Cert_Convert_And_Store( %s ) %d\n", config.ca_list, rc);
            }
            else
            {
                DEBUG_MQTT("[TLS] CA_LIST Stored: %s\n", config.ca_list);
                qapi_FS_Truncate(cert, 0);
                qapi_FS_Unlink(cert);
            }
            return rc == 0;
        }
        if (key && cert && config.cert)
        {
            rc = read_file(cert, (void **)&cert_info.info.cert.cert_Buf, (size_t *)&cert_info.info.cert.cert_Size);
            if (rc)
            {
                DEBUG_MQTT("[WARN] SSL certificate\n");
                return false;
            }
            rc = read_file(key, (void **)&cert_info.info.cert.key_Buf, (size_t *)&cert_info.info.cert.key_Size);
            if (rc)
            {
                DEBUG_MQTT("[WARN] SSL private key\n");
                return false;
            }
            cert_info.cert_Type = QAPI_NET_SSL_CERTIFICATE_E;
            cert_info.info.cert.pass_Key = NULL;
            rc = qapi_Net_SSL_Cert_delete(config.cert, QAPI_NET_SSL_CERTIFICATE_E);
            if (rc)
            {
                DEBUG_MQTT("[WARN] qapi_Net_SSL_Cert_delete( %s ) %d\n", config.cert, rc);
            }
            rc = qapi_Net_SSL_Cert_Convert_And_Store(&cert_info, config.cert);
            if (rc)
            {
                DEBUG_MQTT("[ERROR] qapi_Net_SSL_Cert_Convert_And_Store( %s ) %d\n", config.cert, rc);
            }
            else
            {
                DEBUG_MQTT("[TLS] CERT_KEY Stored: %s\n", config.cert);
                qapi_FS_Truncate(cert, 0);
                qapi_FS_Unlink(cert);
                qapi_FS_Truncate(key, 0);
                qapi_FS_Unlink(key);
            }
            return rc == 0;
        }
        return false;
    }

#if 0
    bool psk(const char *efs_name, const char *psk_name)
    {
        int rc;
        qapi_Net_SSL_Cert_Info_t cert_info;
        rc = read_file(efs_name, (void **)&cert_info.info.psk_Tbl.psk_Buf, (size_t *)&cert_info.info.psk_Tbl.psk_Size);
        if (rc)
        {
            DEBUG_MQTT("[WARN] psk table\n");
            return false;
        }
        cert_info.cert_Type = QAPI_NET_SSL_PSK_TABLE_E;
        rc = qapi_Net_SSL_Cert_delete(psk_name, QAPI_NET_SSL_PSK_TABLE_E);
        if (rc)
        {
            DEBUG_MQTT("[WARN] qapi_Net_SSL_Cert_delete( %s ) %d\n", psk_name, rc);
        }
        rc = qapi_Net_SSL_Cert_Convert_And_Store(&cert_info, psk_name);
        if (rc)
        {
            DEBUG_MQTT("[ERROR] qapi_Net_SSL_Cert_Convert_And_Store( %s ) %d\n", psk_name, rc);
        }
        else
        {
            DEBUG_MQTT("[TLS] CA_LIST Stored: PSK\n", psk_name);
            qapi_FS_Truncate(efs_name, 0);
            qapi_FS_Unlink(efs_name);
        }
        return rc == 0;
    }
#endif
    bool store_ca_list(const char *ssl_file, uint32_t n, ...)
    {
        va_list a;
        va_start(a, n);
        SSL_STORE s;
        s.ca_list(ssl_file, n, a);
    }
};

#endif /* MQTTCLIENT_H_ */
