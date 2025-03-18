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

#ifndef OS_GPIO_H_
#define OS_GPIO_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#define GPIO_BASE 0xA20B0000

    typedef struct
    {
        volatile uint32_t RW[2];
        volatile uint32_t SET[2];
        volatile uint32_t CLR[2];
        uint32_t RESERVED[2];
    } GPIO_REGISTER_2_T;

    typedef struct
    {
        volatile uint32_t R[2];
        uint32_t RESERVED[2];
    } GPIO_DIN_REGISTER_T;
    
    typedef struct
    {
        volatile uint32_t RW[7];
        uint32_t RESERVED1;
        volatile uint32_t SET[7];
        uint32_t RESERVED2;
        volatile uint32_t CLR[7];
        uint32_t RESERVED3;
    } GPIO_MODE_REGISTER_T;

    typedef struct
    {
        GPIO_REGISTER_2_T GPIO_DIR;
        GPIO_REGISTER_2_T GPIO_DOUT;
        GPIO_DIN_REGISTER_T GPIO_DIN;
        GPIO_MODE_REGISTER_T GPIO_MODE;
    } GPIO_BASE_REGISTER_T;

#ifdef __cplusplus
}
#endif
#endif