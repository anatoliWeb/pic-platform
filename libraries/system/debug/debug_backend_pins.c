/*
 * File: libraries/system/debug/debug_backend_pins.c
 *
 * PINS backend for the debug facade. Delegates diagnostic signals to the
 * selected pins transport adapter. Compiled only when DRV_DEBUG_BACKEND_PINS.
 */

#include "libraries/system/debug/debug_backend_pins.h"
#include "libraries/system/debug/debug_pins_gpio.h"

#if DRV_DEBUG_BACKEND_PINS
#if DRV_DEBUG_PINS_INTERFACE_GPIO

void debug_pins_init(void)
{
    debug_gpio_pins_init();
}

void debug_pins_pulse(uint8_t channel)
{
    debug_gpio_pins_pulse(channel);
}

void debug_pins_set(uint8_t channel, uint8_t state)
{
    debug_gpio_pins_set(channel, state);
}

void debug_pins_code(uint8_t value)
{
    debug_gpio_pins_code(value);
}

void debug_pins_error(uint8_t code)
{
    debug_gpio_pins_error(code);
}

#endif /* DRV_DEBUG_PINS_INTERFACE_GPIO */
#endif /* DRV_DEBUG_BACKEND_PINS */
