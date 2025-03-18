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

#include <Arduino.h>
#include "Wire.h"

#define DEBUG_I2C 
//DBG

void TwoWire::callback(const uint32 status, void *cb_para) {}

TwoWire::TwoWire(uint8_t port, uint8_t slave_address, uint32_t brg)
{
	handle = NULL;
	i2c_port = (qapi_I2CM_Instance_t)port;
	config.bus_Frequency_KHz = brg / 1000;
	config.slave_Address = slave_address;
	config.slave_Max_Clock_Stretch_Us = 100000;
	config.SMBUS_Mode = 0;
	config.core_Configuration1 = 0;
	config.core_Configuration2 = 0;
	transmissionBegun = false;
}

void TwoWire::begin(void)
{
	if (handle)
		return; // already
	int r = qapi_I2CM_Open(i2c_port, &handle);
	if (r)
	{
		DEBUG_I2C("[ERROR] I2C[%d] qapi_I2CM_Open: %d\n", i2c_port, r);
		return;
	}
	r = qapi_I2CM_Power_On(handle);
	if (r)
	{
		DEBUG_I2C("[ERROR] I2C[%d] qapi_I2CM_Power_On: %d\n", i2c_port, r);
		return;
	}
	DEBUG_I2C("[I2C] Ready: %X\n", handle);
}

void TwoWire::end()
{
	if (handle)
		qapi_I2CM_Close(handle);
	handle = NULL;
}

void TwoWire::setClock(uint32_t baudrate)
{
	config.bus_Frequency_KHz = baudrate / 1000;
}

uint8_t TwoWire::requestFrom(uint8_t address, size_t quantity, bool stopBit)
{
	int r = -1;
	if (quantity == 0)
		return 0;
	if (!stopBit)
		return 0;
	config.slave_Address = address;
	rxBuffer.clear();
	uint32_t transferred = 0;
	qapi_I2CM_Descriptor_t desc;
	desc.flags = QAPI_I2C_FLAG_START | QAPI_I2C_FLAG_READ | QAPI_I2C_FLAG_STOP;
	desc.buffer = (uint8_t *)(rxBuffer._aucBuffer);
	desc.length = (uint32_t)quantity;
	desc.transferred = (uint32_t)&transferred;
	r = qapi_I2CM_Transfer(handle, &config, &desc, 1, callback, this, 100);
	if (r)
	{
		DEBUG_I2C("[ERROR] I2C Read: %d\n", r);
		quantity = 0;
		return 0;
	}
	//DEBUG_I2C("[I2C] Readed: %d\n", transferred); ???
	rxBuffer._iHead = quantity;
	return rxBuffer.available();
}

uint8_t TwoWire::requestFrom(uint8_t address, size_t quantity)
{
	return requestFrom(address, quantity, true);
}

void TwoWire::beginTransmission(uint8_t address)
{
	txAddress = address;
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
	uint32_t transferred = 0;
	qapi_I2CM_Descriptor_t desc;
	desc.flags = QAPI_I2C_FLAG_START | QAPI_I2C_FLAG_WRITE | QAPI_I2C_FLAG_STOP;
	desc.buffer = (uint8_t *)(txBuffer._aucBuffer);
	desc.length = (uint32_t)txBuffer.available();
	desc.transferred = (uint32_t)&transferred;
	int r = qapi_I2CM_Transfer(handle, &config, &desc, 1, callback, this, 100);
	if (r)
	{
		DEBUG_I2C("[ERROR] I2C Write: %d\n", r);
	}
	//DEBUG_I2C("[I2C] Writed: %d\n", transferred);
	return r;
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

TwoWire Wire = TwoWire(QAPI_I2CM_INSTANCE_004_E, 1, 100000UL);