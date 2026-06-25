/*
 * File: drivers/timers/timer1/example.c
 */

#include "core/compiler.h"
#include "drivers/timers/timer1/timer1.h"
#include "drivers/timers/tick/tick.h"

void main(void)
{
    uint32_t last = 0u;

    tick_init();

    while (1)
    {
        uint32_t now = tick_get();

        /* Software scheduler style: task each 1000 ms. */
        if ((now - last) >= 1000u)
        {
            last = now;
        }
    }
}
