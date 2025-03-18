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

#include "interface.h"

int SYSCALL(int MSG, int A, int B)
{
	ST_MSG m;
	static unsigned int mutex = 0;
	if (0 == mutex)
		mutex = Ql_OS_CreateMutex();
	if (MUTEX_LOCK(mutex))
	{
		Ql_OS_SendMessage(SYSCALL_TASK_ID, MSG, A, B);
		Ql_OS_GetMessage(&m);
		MUTEX_UNLOCK(mutex);
	}
	if (MSG == m.message)
		return m.param1;
	return -1;
}

////////////////////////////////////////////////////////////////////////////
// NO ABI
////////////////////////////////////////////////////////////////////////////

int isascii(int c) { return c >= 0 && c < 128; }
int toascii(int c) { return c & 0177; }

void reverse(char *begin, char *end)
{
	char *is = begin;
	char *ie = end - 1;
	while (is < ie)
	{
		char tmp = *ie;
		*ie = *is;
		*is = tmp;
		++is;
		--ie;
	}
}

static const char *str_digits = "0123456789abcdef";

char *itoa(int value, char *result, int base)
{
	if (result)
	{
		if (base < 2 || base > 16)
		{
			*result = 0;
			return result;
		}
		char *out = result;
		int quotient = abs(value);
		do
		{
			const int tmp = quotient / base;
			*out = str_digits[quotient - (tmp * base)];
			++out;
			quotient = tmp;
		} while (quotient);
		if (value < 0)
			*out++ = '-';
		reverse(result, out);
		*out = 0;
	}
	return result;
}
