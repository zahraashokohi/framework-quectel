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

#ifndef DBG_H_
#define DBG_H_

//#define ENABLE_DEBUG

#ifdef __cplusplus
extern "C"
{
#endif

#include <ql_stdlib.h>
#include <ql_uart.h>

  void debug_init(Enum_SerialPort port) __attribute__((weak));
  void dump(const char *buf, unsigned int len);

#ifdef ENABLE_DEBUG

  extern char DBG_BUFFER[256];
  extern Enum_SerialPort debug_port;
#define DBG(FORMAT, ...)                                                \
  {                                                                     \
    Ql_memset(DBG_BUFFER, 0, sizeof(DBG_BUFFER));                       \
    Ql_sprintf(DBG_BUFFER, FORMAT, ##__VA_ARGS__);                      \
    Ql_UART_Write(debug_port, (u8 *)DBG_BUFFER, Ql_strlen(DBG_BUFFER)); \
  }

#else

#define DBG(FORMAT, ...)

#endif /*ENABLE_DEBUG*/

#ifdef __cplusplus
} // extern "C"
#endif
#endif /* DBG_H_ */
