/*
 * File: drivers/timers/tick/example.c
 */

#include "core/compiler.h"
#include "drivers/timers/tick/tick.h"

void main(void)
{
    uint32_t now;

    tick_init();

    while (1)
    {
        now = tick_get();
        (void)now;
    }
}
