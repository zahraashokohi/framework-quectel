////////////////////////////////////////////////////////////////////////////
//
// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
// Edit: Georgi Angelov
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

#include "os_wizio.h"
#include <time.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <sys/queue.h>

#define CONFIG_PTHREAD_TASK_NAME_DEFAULT "os-pthread"

/** task state */
enum os_pthread_task_state
{
    PTHREAD_TASK_STATE_RUN,
    PTHREAD_TASK_STATE_EXIT
};

typedef struct
{
    size_t stack_size;       ///< The stack size of the pthread
    size_t prio;             ///< The thread's priority
    bool inherit_cfg;        ///< Inherit this configuration further
    const char *thread_name; ///< The thread name.
} os_pthread_cfg_t;

/** pthread thread FreeRTOS wrapper */
typedef struct os_pthread_entry
{
    SLIST_ENTRY(os_pthread_entry)
    list_node;                        ///< Tasks list node struct.
    TaskHandle_t handle;              ///< FreeRTOS task handle
    TaskHandle_t join_task;           ///< Handle of the task waiting to join
    unsigned int event;               ///<
    enum os_pthread_task_state state; ///< pthread task state
    bool detached;                    ///< True if pthread is detached
    void *retval;                     ///< Value supplied to calling thread during join
    void *task_arg;                   ///< Task arguments
} os_pthread_t;

/** pthread wrapper task arg */
typedef struct
{
    void *(*func)(void *); ///< user task entry
    void *arg;             ///< user task argument
    os_pthread_cfg_t cfg;  ///< pthread configuration
} os_pthread_task_arg_t;

/** pthread mutex FreeRTOS wrapper */
typedef struct
{
    void *sem; ///< Handle of the task waiting to join
    int type;  ///< Mutex type. Currently supported PTHREAD_MUTEX_NORMAL and PTHREAD_MUTEX_RECURSIVE
} os_pthread_mutex_t;

static SLIST_HEAD(os_thread_list_head, os_pthread_entry) s_threads_list = SLIST_HEAD_INITIALIZER(s_threads_list);

static void *mutex_pthread = -1;
static int os_pthread_init(void)
{
    if ((void *)-1 == mutex_pthread)
    {
        mutex_pthread = Ql_OS_CreateMutex();
        //// assert
    }
    return 0;
}

static void *pthread_list_find_item(void *(*item_check)(os_pthread_t *, void *arg), void *check_arg)
{
    os_pthread_t *it;
    SLIST_FOREACH(it, &s_threads_list, list_node)
    {
        void *val = item_check(it, check_arg);
        if (val)
        {
            return val;
        }
    }
    return NULL;
}

static void *pthread_get_handle_by_desc(os_pthread_t *item, void *desc)
{
    if (item == desc)
    {
        return item->handle;
    }
    return NULL;
}

static void *pthread_get_desc_by_handle(os_pthread_t *item, void *hnd)
{
    if (hnd == item->handle)
    {
        return item;
    }
    return NULL;
}

static inline TaskHandle_t pthread_find_handle(pthread_t thread)
{
    return pthread_list_find_item(pthread_get_handle_by_desc, (void *)thread);
}

static os_pthread_t *pthread_find(TaskHandle_t task_handle)
{
    return pthread_list_find_item(pthread_get_desc_by_handle, task_handle);
}

static void pthread_delete(os_pthread_t *pthread)
{
    SLIST_REMOVE(&s_threads_list, pthread, os_pthread_entry, list_node);
    free(pthread);
}

static void pthread_task_func(void *arg)
{
    void *rval = NULL;
    os_pthread_task_arg_t *task_arg = (os_pthread_task_arg_t *)arg;
    rval = task_arg->func(task_arg->arg);
    pthread_exit(rval);
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg)
{
    os_pthread_init(); // auto init
    TaskHandle_t xHandle = NULL;
    os_pthread_task_arg_t *a = calloc(1, sizeof(os_pthread_task_arg_t));
    if (a == NULL)
    {
        return ENOMEM;
    }
    os_pthread_t *p = calloc(1, sizeof(os_pthread_t));
    if (p == NULL)
    {
        free(a);
        return ENOMEM;
    }

    uint32_t stack_size = DEFAULT_STACK_SIZE;
    const char *task_name = CONFIG_PTHREAD_TASK_NAME_DEFAULT;

    if (attr)
    {
        /* Overwrite attributes */
        if ((uint32_t)attr < 0x10000)
        {
            stack_size = (uint32_t)attr; // shotcut for stack size
            if (stack_size < DEFAULT_STACK_SIZE)
                stack_size = DEFAULT_STACK_SIZE;
        }
        else
        {
            stack_size = attr->stacksize;
            switch (attr->detachstate)
            {
            case PTHREAD_CREATE_DETACHED:
                p->detached = true;
                break;
            case PTHREAD_CREATE_JOINABLE:
            default:
                p->detached = false;
            }
        }
    }

    /* CREATE TASK */
    a->func = start_routine;
    a->arg = arg;
    p->task_arg = a;
    p->event = Ql_OS_CreateEvent();
    xHandle = os_task_create(&pthread_task_func, a, task_name, (stack_size + sizeof(StackType_t) - 1) / sizeof(StackType_t));
    if (NULL == xHandle)
    {
        free(p);
        free(a);
        return ENOMEM;
    }
    Ql_OS_TaskSuspend(xHandle);
    p->handle = xHandle;

    if (MUTEX_LOCK(mutex_pthread) != true)
    {
        assert(false && "Failed to lock threads list!");
    }
    SLIST_INSERT_HEAD(&s_threads_list, p, list_node);
    MUTEX_UNLOCK(mutex_pthread);

    /* START TASK */
    *thread = (pthread_t)p; // pointer value fit into pthread_t (uint32_t)
    Ql_OS_TaskResume(p->handle);
    return 0;
}

int pthread_join(pthread_t thread, void **retval)
{
    os_pthread_t *p = (os_pthread_t *)thread;
    int ret = 0;
    bool wait = false;
    void *child_task_retval = 0;
    TaskHandle_t handle;

    if (MUTEX_LOCK(mutex_pthread) != true)
    {
        assert(false && "Failed to lock threads list!");
    }
    {
        handle = pthread_find_handle(thread); // find task
        if (!handle)
        {
            ret = ESRCH; // not found
        }
        else if (p->detached)
        {
            ret = EDEADLK; // Thread is detached
        }
        else if (p->join_task)
        {
            ret = EINVAL; // already have waiting task to join
        }
        else if (handle == Ql_OS_GetCurrentTaskHandle())
        {
            ret = EDEADLK; // join to self not allowed
        }
        else
        {
            os_pthread_t *cur_pthread = pthread_find(Ql_OS_GetCurrentTaskHandle());
            if (cur_pthread && cur_pthread->join_task == handle)
            {
                ret = EDEADLK; // join to each other not allowed
            }
            else
            {
                if (p->state == PTHREAD_TASK_STATE_RUN)
                {
                    p->join_task = Ql_OS_GetCurrentTaskHandle();
                    wait = true;
                }
                else
                {
                    child_task_retval = p->retval;
                    pthread_delete(p);
                }
            }
        }
    }
    MUTEX_UNLOCK(mutex_pthread);

    if (0 == ret)
    {
        if (wait)
        {
            EVENT_WAIT(p->event, 1);
            if (MUTEX_LOCK(mutex_pthread) != true)
            {
                assert(false && "Failed to lock threads list!");
            }
            child_task_retval = p->retval;
            pthread_delete(p);
            MUTEX_UNLOCK(mutex_pthread);
        }
        os_task_delete(handle);
    }

    if (retval)
        *retval = child_task_retval;
    return ret;
}

int pthread_detach(pthread_t thread)
{
    os_pthread_t *p = (os_pthread_t *)thread;
    int ret = 0;

    if (MUTEX_LOCK(mutex_pthread) != true)
    {
        assert(false && "Failed to lock threads list!");
    }
    {
        TaskHandle_t handle = pthread_find_handle(thread);
        if (!handle)
        {
            ret = ESRCH;
        }
        else if (p->detached)
        {
            ret = EINVAL; // already detached
        }
        else if (p->join_task)
        {
            ret = EINVAL; // already have waiting task to join
        }
        else if (p->state == PTHREAD_TASK_STATE_RUN)
        {
            p->detached = true; // pthread still running
        }
        else
        {
            pthread_delete(p); // pthread already stopped
            os_task_delete(handle);
        }
    }
    MUTEX_UNLOCK(mutex_pthread);
    return ret;
}

void pthread_exit(void *value_ptr)
{
    bool detached = false;
    os_pthread_t *p;
    if (MUTEX_LOCK(mutex_pthread) != true)
    {
        assert(false && "Failed to lock threads list!");
    }
    {
        p = pthread_find(Ql_OS_GetCurrentTaskHandle());
        if (!p)
        {
            assert(false && "Failed to find pthread for current task!");
        }
        if (p->task_arg)
            free(p->task_arg);
        if (p->detached)
        {
            pthread_delete(p);
            detached = true;
        }
        else
        {
            p->retval = value_ptr;       // Set return value
            if (p->join_task)            // Remove from list, it indicates that task has exited
                EVENT_SEND(p->event, 1); // Notify join
            else
                p->state = PTHREAD_TASK_STATE_EXIT;
        }
    }
    MUTEX_UNLOCK(mutex_pthread);

    if (detached)
        os_task_delete(NULL);
    else
        Ql_OS_TaskSuspend(NULL);

    abort(); // Should never be reached
}

pthread_t pthread_self(void)
{
    os_pthread_t *p;
    if (MUTEX_LOCK(mutex_pthread) != true)
    {
        assert(false && "Failed to lock threads list!");
    }
    {
        p = pthread_find(Ql_OS_GetCurrentTaskHandle());
        if (!p)
        {
            assert(false && "Failed to find current thread ID!");
        }
    }
    MUTEX_UNLOCK(mutex_pthread);
    return (pthread_t)p;
}

int pthread_equal(pthread_t t1, pthread_t t2)
{
    return t1 == t2 ? 1 : 0;
}

int pthread_cancel(pthread_t thread)
{
    return ENOSYS; // not supported!
}

int sched_yield(void)
{
    Ql_Sleep(1);
    return 0;
}