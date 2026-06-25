/*
 * File: drivers/timers/timer3/example.c
 */

#include "core/compiler.h"
#include "drivers/timers/timer3/timer3.h"
#include "drivers/timers/tick/tick.h"

void main(void)
{
    tick_init();
    timer3_init(8u);
    timer3_start();

    while (1)
    {
        /* Optional non-blocking delay pattern based on tick counter. */
        if ((tick_get() % 500u) == 0u)
        {
        }
    }
}
