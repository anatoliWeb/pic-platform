/*
 * File: examples-projects/xc8/debug_pins_gpio.X/main.c
 */

#include "project_config.h"

#include "core/delay.h"
#include "core/debug.h"

void main(void)
{
    debug_init();

    while (1)
    {
        debug_pin_pulse(0u);
        DRV_DELAY_MS(500u);

        debug_pin_set(1u, 1u);
        DRV_DELAY_MS(500u);
        debug_pin_set(1u, 0u);

        debug_pin_code(0x05u);
        DRV_DELAY_MS(1000u);
        debug_pin_code(0x00u);

        debug_pin_error(2u);
        DRV_DELAY_MS(1000u);
    }
}
