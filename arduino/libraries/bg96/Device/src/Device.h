/*
 * Device.h
 *
 *  Created on: 03.07.2018
 *      Author: Georgi Angelov
 */

#ifndef DEVICE_INFO_H_
#define DEVICE_INFO_H_

#include <interface.h>
#include <WString.h>
#include <Printable.h>

class Device // get basic module info
{
public:
	Device();
	~Device();
	bool getInfo(uint32_t id, qapi_Device_Info_t *info);
	bool getValue(uint32_t id, int *val);
	bool getBuffer(uint32_t id, char *dst, uint32_t max_size);
	bool get(uint32_t id, String &str);
	bool reset();
	int getSimState();
	void waitSim();
	bool getIMSI(String &str);
	bool getIMSI(char *buffer, size_t buffer_size);
	bool getIMEI(char *buffer, size_t buffer_size);
	bool getIMEI(String &str);
	bool getVersion(char *buffer, size_t buffer_size);
	bool getVersion(String &str);

private:
	TX_MUTEX *_mutex;
};

#endif /* DEVICE_INFO_H_ */
