////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 Georgi Angelov
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License os_ril
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitos_rilions under the License.
//
////////////////////////////////////////////////////////////////////////////

#ifndef AT_RIL_H_
#define AT_RIL_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ril_util.h>
#include <ril.h>
#include <ril_system.h>
#include <ril_sim.h>
#include <ql_system.h>
#include <ql_power.h>

    bool os_ril_get_version(char *version);
    bool os_ril_get_imsi(char *buffer);
    bool os_ril_get_iccid(char *buffer);
    bool os_ril_get_mcc_mnc(char *buffer);
    bool os_ril_set_band(int band);
    bool os_ril_set_bands(int count, ...);
    int os_ril_get_cereg(void); /* helper for Arduino */
    void os_ril_wait_cereg(void);
    int os_ril_get_sim(void);
    void os_ril_wait_sim(void);                                     /* helper for Arduino */
    bool os_ril_set_dns(const char *dns_prim, const char *dns_sec); /* ip4 servers */
    void os_ril_do_connect(int band, int no_sleep);                 /* default connection, helper for Arduino */

#ifdef __cplusplus
}
#endif

#endif /* INTERFACE_H_ */