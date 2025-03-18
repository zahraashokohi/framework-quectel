/*
 * TWI/I2C library for Arduino Zero
 * Copyright (c) 2015 Arduino LLC. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __WIRE_H__
#define __WIRE_H__

#include "interface.h"
#include "Stream.h"
#include "variant.h"
#include "RingBuffer.h"
#include <qapi_i2c_master.h>

#define BUFFER_LENGTH SERIAL_BUFFER_SIZE

class TwoWire : public Stream
{

public:
  TwoWire(uint8_t port, uint8_t address, uint32_t brg = 400000 /*Hz*/);

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
  void *handle;
  qapi_I2CM_Instance_t i2c_port;
  qapi_I2CM_Config_t config;
  static void callback(const uint32 status, void *cb_para);

  bool transmissionBegun;

  // RX Buffer
  RingBuffer rxBuffer;

  // TX buffer
  RingBuffer txBuffer;
  uint8_t txAddress;

  void (*onRequestCallback)(void){};
  void (*onReceiveCallback)(int){};
};

extern TwoWire Wire;

#endif