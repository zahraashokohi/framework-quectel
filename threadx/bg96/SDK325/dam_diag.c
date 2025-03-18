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
//#include "qapi_diag.h"

#pragma GCC optimize 0

#define SEC_LIB __attribute__((section(".library"))) // just for sorting

////////////////////////////////////////////////////////////////////////////////////////
//
// 	QUALCOMM THREADX [ SDK3 / 4 ] LIBRARIES: DIAG
//
////////////////////////////////////////////////////////////////////////////////////////

SEC_LIB ULONG diag_user_space_dispatcher(ULONG cb_id, ULONG (*app_cb)(ULONG), ULONG cb_param)
{
    if (cb_id == 102 && app_cb)
        return app_cb(cb_param);
    return 0;
}