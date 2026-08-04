/*
 * File: libraries/actuator/cooldown_output/cooldown_output.h
 */

#ifndef LIBRARIES_ACTUATOR_COOLDOWN_OUTPUT_H
#define LIBRARIES_ACTUATOR_COOLDOWN_OUTPUT_H

#include "core/compiler.h"
#include "core/types.h"

typedef void (*cooldown_output_callback_t)(void* context, uint8_t active);

typedef struct
{
    uint32_t cooldown_ms;
    cooldown_output_callback_t callback;
    void* callback_context;
} cooldown_output_config_t;

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
