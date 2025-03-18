/*
 *  Created on: 09.06.2018
 *      Author: Georgi Angelov
 */

#ifndef __TIMER_H__
#define __TIMER_H__

#include <Arduino.h>

typedef void (*TimerCallback)(uint32_t user);

class txTimer
{
public:
    txTimer()
    {
        handle = NULL;
    }

    bool begin(TimerCallback callback, uint32_t data = 0)
    {
        if (NULL == callback)
            return -1;
        if (handle)
            return 0; // already
        memset(&def_attr, 0, sizeof(qapi_TIMER_define_attr_t));
        def_attr.cb_type = QAPI_TIMER_FUNC1_CB_TYPE;
        def_attr.deferrable = false;
        def_attr.sigs_func_ptr = (void *)callback;
        def_attr.sigs_mask_data = data;
        return qapi_Timer_Def(&handle, &def_attr) == 0;
    }

    void end()
    {
        if (handle)
            qapi_Timer_Undef(handle);
        handle = NULL;
    }

    bool start(uint64_t reload, uint64_t time, qapi_TIMER_unit_type type = QAPI_TIMER_UNIT_MSEC)
    {
        if (handle)
        {
            memset(&set_attr, 0, sizeof(qapi_TIMER_set_attr_t));
            set_attr.reload = reload;
            set_attr.time = time;
            set_attr.unit = type;
            return qapi_Timer_Set(handle, &set_attr) == 0;
        }
        return false; // error
    }

    bool start(uint64_t reload)
    {
        return start(reload, 10);
    }

    bool stop()
    {
        if (handle)
            return qapi_Timer_Stop(handle) == 0;
        return false; // error
    }

    bool get(uint64_t *data, qapi_TIMER_unit_type unit = QAPI_TIMER_UNIT_MSEC)
    {
        if (handle && data)
        {
            qapi_TIMER_get_info_attr_t info;
            info.timer_info = QAPI_TIMER_TIMER_INFO_TIMER_REMAINING;
            info.unit = unit;
            return qapi_Timer_Get_Timer_Info(handle, &info, data) == 0;
        }
        return false; // error
    }

private:
    qapi_TIMER_handle_t handle;
    qapi_TIMER_define_attr_t def_attr;
    qapi_TIMER_set_attr_t set_attr;
};

#endif
