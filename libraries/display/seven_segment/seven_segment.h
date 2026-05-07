#ifndef LIBRARIES_DISPLAY_SEVEN_SEGMENT_H
#define LIBRARIES_DISPLAY_SEVEN_SEGMENT_H

#include "core/compiler.h"
#include "core/types.h"

typedef enum
{
    SEVEN_SEGMENT_COMMON_CATHODE = 0,
    SEVEN_SEGMENT_COMMON_ANODE
} seven_segment_type_t;

typedef struct
{
    volatile uint8_t* port;
    volatile uint8_t* tris;
    uint8_t bit;
} seven_segment_pin_t;

typedef struct
{
    seven_segment_pin_t seg_a;
    seven_segment_pin_t seg_b;
    seven_segment_pin_t seg_c;
    seven_segment_pin_t seg_d;
    seven_segment_pin_t seg_e;
    seven_segment_pin_t seg_f;
    seven_segment_pin_t seg_g;

    seven_segment_pin_t dot;
    uint8_t has_dot;

    seven_segment_type_t type;
} seven_segment_config_t;

typedef struct
{
    seven_segment_config_t config;
    uint8_t initialized;
} seven_segment_t;

uint8_t seven_segment_init(seven_segment_t* display, const seven_segment_config_t* config);

void seven_segment_set_digit(seven_segment_t* display, uint8_t digit);
void seven_segment_set_hex(seven_segment_t* display, uint8_t value);
void seven_segment_set_raw(seven_segment_t* display, uint8_t pattern);
void seven_segment_clear(seven_segment_t* display);
void seven_segment_set_dot(seven_segment_t* display, uint8_t on);

#endif /* LIBRARIES_DISPLAY_SEVEN_SEGMENT_H */