////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 Georgi Angelov
//      Basic library and helper for Arduino
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

#ifndef OS_EINT_H_
#define OS_EINT_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ql_eint.h>

#define EINT_CLOSE END_OF_HAL_EINT

    typedef struct eint_s
    {
        int task; //owner
        Enum_PinName pin;
        Enum_EintType type;
        Callback_EINT_Handle callback;
        void *user;
        unsigned int hwDebounce;
        unsigned int swDebounce;
        bool automask;
    } eint_t;

    typedef eint_t *EINT;

    int eint_begin(EINT); /* private */

    EINT eint_open(uint8_t pin,
                   Enum_EintType type,
                   Callback_EINT_Handle callback,
                   uint32_t hwDebounce,
                   uint32_t swDebounce,
                   bool automask);

    int eint_close(EINT eint);

#ifdef __cplusplus
}
#endif
#endif