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

#ifndef _LTIMER_H__
#define _LTIMER_H__

#include <os_timer.h>

class LTIMER
{
private:
    TIMER tmr;

public:
    LTIMER(TIMER_TYPE type, Callback_Timer_OnTimer callback, void *user = NULL)
    {
        tmr = timer_create(type, callback, user);
    }

    ~LTIMER() { timer_free(tmr); }

    bool start(unsigned int interval, bool mode = true)
    {
        return 0 == timer_start(tmr, interval, mode);
    }

    bool stop()
    {
        return 0 == timer_stop(tmr);
    }
};

#endif