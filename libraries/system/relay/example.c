/*
 * File: libraries/system/relay/example.c
 */

#include "core/compiler.h"
#include "core/debug.h"
#include "drivers/timers/tick/tick.h"
#include "libraries/system/relay/relay.h"

void main(void)
{
    relay_t relay_main;
    relay_t relay_active_low;
    uint32_t now;
    uint32_t last_print;

    tick_init();

    /* Active-HIGH relay on RB0, safe startup OFF. */
    (void)relay_init(&relay_main, &PORTB, &TRISB, 0u, 0u, 1u);

    /* Active-LOW relay on RB1, safe startup OFF. */
    (void)relay_init(&relay_active_low, &PORTB, &TRISB, 1u, 1u, 1u);

    relay_on(&relay_main);
    relay_toggle(&relay_main);

    now = tick_get();
    relay_pulse(&relay_main, 500u, now);
    relay_delayed_off(&relay_active_low, 2000u, now);

    last_print = 0u;

    while (1)
    {
        now = tick_get();

        relay_update(&relay_main, now);
        relay_update(&relay_active_low, now);

        if ((now - last_print) >= 1000u)
        {
            last_print = now;
            DBG_PRINT("Relay main state: ");
            DBG_PRINT_INT((int32_t)relay_get_state(&relay_main));
            DBG_PRINT(" | active-low state: ");
            DBG_PRINT_INT((int32_t)relay_get_state(&relay_active_low));
            DBG_PRINTLN("");
        }
    }
}
