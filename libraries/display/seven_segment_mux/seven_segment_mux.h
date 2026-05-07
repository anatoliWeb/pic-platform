#ifndef LIBRARIES_DISPLAY_SEVEN_SEGMENT_MUX_H
#define LIBRARIES_DISPLAY_SEVEN_SEGMENT_MUX_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/display/seven_segment/seven_segment.h"

#define SEVEN_SEGMENT_MUX_MAX_DIGITS 8u

#define SEVEN_SEGMENT_MUX_CHAR_MINUS 0xFEu
#define SEVEN_SEGMENT_MUX_CHAR_BLANK 0xFFu

typedef struct
{
    seven_segment_config_t segment_config;

    const seven_segment_pin_t* digit_pins;
    uint8_t digit_count;

    /* Logic level used to enable one digit select line (0 or 1). */
    uint8_t digit_active_level;

    /* Optional default leading-zero suppression for set_number(). */
    uint8_t suppress_leading_zero;
} seven_segment_mux_config_t;

typedef struct
{
    seven_segment_mux_config_t config;

    uint8_t initialized;
    uint8_t current_digit;

    /* Encoded values per digit: 0..15, MINUS, BLANK. */
    uint8_t values[SEVEN_SEGMENT_MUX_MAX_DIGITS];

    /* Optional decimal point bit mask (bit N = digit N dot). */
    uint8_t dot_mask;
} seven_segment_mux_t;

uint8_t seven_segment_mux_init(seven_segment_mux_t* display, const seven_segment_mux_config_t* config);

void seven_segment_mux_set_digit(seven_segment_mux_t* display, uint8_t position, uint8_t value);
void seven_segment_mux_set_number(seven_segment_mux_t* display, int32_t value);
void seven_segment_mux_set_hex(seven_segment_mux_t* display, uint32_t value);

void seven_segment_mux_clear(seven_segment_mux_t* display);

void seven_segment_mux_set_dot(seven_segment_mux_t* display, uint8_t position, uint8_t on);

/* Non-blocking refresh call: updates exactly one digit per call. */
void seven_segment_mux_update(seven_segment_mux_t* display);

#endif /* LIBRARIES_DISPLAY_SEVEN_SEGMENT_MUX_H */