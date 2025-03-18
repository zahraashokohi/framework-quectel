#include "os_wizio.h"
#include "os_eint.h"

/* private */
int eint_begin(EINT e)
{
    int res = -1;
    if (e)
    {
        res = Ql_EINT_RegisterFast(e->pin, e->callback, (void *)((int)e->user));
        if (0 == res)
            res = Ql_EINT_Init(e->pin, e->type, e->hwDebounce, e->swDebounce, e->automask);
    }
    return res;
}

EINT eint_open(uint8_t pin,
                  Enum_EintType type,
                  Callback_EINT_Handle callback,
                  uint32_t hwDebounce,
                  uint32_t swDebounce,
                  bool automask)
{
    EINT eint = calloc(1, sizeof(eint_t));
    if (eint)
    {
        eint->task = Ql_OS_GetActiveTaskId();
        eint->pin = pin;
        eint->type = type;
        eint->callback = callback;
        eint->hwDebounce = hwDebounce;
        u32 swDebounce = swDebounce;
        eint->automask = automask;
        eint->user = eint;
#ifdef ARDUINO
        int res = SYSCALL(M_EINT_OPEN, eint, 0);
#else
        int res = eint_begin(eint);
#endif
        if (res)
        {
            free(eint);
            eint = NULL;
        }
    }
    return eint;
}

int eint_close(EINT eint)
{
    int res = -1;
    if (eint)
    {
#ifdef ARDUINO
        res = SYSCALL(M_EINT_CLOSE, eint, 0);
#else
        res = Ql_EINT_Uninit(eint->pin);
#endif
    }
    return res;
}