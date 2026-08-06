/*
 * File: libraries/input/digital_input/digital_input.h
 */

#ifndef LIBRARIES_INPUT_DIGITAL_INPUT_H
#define LIBRARIES_INPUT_DIGITAL_INPUT_H

#include "core/compiler.h"
#include "core/types.h"

/*
 * Configuration for a debounced digital input.
 *
 *   debounce_ms           - how long a candidate raw level must persist before
 *                           it becomes the committed stable level. Used as the
 *                           default when activate_debounce_ms and
 *                           release_debounce_ms are both 0.
 *   active_level          - the raw electrical level (0 or 1) that maps to the
 *                           logical "active" state (active-low or active-high).
 *   initial_raw_level     - the electrical level assumed at init; committing it
 *                           does NOT emit an edge event.
 *   latch_active          - when set, an active transition also sets the sticky
 *                           latch until the application clears it.
 *   activate_debounce_ms  - debounce for activation transitions (level becomes
 *                           active). 0 = immediate activation (no debounce).
 *                           When both activate and release are 0, debounce_ms
 *                           is used for both directions (backward compatible).
 *   release_debounce_ms   - debounce for release transitions (level returns to
 *                           inactive). 0 = use debounce_ms as fallback.
 *   immediate_active      - when set, activation is committed on the first
 *                           sample without waiting for the debounce window.
 *                           Release still goes through release_debounce_ms.
 */
typedef struct
{
    uint16_t debounce_ms;
    uint8_t active_level;
    uint8_t initial_raw_level;
    uint8_t latch_active;
    uint16_t activate_debounce_ms;
    uint16_t release_debounce_ms;
    uint8_t immediate_active;
} digital_input_config_t;

/*
 * Caller-owned runtime state.
 *
 *   raw_level         - current candidate level being debounced, not yet final.
 *   stable_raw_level  - the committed, debounced electrical level.
 *   stable_active     - logical state after active-level polarity mapping.
 *   rose_flag/fell_flag - consuming single-consumer edge flags: the first call
 *                       to rose()/fell() returns the flag and clears it.
 *   latched           - sticky active event, held until clear_latch().
 */
typedef struct
{
    digital_input_config_t config;
    uint8_t initialized;
    uint8_t raw_level;
    uint8_t stable_raw_level;
    uint8_t stable_active;
    uint8_t latched;
    uint8_t rose_flag;
    uint8_t fell_flag;
    uint32_t pending_since_ms;
} digital_input_t;

drv_status_t digital_input_init(digital_input_t* input,
                                const digital_input_config_t* config);
void digital_input_update(digital_input_t* input,
                          uint8_t raw_level,
                          uint32_t now_ms);
uint8_t digital_input_is_active(const digital_input_t* input);
uint8_t digital_input_rose(digital_input_t* input);
uint8_t digital_input_fell(digital_input_t* input);
uint8_t digital_input_is_latched(const digital_input_t* input);
void digital_input_clear_latch(digital_input_t* input);

#endif /* LIBRARIES_INPUT_DIGITAL_INPUT_H */
