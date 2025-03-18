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

#ifndef LDEV_h
#define LDEV_h

#include <Arduino.h>

class LDEV
{

public:
    LDEV() {}

    const char *version() { return os_get_firmware(); }
    void version(String &s) { s = os_get_firmware(); }

    void urc(void (*onUrc)(ST_MSG *msg)) { setOnUrc(onUrc); }
    int sim(){ return os_ril_get_sim(); }

    void Sleep() { Ql_SleepEnable(); }
    void noSleep(bool f = true)
    {
        if (f)
            Ql_SleepDisable();
        else
            Ql_SleepEnable();
    }

    void reset() { Ql_Reset(0); }
    void off() { Ql_PowerDown(0); }
    int wakeupReason() { return Ql_GetWakeUpReason(); }
    int powerReason() { return Ql_GetPowerOnReason(); }
    unsigned int battery()
    {
        unsigned int value;
        return 0 == Ql_GetPowerVol(&value) ? value : 0;
    }

    /* UID is U32[4] */
    void getUid(void *buffer) // [16]
    {
        if (buffer)
            memcpy(buffer, os_get_uid(), 4 * sizeof(uint32_t));
    }
    void getUid(String &s) // return hex string
    {
        char buf[16], hex[3];
        getUid(buf);
        for (int i = 0; i < 16; i++)
        {
            sprintf(hex, "%02X", buf[i]);
            s += hex;
        }
    }

    void getImei(char *imei) // [16]
    {
        if (imei)
        {
            memset(imei, 0, 16);
            int res = RIL_GetIMEI(imei);
        }
    }
    void getImei(String &s)
    {
        char imei[32];
        getImei(imei);
        s = imei;
    }

    bool getImsi(char *buffer) { return os_ril_get_imsi(buffer); }
    void getImsi(String &s)
    {
        char buffer[32];
        os_ril_get_imsi(buffer);
        s = buffer;
    }

    bool getIccid(char *buffer) { return os_ril_get_iccid(buffer); }
    void getIccid(String &s)
    {
        char buffer[32];
        os_ril_get_iccid(buffer);
        s = buffer;
    }

    bool getMccMnc(char *buffer) { return os_ril_get_mcc_mnc(buffer); }
    void getMccMnc(String &s)
    {
        char buffer[32];
        os_ril_get_mcc_mnc(buffer);
        s = buffer;
    }

    bool send(const char *cmd, u32 timeout = 300, Callback_ATResponse callback = NULL, void *user = NULL)
    {
        return Ql_RIL_SendATCmd((char *)cmd, Ql_strlen(cmd), callback, user, timeout) == RIL_AT_SUCCESS;
    }
    bool send(String cmd, u32 timeout = 300, Callback_ATResponse callback = NULL, void *user = NULL)
    {
        return send(cmd.c_str(), timeout, callback, user);
    }

    bool band(int band) { return os_ril_set_band(band); }

    int cereg(bool wait = true)
    {
        if (wait)
            os_ril_wait_cereg();
        return os_ril_get_cereg();
    }

    bool dns(const char *dns_1 = NULL, const char *dns_2 = NULL)
    {
        return os_ril_set_dns(dns_1, dns_2); // NULL, NULL = google
    }
};

#endif