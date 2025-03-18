/*
	ftp://ftp.dante.de/tex-archive/graphics/sam2p/snprintf.h
	Georgi Angelov 18.04.2018
*/

#ifndef SNPRINTF_H
#define SNPRINTF_H 1
#ifdef __cplusplus
extern "C"
{
#endif

#include <sys/types.h>
#include <stdarg.h>
#include "common.h"

#define USE_FLOAT

int vsnprintf(char *__restrict, size_t, const char *__restrict, __gnuc_va_list) __attribute__ ((__format__ (__printf__, 3, 0)));
int vsprintf(char *__restrict, const char *__restrict, __gnuc_va_list) __attribute__ ((__format__ (__printf__, 2, 0)));
int sprintf(char *__restrict, const char *__restrict, ...) __attribute__ ((__format__ (__printf__, 2, 3)));
int snprintf(char *__restrict, size_t, const char *__restrict, ...) __attribute__ ((__format__ (__printf__, 3, 4)));

#ifdef __cplusplus
} // close extern "C"
#endif
#endif /* snprintf.h */


