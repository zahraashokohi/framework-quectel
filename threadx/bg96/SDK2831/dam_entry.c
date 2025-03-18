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

#pragma GCC optimize 0

#define SEC_LIB __attribute__((section(".library"))) // just for sorting

SEC_LIB void fake_make_used(void) /* GCC MAKE PREAMBLE USED - DONT TOUCH */
{
	extern volatile TXM_MODULE_PREAMBLE _txm_module_preamble;
	if (_txm_module_preamble.txm_module_preamble_checksum)
		__asm__ volatile("NOP");
}

////////////////////////////////////////////////////////////////////////////////////////
//
// 	QUALCOMM THREADX LIBRARIES: CALL DISPATCHER
//
////////////////////////////////////////////////////////////////////////////////////////

ULONG(*_txm_module_kernel_call_dispatcher)
(ULONG, ULONG, ULONG, ULONG) = NULL;
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
		{
		}
		tx_thread_resume(thread_info->txm_module_thread_entry_info_callback_request_thread); // call(66)
	}
	thread_info->txm_module_thread_entry_info_entry(thread_info->txm_module_thread_entry_info_parameter);
	txm_module_thread_system_suspend(thread_ptr); // call(91)
}

SEC_LIB void TXM_MODULE_CALLBACK_REQUEST(ULONG id)
{
	TXM_MODULE_CALLBACK_NOTIFY msg;
	TX_QUEUE *request = _txm_module_entry_info->txm_module_thread_entry_info_callback_request_queue;
	TX_QUEUE *response = _txm_module_entry_info->txm_module_thread_entry_info_callback_response_queue;
	do
	{
		if (tx_queue_receive(request, &msg, -1)) // call(42)
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
	} while (TX_SUCCESS == tx_queue_send(response, &msg, -1)); // call(43)
}

SEC_LIB ULONG _txm_module_system_call4(ULONG request,
									   ULONG param_1, ULONG param_2,
									   ULONG param_3, ULONG param_4)
{
	ULONG extra_parameters[2];
	extra_parameters[0] = param_3;
	extra_parameters[1] = param_4;
	return _txm_module_kernel_call_dispatcher(request, param_1, param_2, (ULONG)extra_parameters);
}

SEC_LIB ULONG _txm_module_system_call5(ULONG request,
									   ULONG param_1, ULONG param_2,
									   ULONG param_3, ULONG param_4,
									   ULONG param_5)
{
	ULONG extra_parameters[3];
	extra_parameters[0] = param_3;
	extra_parameters[1] = param_4;
	extra_parameters[2] = param_5;
	return _txm_module_kernel_call_dispatcher(request, param_1, param_2, (ULONG)extra_parameters);
}

SEC_LIB ULONG _txm_module_system_call6(ULONG request,
									   ULONG param_1, ULONG param_2,
									   ULONG param_3, ULONG param_4,
									   ULONG param_5, ULONG param_6)
{
	ULONG extra_parameters[4];
	extra_parameters[0] = param_3;
	extra_parameters[1] = param_4;
	extra_parameters[2] = param_5;
	extra_parameters[3] = param_6;
	return _txm_module_kernel_call_dispatcher(request, param_1, param_2, (ULONG)extra_parameters);
}

SEC_LIB ULONG _txm_module_system_call7(ULONG request,
									   ULONG param_1, ULONG param_2,
									   ULONG param_3, ULONG param_4,
									   ULONG param_5, ULONG param_6, ULONG param_7)
{
	ULONG extra_parameters[5];
	extra_parameters[0] = param_3;
	extra_parameters[1] = param_4;
	extra_parameters[2] = param_5;
	extra_parameters[3] = param_6;
	extra_parameters[4] = param_7;
	return _txm_module_kernel_call_dispatcher(request, param_1, param_2, (ULONG)extra_parameters);
}

SEC_LIB ULONG _txm_module_system_call8(ULONG request,
									   ULONG param_1, ULONG param_2,
									   ULONG param_3, ULONG param_4,
									   ULONG param_5, ULONG param_6,
									   ULONG param_7, ULONG param_8)
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

SEC_LIB ULONG _txm_module_system_call9(ULONG request,
									   ULONG param_1, ULONG param_2,
									   ULONG param_3, ULONG param_4,
									   ULONG param_5, ULONG param_6,
									   ULONG param_7, ULONG param_8,
									   ULONG param_9)
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

SEC_LIB ULONG _txm_module_system_call10(ULONG request,
										ULONG param_1, ULONG param_2,
										ULONG param_3, ULONG param_4,
										ULONG param_5, ULONG param_6,
										ULONG param_7, ULONG param_8,
										ULONG param_9, ULONG param_10)
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

SEC_LIB ULONG _txm_module_system_call11(ULONG request,
										ULONG param_1, ULONG param_2,
										ULONG param_3, ULONG param_4,
										ULONG param_5, ULONG param_6,
										ULONG param_7, ULONG param_8,
										ULONG param_9, ULONG param_10,
										ULONG param_11)
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

SEC_LIB ULONG _txm_module_system_call12(ULONG request,
										ULONG param_1, ULONG param_2,
										ULONG param_3, ULONG param_4,
										ULONG param_5, ULONG param_6,
										ULONG param_7, ULONG param_8,
										ULONG param_9, ULONG param_10,
										ULONG param_11, ULONG param_12)
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
