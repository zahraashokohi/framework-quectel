////////////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////////////

#include "txm_module.h"
#include "tx_api.h"
#include "qapi_timer.h"
#include "qapi_timer_id.h"

#pragma GCC optimize 0

#define SEC_LIB __attribute__((section(".library"))) // just for sorting

////////////////////////////////////////////////////////////////////////////////////////
//
// 	QUALCOMM THREADX [ SDK3 / 4 ] LIBRARIES: TIMERS, PSM, DIAG
//
////////////////////////////////////////////////////////////////////////////////////////

#define HIDWORD(Z) Z >> 32
static int qapi_timer_user_initialized = 0;
static ULONG timer_user_handle; // qapi_TIMER_handle_t
static TX_THREAD *timer_client_module_thread;
static int timer_thread_state;
static char timer_client_stack[4096];

SEC_LIB int timer_client_cb_handler(ULONG timer_user_handle);
SEC_LIB qapi_Status_t qapi_Timer_Get_Cbinfo(ULONG handle, void *cb);

SEC_LIB void qapi_timer_user_init(void) // need test
{
    if (qapi_timer_user_initialized != 1)
    {
        qapi_timer_user_initialized = 1;
        _txm_module_system_call12(TXM_QAPI_TIMER_GET_USER_HANDLE, (ULONG)&timer_user_handle, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); // 65794
        if (timer_user_handle)
        {
            _txm_module_kernel_call_dispatcher(TXM_MODULE_OBJECT_ALLOCATE_CALL, (ULONG)&timer_client_module_thread, 280, 0); // 93
            timer_thread_state = _txm_module_system_call11(
                TXM_THREAD_CREATE_CALL, // 55
                (ULONG)timer_client_module_thread,
                (ULONG) "TIMER_USER_THREAD",
                (ULONG)timer_client_cb_handler,
                (ULONG)timer_user_handle,
                (ULONG)timer_client_stack,
                (ULONG)4096, // stack size
                (ULONG)15,
                (ULONG)1,
                (ULONG)0,
                (ULONG)1,
                (ULONG)280);
            if (timer_thread_state)
                qapi_timer_user_initialized = 0;
        }
        else
        {
            qapi_timer_user_initialized = 0;
        }
    }
}

SEC_LIB int timer_client_cb_handler(ULONG timer_user_handle) // need test
{
    qapi_TIMER_get_cbinfo_t tmr;
    int result = 0;
    if (timer_user_handle)
    {
        while (0 == (result = qapi_Timer_Get_Cbinfo(timer_user_handle, &tmr)))
        {
            if (tmr.func_ptr)
                ((void (*)(int))tmr.func_ptr)(tmr.data);
        }
    }
    return result;
}

SEC_LIB qapi_Status_t qapi_Timer_Get_Cbinfo(ULONG handle, void *cb)
{
    if (cb && handle)
        return _txm_module_system_call12(TXM_QAPI_TIMER_GET_CBINFO, (ULONG)handle, (ULONG)cb, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); // 65793
    else
        return QAPI_ERR_INVALID_PARAM;
}

SEC_LIB qapi_Status_t qapi_Timer_Def(qapi_TIMER_handle_t *timer_handle, qapi_TIMER_define_attr_t *timer_attr)
{
    if (!timer_handle || !timer_attr)
        return QAPI_ERR_INVALID_PARAM;
    if (qapi_timer_user_initialized != 1)
        qapi_timer_user_init();
    return _txm_module_system_call12(TXM_QAPI_TIMER_DEF, (ULONG)timer_handle, (ULONG)timer_attr, (ULONG)timer_user_handle, 0, 0, 0, 0, 0, 0, 0, 0, 0); // 65786
}

SEC_LIB qapi_Status_t qapi_Timer_Undef(qapi_TIMER_handle_t timer_handle)
{
    if (timer_handle)
        return _txm_module_system_call12(TXM_QAPI_TIMER_UNDEF, (ULONG)timer_handle, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); // 65789
    return QAPI_ERR_INVALID_PARAM;
}

SEC_LIB qapi_Status_t qapi_Timer_Set(qapi_TIMER_handle_t timer_handle, qapi_TIMER_set_attr_t *timer_attr)
{
    if (timer_handle && timer_attr)
        return _txm_module_system_call12(TXM_QAPI_TIMER_SET, (ULONG)timer_handle, (ULONG)timer_attr, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); // 65787
    return QAPI_ERR_INVALID_PARAM;
}

SEC_LIB qapi_Status_t qapi_Timer_Get_Timer_Info(qapi_TIMER_handle_t timer_handle, qapi_TIMER_get_info_attr_t *timer_get_info_attr, uint64_t *data)
{
    if (timer_handle && timer_get_info_attr && data)
        return _txm_module_system_call12(TXM_QAPI_TIMER_GET_INFO, (ULONG)timer_handle, (ULONG)timer_get_info_attr, (ULONG)data, 0, 0, 0, 0, 0, 0, 0, 0, 0); // 65790
    return QAPI_ERR_INVALID_PARAM;
}

SEC_LIB qapi_Status_t qapi_Timer_Stop(qapi_TIMER_handle_t timer_handle)
{
    if (timer_handle)
        return _txm_module_system_call12(TXM_QAPI_TIMER_STOP, (ULONG)timer_handle, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); // 65788
    return QAPI_ERR_INVALID_PARAM;
}

SEC_LIB qapi_Status_t qapi_time_get(qapi_time_unit_type time_get_unit, qapi_time_get_t *time)
{
    if (time_get_unit >= QAPI_TIME_STAMP && time_get_unit <= QAPI_TIME_JULIAN && time)
        return _txm_module_system_call12(TXM_QAPI_TIME_GET, time_get_unit, (ULONG)time, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); // 65792
    return QAPI_ERR_INVALID_PARAM;
}

SEC_LIB qapi_Status_t qapi_Timer_Sleep(uint64_t timeout, qapi_TIMER_unit_type unit, qbool_t non_deferrable)
{
    if (unit < 6)
        return _txm_module_system_call12(TXM_QAPI_TIMER_SLEEP, HIDWORD(timeout), timeout, unit, non_deferrable, 0, 0, 0, 0, 0, 0, 0, 0); // 65791
    return QAPI_ERR_INVALID_PARAM;
}
