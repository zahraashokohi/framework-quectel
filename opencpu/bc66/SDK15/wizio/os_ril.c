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

#include "os_ril.h"

void removeChar(char *str, char garbage)
{
    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++)
    {
        *dst = *src;
        if (*dst != garbage)
            dst++;
    }
    *dst = '\0';
}

/**************************************************************************************************/
static int ATI(char *line, u32 len, void *user)
{
    char *h = Ql_RIL_FindString(line, len, (char *)"Revision:");
    if (h && user)
    {
        char *p1 = NULL;
        char *p2 = NULL;
        p1 = strstr(h, ":");
        p2 = strstr(p1 + 1, "\r\n");
        if (p1 && p2)
        {
            memcpy((char *)user, p1 + 2, p2 - p1 - 2);
            return RIL_ATRSP_SUCCESS;
        }
    }
    return RIL_ATRSP_CONTINUE;
}
bool os_ril_get_version(char *version)
{
    if (version)
        return RIL_AT_SUCCESS == Ql_RIL_SendATCmd("ATI\n", 4, ATI, version, 300);
    return false;
}

/**************************************************************************************************/
/* return CEREG = 0...7, 1 & 5 is OK */
static int AT_CEREG(char *line, u32 len, void *user)
{
    char *cmd = Ql_RIL_FindString(line, len, (char *)"+CEREG:");
    if (cmd && user)
    {
        int n, s;
        if (2 == sscanf(cmd, "+CEREG: %d,%d", &n, &s))
        {
            //printf("[RIL] %s", cmd);
            *(int *)user = s;
            return RIL_ATRSP_SUCCESS;
        }
    }
    return RIL_ATRSP_CONTINUE;
}
int os_ril_get_cereg(void)
{
    int val = -1;
    return Ql_RIL_SendATCmd("AT+CEREG?\r\n", 12, AT_CEREG, &val, 300) == RIL_AT_SUCCESS ? val : 0;
}

/* helper for Arduino */
void os_ril_wait_cereg(void)
{
    int val = -1;
    while (1)
    {
        val = os_ril_get_cereg();
        if (1 == val || 5 == val) // home & roaming
            break;
        Ql_Sleep(500);
    }
    Ql_Sleep(100); // yield !!!
}

bool os_ril_set_band(int band)
{
    int val;
    char cmd[64];
    int len = snprintf(cmd, sizeof(cmd), "AT+QBAND=1,%d\r\n", band);
    return RIL_AT_SUCCESS == Ql_RIL_SendATCmd(cmd, len, 0, 0, 300);
}

bool os_ril_set_bands(int count, ...)
{
    count &= 7; // max 8
    if (count)
    {
        char cmd[64], b[8];
        snprintf(cmd, sizeof(cmd), "AT+QBAND=%d", count);
        va_list vl;
        va_start(vl, count);
        while (count--)
        {
            snprintf(b, sizeof(b), ",%d", va_arg(vl, int));
            strcat(cmd, b);
        }
        strcat(cmd, "\r\n");
        return RIL_AT_SUCCESS == Ql_RIL_SendATCmd(cmd, strlen(cmd), 0, 0, 300);
    }
    return false;
}

bool os_ril_set_dns(const char *dns_1, const char *dns_2)
{
    if (NULL == dns_1)
        dns_1 = "8.8.4.4"; // google
    if (NULL == dns_2)
        dns_2 = "8.8.8.8"; // google
    char cmd[64];
    int res = 0,
        len = snprintf(cmd, sizeof(cmd), "AT+QIDNSCFG=1,\"%s\",\"%s\"\r\n", dns_1, dns_2); // only <contextID>=1 is supported.
    if (len > 33)
        res = RIL_AT_SUCCESS == Ql_RIL_SendATCmd(cmd, len, 0, 0, 300);
    Ql_Sleep(100); // yield !!!
    return res;
}

/* default, helper for Arduino */
void os_ril_do_connect(int band, int no_sleep)
{
    os_ril_set_band(band);
    os_ril_wait_cereg();
    os_ril_set_dns(NULL, NULL); // google
    if (no_sleep)
        Ql_SleepDisable();
}

int os_ril_get_sim(void)
{
    Enum_SIMState state;
    int res = RIL_SIM_GetSimState(&state);
    return 0 == res ? state : res;
}

/* helper for Arduino */
void os_ril_wait_sim(void)
{
    while (1 != os_ril_get_sim())
        Ql_Sleep(100);
    Ql_Sleep(100); // yield !!!
}

void os_ril_apn_init(const char *apn)
{
    // TODO
}

/**************************************************************************************************/
static int AT_CIMI(char *line, u32 len, void *user)
{
    //printf("[AT]%s", line); // []284011911698783[]OK[]
    if (Ql_RIL_FindString(line, len, (char *)"OK"))
        return RIL_ATRSP_SUCCESS;
    if (len > 16)
    {
        removeChar(line, '\n');
        removeChar(line, '\r');
        strncpy(user, line, 15); // continue to OK
    }
    return RIL_ATRSP_CONTINUE;
}
bool os_ril_get_imsi(char *buffer)
{
    bool res = false;
    if (buffer)
    {
        if (RIL_AT_SUCCESS == Ql_RIL_SendATCmd("AT+CIMI", 7, AT_CIMI, buffer, 300))
            res = true;
    }
    return res;
}

/**************************************************************************************************/
static int AT_QCCID(char *line, u32 len, void *user) // +QCCID: 8935901187016987830F
{
    if (Ql_RIL_FindString(line, len, (char *)"OK"))
        return RIL_ATRSP_SUCCESS;
    char *h = Ql_RIL_FindString(line, len, (char *)"+QCCID: ");
    if (h && len > 29)
    {
        removeChar(line, '\n');
        removeChar(line, '\r');
        strncpy(user, line + 8, len - 8);
    }
    return RIL_ATRSP_CONTINUE;
}
bool os_ril_get_iccid(char *buffer)
{
    bool res = false;
    if (buffer)
    {
        if (RIL_AT_SUCCESS == Ql_RIL_SendATCmd("AT+QCCID", 8, AT_QCCID, buffer, 300))
            res = true;
    }
    return res;
}

/**************************************************************************************************/
static int AT_COPS(char *line, u32 len, void *user) //+COPS: 0,2,"28401",9
{
    if (Ql_RIL_FindString(line, len, (char *)"OK"))
        return RIL_ATRSP_SUCCESS;
    char *h = Ql_RIL_FindString(line, len, (char *)"+COPS: ");
    if (h)
    {
        removeChar(line, '"');
        sscanf(h, "+COPS: %*d,%*d,%5s", (char *)user);
    }
    return RIL_ATRSP_CONTINUE;
}
bool os_ril_get_mcc_mnc(char *buffer) // size min 6
{
    bool res = false;
    if (buffer)
    {
        memset(buffer, 0, 6);
        if (RIL_AT_SUCCESS == Ql_RIL_SendATCmd("AT+COPS?", 8, AT_COPS, buffer, 300))
            res = true;
    }
    return res;
}