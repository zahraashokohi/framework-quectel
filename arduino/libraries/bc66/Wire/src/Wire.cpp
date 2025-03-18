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

#include <Arduino.h>
#include "Wire.h"

TwoWire Wire(1, 0x01, I2C_FREQUENCY_100K);

#define DEBUG_I2C 
//::printf

#define SLC0 PINNAME_GPIO4
#define SDA0 PINNAME_GPIO5

TwoWire::TwoWire(uint8_t port, uint8_t address, u32 brg)
{
	i2c_port = port;
	i2c_address = address << 1; // Arduino is 7bit
	i2c_speed = brg;
	i2c_pinC = PINNAME_RI;
	i2c_pinD = PINNAME_DCD;
	transmissionBegun = false;
}

void TwoWire::init(void)
{
	Ql_IIC_Uninit(i2c_port);
	Ql_IIC_Init(i2c_port, i2c_pinC, i2c_pinD, true);
	Ql_IIC_Config(i2c_port, true, i2c_address, i2c_speed);
}

void TwoWire::begin(void)
{
	int res;
	if (1 == i2c_port)
	{
		i2c_pinC = PINNAME_RI;
		i2c_pinD = PINNAME_DCD;
	}
	else
	{
		i2c_pinC = SLC0;
		i2c_pinD = SDA0;
	}
	init();
}

void TwoWire::setClock(uint32_t baudrate)
{
	int res = -1;
	switch (baudrate)
	{
	case 1000000:
		i2c_speed = I2C_FREQUENCY_1M;
		break;
	case 400000:
		i2c_speed = I2C_FREQUENCY_400K;
		break;
	case 200000:
		i2c_speed = I2C_FREQUENCY_200K;
		break;
	case 100000:
		i2c_speed = I2C_FREQUENCY_100K;
		break;
	default:
		i2c_speed = I2C_FREQUENCY_50K;
		break;
	}
	init();
}

void TwoWire::end()
{
	Ql_IIC_Uninit(i2c_port);
}

uint8_t TwoWire::requestFrom(uint8_t address, size_t quantity, bool stopBit)
{
	i2c_address = address << 1; // Arduino is 7bit
	int res = -1;
	if (quantity == 0)
		return 0;
	if (!stopBit)
		return 0;
	rxBuffer.clear();
	res = Ql_IIC_Read(i2c_port, i2c_address, (uint8_t *)(rxBuffer._aucBuffer), (uint32_t)quantity);
	//DEBUG_I2C("[I2C] Ql_IIC_Read: %d %d\n", res, quantity);
	if (res < 0)
		quantity = 0;
	rxBuffer._iHead = quantity;
	return rxBuffer.available();
}

uint8_t TwoWire::requestFrom(uint8_t address, size_t quantity)
{
	return requestFrom(address, quantity, true);
}

void TwoWire::beginTransmission(uint8_t address)
{
	i2c_address = address << 1; // Arduino is 7bit
	txBuffer.clear();
	transmissionBegun = true;
}

uint8_t TwoWire::endTransmission(bool stopBit)
{
	if (!stopBit)
		return 4;
	transmissionBegun = false;
	if (txBuffer.available() == 0)
		return 0;
	int res = Ql_IIC_Write(i2c_port, i2c_address, (uint8_t *)(txBuffer._aucBuffer), (uint32_t)txBuffer.available());
	//DEBUG_I2C("[I2C] Ql_IIC_Write: %d\n", res);
	if (res < 0)
		return 4;
	return 0;
}

uint8_t TwoWire::endTransmission()
{
	return endTransmission(true);
}

size_t TwoWire::write(uint8_t ucData)
{
	if (!transmissionBegun || txBuffer.isFull())
		return 0;
	txBuffer.store_char(ucData);
	return 1;
}

size_t TwoWire::write(const uint8_t *data, size_t quantity)
{
	for (size_t i = 0; i < quantity; ++i)
	{
		if (!write(data[i]))
			return i;
	}
	return quantity;
}

int TwoWire::available(void)
{
	return rxBuffer.available();
}

int TwoWire::read(void)
{
	return rxBuffer.read_char();
}

int TwoWire::peek(void)
{
	return rxBuffer.peek();
}

void TwoWire::flush(void)
{
	// Do nothing, use endTransmission(..) to force data transfer.
}
