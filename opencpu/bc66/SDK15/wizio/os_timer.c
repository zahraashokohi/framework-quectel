#include "os_wizio.h"
#include "os_timer.h"

TIMER timer_create(TIMER_TYPE type, Callback_Timer_OnTimer callback, void *user)
{
    if ((type < TIMER_USEC && type >= TIMER_END) || NULL == callback)
        return NULL;

    TIMER t = calloc(1, sizeof(timer_ctx));
    if (t)
    {
        t->id = type;
        t->callback = callback;
        t->user = user;
    }
    return t;
}

/* private */
int timer_begin(TIMER t)
{
    int res = -1;
    if (t)
    {
        if (0 == t->registered)
        {
            if (TIMER_USEC == t->id)
                res = Ql_Timer_Register_us(t->id, t->callback, t->user);
            else if (TIMER_FAST == t->id)
                res = Ql_Timer_RegisterFast(t->id, t->callback, t->user);
            else
                res = Ql_Timer_Register(t->id, t->callback, t->user);
            if (res)
                return res;
            else
                t->registered = 1;
        }
        return Ql_Timer_Start(t->id, t->interval, t->mode);
    }
    return res;
}

int timer_start(TIMER t, unsigned int interval, bool mode)
{
    int res = -1;
    if (t)
    {
        t->task = Ql_OS_GetActiveTaskId();
        t->interval = interval;
        t->mode = mode;
#ifdef ARDUINO
        res = SYSCALL(M_TIMER_START, t, 0);
#else
        res = timer_begin(t);
#endif
    }
    return res;
}

int timer_stop(TIMER t)
{
    int res = -1;
    if (t)
    {
#ifdef ARDUINO
        res = SYSCALL(M_TIMER_STOP, t, 0);
#else
        res = Ql_Timer_Stop(t->id);
#endif
    }
    return res;
}

void timer_free(TIMER t)
{
#ifdef ARDUINO
    if (t && 0 == SYSCALL(M_TIMER_CLOSE, t, 0))
#else
    if (t && 0 == Ql_Timer_Delete(t->id))
#endif
        free(t);
}