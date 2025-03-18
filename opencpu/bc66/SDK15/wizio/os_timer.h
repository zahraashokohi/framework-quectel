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

#ifndef OS_TIMER_H_
#define OS_TIMER_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <ql_timer.h>

    typedef enum
    {
        TIMER_USEC = 0xFF,
        TIMER_FAST,
        TIMER_1,
        TIMER_2,
        TIMER_3,
        TIMER_4,
        TIMER_5,
        TIMER_6,
        TIMER_7,
        TIMER_8,
        TIMER_9,
        TIMER_END,
    } TIMER_TYPE;

    typedef enum
    {
        TIMER_ONESHOT = 0,
        TIMER_REPEAT
    } TIMER_MODE;

    typedef struct
    {
        int task; // owner
        int id;
        bool mode;
        unsigned int interval;
        void *user;
        Callback_Timer_OnTimer callback;
        bool registered;
    } timer_ctx;
    typedef timer_ctx *TIMER;

    int timer_begin(TIMER t); /* private */

    TIMER timer_create(TIMER_TYPE type, Callback_Timer_OnTimer callback, void *user);
    int timer_start(TIMER t, unsigned int interval, bool mode);
    int timer_stop(TIMER t);
    void timer_free(TIMER t);

#ifdef __cplusplus
}
#endif
#endif