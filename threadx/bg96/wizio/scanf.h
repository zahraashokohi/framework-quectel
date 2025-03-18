/*
 * scanf.h
 *
 *  Created on: 27.06.2018
 *      Author: georgi.angelov
 */

#ifndef SCANF_H_
#define SCANF_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include <sys/types.h>
#include <stdarg.h>
#include "common.h"

#undef sscanf
int sscanf(const char *ibuf, const char *fmt, ...);

#ifdef __cplusplus
} // close extern "C"
#endif
#endif /* SCANF_H_ */
