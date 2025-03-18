#include <interface.h>
#include <time.h>

extern void setup();
extern void loop();
extern struct timespec prog_start_time;

int main(void)
{
    clock_gettime(CLOCK_MONOTONIC, &prog_start_time);
    setup();
    while (true)
    {
        loop();
    }
}