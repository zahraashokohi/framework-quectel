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
// qapi_psm.h

#pragma GCC optimize 0

#define SEC_LIB __attribute__((section(".library"))) // just for sorting

////////////////////////////////////////////////////////////////////////////////////////
//
// 	QUALCOMM THREADX [ SDK3 / 4 ] LIBRARIES: PSM
//
////////////////////////////////////////////////////////////////////////////////////////

SEC_LIB void psm_user_space_dispatcher(ULONG request,
                                       ULONG (*app_cb)(ULONG, ULONG, ULONG, ULONG),
                                       UINT cb_param1,
                                       UINT cb_param2,
                                       UINT cb_param3,
                                       UINT cb_param4)
{
    if (request == 108 && app_cb)
        app_cb(cb_param1, cb_param2, cb_param3, cb_param4);
}
