#include <stdint.h>
#include <time.h>

#if 0
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
int isblank(int c) { return (c == ' ' || c == '\t'); }
int isascii(int c) { return c >= 0 && c < 128; }
int toascii(int c) { return c & 0177; }
#endif

#ifdef abs
#undef abs
#endif
#define abs(x) ((x) > 0 ? (x) : -(x))

static const char *str_digits = "0123456789abcdef";

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

char *utoa(unsigned value, char *result, int base)
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

char *ultoa(unsigned long value, char *result, int base)
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

char *ltoa(long value, char *result, int base)
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

uint32_t utc(void)
{
    time_t t;
    time(&t);
    return t;
}