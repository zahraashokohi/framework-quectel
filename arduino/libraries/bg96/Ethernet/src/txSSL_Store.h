#ifndef SSL_STORE_H_
#define SSL_STORE_H_

#include <interface.h>

#define SSL_CA_LIST_BIN "CA_LIST.BIN"
#define SSL_CERT_KEY_BIN "CERT_KEY.BIN"

class SSL_STORE
{
private:
    int read_file(const char *name, void **buffer_ptr, size_t *buffer_size)
    {
        int bytes_read;
        int efs_fd = -1;
        struct qapi_FS_Stat_Type_s stat;
        uint8_t *file_buf = NULL;
        stat.st_size = 0;
        if ((!name) || (!buffer_ptr) || (!buffer_size))
        {
            DBG("[ERROR] Reading SSL from EFS file failed!\n");
            return QAPI_ERROR;
        }
        if (qapi_FS_Open(name, QAPI_FS_O_RDONLY_E, &efs_fd) < 0)
        {
            DBG("[ERROR] Opening EFS file %s failed\n", name);
            return QAPI_ERROR;
        }
        if (qapi_FS_Stat_With_Handle(efs_fd, &stat) < 0)
        {
            DBG("[ERROR] Reading EFS file %s failed\n", name);
            return QAPI_ERROR;
        }
        DBG("[EFS] Reading EFS file size %d \n", stat.st_size);
        file_buf = (uint8_t *)malloc(stat.st_size);
        if (file_buf == NULL)
        {
            DBG("[EFS] SSL_cert_store: QAPI_ERR_NO_MEMORY \n");
            return QAPI_ERR_NO_MEMORY;
        }
        qapi_FS_Read(efs_fd, file_buf, stat.st_size, &bytes_read);
        if ((bytes_read < 0) || (bytes_read != stat.st_size))
        {
            free(file_buf);
            DBG("[EFS] SSL_cert_store: Reading EFS file error\n");
            return QAPI_ERROR;
        }
        *buffer_ptr = file_buf;
        *buffer_size = stat.st_size;
        qapi_FS_Close(efs_fd);
        return QAPI_OK;
    }

    bool ca_root_add(qapi_NET_SSL_CA_Info_t *ca_info, const char *efs_file)
    {
        int rc = -1;
        if (ca_info && efs_file)
        {
            rc = read_file(efs_file, (void **)&ca_info->ca_Buf, (size_t *)&ca_info->ca_Size);
            if (rc)
            {
                DBG("[WARN] ca_root_add: %s\n", efs_file);
                return false;
            }
        }
        DBG("[SSL] read efs: %s\n", efs_file);
        return rc == 0;
    }

public:
    bool ca_list(const char *ssl_file, uint32_t n, va_list args)
    {
        int rc;
        va_list ARGS = args;
        qapi_NET_SSL_CA_Info_t ca_info[QAPI_NET_SSL_MAX_CA_LIST];
        qapi_Net_SSL_Cert_Info_t cert_info;
        if (ssl_file && n && n < 10)
        {
            //READ ALL FILES
            const char *efs_file;
            //va_list args;
            //va_start(a, n);
            for (int i = 0; i < n; i++)
            {
                efs_file = va_arg(args, const char *);
                if (efs_file)
                    if (0 == ca_root_add(&ca_info[i], efs_file))
                        return false;
                cert_info.info.ca_List.ca_Info[i] = &ca_info[i];
            }
            //DELETE STORED SSL FILE
            rc = qapi_Net_SSL_Cert_delete(ssl_file, QAPI_NET_SSL_CA_LIST_E);
            if (rc)
            {
                DBG("[WARN] qapi_Net_SSL_Cert_delete( %s ) %d\n", ssl_file, rc);
            }
            DBG("[SSL] delete ssl: %s\n", ssl_file);
            //CONVERT AND STORE SSL FILE
            cert_info.info.ca_List.ca_Cnt = n;
            cert_info.cert_Type = QAPI_NET_SSL_CA_LIST_E;
            rc = qapi_Net_SSL_Cert_Convert_And_Store(&cert_info, ssl_file);
            if (rc)
            {
                DBG("[ERROR] qapi_Net_SSL_Cert_Convert_And_Store( %s ) %d\n", ssl_file, rc);
                return false;
            }
            //DELETE EFS FILES
            //va_start(a, n);
            for (int i = 0; i < n; i++)
            {
                efs_file = va_arg(ARGS, const char *);
                if (efs_file)
                {
                    qapi_FS_Truncate(efs_file, 0);
                    qapi_FS_Unlink(efs_file);
                }
            }
            DBG("[TLS] CA_LIST Stored: %s\n", ssl_file);
            return true;
        }
        return false;
    }

    bool ca_root(const char *ssl_file, const char *efs_file){
        //return ca_list(ssl_file, 0, efs_file);
    }
};

#endif /* SSL_STORE_H_ */