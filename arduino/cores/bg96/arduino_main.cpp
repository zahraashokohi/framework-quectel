#include <interface.h>

#include <txm_module.h>
#include <qapi_timer.h>

extern "C" void dbg_init(void);
extern "C" void heap_init(void);
extern "C" void __libc_init_array(void);

extern void setup();
extern void loop();

/// Arduino Task
extern "C" __attribute__((section(".library"))) int TXM_MODULE_THREAD_ENTRY(void)
{
    
#ifdef ENABLE_DEBUG
    dbg_init();
#endif

    heap_init();
    __libc_init_array();
    tx_thread_sleep(1);
    setup();
    tx_thread_sleep(1);
    while (true)
    {
        loop();
        tx_thread_sleep(1);
    }
}