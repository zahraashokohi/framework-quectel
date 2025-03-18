////////////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 Georgi Angelov ver 2.0
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

#ifndef HardwareSerial_h
#define HardwareSerial_h

#include "Stream.h"
#include "interface.h"
#include "hal_uart.h"

class HardwareSerial : public Stream
{
private:
    UART uart;

public:
	HardwareSerial(qapi_UART_Port_Id_e id, void *config) { uart = uart_create(id, config); }

	HardwareSerial(qapi_UART_Port_Id_e id) { uart = uart_create(id, NULL); }

	~HardwareSerial() { 
		uart_close(uart);
		uart_destroy(uart);
	}

	void begin(qapi_UART_Open_Config_t *cfg) {
		uart->cfg.baud_Rate = cfg->baud_Rate;		
        uart->cfg.bits_Per_Char = cfg->bits_Per_Char;
        uart->cfg.num_Stop_Bits = cfg->num_Stop_Bits;	
        uart->cfg.parity_Mode = cfg->parity_Mode; 
        uart->cfg.enable_Flow_Ctrl = cfg->enable_Flow_Ctrl; 			
		uart_open(uart);
	}

	void begin(unsigned long brg) {
		uart->cfg.baud_Rate = brg;
		uart_open(uart);
	}

	void end(void) { uart_close(uart); }

	virtual size_t write(uint8_t c) { return uart_putchar(uart, c); }
	virtual size_t write(uint8_t * buf, size_t size) { return uart_write(uart, (const char *)buf, size); }

	virtual int read(void) { return uart_getchar(uart); }
	virtual int read(uint8_t * buf, size_t size) { return uart_read(uart, (char *)buf, size); }

	virtual int available(void) { return uart_available(uart); }
	virtual int peek(void) { return uart_peek(uart); }
	virtual void flush(void) { while(available()); }

	int setSpeed(int brg){ return uart_ioctl(uart, QAPI_SET_BAUD_RATE_E, &brg); }
	int getSpeed() { 
		if (uart)
			return uart->cfg.baud_Rate;
		return -1;
	}

	operator bool() { 
		if (uart) 
			return uart->handle;
		return 0; 
	}

	using Print::write;
};

extern HardwareSerial Serial;
extern HardwareSerial Serial1;
extern HardwareSerial Serial2;

#endif