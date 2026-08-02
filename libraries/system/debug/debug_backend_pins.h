/*
 * File: libraries/system/debug/debug_backend_pins.h
 *
 * PINS backend for the debug facade. Routes diagnostic signals to GPIO
 * outputs through the pins transport adapter.
 * Compiled only when DRV_DEBUG_BACKEND_PINS is 1.
 * Internal header including the debug module.
 */

#ifndef LIBRARIES_SYSTEM_DEBUG_DEBUG_BACKEND_PINS_H
#define LIBRARIES_SYSTEM_DEBUG_DEBUG_BACKEND_PINS_H

#include "core/compiler.h"
#include "core/types.h"
#include "core/config.h"

#if DRV_DEBUG_BACKEND_PINS

void debug_pins_init(void);
void debug_pins_pulse(uint8_t channel);
void debug_pins_set(uint8_t channel, uint8_t state);
void debug_pins_code(uint8_t value);
void debug_pins_error(uint8_t code);

#endif /* DRV_DEBUG_BACKEND_PINS */

#endif /* LIBRARIES_SYSTEM_DEBUG_DEBUG_BACKEND_PINS_H */