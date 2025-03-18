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

#include "os_dbg.h"

#ifdef ENABLE_DEBUG

static Enum_SerialPort debugPort = UART_PORT_END;
char DBG_BUFFER[256];
void uart_callback(Enum_SerialPort port, Enum_UARTEventType event, bool level, void *customizedPara);
Enum_SerialPort debug_port = -1;
void debug_init(Enum_SerialPort port)
{
	debug_port = port;
	Ql_UART_Register(port, uart_callback, NULL); //UART_PORT1
	Ql_UART_Open(port, 115200, FC_NONE);
	DBG("[DEBUG MODE]\n");
}

#endif


#include <stdio.h>
#define PRINT_LINE puts
void dump(const char *buf, unsigned int len)
{
	char str[1024];
	char txt[17];
	unsigned int i, idx = 0;
	Ql_snprintf(str + idx, sizeof(str) - idx, "[DUMP] 0x%X (%u bytes)", buf, (unsigned int)len);
	PRINT_LINE(str);
	idx = 0;
	Ql_memset(txt, 0, sizeof(txt));
	for (i = 0; i < len; i++)
	{
		if (i >= 4096)
			break;
		if (i % 16 == 0)
		{
			if (i > 0)
			{
				Ql_snprintf(str + idx, sizeof(str) - idx, "  %s", txt);
				PRINT_LINE(str);
				idx = 0;
				Ql_memset(txt, 0, sizeof(txt));
			}
			idx += Ql_snprintf(str + idx, sizeof(str) - idx, "%04X: ", (unsigned int)i);
		}
		idx += Ql_snprintf(str + idx, sizeof(str) - idx, " %02X", (unsigned int)buf[i]);
		txt[i % 16] = (buf[i] > 31 && buf[i] < 127) ? buf[i] : '.';
	}
	if (len > 0)
	{
		for (/* i = i */; i % 16 != 0; i++)
			idx += Ql_snprintf(str + idx, sizeof(str) - idx, "   ");
		Ql_snprintf(str + idx, sizeof(str) - idx, "  %s", txt);
		PRINT_LINE(str);
	}
}
