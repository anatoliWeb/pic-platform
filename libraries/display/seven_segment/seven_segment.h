/*
 * File: libraries/display/seven_segment/seven_segment.h
 */

#ifndef LIBRARIES_DISPLAY_SEVEN_SEGMENT_H
#define LIBRARIES_DISPLAY_SEVEN_SEGMENT_H

#include "core/compiler.h"
#include "core/types.h"

#define SEVEN_SEGMENT_MAX_DIGITS        8u
#define SEVEN_SEGMENT_DEFAULT_BLINK_MS  500u

#define SEVEN_SEGMENT_MASK_A            0x01u
#define SEVEN_SEGMENT_MASK_B            0x02u
#define SEVEN_SEGMENT_MASK_C            0x04u
#define SEVEN_SEGMENT_MASK_D            0x08u
#define SEVEN_SEGMENT_MASK_E            0x10u
#define SEVEN_SEGMENT_MASK_F            0x20u
#define SEVEN_SEGMENT_MASK_G            0x40u
#define SEVEN_SEGMENT_MASK_DP           0x80u
#define SEVEN_SEGMENT_ALL_SEGMENTS_MASK 0x7Fu

#define SEVEN_SEGMENT_SYMBOL_MINUS      0xFEu
#define SEVEN_SEGMENT_SYMBOL_BLANK      0xFFu

typedef enum
{
    SEVEN_SEGMENT_COMMON_CATHODE = 0,
    SEVEN_SEGMENT_COMMON_ANODE
} seven_segment_type_t;

typedef enum
{
    SEVEN_SEGMENT_ALIGN_LEFT = 0,
    SEVEN_SEGMENT_ALIGN_RIGHT
} seven_segment_alignment_t;

typedef enum
{
    SEVEN_SEGMENT_REFRESH_MANUAL = 0,
    SEVEN_SEGMENT_REFRESH_TIMER
} seven_segment_refresh_mode_t;

typedef enum
{
    SEVEN_SEGMENT_TIMER_NONE = 0,
    SEVEN_SEGMENT_TIMER0,
    SEVEN_SEGMENT_TIMER1,
    SEVEN_SEGMENT_TIMER2,
    SEVEN_SEGMENT_TIMER3
} seven_segment_timer_t;

typedef enum
{
    SEVEN_SEGMENT_INIT_ERROR_NONE = 0,
    SEVEN_SEGMENT_INIT_ERROR_ARGUMENT,
    SEVEN_SEGMENT_INIT_ERROR_TIMER_NONE,
    SEVEN_SEGMENT_INIT_ERROR_PERIOD_ZERO,
    SEVEN_SEGMENT_INIT_ERROR_TIMER_BACKEND_DISABLED,
    SEVEN_SEGMENT_INIT_ERROR_TIMER_ALREADY_OWNED,
    SEVEN_SEGMENT_INIT_ERROR_TIMER_CALLBACK_BUSY,
    SEVEN_SEGMENT_INIT_ERROR_TIMER_PERIOD_UNSUPPORTED,
    SEVEN_SEGMENT_INIT_ERROR_TIMER_START_FAILED
} seven_segment_init_error_t;

typedef struct
{
    volatile uint8_t* port;
    volatile uint8_t* tris;
    uint8_t bit;
} seven_segment_pin_t;

typedef struct
{
    uint8_t start_digit;
    uint8_t width;
    seven_segment_alignment_t alignment;
    uint8_t leading_zero;
    uint8_t clear_unused;
} seven_segment_number_format_t;

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
    const seven_segment_pin_t* digit_pins;
    uint8_t digit_count;
    uint8_t digit_active_level;
    const uint8_t* custom_symbol_table;
    uint8_t custom_symbol_count;

    seven_segment_type_t type;
    seven_segment_refresh_mode_t refresh_mode;
    seven_segment_timer_t timer;
    uint16_t refresh_period_us;
} seven_segment_config_t;

typedef struct
{
    seven_segment_config_t config;
    uint8_t initialized;
    uint8_t current_digit;
    uint8_t brightness_percent;
    uint8_t brightness_phase;
    uint8_t raw_patterns[SEVEN_SEGMENT_MAX_DIGITS];
    uint8_t dot_mask;
    uint16_t blink_mask;
    uint16_t blink_period_ms;
    uint32_t blink_last_toggle_ms;
    uint8_t blink_enabled;
    uint8_t blink_phase_visible;
    uint8_t timer_owned_refresh;
    uint16_t timer_reload_value;
    uint16_t timer_tick_us;
    uint32_t timer_blink_us_accumulator;
    uint32_t timer_blink_now_ms;
    volatile uint32_t refresh_count;
    seven_segment_init_error_t init_error;
} seven_segment_t;

uint8_t seven_segment_init(seven_segment_t* display, const seven_segment_config_t* config);

void seven_segment_set_digit(seven_segment_t* display, uint8_t digit);
void seven_segment_set_digit_at(seven_segment_t* display, uint8_t position, uint8_t digit);
void seven_segment_set_hex(seven_segment_t* display, uint8_t value);
void seven_segment_set_symbol_at(seven_segment_t* display, uint8_t position, uint8_t symbol);
void seven_segment_set_raw(seven_segment_t* display, uint8_t pattern);
void seven_segment_set_raw_at(seven_segment_t* display, uint8_t position, uint8_t pattern);
void seven_segment_set_number(seven_segment_t* display, int32_t value);
drv_status_t seven_segment_set_number_formatted(seven_segment_t* display,
                                                int32_t value,
                                                const seven_segment_number_format_t* format);
drv_status_t seven_segment_set_fixed(seven_segment_t* display,
                                     int32_t scaled_value,
                                     uint8_t decimal_places);
drv_status_t seven_segment_set_fixed_formatted(seven_segment_t* display,
                                               int32_t scaled_value,
                                               uint8_t decimal_places,
                                               const seven_segment_number_format_t* format);
drv_status_t seven_segment_set_time_hhmm(seven_segment_t* display,
                                         uint8_t start_digit,
                                         uint8_t hours,
                                         uint8_t minutes,
                                         uint8_t leading_zero_hours);
void seven_segment_clear(seven_segment_t* display);
void seven_segment_set_dot(seven_segment_t* display, uint8_t on);
void seven_segment_set_dot_at(seven_segment_t* display, uint8_t position, uint8_t on);
void seven_segment_set_brightness(seven_segment_t* display, uint8_t percent);
void seven_segment_set_blink_mask(seven_segment_t* display, uint16_t digit_mask);
void seven_segment_set_blink_period_ms(seven_segment_t* display, uint16_t period_ms);
void seven_segment_set_blink_enabled(seven_segment_t* display, uint8_t enabled);
void seven_segment_blink_update(seven_segment_t* display, uint32_t now_ms);
void seven_segment_refresh(seven_segment_t* display);
void seven_segment_process(seven_segment_t* display);
uint8_t seven_segment_is_timer_running(const seven_segment_t* display);
uint32_t seven_segment_get_refresh_count(const seven_segment_t* display);
seven_segment_init_error_t seven_segment_get_init_error(const seven_segment_t* display);
void seven_segment_stop(seven_segment_t* display);
void seven_segment_deinit(seven_segment_t* display);
void seven_segment_irq_handler(void);

#endif /* LIBRARIES_DISPLAY_SEVEN_SEGMENT_H */
