/*
 * Device.cpp
 *
 *  Created on: 03.07.2018
 *      Author: Georgi Angelov
 */

#include <Arduino.h>
#include "Device.h"

#define DEBUG_DEV

Device::~Device()
{
	if (_mutex)
	{
		tx_mutex_delete(_mutex);
		txm_module_object_deallocate(_mutex);
	}
	qapi_Device_Info_Release();
}

Device::Device()
{
	_mutex = NULL;
	int r;
	if ((r = txm_module_object_allocate(&_mutex, sizeof(TX_MUTEX))))
	{
		DEBUG_DEV("[ERROR] DEV MUTEX txm_module_object_allocate: %d\n", r);
		abort();
	}
	if ((r = tx_mutex_create(_mutex, "device_info_mutex", TX_INHERIT)))
	{
		DEBUG_DEV("[ERROR] DEV tx_mutex_create: %d\n", r);
		abort();
	}
	if ((r = qapi_Device_Info_Init()))
	{
		DEBUG_DEV("[ERROR] DEV qapi_Device_Info_Init = %d\n", r);
		abort();
	}
}

bool Device::getInfo(uint32_t id, qapi_Device_Info_t *info)
{
	if (NULL == info || id > QAPI_DEVICE_INFO_SIM_STATE_E)
		return false;
	int r = tx_mutex_get(_mutex, TX_WAIT_FOREVER);
	if (r)
	{
		DEBUG_DEV("[ERROR] DEV tx_mutex_get: %d, %X\n\n", r, _mutex);
		return false;
	}
	memset(info, 0, sizeof(qapi_Device_Info_t));
	r = qapi_Device_Info_Get((qapi_Device_Info_ID_t)id, info);
	if (tx_mutex_put(_mutex))
	{
		DEBUG_DEV("[ERROR] DEV tx_mutex_put\n");
	}
	return r == 0;
}

/**
 * @brief Device Info GET integer value.
 *
 * @param[in]  id   Information ID.
 * @param[out] val  Information received for the specified ID.
 * @return 			true on success.
 */
bool Device::getValue(uint32_t id, int *val)
{
	bool r = false;
	if (val)
	{
		*val = 0;
		qapi_Device_Info_t info;
		if ((r = getInfo(id, &info)))
		{
			if (QAPI_DEVICE_INFO_TYPE_INTEGER_E == info.info_type)
				*val = info.u.valueint;
			else if (QAPI_DEVICE_INFO_TYPE_BOOLEAN_E == info.info_type)
				*val = info.u.valuebool;
			else
				r = false;
		}
	}
	return r;
}

/**
 * @brief Device Info GET char buffer.
 *
 * @param[in]  id    	Information ID.
 * @param[out] buffer   Information received for the specified ID. Maximum size 128
 * @param[out] size  	buffer max size
 * @return 				true on success.
 */
bool Device::getBuffer(uint32_t id, char *dst, uint32_t max_size)
{
	if (dst && max_size)
	{
		memset(dst, 0, max_size);
		qapi_Device_Info_t info;
		if (getInfo(id, &info))
		{
			if (max_size > QAPI_DEVICE_INFO_BUF_SIZE)
				max_size = QAPI_DEVICE_INFO_BUF_SIZE;
			if (info.u.valuebuf.len > max_size)
				info.u.valuebuf.len = max_size;
			if (info.u.valuebuf.buf && info.u.valuebuf.len)
			{
				memcpy(dst, info.u.valuebuf.buf, info.u.valuebuf.len);
				return info.u.valuebuf.len;
			}
		}
	}
	return false;
}

bool Device::get(uint32_t id, String &str)
{
	qapi_Device_Info_t info;
	bool r;
	if ((r = getInfo(id, &info)))
	{
		if (QAPI_DEVICE_INFO_TYPE_BUFFER_E == info.info_type)
			str = info.u.valuebuf.buf;
		else
			str = "";
	}
	return r;
}

/**
 * @brief Resets the device.
 *
 * @return true on success.
 */
bool Device::reset()
{
	return qapi_Device_Info_Reset() == 0;
}

int Device::getSimState()
{
	int state = 0;
	int r = getValue(QAPI_DEVICE_INFO_SIM_STATE_E, &state);
	if (false == r)
		return -1; // NO SIM
	return state;
}

void Device::waitSim()
{
	int state;
	while (1)
	{
		state = getSimState();
		if (state == 7) // see AT command
			break;
		qapi_Timer_Sleep(200, QAPI_TIMER_UNIT_MSEC, 1);
	}
}

bool Device::getIMSI(char *buffer, size_t buffer_size)
{
	return getBuffer(QAPI_DEVICE_INFO_IMSI_E, buffer, buffer_size);
}

bool Device::getIMSI(String &str)
{
	return get(QAPI_DEVICE_INFO_IMSI_E, str);
}

bool Device::getIMEI(char *buffer, size_t buffer_size)
{
	return getBuffer(QAPI_DEVICE_INFO_IMEI_E, buffer, buffer_size);
}

bool Device::getIMEI(String &str)
{
	return get(QAPI_DEVICE_INFO_IMEI_E, str);
}

bool Device::getVersion(char *buffer, size_t buffer_size)
{
	return getBuffer(QAPI_DEVICE_INFO_OS_VERSION_E, buffer, buffer_size);
}

bool Device::getVersion(String &str)
{
	return get(QAPI_DEVICE_INFO_OS_VERSION_E, str);
}
