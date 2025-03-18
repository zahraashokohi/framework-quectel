#ifndef __PSM_H__
#define __PSM_H__

#include <interface.h>

#define DEBUG_PSM 
//::printf

#define MAX_LINE 64
typedef struct
{
    bool res;
    char line[MAX_LINE]; // without command
} aline_t;

static void uint_to_binary_str(uint32_t num, char *str, int str_size, int bit_cnt)
{
    //https://github.com/ARMmbed/mbed-os/blob/be4bb9f610917be9bc87026e61608e29a29496d4/features/cellular/framework/common/CellularUtil.cpp
    if (!str || str_size < bit_cnt)
        return;
    int tmp, pos = 0;
    for (int i = 31; i >= 0; i--)
    {
        tmp = num >> i;
        if (i < bit_cnt)
        {
            if (tmp & 1)
                str[pos] = 1 + '0';
            else
                str[pos] = 0 + '0';
            pos++;
        }
    }
}

static uint32_t binary_str_to_uint(const char *binary_string, int binary_string_length)
{
    if (!binary_string || !binary_string_length)
        return 0;
    int integer_output = 0, base_exp = 1;
    for (int i = binary_string_length - 1; i >= 0; i--)
    {
        if (binary_string[i] == '1')
        {
            integer_output += base_exp;
        }
        if (binary_string[i] != '\0')
        {
            base_exp <<= 1;
        }
    }
    return integer_output;
}

static uint32_t tau_seconds[8] = {
    10 * 60, // 10 minutes
    3600,    // 1 hour
    36000,   // 10 hours
    2,
    30,
    60,
    320 * 3600,
    (uint32_t)-1,
};

static inline uint32_t tauToSeconds(uint8_t tau)
{
    return (tau & 31) * tau_seconds[tau >> 5];
}

static uint32_t act_seconds[8] = {2, 60, 6 * 60, 60, 60, 60, 60, 60};

static inline uint32_t actToSeconds(uint8_t act)
{
    return (act & 31) * act_seconds[act >> 5];
}

static inline uint32_t cycleToSeconds(uint8_t act)
{
    return (uint32_t)(((act & 15) - 1) * 20.48);
}

static int GET_CPSMS(char *line, u32 len, void *user)
{
    if (NULL == user)
        return RIL_ATRSP_FAILED;
    aline_t *p = (aline_t *)user;

    if (Ql_RIL_FindLine(line, len, (char *)"OK"))
        return RIL_ATRSP_SUCCESS;

    if (Ql_RIL_FindLine(line, len, (char *)"ERROR"))
        return RIL_ATRSP_FAILED;

    char *h = Ql_RIL_FindString(line, len, (char *)"+CPSMS: ");
    if (h) // +CPSMS: 1,,,"00100011","00100010"
    {
        strncpy(p->line, h + 8, MAX_LINE); // without cmd
        p->res = true;                     // continue to OK
    }
    return RIL_ATRSP_CONTINUE; //continue wait
}

static int GET_EDRXS(char *line, u32 len, void *user)
{
    if (NULL == user)
        return RIL_ATRSP_FAILED;
    aline_t *p = (aline_t *)user;

    if (Ql_RIL_FindLine(line, len, (char *)"OK"))
        return RIL_ATRSP_SUCCESS;

    if (Ql_RIL_FindLine(line, len, (char *)"ERROR"))
        return RIL_ATRSP_FAILED;

    char *h = Ql_RIL_FindString(line, len, (char *)"+CEDRXS: ");
    if (h) // +CEDRXS: 5,"0101"
    {
        strncpy(p->line, h + 9, MAX_LINE); // without cmd
        p->res = true;                     // continue to OK
    }
    return RIL_ATRSP_CONTINUE; //continue wait
}

class LPSM
{
private:
    uint32_t m_wakeup_interval;

    static void onEintPsm(void *user_data)
    { /*empty*/
    }
    static void onRtc(u32 rtcId, void *param)
    { /*empty*/
    }
    static void onBeforeSleepCB(void *THIS, uint32_t mode)
    {
        if (THIS && mode >= 4)
        {
            LPSM *p = (LPSM *)THIS;
            if (p->m_wakeup_interval)
            {
                Ql_Rtc_RegisterFast(0x100, onRtc, NULL);
                Ql_Rtc_Start(0x100, p->m_wakeup_interval, false);
                DEBUG_PSM("\n[PSM] GO SLEEP: %u\n", seconds());
            }
        }
    }

public:
    LPSM() { m_wakeup_interval = 0; }

    /// exist in lib Dev
    void Sleep() { Ql_SleepEnable(); }
    void noSleep() { Ql_SleepDisable(); }
    int wakeupReason() { return Ql_GetWakeUpReason(); }    
    int powerReason() { return Ql_GetPowerOnReason(); }    

    /// Register RTC wakeup
    void rtc(uint32_t seconds, sleep_callback_t userCB = NULL)
    {
        Ql_SleepEnable();
        if (seconds < 10)
            seconds = 10;
        m_wakeup_interval = seconds * 1000;
        if (m_wakeup_interval)
        {
            if (userCB)
                API->hal_sleep_manager_register_suspend_callback(userCB, this, 0xC);
            else
            {
                API->hal_sleep_manager_register_suspend_callback(onBeforeSleepCB, this, 0xC);
                DEBUG_PSM("[PSM] SET WAKEUP FROM RTC: %lu\n", m_wakeup_interval);
            }
        }
    }

    /// Register EINT wakeup
    bool pin(Callback_Psm_Eint_Func onUserEintPsm = NULL, void *param = NULL)
    {
        Ql_SleepEnable();
        int res = -1;
        if (onUserEintPsm)
            res = Ql_Psm_Eint_Register(onUserEintPsm, param);
        else
            res = Ql_Psm_Eint_Register(onEintPsm, 0);
        return QL_RET_OK == res;
    }

    /// Get PSM values
    bool get(int *period, int *active) // return mode
    {
        bool res = false;
        if (period && active)
        {
            aline_t c = {0};
            //memset(&c, 0, sizeof(aline_t));
            res = RIL_AT_SUCCESS == Ql_RIL_SendATCmd((char *)"AT+CPSMS?\n", 10, GET_CPSMS, &c, 1000);
            if (res && c.res)
            {
                *period = 0;
                *active = 0;
                int mode = 0;
                char str_p[16] = {0}, str_a[16] = {0};
                if (3 == sscanf(c.line, "%d,,,\"%8s\",\"%8s\"", &mode, str_p, str_a))
                {
                    *period = binary_str_to_uint(str_p, strlen(str_p));
                    *period = tauToSeconds(*period);
                    *active = binary_str_to_uint(str_a, strlen(str_a));
                    *active = actToSeconds(*active);
                    res = mode;
                }
            }
        }
        return res;
    }

    /// Get eDRX period
    bool getedrx(int *period)
    {
        bool res = false;
        if (period)
        {
            aline_t c = {0};
            //memset(&c, 0, sizeof(aline_t));
            res = Ql_RIL_SendATCmd((char *)"AT+CEDRXS?\n", 10, GET_EDRXS, &c, 1000) == RIL_AT_SUCCESS;
            if (res && c.res)
            {
                *period = 0;
                int mode = 0;
                char str_req[8] = {0};
                if (2 == sscanf(c.line, "%d,\"%4s\"", &mode, str_req))
                {
                    *period = binary_str_to_uint(str_req, strlen(str_req));
                    *period = cycleToSeconds(*period);
                    if (*period)
                        res = (bool)mode;
                    else
                        return false;
                }
            }
        }
        return res;
    }

    // TODO
    bool set(int periodic_time, int active_time, int mode) { return 0; }
    bool setedrx(int periodic_time, int mode) { return 0; }
};

#endif