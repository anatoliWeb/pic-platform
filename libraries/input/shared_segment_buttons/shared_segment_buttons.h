/*
 * File: libraries/input/shared_segment_buttons/shared_segment_buttons.h
 */

#ifndef LIBRARIES_INPUT_SHARED_SEGMENT_BUTTONS_H
#define LIBRARIES_INPUT_SHARED_SEGMENT_BUTTONS_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/display/seven_segment_mux/seven_segment_mux.h"

#define SHARED_SEGMENT_BUTTONS_MAX 16u

typedef enum
{
    SHARED_SEG_SEG_A = 0,
    SHARED_SEG_SEG_B,
    SHARED_SEG_SEG_C,
    SHARED_SEG_SEG_D,
    SHARED_SEG_SEG_E,
    SHARED_SEG_SEG_F,
    SHARED_SEG_SEG_G,
    SHARED_SEG_SEG_DOT
} shared_segment_id_t;

typedef struct
{
    uint8_t digit_index;
    shared_segment_id_t segment;
    char key;
} shared_segment_button_map_t;

typedef struct
{
    const seven_segment_mux_t* display;

    const shared_segment_button_map_t* map;
    uint8_t button_count;

    uint8_t active_level;
    uint8_t debounce_scans;
} shared_segment_buttons_config_t;

typedef struct
{
    shared_segment_buttons_config_t config;

    uint8_t initialized;
    uint8_t scan_index;

    uint8_t stable_state[SHARED_SEGMENT_BUTTONS_MAX];
    uint8_t debounce_count[SHARED_SEGMENT_BUTTONS_MAX];

    char last_key;
} shared_segment_buttons_t;

uint8_t shared_segment_buttons_init(shared_segment_buttons_t* ctx,
                                    const shared_segment_buttons_config_t* config);

void shared_segment_buttons_update(shared_segment_buttons_t* ctx);

uint8_t shared_segment_buttons_is_pressed(const shared_segment_buttons_t* ctx, uint8_t button_index);

char shared_segment_buttons_get_key(shared_segment_buttons_t* ctx);

#endif /* LIBRARIES_INPUT_SHARED_SEGMENT_BUTTONS_H */