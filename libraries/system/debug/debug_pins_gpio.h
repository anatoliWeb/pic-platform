/*
 * File: libraries/system/debug/debug_pins_gpio.h
 *
 * Direct GPIO pins transport for the PINS backend. Each channel is driven
 * on a contiguous range of bits of one port (DRV_DEBUG_PINS_PORT/TRIS).
 * Compiled only when DRV_DEBUG_PINS_INTERFACE_GPIO is 1.
 */

#ifndef LIBRARIES_SYSTEM_DEBUG_DEBUG_PINS_GPIO_H
#define LIBRARIES_SYSTEM_DEBUG_DEBUG_PINS_GPIO_H

#include "core/compiler.h"
#include "core/types.h"
#include "core/config.h"

#if DRV_DEBUG_BACKEND_PINS && DRV_DEBUG_PINS_INTERFACE_GPIO

void debug_gpio_pins_init(void);
void debug_gpio_pins_pulse(uint8_t channel);
void debug_gpio_pins_set(uint8_t channel, uint8_t state);
void debug_gpio_pins_code(uint8_t value);
void debug_gpio_pins_error(uint8_t code);

#endif /* DRV_DEBUG_BACKEND_PINS && GPIO */

#endif /* LIBRARIES_SYSTEM_DEBUG_DEBUG_PINS_GPIO_H */