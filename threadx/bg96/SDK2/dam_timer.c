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
// 	QUALCOMM THREADX [ SDK2 ] LIBRARIES: TIMERS
//
////////////////////////////////////////////////////////////////////////////////////////

static int timer_type_size = 0;
static int qapi_timer_user_initialized = 0;

SEC_LIB void qapi_timer_user_init(void)
{
    if (!timer_type_size && !qapi_timer_user_initialized)
    {
        _txm_module_system_call12(TXM_QAPI_TIMER_GET_TIMER_TYPE_SIZE, (ULONG)&timer_type_size, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); // 65794
        qapi_timer_user_initialized = 1;
    }
}

SEC_LIB qapi_Status_t qapi_Timer_Def(qapi_TIMER_handle_t *timer_handle, qapi_TIMER_define_attr_t *timer_attr)
{
    qapi_Status_t result;
    if (qapi_timer_user_initialized != 1)
        qapi_timer_user_init();
    if (!timer_handle)
        return -2;
    if (!timer_type_size)
        return -9;
    if (_txm_module_kernel_call_dispatcher(TXM_MODULE_OBJECT_ALLOCATE_CALL, (ULONG)timer_handle, timer_type_size, 0)) // 93
        return -3;
    if (!*timer_handle)
        return -3;
    result = _txm_module_system_call12(TXM_QAPI_TIMER_DEF, (ULONG)timer_handle, (ULONG)timer_attr, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); // 65786
    if (result)
        _txm_module_kernel_call_dispatcher(TXM_MODULE_OBJECT_DEALLOCATE_CALL, (ULONG)timer_handle, 0, 0); // 94
    return result;
}

SEC_LIB qapi_Status_t qapi_Timer_Set(qapi_TIMER_handle_t timer_handle, qapi_TIMER_set_attr_t *timer_attr)
{
    return _txm_module_system_call12(TXM_QAPI_TIMER_SET, (ULONG)timer_handle, (ULONG)timer_attr, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); // 65787
}

SEC_LIB qapi_Status_t qapi_Timer_Get_Timer_Info(qapi_TIMER_handle_t timer_handle, qapi_TIMER_get_info_attr_t *timer_get_info_attr, uint64_t *data)
{
    return _txm_module_system_call12(TXM_QAPI_TIMER_GET_INFO, (ULONG)timer_handle, (ULONG)timer_get_info_attr, (ULONG)data, 0, 0, 0, 0, 0, 0, 0, 0, 0); // 65791
}

SEC_LIB qapi_Status_t qapi_Timer_Undef(qapi_TIMER_handle_t timer_handle)
{
    qapi_Status_t result;
    result = _txm_module_system_call12(TXM_QAPI_TIMER_UNDEF, (ULONG)timer_handle, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); // 65790
    if (!result)
        _txm_module_kernel_call_dispatcher(TXM_MODULE_OBJECT_DEALLOCATE_CALL, (ULONG)timer_handle, 0, 0); // 94
    return result;
}

SEC_LIB qapi_Status_t qapi_Timer_Stop(qapi_TIMER_handle_t timer_handle)
{
    return _txm_module_system_call12(TXM_QAPI_TIMER_STOP, (ULONG)timer_handle, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); // 65789
}

SEC_LIB qapi_Status_t qapi_Timer_set_absolute(qapi_TIMER_handle_t timer_handle, uint64_t abs_time)
{
    return _txm_module_system_call12(TXM_QAPI_TIMER_SET_ABS, (ULONG)timer_handle, (ULONG)(abs_time >> 32), (ULONG)abs_time, 0, 0, 0, 0, 0, 0, 0, 0, 0); // 65788
}

SEC_LIB qapi_Status_t qapi_Timer_Sleep(uint64_t timeout, qapi_TIMER_unit_type unit, qbool_t non_deferrable)
{
    return _txm_module_system_call12(TXM_QAPI_TIMER_SLEEP, (ULONG)(timeout >> 32), (ULONG)timeout, (ULONG)unit, (ULONG)non_deferrable, 0, 0, 0, 0, 0, 0, 0, 0); // 65792
}

SEC_LIB qapi_Status_t qapi_time_get(qapi_time_unit_type time_get_unit, qapi_time_get_t *time)
{
    return _txm_module_system_call12(TXM_QAPI_TIME_GET, (ULONG)time_get_unit, (ULONG)time, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); // 65793
}