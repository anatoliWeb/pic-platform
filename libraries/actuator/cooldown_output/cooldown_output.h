/*
 * File: libraries/actuator/cooldown_output/cooldown_output.h
 */

#ifndef LIBRARIES_ACTUATOR_COOLDOWN_OUTPUT_H
#define LIBRARIES_ACTUATOR_COOLDOWN_OUTPUT_H

#include "core/compiler.h"
#include "core/types.h"

/*
 * Optional callback fired only when the physical active state actually changes
 * (on -> off or off -> on). It reports the physical active state, not the
 * caller's latest request, so the application can drive the real output.
 * Arguments: (application context, active 0/1).
 */
typedef void (*cooldown_output_callback_t)(void* context, uint8_t active);

/*
 * Configuration for the cooldown output.
 *
 *   cooldown_ms      - how long the output stays active after the last request
 *                      goes OFF before it physically turns off. The value must
 *                      stay below 2^31 ms so the wrap-safe deadline comparison
 *                      in the implementation is valid. 0 means immediate off.
 *   callback         - optional hardware adapter; NULL disables notifications.
 *   callback_context - opaque pointer passed back unchanged to the callback.
 */
typedef struct
{
    uint32_t cooldown_ms;
    cooldown_output_callback_t callback;
    void* callback_context;
} cooldown_output_config_t;

/*
 * Caller-owned runtime state.
 *
 *   requested       - the logical caller command (on/off), independent of the
 *                     physical output.
 *   active          - the physical output state (on/off).
 *   cooling_down    - request is OFF, but the output is still on and waiting
 *                     out the cooldown before it turns off.
 *   cooldown_end_ms - monotonic deadline for the current cooldown.
 */
typedef struct
{
    cooldown_output_config_t config;
    uint8_t initialized;
    uint8_t requested;
    uint8_t active;
    uint8_t cooling_down;
    uint32_t cooldown_end_ms;
} cooldown_output_t;

drv_status_t cooldown_output_init(cooldown_output_t* output,
                                  const cooldown_output_config_t* config);
void cooldown_output_set_requested(cooldown_output_t* output,
                                   uint8_t requested,
                                   uint32_t now_ms);
void cooldown_output_process(cooldown_output_t* output, uint32_t now_ms);
uint8_t cooldown_output_is_active(const cooldown_output_t* output);
uint8_t cooldown_output_is_cooling_down(const cooldown_output_t* output);
uint32_t cooldown_output_get_remaining_ms(const cooldown_output_t* output,
                                          uint32_t now_ms);
void cooldown_output_force_off(cooldown_output_t* output);

#endif /* LIBRARIES_ACTUATOR_COOLDOWN_OUTPUT_H */
