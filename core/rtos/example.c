/*
 * File: core/rtos/example.c
 */

#include "core/rtos/rtos.h"
#include "drivers/timers/tick/tick.h"

/*
 * Minimal RTOS abstraction example.
 *
 * In bare-metal mode, delay uses delay_ms() and tick can be obtained from
 * tick driver when initialized.
 */
void rtos_example_run(void)
{
    uint32_t last_ms;

    tick_init();
    rtos_init();

    last_ms = rtos_get_tick_ms();

    while (1)
    {
        uint32_t now;

        now = rtos_get_tick_ms();
        if ((now - last_ms) >= 1000u)
        {
            last_ms = now;

            /* Periodic application action every 1000 ms. */
        }

        rtos_sleep_ms(10u);
    }
}
