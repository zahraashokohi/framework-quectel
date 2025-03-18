/*
 * common.c
 *
 *  Created on: 11.06.2018
 *      Author: Georgi Angelov
 */

#include "common.h"
#include "snprintf.h"

int strStartsWith(const char *line, const char *prefix)
{
	for (; *line != '\0' && *prefix != '\0'; line++, prefix++)
	{
		if (*line != *prefix)
		{
			return 0;
		}
	}
	return *prefix == '\0';
}

int inet_ntoa(const unsigned int inaddr, unsigned char *buf, unsigned int len)
{
	unsigned char *p = (unsigned char *)&inaddr;
	int rc = 0;
	if (!p || !buf || len < 16)
	{
		rc = -1;
	}
	else
	{
		memset(buf, 0, len);
		if (-1 == snprintf((char *)buf, (unsigned int)len, "%d.%d.%d.%d", p[0], p[1], p[2], p[3]))
			rc = -1;
	}
	return rc;
}

void bcopy(const void *src, void *dest, size_t len)
{
	memmove(dest, src, len);
}
