/*
 * File: examples-projects/xc8/debug_pins_gpio.X/main.c
 */

#include "project_config.h"

#include "core/delay.h"
#include "core/debug.h"

void main(void)
{
    debug_init();
    debug_pin_code(0x01u);
    debug_pin_error(2u);

    while (1)
    {
        debug_pin_pulse(0u);
        DRV_DELAY_MS(500u);
    }
}
