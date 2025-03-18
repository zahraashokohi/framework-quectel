/*
 * common.h
 *
 *  Created on: 11.06.2018
 *      Author: 1124
 */

#ifndef COMMON_H_
#define COMMON_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stddef.h>
#include <string.h>

void bcopy(const void *src, void *dest, size_t len);
int strStartsWith(const char *line, const char *prefix);
int inet_ntoa(const unsigned int inaddr, unsigned char *buf, unsigned int len);

#ifdef __cplusplus
} // close extern "C"
#endif
#endif /* COMMON_H_ */
