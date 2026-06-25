#ifndef LIBRARIES_INPUT_SEGMENT_KEYS_H
#define LIBRARIES_INPUT_SEGMENT_KEYS_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/display/seven_segment/seven_segment.h"
#include "libraries/input/button/button.h"

#define SEGMENT_KEYS_MAX_KEYS 8u

typedef struct
{
    uint8_t segment_mask;
    char key;
    button_t* button;
} segment_keys_entry_t;

typedef struct
{
    seven_segment_t* display;
    const segment_keys_entry_t* keys;
    uint8_t key_count;
    uint8_t active_level;
} segment_keys_config_t;

typedef struct
{
    segment_keys_config_t config;
    uint8_t initialized;
    uint8_t raw_mask;
    uint8_t unknown_state;
    uint8_t pressed_state[SEGMENT_KEYS_MAX_KEYS];
    char last_key;
} segment_keys_t;

uint8_t segment_keys_init(segment_keys_t* ctx,
                          const segment_keys_config_t* config);
void segment_keys_update(segment_keys_t* ctx);
uint8_t segment_keys_is_pressed(const segment_keys_t* ctx, uint8_t key_index);
uint8_t segment_keys_get_raw_mask(const segment_keys_t* ctx);
uint8_t segment_keys_is_unknown(const segment_keys_t* ctx);
char segment_keys_get_key(segment_keys_t* ctx);

#endif /* LIBRARIES_INPUT_SEGMENT_KEYS_H */
