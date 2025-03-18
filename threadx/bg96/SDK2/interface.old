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

SEC_LIB void fake_make_used(void) /* GCC MAKE PREAMBLE USED - DONT TOUCH */
{
	extern volatile TXM_MODULE_PREAMBLE _txm_module_preamble;
	if (_txm_module_preamble.txm_module_preamble_checksum)
		__asm__ volatile("NOP");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 	QUALCOMM THREADX LIBRARIES - SDK2
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ULONG(*_txm_module_kernel_call_dispatcher)(ULONG, ULONG, ULONG, ULONG) = NULL;
static TXM_MODULE_THREAD_ENTRY_INFO *_txm_module_entry_info = NULL;
static TXM_MODULE_INSTANCE *_txm_module_instance_ptr = NULL;

SEC_LIB void TXM_MODULE_SHELL_ENTRY(TX_THREAD *thread_ptr, TXM_MODULE_THREAD_ENTRY_INFO *thread_info)
{
	fake_make_used();
	if (thread_info->txm_module_thread_entry_info_start_thread)
	{
		_txm_module_instance_ptr = thread_info->txm_module_thread_entry_info_module;
		_txm_module_entry_info = thread_info;
		_txm_module_kernel_call_dispatcher = thread_info->txm_module_thread_entry_info_kernel_call_dispatcher;
		while (NULL == _txm_module_kernel_call_dispatcher)
			;
		tx_thread_resume(thread_info->txm_module_thread_entry_info_callback_request_thread); // 66
	}
	thread_info->txm_module_thread_entry_info_entry(thread_info->txm_module_thread_entry_info_parameter);
	txm_module_thread_system_suspend(thread_ptr); // 91
}

SEC_LIB void TXM_MODULE_CALLBACK_REQUEST(ULONG id)
{
	TXM_MODULE_CALLBACK_NOTIFY msg;
	TX_QUEUE *req_queue = _txm_module_entry_info->txm_module_thread_entry_info_callback_request_queue;
	TX_QUEUE *resp_queue = _txm_module_entry_info->txm_module_thread_entry_info_callback_response_queue;
	do
	{
		if (tx_queue_receive(req_queue, &msg, TX_NO_WAIT)) // 42
			break;
		if (msg.txm_module_callback_notify_application_function)
		{
			switch (msg.txm_module_callback_notify_type)
			{
			case 0u:
			case 1u:
			case 2u:
			case 3u:
				((void (*)(ULONG))msg.txm_module_callback_notify_application_function)(
					msg.txm_module_callback_notify_param_1);
				break;
			case 4u:
				((void (*)(ULONG, ULONG))msg.txm_module_callback_notify_application_function)(
					msg.txm_module_callback_notify_param_1,
					msg.txm_module_callback_notify_param_2);
				break;
			default:
				msg.txm_module_callback_notify_param_1 = ((int (*)(int, void *, int, int, int, int, int, int, int, int))
						                            msg.txm_module_callback_notify_application_function)(
					msg.txm_module_callback_notify_type,
					msg.txm_module_callback_notify_saved_app_cb,
					msg.txm_module_callback_notify_param_1,
					msg.txm_module_callback_notify_param_2,
					msg.txm_module_callback_notify_param_3,
					msg.txm_module_callback_notify_param_4,
					msg.txm_module_callback_notify_param_5,
					msg.txm_module_callback_notify_param_6,
					msg.txm_module_callback_notify_param_7,
					msg.txm_module_callback_notify_param_8);
				break;
			}
		}
	} while (TX_SUCCESS == tx_queue_send(resp_queue, &msg, TX_NO_WAIT)); // 43
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SEC_LIB ULONG _txm_module_system_call4(ULONG request, ULONG param_1, ULONG param_2, ULONG param_3, ULONG param_4)
{
	ULONG extra_parameters[2];
	extra_parameters[0] = param_3;
	extra_parameters[1] = param_4;
	return _txm_module_kernel_call_dispatcher(request, param_1, param_2, (ULONG)extra_parameters);
}

SEC_LIB ULONG _txm_module_system_call5(ULONG request, ULONG param_1, ULONG param_2, ULONG param_3, ULONG param_4, ULONG param_5)
{
	ULONG extra_parameters[3];
	extra_parameters[0] = param_3;
	extra_parameters[1] = param_4;
	extra_parameters[2] = param_5;
	return _txm_module_kernel_call_dispatcher(request, param_1, param_2, (ULONG)extra_parameters);
}

SEC_LIB ULONG _txm_module_system_call6(ULONG request, ULONG param_1, ULONG param_2, ULONG param_3, ULONG param_4, ULONG param_5, ULONG param_6)
{
	ULONG extra_parameters[4];
	extra_parameters[0] = param_3;
	extra_parameters[1] = param_4;
	extra_parameters[2] = param_5;
	extra_parameters[3] = param_6;
	return _txm_module_kernel_call_dispatcher(request, param_1, param_2, (ULONG)extra_parameters);
}

SEC_LIB ULONG _txm_module_system_call7(ULONG request, ULONG param_1, ULONG param_2, ULONG param_3, ULONG param_4, ULONG param_5, ULONG param_6, ULONG param_7)
{
	ULONG extra_parameters[5];
	extra_parameters[0] = param_3;
	extra_parameters[1] = param_4;
	extra_parameters[2] = param_5;
	extra_parameters[3] = param_6;
	extra_parameters[4] = param_7;
	return _txm_module_kernel_call_dispatcher(request, param_1, param_2, (ULONG)extra_parameters);
}

SEC_LIB ULONG _txm_module_system_call8(ULONG request, ULONG param_1, ULONG param_2, ULONG param_3, ULONG param_4, ULONG param_5, ULONG param_6, ULONG param_7, ULONG param_8)
{
	ULONG extra_parameters[6];
	extra_parameters[0] = param_3;
	extra_parameters[1] = param_4;
	extra_parameters[2] = param_5;
	extra_parameters[3] = param_6;
	extra_parameters[4] = param_7;
	extra_parameters[5] = param_8;
	return _txm_module_kernel_call_dispatcher(request, param_1, param_2, (ULONG)extra_parameters);
}

SEC_LIB ULONG _txm_module_system_call9(ULONG request, ULONG param_1, ULONG param_2, ULONG param_3, ULONG param_4, ULONG param_5, ULONG param_6, ULONG param_7, ULONG param_8, ULONG param_9)
{
	ULONG extra_parameters[7];
	extra_parameters[0] = param_3;
	extra_parameters[1] = param_4;
	extra_parameters[2] = param_5;
	extra_parameters[3] = param_6;
	extra_parameters[4] = param_7;
	extra_parameters[5] = param_8;
	extra_parameters[6] = param_9;
	return _txm_module_kernel_call_dispatcher(request, param_1, param_2, (ULONG)extra_parameters);
}

SEC_LIB ULONG _txm_module_system_call10(ULONG request, ULONG param_1, ULONG param_2, ULONG param_3, ULONG param_4, ULONG param_5, ULONG param_6, ULONG param_7, ULONG param_8, ULONG param_9, ULONG param_10)
{
	ULONG extra_parameters[8];
	extra_parameters[0] = param_3;
	extra_parameters[1] = param_4;
	extra_parameters[2] = param_5;
	extra_parameters[3] = param_6;
	extra_parameters[4] = param_7;
	extra_parameters[5] = param_8;
	extra_parameters[6] = param_9;
	extra_parameters[7] = param_10;
	return _txm_module_kernel_call_dispatcher(request, param_1, param_2, (ULONG)extra_parameters);
}

SEC_LIB ULONG _txm_module_system_call11(ULONG request, ULONG param_1, ULONG param_2, ULONG param_3, ULONG param_4, ULONG param_5, ULONG param_6, ULONG param_7, ULONG param_8, ULONG param_9, ULONG param_10, ULONG param_11)
{
	ULONG extra_parameters[9];
	extra_parameters[0] = param_3;
	extra_parameters[1] = param_4;
	extra_parameters[2] = param_5;
	extra_parameters[3] = param_6;
	extra_parameters[4] = param_7;
	extra_parameters[5] = param_8;
	extra_parameters[6] = param_9;
	extra_parameters[7] = param_10;
	extra_parameters[8] = param_11;
	return _txm_module_kernel_call_dispatcher(request, param_1, param_2, (ULONG)extra_parameters);
}

SEC_LIB ULONG _txm_module_system_call12(ULONG request, ULONG param_1, ULONG param_2, ULONG param_3, ULONG param_4, ULONG param_5, ULONG param_6, ULONG param_7, ULONG param_8, ULONG param_9, ULONG param_10, ULONG param_11, ULONG param_12)
{
	ULONG extra_parameters[10];
	extra_parameters[0] = param_3;
	extra_parameters[1] = param_4;
	extra_parameters[2] = param_5;
	extra_parameters[3] = param_6;
	extra_parameters[4] = param_7;
	extra_parameters[5] = param_8;
	extra_parameters[6] = param_9;
	extra_parameters[7] = param_10;
	extra_parameters[8] = param_11;
	extra_parameters[9] = param_12;
	return _txm_module_kernel_call_dispatcher(request, param_1, param_2, (ULONG)extra_parameters);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SEC_LIB qapi_Status_t qapi_Timer_Sleep(uint64_t timeout, qapi_TIMER_unit_type unit, qbool_t non_deferrable)
{
	return _txm_module_system_call12((ULONG)0x10100u, (ULONG)(timeout >> 32), (ULONG)timeout, (ULONG)unit, (ULONG)non_deferrable, 0, 0, 0, 0, 0, 0, 0, 0);
}

SEC_LIB qapi_Status_t qapi_time_get(qapi_time_unit_type time_get_unit, qapi_time_get_t *time)
{
	return _txm_module_system_call12((ULONG)0x10101, (ULONG)time_get_unit, (ULONG)time, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

static int timer_type_size = 0;
static int qapi_timer_user_initialized = 0;

SEC_LIB void qapi_timer_user_init(void)
{
	if (!timer_type_size && !qapi_timer_user_initialized)
	{
		_txm_module_system_call12((ULONG)0x10102u, (ULONG)&timer_type_size, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
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
	if (_txm_module_kernel_call_dispatcher(0x5Du, (ULONG)timer_handle, timer_type_size, 0))
		return -3;
	if (!*timer_handle)
		return -3;
	result = _txm_module_system_call12((ULONG)0x100FAu, (ULONG)timer_handle, (ULONG)timer_attr, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	if (result)
		_txm_module_kernel_call_dispatcher(0x5Eu, (ULONG)timer_handle, 0, 0);
	return result;
}

SEC_LIB qapi_Status_t qapi_Timer_Set(qapi_TIMER_handle_t timer_handle, qapi_TIMER_set_attr_t *timer_attr)
{
	return _txm_module_system_call12((ULONG)0x100FBu, (ULONG)timer_handle, (ULONG)timer_attr, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

SEC_LIB qapi_Status_t qapi_Timer_Get_Timer_Info(qapi_TIMER_handle_t timer_handle, qapi_TIMER_get_info_attr_t *timer_get_info_attr, uint64_t *data)
{
	return _txm_module_system_call12((ULONG)0x100FFu, (ULONG)timer_handle, (ULONG)timer_get_info_attr, (ULONG)data, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

SEC_LIB qapi_Status_t qapi_Timer_Undef(qapi_TIMER_handle_t timer_handle)
{
	qapi_Status_t result;
	result = _txm_module_system_call12((ULONG)0x100FEu, (ULONG)timer_handle, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	if (!result)
		_txm_module_kernel_call_dispatcher(0x5Eu, (ULONG)timer_handle, 0, 0);
	return result;
}

SEC_LIB qapi_Status_t qapi_Timer_Stop(qapi_TIMER_handle_t timer_handle)
{
	return _txm_module_system_call12((ULONG)0x100FDu, (ULONG)timer_handle, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

SEC_LIB qapi_Status_t qapi_Timer_set_absolute(qapi_TIMER_handle_t timer_handle, uint64_t abs_time)
{
	return _txm_module_system_call12((ULONG)0x100FCu, (ULONG)timer_handle, (ULONG)(abs_time >> 32), (ULONG)abs_time, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

