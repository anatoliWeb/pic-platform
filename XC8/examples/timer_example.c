/*
 * File: XC8/examples/timer_example.c
 */

#include "core/compiler.h"
#include "drivers/timers/timer0/timer0.h"
#include "drivers/timers/timer1/timer1.h"
#include "drivers/timers/timer2/timer2.h"
#include "drivers/timers/timer3/timer3.h"
#include "drivers/timers/tick/tick.h"

void main(void)
{
    timer0_init(256u);
    timer1_init(8u);
    timer2_init(16u);
    timer3_init(8u);

    tick_init();

    while (1)
    {
        if ((tick_get() % 1000u) == 0u)
        {
        }
    }
}

