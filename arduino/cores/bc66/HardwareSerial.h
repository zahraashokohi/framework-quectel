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

#ifndef HardwareSerial_h
#define HardwareSerial_h

#include <inttypes.h>
#include <Stream.h>
#include <interface.h>
#include <os_uart.h>

class HardwareSerial : public Stream
{
protected:
  UART uart;

public:
  HardwareSerial(uint32_t port) { uart = uart_create(port); }
  ~HardwareSerial() { uart_destroy(uart); }

  void debug(void) { uart_retarget(uart); } // enable printf for this serial

  void begin(unsigned long brg) { uart_open(uart, brg, false); }

  void begin(unsigned long brg, bool retarget) { uart_open(uart, brg, retarget); }

  void begin(ST_UARTDCB *dcb, bool retarget = false)
  {
    uart_open_ex(uart, dcb);
    if (retarget)
      debug();
  }

  void end() { uart_close(uart); }
  virtual int available(void) { return uart_available(uart); }
  virtual int peek(void) { return uart_peek(uart); }
  virtual void flush(void)
  {
    while (available())
      ;
  }

  virtual int read(uint8_t *buf, size_t size) { return uart_read(uart, (char *)buf, size); }
  virtual int read(void)
  {
    uint8_t b;
    return read(&b, 1) > -1 ? b : -1;
  }

  virtual size_t write(const uint8_t *buf, size_t size) { return uart_write(uart, (char *)buf, size); }
  virtual size_t write(uint8_t c) { return write(&c, 1); }

  using Print::write;
  operator bool() { return 1; }
};

#endif
