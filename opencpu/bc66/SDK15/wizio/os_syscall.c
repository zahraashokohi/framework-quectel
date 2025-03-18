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

#include "os_api.h"
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <time.h>
#include <reent.h>
#include <string.h>
#include <ql_system.h>
#include <ql_time.h>

#include "os_uart.h"
#include "os_fs.h"

////////////////////////////////////////////////////////////////////////////
// INIT CPP
////////////////////////////////////////////////////////////////////////////

extern void (*__preinit_array_start[])(void) __attribute__((weak));
extern void (*__preinit_array_end[])(void) __attribute__((weak));
extern void (*__init_array_start[])(void) __attribute__((weak));
extern void (*__init_array_end[])(void) __attribute__((weak));
extern void (*__fini_array_start[])(void) __attribute__((weak));
extern void (*__fini_array_end[])(void) __attribute__((weak));
extern void _init(void) __attribute__((weak));
extern void _fini(void) __attribute__((weak));

void __libc_init_array(void)
{
    size_t count;
    size_t i;
    count = __preinit_array_end - __preinit_array_start;
    for (i = 0; i < count; i++)
        __preinit_array_start[i]();
    _init();
    count = __init_array_end - __init_array_start;
    for (i = 0; i < count; i++)
        __init_array_start[i]();
}

void __libc_fini_array(void)
{
    size_t count;
    size_t i;
    count = __fini_array_end - __fini_array_start;
    for (i = count; i > 0; i--)
        __fini_array_start[i - 1]();
    _fini();
}

////////////////////////////////////////////////////////////////////////////
// SYSCALL
////////////////////////////////////////////////////////////////////////////

void abort(void)
{
    while (1)
        Ql_Sleep(-1);
}

void __cxa_finalize(void *handle) {}
void __cxa_pure_virtual(void) { abort(); }
void __cxa_deleted_virtual(void) { abort(); }

void _exit(int status)
{
    abort();
    while (1){}
}

void _kill(int pid, int sig) { return; }
int _getpid(void) { return -1; }
int _getpid_r(struct _reent *r) { return -1; }
void _raise(void) { return; }
int __attribute__((weak)) _link(const char *__path1 __attribute__((unused)), const char *__path2 __attribute__((unused))) { return 0; }

int _unlink(const char *path)
{
    return -1;
}

int _stat(const char *path, struct stat *st)
{
#ifdef USE_FS
// TODO
#endif
    return -1;
}

int _fstat_r(struct _reent *ignore, int fd, struct stat *st)
{
#ifdef USE_FS
// TODO
#endif
    return -1;
}

int _isatty(int fd) { return fd >= 0; }

int _open_r(struct _reent *ignore, const char *path, int flags, int mode)
{
#ifdef USE_FS
    return vfs_open(path, flags, mode);
#endif
    return -1;
}

int _close_r(struct _reent *ignore, int fd)
{
#ifdef USE_FS
    return vfs_close(fd);
#endif
    return -1;
}

_ssize_t _write_r(struct _reent *ignore, int fd, const void *buf, size_t len)
{ // do not use for socket
    if (buf && len)
    {
        if (fd >= 0 && fd < UART_MAX_PORTS && stdout->_cookie && stdout->_write) // here fd is uart port
            return stdout->_write(0, stdout->_cookie, buf, len);
#ifdef USE_FS
        return vfs_write(fd, buf, len);
#endif
    }
    return 0;
}

_ssize_t _read_r(struct _reent *ignore, int fd, void *buf, size_t len)
{ // do not use for socket
    if (buf && len)
    {
        if (fd >= 0 && fd < UART_MAX_PORTS && stdin->_cookie && stdout->_read) // here fd is uart port
            return stdout->_read(0, stdout->_cookie, buf, len);
#ifdef USE_FS
        return vfs_read(fd, buf, len);
#endif
    }
    return 0;
}

_off_t _lseek_r(struct _reent *ignore, int fd, _off_t where, int whence)
{
#ifdef USE_FS
    return vfs_seek(fd, where, whence);
#endif
    return -1;
}

////////////////////////////////////////////////////////////////////////////
// TIME
////////////////////////////////////////////////////////////////////////////

time_t now(void)
{
    ST_Time st;
    if (NULL == Ql_GetLocalTime(&st))
        return 0;
    return Ql_Mktime(&st);
}

time_t _Time(time_t *timer) { return now(); }

time_t _time(time_t *tod)
{
    time_t t = _Time(NULL);
    if (tod)
        *tod = t;
    return (t);
}

clock_t _times(struct tms *buf) { return Ql_OS_GetTaskTickCount(); }

int _gettimeofday_r(struct _reent *ignore, struct timeval *tv, void *tz)
{
    /* time() */
    (void)tz;
    if (tv)
    {
        tv->tv_sec = now();
        tv->tv_usec = 0;
        return 0;
    }
    return -1;
}

////////////////////////////////////////////////////////////////////////////
// MEMORY
////////////////////////////////////////////////////////////////////////////

void *malloc(size_t size)
{
    return Ql_MEM_Alloc(size);
}
void *_malloc_r(struct _reent *ignore, size_t size) { return malloc(size); }

void free(void *p)
{
    if (p)
        Ql_MEM_Free(p);
}
void _free_r(struct _reent *ignore, void *ptr) { free(ptr); }

void *realloc(void *mem, size_t newsize)
{
    void *new = malloc(newsize);
    if ((new) && (mem))
        memcpy(new, mem, newsize);
    free(mem);
    return new;
}
void *_realloc_r(struct _reent *ignored, void *ptr, size_t size) { return realloc(ptr, size); }

void *calloc(size_t element, size_t size)
{
    size_t total = element * size;
    void *ret = malloc(total);
    if (ret)
        memset(ret, 0, total);
    return ret;
}
void *_calloc_r(struct _reent *ignored, size_t element, size_t size) { return calloc(element, size); }

////////////////////////////////////////////////////////////////////////////
// SYSTEM INIT
////////////////////////////////////////////////////////////////////////////

void os_init(void)
{
    os_api_setup();      // init API
    __libc_init_array(); // init CPP
}
