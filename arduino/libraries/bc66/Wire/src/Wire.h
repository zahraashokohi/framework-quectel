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

#ifndef __WIRE_H__
#define __WIRE_H__

#include "interface.h"
#include "Stream.h"
#include "variant.h"
#include "RingBuffer.h"

//BUG-TODO: Ring buffers size must be 8 bytes max
//          hw I2C not support DMA, hardware fifo is 8 bytes
#define BUFFER_LENGTH SERIAL_BUFFER_SIZE

class TwoWire : public Stream
{
public:
  TwoWire(uint8_t port, uint8_t address, u32 brg = I2C_FREQUENCY_400K);

  void begin();
  void end();
  void setClock(uint32_t);
  void beginTransmission(uint8_t);
  uint8_t endTransmission(bool stopBit);
  uint8_t endTransmission(void);
  uint8_t requestFrom(uint8_t address, size_t quantity, bool stopBit);
  uint8_t requestFrom(uint8_t address, size_t quantity);
  size_t write(uint8_t data);
  size_t write(const uint8_t *data, size_t quantity);
  virtual int available(void);
  virtual int read(void);
  virtual int peek(void);
  virtual void flush(void);
  using Print::write;

  void onService(void){};
  void onReceive(void (*)(int)){};
  void onRequest(void (*)(void)){};

private:
  bool i2c_type;
  uint8_t i2c_port;
  u32 i2c_speed;
  uint8_t i2c_address;  
  Enum_PinName i2c_pinD, i2c_pinC;
  void init();

  bool transmissionBegun;

  RingBuffer rxBuffer;
  RingBuffer txBuffer;

  void (*onRequestCallback)(void){};
  void (*onReceiveCallback)(int){};
};

extern TwoWire Wire;

#endif
