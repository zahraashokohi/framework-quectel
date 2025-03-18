/*
 * Psm.h
 *
 *  Created on: 28.06.2018
 *      Author: georgi.angelov
 */

#ifndef PSM_H_
#define PSM_H_

#include <interface.h>

class Psm
{
private:
    int32_t id;

public:
    Psm()
    {
        id = 0;
    }

    ~Psm()
    {
        end();
    }

    int begin(psm_client_cb_type callback, int timeout = 5000)
    {
        if (id)
            return 0;
        int rc = -1;
        if (callback)
        {
            do
            {
                rc = qapi_PSM_Client_Register(&id, callback);
                qapi_Timer_Sleep(100, QAPI_TIMER_UNIT_MSEC, 1);
                timeout -= 100;
            } while (rc && timeout > 0);
        }
        return rc;
    }

    void end()
    {
        if (id)
        {
            qapi_PSM_Client_Unregister(id);
            id = 0;
        }
    }

    int enter(psm_info_type *info)
    {
        int rc = -1;
        if (id && info)
            rc = qapi_PSM_Client_Enter_Psm(id, info);
        return rc;
    }

    int backoff()
    {
        int rc = -1;
        if (id)
            rc = qapi_PSM_Client_Enter_Backoff(id);
        return rc;
    }

    int cancel()
    {
        int rc = -1;
        if (id)
            rc = qapi_PSM_Client_Cancel_Psm(id);
        return rc;
    }

    int load()
    {
        int rc = -1;
        if (id)
            rc = qapi_PSM_Client_Load_Modem(id);
        return rc;
    }

    int ack()
    {
        int rc = -1;
        if (id)
            rc = qapi_PSM_Client_Hc_Ack(id);
        return rc;
    }

    operator bool()
    {
        return id != 0;
    }
};

#endif /* PSM_H_ */
