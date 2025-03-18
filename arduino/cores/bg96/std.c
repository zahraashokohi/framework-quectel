/*
  BG96 - std
    Created on: 01.01.2019
    Author: Georgi Angelov

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA   
 */

#include <interface.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 	HEAP MEMORY
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TX_BYTE_POOL *heap;
static char heap_buffer[HEAP]; // from -D, user defined
void heap_init(void)
{
    if (txm_module_object_allocate(&heap, sizeof(TX_BYTE_POOL)))
        abort();
    if (tx_byte_pool_create(heap, "heap_byte_pool", heap_buffer, HEAP))
        abort();
}

extern void free(void *ptr)
{
    if (ptr)
        tx_byte_release(ptr);
}

extern void *malloc(size_t size)
{
    if (!size)
        return NULL;
    void *ptr;
    if (tx_byte_allocate(heap, (VOID **)&ptr, size, TX_NO_WAIT))
    {
        return NULL;
    }
    return ptr;
}

extern void *realloc(void *mem, size_t newsize)
{
    if (newsize == 0)
    {
        free(mem);
        return NULL;
    }
    void *p;
    p = malloc(newsize);
    if (p)
    {
        if (mem != NULL)
        {
            memcpy(p, mem, newsize); // newsize !
            free(mem);
        }
    }
    return p;
}

extern void *calloc(size_t nmemb, size_t size)
{
    uint64_t total = (uint64_t)nmemb * size;
    void *ret = malloc((size_t)total);
    if (NULL == ret)
    {
        return NULL;
    }
    memset(ret, 0, (size_t)total);
    return ret;
}

extern char *strdup(const char *s)
{
    size_t len = strlen(s) + 1;
    void *new = malloc(len);
    if (NULL == new)
    {
        return NULL;
    }
    return (char *)memcpy(new, s, len);
}

////////////////////////////////////////////////////////////////////////////
// CPP
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

////////////////////////////////////////////////////////////////////////////////////////////

extern void abort(void)
{
    while (1)
    {
        qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_SEC, 1); // dont block app
    }
}

extern void __cxa_finalize(void *handle)
{
}

extern void __cxa_pure_virtual(void)
{
    abort();
}

extern void __cxa_deleted_virtual(void)
{
    abort();
}

int *__errno(void)
{
    return 0;
}

//caddr_t _sbrk(int size) { return -1; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int isdigit(int c) { return (c >= '0' && c <= '9'); }
int isalpha(int c) { return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')); }
int isalnum(int c) { return (isalpha(c) || isdigit(c)); }
int iscntrl(int c) { return (c == 127 || (c >= 0 && c <= 31)); }
int islower(int c) { return (c >= 'a' && c <= 'z'); }
int isprint(int c) { return (c >= 0x20 && c <= 0x7E); }
int isgraph(int c) { return (isprint(c) && c != ' '); }
int isspace(int c) { return (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v'); }
int ispunct(int c) { return (isprint(c) && !isspace(c) && !isalnum(c)); }
int isupper(int c) { return (c >= 'A' && c <= 'Z'); }
int isxdigit(int c) { return (isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')); }

int isascii(int c) { return c >= 0 && c < 128; }
int toascii(int c) { return c & 0177; }

#if 1
int tolower(int c)
{
    if (isupper(c))
        return 'a' - 'A' + c;
    return c;
}

int toupper(int c)
{
    if (islower(c))
        return 'A' - 'a' + c;
    return c;
}
#endif

int abs(int j)
{
    return (j < 0 ? -j : j);
}

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

extern long atol(const char *s)
{
    long val = 0;
    if (s)
    {
        extern int sscanf(const char *s, const char *format, ...);
        sscanf(s, "%ld", &val);
    }
    return val;
}

static const char *str_digits = "0123456789abcdef";
extern char *itoa(int value, char *result, int base)
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

extern char *ltoa(long value, char *result, int base)
{
    if (result)
    {
        if (base < 2 || base > 16)
        {
            *result = 0;
            return result;
        }
        char *out = result;
        long quotient = abs(value);
        do
        {
            const long tmp = quotient / base;
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

extern char *utoa(unsigned value, char *result, int base)
{
    if (result)
    {
        if (base < 2 || base > 16)
        {
            *result = 0;
            return result;
        }
        char *out = result;
        unsigned quotient = value;
        do
        {
            const unsigned tmp = quotient / base;
            *out = str_digits[quotient - (tmp * base)];
            ++out;
            quotient = tmp;
        } while (quotient);
        reverse(result, out);
        *out = 0;
    }
    return result;
}

extern char *ultoa(unsigned long value, char *result, int base)
{
    if (result)
    {
        if (base < 2 || base > 16)
        {
            *result = 0;
            return result;
        }
        char *out = result;
        unsigned long quotient = value;
        do
        {
            const unsigned long tmp = quotient / base;
            *out = str_digits[quotient - (tmp * base)];
            ++out;
            quotient = tmp;
        } while (quotient);
        reverse(result, out);
        *out = 0;
    }
    return result;
}

extern double atof(const char *s)
{
    double val;
    sscanf(s, "%lf", &val);
    return val;
}
