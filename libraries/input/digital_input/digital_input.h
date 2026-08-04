/*
 * File: libraries/input/digital_input/digital_input.h
 */

#ifndef LIBRARIES_INPUT_DIGITAL_INPUT_H
#define LIBRARIES_INPUT_DIGITAL_INPUT_H

#include "core/compiler.h"
#include "core/types.h"

typedef struct
{
    uint16_t debounce_ms;
    uint8_t active_level;
    uint8_t initial_raw_level;
    uint8_t latch_active;
} digital_input_config_t;

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
