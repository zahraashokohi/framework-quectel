////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 Georgi Angelov
//  * Missing functions *
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

#ifndef OS_TASK_H_
#define OS_TASK_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "sys/socket.h"
#include "sys/select.h"
#include "os_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ql_freertos.h>

#define tmrCOMMAND_START 0
#define tmrCOMMAND_STOP 1
#define tmrCOMMAND_CHANGE_PERIOD 2
#define tmrCOMMAND_DELETE 3

    typedef int BaseType_t;
    typedef unsigned int UBaseType_t;
    typedef unsigned int TickType_t;
    typedef unsigned int StackType_t;

    typedef void *TaskHandle_t;
    typedef void (*TaskFunction_t)(void *);

    TaskHandle_t os_task_create(void (*fun)(void *), void *const arg, const char *const name, const uint16_t stack);
    BaseType_t os_task_create_ex(TaskFunction_t pxTaskCode,
                                 const char *const pcName,
                                 const uint16_t usStackDepth,
                                 void *const pvParameters,
                                 UBaseType_t uxPriority,
                                 TaskHandle_t *const pxCreatedTask,
                                 StackType_t *const puxStackBuffer,
                                 const void *const xRegions);

#define os_task_suspend(X) Ql_OS_TaskSuspend(X)
#define os_task_resume(X) Ql_OS_TaskResume(X)

    typedef void *TimerHandle_t;
    typedef void (*TimerCallbackFunction_t)(TimerHandle_t xTimer);

#define os_timer_start(xTimer, xBlockTime) os_timer_generic_command((xTimer), tmrCOMMAND_START, (Ql_OS_GetTaskTickCount()), NULL, (xBlockTime))
#define os_timer_stop(xTimer, xBlockTime) os_timer_generic_command((xTimer), tmrCOMMAND_STOP, 0U, NULL, (xBlockTime))
#define os_timer_delete(xTimer, xBlockTime) os_timer_generic_command((xTimer), tmrCOMMAND_DELETE, 0U, NULL, (xBlockTime))
#define os_timer_reset(xTimer, xBlockTime) os_timer_generic_command((xTimer), tmrCOMMAND_START, (Ql_OS_GetTaskTickCount()), NULL, (xBlockTime))

    typedef enum
    {
        eNoAction = 0,            /*!< Notify the task without updating its notify value. */
        eSetBits,                 /*!< Set bits in the task's notification value. */
        eIncrement,               /*!< Increment the task's notification value. */
        eSetValueWithOverwrite,   /*!< Set the task's notification value to a specific value even if the previous value has not yet been read by the task. */
        eSetValueWithoutOverwrite /*!< Set the task's notification value if the previous value has been read by the task. */
    } eNotifyAction;

#ifdef __cplusplus
}
#endif
#endif