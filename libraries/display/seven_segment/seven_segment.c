/*
 * File: libraries/display/seven_segment/seven_segment.c
 */

#include "core/pic_platform_config.h"
#include "libraries/display/seven_segment/seven_segment.h"
#include "core/device.h"
#include "drivers/gpio/gpio.h"
#include "drivers/timers/timer0/timer0.h"
#include "drivers/timers/timer1/timer1.h"
#include "drivers/timers/timer2/timer2.h"
#include "drivers/timers/timer3/timer3.h"

/*
 * Timer backends are selected at compile time.
 *
 * Default: all timer backends are disabled.
 * In that case the library remains fully usable in manual refresh mode.
 *
 * Enable only the timer driver that is actually added to the project,
 * for example:
 *
 *   #define SEVEN_SEGMENT_ENABLE_TIMER2 1
 *
 * The runtime configuration passed to seven_segment_init() then selects
 * whether manual refresh or that compiled timer backend is used.
 */
typedef void (*seven_segment_timer_callback_t)(void);

typedef struct
{
    seven_segment_t* owner;
    seven_segment_timer_callback_t callback;
} seven_segment_timer_slot_t;

static seven_segment_timer_slot_t g_timer0_slot = { (seven_segment_t*)0, (seven_segment_timer_callback_t)0 };
static seven_segment_timer_slot_t g_timer1_slot = { (seven_segment_t*)0, (seven_segment_timer_callback_t)0 };
static seven_segment_timer_slot_t g_timer2_slot = { (seven_segment_t*)0, (seven_segment_timer_callback_t)0 };
static seven_segment_timer_slot_t g_timer3_slot = { (seven_segment_t*)0, (seven_segment_timer_callback_t)0 };

static void seven_segment_blink_update_core(seven_segment_t* display, uint32_t now_ms);

static const uint8_t g_seven_segment_digits[10] =
{
    0x3Fu, /* 0 */
    0x06u, /* 1 */
    0x5Bu, /* 2 */
    0x4Fu, /* 3 */
    0x66u, /* 4 */
    0x6Du, /* 5 */
    0x7Du, /* 6 */
    0x07u, /* 7 */
    0x7Fu, /* 8 */
    0x6Fu  /* 9 */
};

static void seven_segment_write_level(const seven_segment_pin_t* pin,
                                      uint8_t on,
                                      seven_segment_type_t type)
{
    uint8_t level;

    if ((pin == (const seven_segment_pin_t*)0) ||
        (pin->port == (volatile uint8_t*)0) ||
        (pin->tris == (volatile uint8_t*)0))
    {
        return;
    }

    level = on;

    if (type == SEVEN_SEGMENT_COMMON_ANODE)
    {
        level = (uint8_t)(on == 0u ? 1u : 0u);
    }

    if (level != 0u)
    {
        gpio_write_high(pin->port, pin->bit);
    }
    else
    {
        gpio_write_low(pin->port, pin->bit);
    }
}

static uint8_t seven_segment_get_digit_count(const seven_segment_t* display)
{
    if (display == (const seven_segment_t*)0)
    {
        return 0u;
    }

    return display->config.digit_count;
}

static uint16_t seven_segment_get_valid_blink_mask(const seven_segment_t* display,
                                                   uint16_t digit_mask)
{
    uint16_t valid_mask;
    uint8_t i;

    valid_mask = 0u;

    if (display == (const seven_segment_t*)0)
    {
        return 0u;
    }

    for (i = 0u; i < display->config.digit_count; i++)
    {
        valid_mask = (uint16_t)(valid_mask | ((uint16_t)1u << i));
    }

    return (uint16_t)(digit_mask & valid_mask);
}

static void seven_segment_digit_line_set(const seven_segment_t* display,
                                         uint8_t index,
                                         uint8_t enable)
{
    uint8_t level;

    if ((display == (const seven_segment_t*)0) ||
        (display->config.digit_pins == (const seven_segment_pin_t*)0) ||
        (index >= display->config.digit_count))
    {
        return;
    }

    level = (enable != 0u) ?
        display->config.digit_active_level :
        (uint8_t)(display->config.digit_active_level == 0u ? 1u : 0u);

    if (level != 0u)
    {
        gpio_write_high(display->config.digit_pins[index].port,
                        display->config.digit_pins[index].bit);
    }
    else
    {
        gpio_write_low(display->config.digit_pins[index].port,
                       display->config.digit_pins[index].bit);
    }
}

static void seven_segment_disable_all_digits(const seven_segment_t* display)
{
    uint8_t i;

    if (display == (const seven_segment_t*)0)
    {
        return;
    }

    for (i = 0u; i < display->config.digit_count; i++)
    {
        seven_segment_digit_line_set(display, i, 0u);
    }
}

static void seven_segment_apply_pattern(const seven_segment_t* display,
                                        uint8_t pattern,
                                        uint8_t dot_on)
{
    seven_segment_write_level(&display->config.seg_a,
                              (uint8_t)((pattern & SEVEN_SEGMENT_MASK_A) != 0u),
                              display->config.type);
    seven_segment_write_level(&display->config.seg_b,
                              (uint8_t)((pattern & SEVEN_SEGMENT_MASK_B) != 0u),
                              display->config.type);
    seven_segment_write_level(&display->config.seg_c,
                              (uint8_t)((pattern & SEVEN_SEGMENT_MASK_C) != 0u),
                              display->config.type);
    seven_segment_write_level(&display->config.seg_d,
                              (uint8_t)((pattern & SEVEN_SEGMENT_MASK_D) != 0u),
                              display->config.type);
    seven_segment_write_level(&display->config.seg_e,
                              (uint8_t)((pattern & SEVEN_SEGMENT_MASK_E) != 0u),
                              display->config.type);
    seven_segment_write_level(&display->config.seg_f,
                              (uint8_t)((pattern & SEVEN_SEGMENT_MASK_F) != 0u),
                              display->config.type);
    seven_segment_write_level(&display->config.seg_g,
                              (uint8_t)((pattern & SEVEN_SEGMENT_MASK_G) != 0u),
                              display->config.type);

    if (display->config.has_dot != 0u)
    {
        seven_segment_write_level(&display->config.dot, dot_on, display->config.type);
    }
}

static uint8_t seven_segment_encode_symbol(const seven_segment_t* display,
                                           uint8_t symbol)
{
    if (symbol <= 9u)
    {
        return g_seven_segment_digits[symbol];
    }

    if (symbol == SEVEN_SEGMENT_SYMBOL_MINUS)
    {
        return SEVEN_SEGMENT_MASK_G;
    }

    if (symbol == SEVEN_SEGMENT_SYMBOL_BLANK)
    {
        return 0u;
    }

    if ((symbol >= 10u) &&
        (display->config.custom_symbol_table != (const uint8_t*)0))
    {
        uint8_t index;

        index = (uint8_t)(symbol - 10u);
        if (index < display->config.custom_symbol_count)
        {
            return (uint8_t)(display->config.custom_symbol_table[index] &
                             SEVEN_SEGMENT_ALL_SEGMENTS_MASK);
        }
    }

    return 0u;
}

static drv_status_t seven_segment_validate_field(const seven_segment_t* display,
                                                 uint8_t start_digit,
                                                 uint8_t width)
{
    if ((display == (const seven_segment_t*)0) ||
        (display->initialized == 0u) ||
        (width == 0u) ||
        (start_digit >= display->config.digit_count) ||
        ((uint16_t)start_digit + (uint16_t)width > (uint16_t)display->config.digit_count))
    {
        return DRV_STATUS_ERROR;
    }

    return DRV_STATUS_OK;
}

static void seven_segment_commit_buffer(seven_segment_t* display,
                                        const uint8_t* patterns,
                                        uint8_t dot_mask)
{
    uint8_t i;
    uint8_t global_interrupt_state;

    if ((display == (seven_segment_t*)0) ||
        (patterns == (const uint8_t*)0))
    {
        return;
    }

    global_interrupt_state = INTCONbits.GIE;

    DRV_INT_DISABLE();

    for (i = 0u; i < display->config.digit_count; i++)
    {
        display->raw_patterns[i] = patterns[i];
    }

    display->dot_mask = dot_mask;

    if (global_interrupt_state != 0u)
    {
        DRV_INT_ENABLE();
    }
}

static void seven_segment_blink_advance_ms(seven_segment_t* display,
                                           uint32_t elapsed_ms)
{
    if ((display == (seven_segment_t*)0) || (display->initialized == 0u))
    {
        return;
    }

    seven_segment_blink_update_core(display, (uint32_t)(display->timer_blink_now_ms + elapsed_ms));
    display->timer_blink_now_ms = (uint32_t)(display->timer_blink_now_ms + elapsed_ms);
}

static uint8_t seven_segment_get_magnitude_digits(uint32_t value)
{
    uint8_t digits;

    digits = 1u;

    while (value >= 10u)
    {
        value /= 10u;
        digits++;
    }

    return digits;
}

static drv_status_t seven_segment_prepare_number_field(int32_t value,
                                                       const seven_segment_number_format_t* format,
                                                       uint8_t* field_symbols,
                                                       uint8_t* used_positions)
{
    uint32_t magnitude;
    uint8_t digits[10];
    uint8_t digit_count;
    uint8_t total_symbols;
    uint8_t negative;
    uint8_t i;
    uint8_t pos;

    if ((format == (const seven_segment_number_format_t*)0) ||
        (field_symbols == (uint8_t*)0) ||
        (used_positions == (uint8_t*)0) ||
        (format->width == 0u))
    {
        return DRV_STATUS_ERROR;
    }

    negative = (uint8_t)(value < 0 ? 1u : 0u);
    if (value < 0)
    {
        /* Avoid signed overflow for the most-negative 32-bit value. */
        magnitude = (uint32_t)(-(value + 1));
        magnitude++;
    }
    else
    {
        magnitude = (uint32_t)value;
    }
    digit_count = seven_segment_get_magnitude_digits(magnitude);
    total_symbols = (uint8_t)(digit_count + negative);

    if (total_symbols > format->width)
    {
        return DRV_STATUS_ERROR;
    }

    for (i = 0u; i < format->width; i++)
    {
        field_symbols[i] = SEVEN_SEGMENT_SYMBOL_BLANK;
        used_positions[i] = 0u;
    }

    for (i = 0u; i < digit_count; i++)
    {
        digits[i] = (uint8_t)(magnitude % 10u);
        magnitude /= 10u;
    }

    if (format->alignment == SEVEN_SEGMENT_ALIGN_LEFT)
    {
        pos = 0u;

        if (negative != 0u)
        {
            field_symbols[pos] = SEVEN_SEGMENT_SYMBOL_MINUS;
            used_positions[pos] = 1u;
            pos++;
        }

        for (i = 0u; i < digit_count; i++)
        {
            field_symbols[pos] = digits[(uint8_t)(digit_count - 1u - i)];
            used_positions[pos] = 1u;
            pos++;
        }

        return DRV_STATUS_OK;
    }

    pos = format->width;
    for (i = 0u; i < digit_count; i++)
    {
        pos--;
        field_symbols[pos] = digits[i];
        used_positions[pos] = 1u;
    }

    if (negative != 0u)
    {
        if (format->leading_zero != 0u)
        {
            field_symbols[0] = SEVEN_SEGMENT_SYMBOL_MINUS;
            used_positions[0] = 1u;

            for (i = 1u; i < pos; i++)
            {
                field_symbols[i] = 0u;
                used_positions[i] = 1u;
            }
        }
        else
        {
            pos--;
            field_symbols[pos] = SEVEN_SEGMENT_SYMBOL_MINUS;
            used_positions[pos] = 1u;
        }
    }
    else if (format->leading_zero != 0u)
    {
        for (i = 0u; i < pos; i++)
        {
            field_symbols[i] = 0u;
            used_positions[i] = 1u;
        }
    }

    return DRV_STATUS_OK;
}

static drv_status_t seven_segment_prepare_fixed_field(int32_t scaled_value,
                                                      uint8_t decimal_places,
                                                      const seven_segment_number_format_t* format,
                                                      uint8_t* field_symbols,
                                                      uint8_t* used_positions,
                                                      uint8_t* dot_position)
{
    uint32_t magnitude;
    uint8_t digits[10];
    uint8_t sequence[10];
    uint8_t digit_count;
    uint8_t required_digits;
    uint8_t total_symbols;
    uint8_t negative;
    uint8_t pad_count;
    uint8_t seq_pos;
    uint8_t i;

    if ((format == (const seven_segment_number_format_t*)0) ||
        (field_symbols == (uint8_t*)0) ||
        (used_positions == (uint8_t*)0) ||
        (dot_position == (uint8_t*)0) ||
        (format->width == 0u))
    {
        return DRV_STATUS_ERROR;
    }

    negative = (uint8_t)(scaled_value < 0 ? 1u : 0u);
    if (scaled_value < 0)
    {
        /* Avoid signed overflow for the most-negative 32-bit value. */
        magnitude = (uint32_t)(-(scaled_value + 1));
        magnitude++;
    }
    else
    {
        magnitude = (uint32_t)scaled_value;
    }

    digit_count = seven_segment_get_magnitude_digits(magnitude);
    required_digits = digit_count;
    if ((uint16_t)decimal_places + 1u > required_digits)
    {
        required_digits = (uint8_t)((uint16_t)decimal_places + 1u);
    }

    total_symbols = (uint8_t)(required_digits + negative);
    if ((required_digits > 9u) || (total_symbols > format->width))
    {
        return DRV_STATUS_ERROR;
    }

    for (i = 0u; i < format->width; i++)
    {
        field_symbols[i] = SEVEN_SEGMENT_SYMBOL_BLANK;
        used_positions[i] = 0u;
    }

    for (i = 0u; i < digit_count; i++)
    {
        digits[i] = (uint8_t)(magnitude % 10u);
        magnitude /= 10u;
    }

    seq_pos = 0u;
    if (negative != 0u)
    {
        sequence[seq_pos] = SEVEN_SEGMENT_SYMBOL_MINUS;
        seq_pos++;
    }

    pad_count = (uint8_t)(required_digits - digit_count);
    for (i = 0u; i < pad_count; i++)
    {
        sequence[seq_pos] = 0u;
        seq_pos++;
    }

    for (i = 0u; i < digit_count; i++)
    {
        sequence[seq_pos] = digits[(uint8_t)(digit_count - 1u - i)];
        seq_pos++;
    }

    *dot_position = 0xFFu;
    if (decimal_places != 0u)
    {
        *dot_position = (uint8_t)(negative + required_digits - decimal_places - 1u);
    }

    if (format->alignment == SEVEN_SEGMENT_ALIGN_LEFT)
    {
        for (i = 0u; i < total_symbols; i++)
        {
            field_symbols[i] = sequence[i];
            used_positions[i] = 1u;
        }

        return DRV_STATUS_OK;
    }

    pad_count = (uint8_t)(format->width - total_symbols);
    if ((format->leading_zero != 0u) && (pad_count != 0u))
    {
        if (negative != 0u)
        {
            field_symbols[0] = SEVEN_SEGMENT_SYMBOL_MINUS;
            used_positions[0] = 1u;

            for (i = 0u; i < pad_count; i++)
            {
                field_symbols[(uint8_t)(1u + i)] = 0u;
                used_positions[(uint8_t)(1u + i)] = 1u;
            }

            for (i = 1u; i < total_symbols; i++)
            {
                field_symbols[(uint8_t)(pad_count + i)] = sequence[i];
                used_positions[(uint8_t)(pad_count + i)] = 1u;
            }
        }
        else
        {
            for (i = 0u; i < pad_count; i++)
            {
                field_symbols[i] = 0u;
                used_positions[i] = 1u;
            }

            for (i = 0u; i < total_symbols; i++)
            {
                field_symbols[(uint8_t)(pad_count + i)] = sequence[i];
                used_positions[(uint8_t)(pad_count + i)] = 1u;
            }
        }

        if (*dot_position != 0xFFu)
        {
            *dot_position = (uint8_t)(*dot_position + pad_count);
        }

        return DRV_STATUS_OK;
    }

    for (i = 0u; i < total_symbols; i++)
    {
        field_symbols[(uint8_t)(pad_count + i)] = sequence[i];
        used_positions[(uint8_t)(pad_count + i)] = 1u;
    }

    if (*dot_position != 0xFFu)
    {
        *dot_position = (uint8_t)(*dot_position + pad_count);
    }

    return DRV_STATUS_OK;
}

static void seven_segment_refresh_core(seven_segment_t* display)
{
    uint8_t digit_index;
    uint8_t dot_on;
    uint16_t digit_mask;

    if ((display == (seven_segment_t*)0) || (display->initialized == 0u))
    {
        return;
    }

    if (seven_segment_get_digit_count(display) == 0u)
    {
        return;
    }

    digit_index = display->current_digit;
    if (digit_index >= display->config.digit_count)
    {
        digit_index = 0u;
    }

    seven_segment_disable_all_digits(display);
    seven_segment_apply_pattern(display, 0u, 0u);

    if (display->brightness_phase >= 99u)
    {
        display->brightness_phase = 0u;
    }
    else
    {
        display->brightness_phase++;
    }

    digit_mask = (uint16_t)((uint16_t)1u << digit_index);

    if ((display->brightness_percent != 0u) &&
        (display->brightness_phase < display->brightness_percent) &&
        ((display->blink_enabled == 0u) ||
         (display->blink_phase_visible != 0u) ||
         ((display->blink_mask & digit_mask) == 0u)))
    {
        dot_on = (uint8_t)(((display->dot_mask >> digit_index) & 0x01u) != 0u ? 1u : 0u);
        seven_segment_apply_pattern(display, display->raw_patterns[digit_index], dot_on);
        seven_segment_digit_line_set(display, digit_index, 1u);
    }

    digit_index++;
    if (digit_index >= display->config.digit_count)
    {
        digit_index = 0u;
    }
    display->current_digit = digit_index;
}

static void seven_segment_timer_tick(seven_segment_t* display)
{
    uint32_t elapsed_ms;

    if ((display == (seven_segment_t*)0) || (display->initialized == 0u))
    {
        return;
    }

    seven_segment_refresh_core(display);
    display->refresh_count++;

    display->timer_blink_us_accumulator =
        (uint32_t)(display->timer_blink_us_accumulator + display->timer_tick_us);

    elapsed_ms = 0u;
    while (display->timer_blink_us_accumulator >= 1000u)
    {
        display->timer_blink_us_accumulator =
            (uint32_t)(display->timer_blink_us_accumulator - 1000u);
        elapsed_ms++;
    }

    if (elapsed_ms != 0u)
    {
        seven_segment_blink_advance_ms(display, elapsed_ms);
    }
}

#if SEVEN_SEGMENT_ENABLE_TIMER0
static void seven_segment_timer0_callback(void)
{
    if (g_timer0_slot.owner != (seven_segment_t*)0)
    {
        timer0_set(g_timer0_slot.owner->timer_reload_value);
        seven_segment_timer_tick(g_timer0_slot.owner);
    }
}
#endif

#if SEVEN_SEGMENT_ENABLE_TIMER1
static void seven_segment_timer1_callback(void)
{
    if (g_timer1_slot.owner != (seven_segment_t*)0)
    {
        timer1_set(g_timer1_slot.owner->timer_reload_value);
        seven_segment_timer_tick(g_timer1_slot.owner);
    }
}
#endif

#if SEVEN_SEGMENT_ENABLE_TIMER2
static void seven_segment_timer2_callback(void)
{
    if (g_timer2_slot.owner != (seven_segment_t*)0)
    {
        seven_segment_timer_tick(g_timer2_slot.owner);
    }
}
#endif

#if SEVEN_SEGMENT_ENABLE_TIMER3
static void seven_segment_timer3_callback(void)
{
    if (g_timer3_slot.owner != (seven_segment_t*)0)
    {
        timer3_set(g_timer3_slot.owner->timer_reload_value);
        seven_segment_timer_tick(g_timer3_slot.owner);
    }
}
#endif

static uint16_t seven_segment_compute_period_counts(uint16_t prescaler,
                                                    uint16_t period_us)
{
    uint32_t counts;

    if ((prescaler == 0u) || (period_us == 0u))
    {
        return 0u;
    }

    counts = ((uint32_t)DRV_XTAL_FREQ / 4u);
    counts = (counts * (uint32_t)period_us) / ((uint32_t)prescaler * 1000000ul);

    if (counts == 0u)
    {
        counts = 1u;
    }

    if (counts > 65535u)
    {
        return 0u;
    }

    return (uint16_t)counts;
}

static uint8_t seven_segment_timer16_prepare(seven_segment_t* display,
                                             const uint16_t* prescalers,
                                             uint8_t prescaler_count,
                                             uint16_t* selected_prescaler,
                                             uint16_t* reload_value,
                                             uint16_t* tick_us)
{
    uint8_t i;
    uint16_t counts;
    uint32_t best_error;
    uint16_t best_prescaler;
    uint16_t best_counts;
    uint32_t actual_us;
    uint32_t target_us;
    uint32_t error;

    if ((display == (seven_segment_t*)0) ||
        (prescalers == (const uint16_t*)0) ||
        (selected_prescaler == (uint16_t*)0) ||
        (reload_value == (uint16_t*)0) ||
        (tick_us == (uint16_t*)0) ||
        (display->config.refresh_period_us == 0u))
    {
        return 0u;
    }

    best_error = 0xFFFFFFFFul;
    best_prescaler = 0u;
    best_counts = 0u;
    target_us = display->config.refresh_period_us;

    for (i = 0u; i < prescaler_count; i++)
    {
        counts = seven_segment_compute_period_counts(prescalers[i],
                                                     display->config.refresh_period_us);
        if ((counts == 0u) || (counts > 65535u))
        {
            continue;
        }

        actual_us =
            ((uint32_t)counts * (uint32_t)prescalers[i] * 1000000ul) /
            ((uint32_t)DRV_XTAL_FREQ / 4u);

        if (actual_us >= target_us)
        {
            error = actual_us - target_us;
        }
        else
        {
            error = target_us - actual_us;
        }

        if (error < best_error)
        {
            best_error = error;
            best_prescaler = prescalers[i];
            best_counts = counts;
        }
    }

    if ((best_prescaler == 0u) || (best_counts == 0u))
    {
        return 0u;
    }

    *selected_prescaler = best_prescaler;
    *reload_value = (uint16_t)(65536u - best_counts);
    *tick_us = (uint16_t)(
        ((uint32_t)best_counts * (uint32_t)best_prescaler * 1000000ul) /
        ((uint32_t)DRV_XTAL_FREQ / 4u)
    );
    return 1u;
}

static uint8_t seven_segment_timer2_prepare(seven_segment_t* display,
                                            uint16_t* prescaler,
                                            uint8_t* period_value,
                                            uint8_t* postscaler,
                                            uint16_t* tick_us)
{
    static const uint16_t g_prescalers[3] = { 1u, 4u, 16u };
    uint8_t pr2;
    uint8_t post;
    uint8_t i;
    uint32_t best_error;
    uint16_t best_prescaler;
    uint8_t best_pr2;
    uint8_t best_post;
    uint32_t target_us;
    uint32_t actual_us;
    uint32_t counts;
    uint32_t error;

    if ((display == (seven_segment_t*)0) ||
        (prescaler == (uint16_t*)0) ||
        (period_value == (uint8_t*)0) ||
        (postscaler == (uint8_t*)0) ||
        (tick_us == (uint16_t*)0) ||
        (display->config.refresh_period_us == 0u))
    {
        if (display != (seven_segment_t*)0)
        {
            display->init_error = SEVEN_SEGMENT_INIT_ERROR_TIMER_PERIOD_UNSUPPORTED;
        }
        return 0u;
    }

    best_error = 0xFFFFFFFFul;
    best_prescaler = 0u;
    best_pr2 = 0u;
    best_post = 0u;
    target_us = display->config.refresh_period_us;

    for (i = 0u; i < 3u; i++)
    {
        for (post = 1u; post <= 16u; post++)
        {
            counts = ((uint32_t)DRV_XTAL_FREQ / 4u);
            counts = (counts * target_us) / ((uint32_t)g_prescalers[i] * (uint32_t)post * 1000000ul);

            if (counts == 0u)
            {
                counts = 1u;
            }

            if (counts > 256u)
            {
                continue;
            }

            pr2 = (uint8_t)(counts - 1u);
            actual_us =
                ((uint32_t)(pr2 + 1u) * (uint32_t)g_prescalers[i] * (uint32_t)post * 1000000ul) /
                ((uint32_t)DRV_XTAL_FREQ / 4u);

            if (actual_us >= target_us)
            {
                error = actual_us - target_us;
            }
            else
            {
                error = target_us - actual_us;
            }

            if (error < best_error)
            {
                best_error = error;
                best_prescaler = g_prescalers[i];
                best_pr2 = pr2;
                best_post = post;
            }
        }
    }

    if (best_prescaler == 0u)
    {
        display->init_error = SEVEN_SEGMENT_INIT_ERROR_TIMER_PERIOD_UNSUPPORTED;
        return 0u;
    }

    *prescaler = best_prescaler;
    *period_value = best_pr2;
    *postscaler = best_post;
    *tick_us = (uint16_t)(
        ((uint32_t)(best_pr2 + 1u) * (uint32_t)best_prescaler * (uint32_t)best_post * 1000000ul) /
        ((uint32_t)DRV_XTAL_FREQ / 4u)
    );
    return 1u;
}

static void seven_segment_release_timer_slot(seven_segment_t* display);

static uint8_t seven_segment_attach_timer_slot(seven_segment_t* display)
{
    static const uint16_t g_timer0_prescalers[8] = { 2u, 4u, 8u, 16u, 32u, 64u, 128u, 256u };
    static const uint16_t g_timer13_prescalers[4] = { 1u, 2u, 4u, 8u };
    seven_segment_timer_callback_t existing_callback;
    uint16_t prescaler;
    uint16_t reload_value;
    uint16_t tick_us;
    uint8_t period_value;
    uint8_t postscaler;

    if ((display == (seven_segment_t*)0) ||
        (display->config.timer == SEVEN_SEGMENT_TIMER_NONE) ||
        (display->config.refresh_period_us == 0u))
    {
        if (display != (seven_segment_t*)0)
        {
            if (display->config.timer == SEVEN_SEGMENT_TIMER_NONE)
            {
                display->init_error = SEVEN_SEGMENT_INIT_ERROR_TIMER_NONE;
            }
            else
            {
                display->init_error = SEVEN_SEGMENT_INIT_ERROR_PERIOD_ZERO;
            }
        }
        return 0u;
    }

    prescaler = 0u;
    reload_value = 0u;
    tick_us = 0u;
    period_value = 0u;
    postscaler = 0u;

    switch (display->config.timer)
    {
        case SEVEN_SEGMENT_TIMER0:
#if SEVEN_SEGMENT_ENABLE_TIMER0
            if (g_timer0_slot.owner != (seven_segment_t*)0)
            {
                display->init_error = SEVEN_SEGMENT_INIT_ERROR_TIMER_ALREADY_OWNED;
                return 0u;
            }

            existing_callback = timer0_get_callback();
            if ((existing_callback != (seven_segment_timer_callback_t)0) &&
                (existing_callback != seven_segment_timer0_callback))
            {
                display->init_error = SEVEN_SEGMENT_INIT_ERROR_TIMER_CALLBACK_BUSY;
                return 0u;
            }

            if (seven_segment_timer16_prepare(display,
                                              g_timer0_prescalers,
                                              8u,
                                              &prescaler,
                                              &reload_value,
                                              &tick_us) == 0u)
            {
                return 0u;
            }

            g_timer0_slot.owner = display;
            g_timer0_slot.callback = seven_segment_timer0_callback;

            timer0_stop();
            timer0_disable_interrupt();
            timer0_init(prescaler);
            timer0_set(reload_value);
            timer0_set_callback(seven_segment_timer0_callback);
            timer0_enable_interrupt();
            timer0_start();

            if ((g_timer0_slot.owner != display) ||
                (timer0_get_callback() != seven_segment_timer0_callback))
            {
                timer0_stop();
                timer0_disable_interrupt();
                timer0_set_callback((void (*)(void))0);
                g_timer0_slot.owner = (seven_segment_t*)0;
                g_timer0_slot.callback = (seven_segment_timer_callback_t)0;
                display->init_error = SEVEN_SEGMENT_INIT_ERROR_TIMER_START_FAILED;
                return 0u;
            }
            break;
#else
            display->init_error = SEVEN_SEGMENT_INIT_ERROR_TIMER_BACKEND_DISABLED;
            return 0u;
#endif

        case SEVEN_SEGMENT_TIMER1:
#if SEVEN_SEGMENT_ENABLE_TIMER1
            if (g_timer1_slot.owner != (seven_segment_t*)0)
            {
                display->init_error = SEVEN_SEGMENT_INIT_ERROR_TIMER_ALREADY_OWNED;
                return 0u;
            }

            existing_callback = timer1_get_callback();
            if ((existing_callback != (seven_segment_timer_callback_t)0) &&
                (existing_callback != seven_segment_timer1_callback))
            {
                display->init_error = SEVEN_SEGMENT_INIT_ERROR_TIMER_CALLBACK_BUSY;
                return 0u;
            }

            if (seven_segment_timer16_prepare(display,
                                              g_timer13_prescalers,
                                              4u,
                                              &prescaler,
                                              &reload_value,
                                              &tick_us) == 0u)
            {
                return 0u;
            }

            g_timer1_slot.owner = display;
            g_timer1_slot.callback = seven_segment_timer1_callback;

            timer1_stop();
            timer1_disable_interrupt();
            timer1_init(prescaler);
            timer1_set(reload_value);
            timer1_set_callback(seven_segment_timer1_callback);
            timer1_enable_interrupt();
            timer1_start();

            if ((g_timer1_slot.owner != display) ||
                (timer1_get_callback() != seven_segment_timer1_callback))
            {
                timer1_stop();
                timer1_disable_interrupt();
                timer1_set_callback((void (*)(void))0);
                g_timer1_slot.owner = (seven_segment_t*)0;
                g_timer1_slot.callback = (seven_segment_timer_callback_t)0;
                display->init_error = SEVEN_SEGMENT_INIT_ERROR_TIMER_START_FAILED;
                return 0u;
            }
            break;
#else
            display->init_error = SEVEN_SEGMENT_INIT_ERROR_TIMER_BACKEND_DISABLED;
            return 0u;
#endif

        case SEVEN_SEGMENT_TIMER2:
#if SEVEN_SEGMENT_ENABLE_TIMER2
            if (g_timer2_slot.owner != (seven_segment_t*)0)
            {
                display->init_error = SEVEN_SEGMENT_INIT_ERROR_TIMER_ALREADY_OWNED;
                return 0u;
            }

            existing_callback = timer2_get_callback();
            if ((existing_callback != (seven_segment_timer_callback_t)0) &&
                (existing_callback != seven_segment_timer2_callback))
            {
                display->init_error = SEVEN_SEGMENT_INIT_ERROR_TIMER_CALLBACK_BUSY;
                return 0u;
            }

            if (seven_segment_timer2_prepare(display,
                                             &prescaler,
                                             &period_value,
                                             &postscaler,
                                             &tick_us) == 0u)
            {
                return 0u;
            }

            g_timer2_slot.owner = display;
            g_timer2_slot.callback = seven_segment_timer2_callback;

            timer2_stop();
            timer2_disable_interrupt();
            timer2_init(prescaler);
            timer2_set_period(period_value);
            timer2_set_postscaler(postscaler);
            timer2_set_callback(seven_segment_timer2_callback);
            timer2_clear_interrupt_flag();
            timer2_enable_interrupt();
            timer2_start();

            if ((g_timer2_slot.owner != display) ||
                (timer2_get_callback() != seven_segment_timer2_callback) ||
                (T2CONbits.TMR2ON == 0u))
            {
                timer2_stop();
                timer2_disable_interrupt();
                timer2_set_callback((void (*)(void))0);
                g_timer2_slot.owner = (seven_segment_t*)0;
                g_timer2_slot.callback = (seven_segment_timer_callback_t)0;
                display->init_error = SEVEN_SEGMENT_INIT_ERROR_TIMER_START_FAILED;
                return 0u;
            }
            break;
#else
            display->init_error = SEVEN_SEGMENT_INIT_ERROR_TIMER_BACKEND_DISABLED;
            return 0u;
#endif

        case SEVEN_SEGMENT_TIMER3:
#if SEVEN_SEGMENT_ENABLE_TIMER3
            if (g_timer3_slot.owner != (seven_segment_t*)0)
            {
                display->init_error = SEVEN_SEGMENT_INIT_ERROR_TIMER_ALREADY_OWNED;
                return 0u;
            }

            existing_callback = timer3_get_callback();
            if ((existing_callback != (seven_segment_timer_callback_t)0) &&
                (existing_callback != seven_segment_timer3_callback))
            {
                display->init_error = SEVEN_SEGMENT_INIT_ERROR_TIMER_CALLBACK_BUSY;
                return 0u;
            }

            if (seven_segment_timer16_prepare(display,
                                              g_timer13_prescalers,
                                              4u,
                                              &prescaler,
                                              &reload_value,
                                              &tick_us) == 0u)
            {
                return 0u;
            }

            g_timer3_slot.owner = display;
            g_timer3_slot.callback = seven_segment_timer3_callback;

            timer3_stop();
            timer3_disable_interrupt();
            timer3_init(prescaler);
            timer3_set(reload_value);
            timer3_set_callback(seven_segment_timer3_callback);
            timer3_enable_interrupt();
            timer3_start();

            if ((g_timer3_slot.owner != display) ||
                (timer3_get_callback() != seven_segment_timer3_callback))
            {
                timer3_stop();
                timer3_disable_interrupt();
                timer3_set_callback((void (*)(void))0);
                g_timer3_slot.owner = (seven_segment_t*)0;
                g_timer3_slot.callback = (seven_segment_timer_callback_t)0;
                display->init_error = SEVEN_SEGMENT_INIT_ERROR_TIMER_START_FAILED;
                return 0u;
            }
            break;
#else
            display->init_error = SEVEN_SEGMENT_INIT_ERROR_TIMER_BACKEND_DISABLED;
            return 0u;
#endif

        default:
            display->init_error = SEVEN_SEGMENT_INIT_ERROR_TIMER_BACKEND_DISABLED;
            return 0u;
    }

    display->timer_reload_value = reload_value;
    display->timer_tick_us = tick_us;
    display->timer_blink_us_accumulator = 0u;
    display->timer_blink_now_ms = 0u;
    return 1u;
}

static void seven_segment_release_timer_slot(seven_segment_t* display)
{
    if (display == (seven_segment_t*)0)
    {
        return;
    }

    switch (display->config.timer)
    {
        case SEVEN_SEGMENT_TIMER0:
#if SEVEN_SEGMENT_ENABLE_TIMER0
            if (g_timer0_slot.owner == display)
            {
                timer0_stop();
                timer0_disable_interrupt();
                timer0_set_callback((void (*)(void))0);
                g_timer0_slot.owner = (seven_segment_t*)0;
                g_timer0_slot.callback = (seven_segment_timer_callback_t)0;
            }
#endif
            break;

        case SEVEN_SEGMENT_TIMER1:
#if SEVEN_SEGMENT_ENABLE_TIMER1
            if (g_timer1_slot.owner == display)
            {
                timer1_stop();
                timer1_disable_interrupt();
                timer1_set_callback((void (*)(void))0);
                g_timer1_slot.owner = (seven_segment_t*)0;
                g_timer1_slot.callback = (seven_segment_timer_callback_t)0;
            }
#endif
            break;

        case SEVEN_SEGMENT_TIMER2:
#if SEVEN_SEGMENT_ENABLE_TIMER2
            if (g_timer2_slot.owner == display)
            {
                timer2_stop();
                timer2_disable_interrupt();
                timer2_set_callback((void (*)(void))0);
                g_timer2_slot.owner = (seven_segment_t*)0;
                g_timer2_slot.callback = (seven_segment_timer_callback_t)0;
            }
#endif
            break;

        case SEVEN_SEGMENT_TIMER3:
#if SEVEN_SEGMENT_ENABLE_TIMER3
            if (g_timer3_slot.owner == display)
            {
                timer3_stop();
                timer3_disable_interrupt();
                timer3_set_callback((void (*)(void))0);
                g_timer3_slot.owner = (seven_segment_t*)0;
                g_timer3_slot.callback = (seven_segment_timer_callback_t)0;
            }
#endif
            break;

        default:
            break;
    }

    display->timer_owned_refresh = 0u;
    display->timer_reload_value = 0u;
    display->timer_tick_us = 0u;
    display->timer_blink_us_accumulator = 0u;
    display->timer_blink_now_ms = 0u;
    display->refresh_count = 0u;
}

uint8_t seven_segment_init(seven_segment_t* display,
                           const seven_segment_config_t* config)
{
    uint8_t i;
    uint8_t refresh_mode;

    if ((display == (seven_segment_t*)0) ||
        (config == (const seven_segment_config_t*)0) ||
        (config->digit_pins == (const seven_segment_pin_t*)0) ||
        (config->digit_count == 0u) ||
        (config->digit_count > SEVEN_SEGMENT_MAX_DIGITS))
    {
        if (display != (seven_segment_t*)0)
        {
            display->init_error = SEVEN_SEGMENT_INIT_ERROR_ARGUMENT;
        }
        return 0u;
    }

    display->config = *config;
    display->initialized = 0u;
    display->current_digit = 0u;
    display->brightness_percent = 100u;
    display->brightness_phase = 0u;
    display->dot_mask = 0u;
    display->blink_mask = 0u;
    display->blink_period_ms = SEVEN_SEGMENT_DEFAULT_BLINK_MS;
    display->blink_last_toggle_ms = 0u;
    display->blink_enabled = 0u;
    display->blink_phase_visible = 1u;
    display->timer_owned_refresh = 0u;
    display->timer_reload_value = 0u;
    display->timer_tick_us = 0u;
    display->timer_blink_us_accumulator = 0u;
    display->timer_blink_now_ms = 0u;
    display->refresh_count = 0u;
    display->init_error = SEVEN_SEGMENT_INIT_ERROR_NONE;

    gpio_set_output(display->config.seg_a.tris, display->config.seg_a.bit);
    gpio_set_output(display->config.seg_b.tris, display->config.seg_b.bit);
    gpio_set_output(display->config.seg_c.tris, display->config.seg_c.bit);
    gpio_set_output(display->config.seg_d.tris, display->config.seg_d.bit);
    gpio_set_output(display->config.seg_e.tris, display->config.seg_e.bit);
    gpio_set_output(display->config.seg_f.tris, display->config.seg_f.bit);
    gpio_set_output(display->config.seg_g.tris, display->config.seg_g.bit);

    if (display->config.has_dot != 0u)
    {
        gpio_set_output(display->config.dot.tris, display->config.dot.bit);
    }

    for (i = 0u; i < display->config.digit_count; i++)
    {
        gpio_set_output(display->config.digit_pins[i].tris,
                        display->config.digit_pins[i].bit);
        display->raw_patterns[i] = 0u;
    }

    for (i = display->config.digit_count; i < SEVEN_SEGMENT_MAX_DIGITS; i++)
    {
        display->raw_patterns[i] = 0u;
    }

    seven_segment_disable_all_digits(display);
    seven_segment_apply_pattern(display, 0u, 0u);

    refresh_mode = (uint8_t)config->refresh_mode;
    if (refresh_mode > (uint8_t)SEVEN_SEGMENT_REFRESH_TIMER)
    {
        refresh_mode = (uint8_t)SEVEN_SEGMENT_REFRESH_MANUAL;
        display->config.refresh_mode = SEVEN_SEGMENT_REFRESH_MANUAL;
    }

    if (display->config.refresh_mode == SEVEN_SEGMENT_REFRESH_TIMER)
    {
        if ((display->config.timer == SEVEN_SEGMENT_TIMER_NONE) ||
            (display->config.refresh_period_us == 0u) ||
            (seven_segment_attach_timer_slot(display) == 0u))
        {
            seven_segment_release_timer_slot(display);
            seven_segment_disable_all_digits(display);
            seven_segment_apply_pattern(display, 0u, 0u);
            display->initialized = 0u;
            return 0u;
        }

        display->timer_owned_refresh = 1u;
    }

    display->initialized = 1u;
    display->init_error = SEVEN_SEGMENT_INIT_ERROR_NONE;
    return 1u;
}

void seven_segment_set_digit(seven_segment_t* display, uint8_t digit)
{
    seven_segment_set_digit_at(display, 0u, digit);
}

void seven_segment_set_digit_at(seven_segment_t* display,
                                uint8_t position,
                                uint8_t digit)
{
    if (digit > 9u)
    {
        seven_segment_set_symbol_at(display, position, SEVEN_SEGMENT_SYMBOL_BLANK);
        return;
    }

    seven_segment_set_symbol_at(display, position, digit);
}

void seven_segment_set_hex(seven_segment_t* display, uint8_t value)
{
    if (value <= 9u)
    {
        seven_segment_set_digit(display, value);
        return;
    }

    seven_segment_set_symbol_at(display, 0u, (uint8_t)(10u + (value - 10u)));
}

void seven_segment_set_symbol_at(seven_segment_t* display,
                                 uint8_t position,
                                 uint8_t symbol)
{
    uint8_t patterns[SEVEN_SEGMENT_MAX_DIGITS];
    uint8_t i;
    uint8_t dot_mask;

    if ((display == (seven_segment_t*)0) ||
        (display->initialized == 0u) ||
        (position >= display->config.digit_count))
    {
        return;
    }

    for (i = 0u; i < display->config.digit_count; i++)
    {
        patterns[i] = display->raw_patterns[i];
    }

    dot_mask = display->dot_mask;
    patterns[position] = seven_segment_encode_symbol(display, symbol);
    seven_segment_commit_buffer(display, patterns, dot_mask);
}

void seven_segment_set_raw(seven_segment_t* display, uint8_t pattern)
{
    seven_segment_set_raw_at(display, 0u, pattern);
}

void seven_segment_set_raw_at(seven_segment_t* display,
                              uint8_t position,
                              uint8_t pattern)
{
    uint8_t patterns[SEVEN_SEGMENT_MAX_DIGITS];
    uint8_t i;
    uint8_t dot_mask;

    if ((display == (seven_segment_t*)0) ||
        (display->initialized == 0u) ||
        (position >= display->config.digit_count))
    {
        return;
    }

    for (i = 0u; i < display->config.digit_count; i++)
    {
        patterns[i] = display->raw_patterns[i];
    }

    dot_mask = display->dot_mask;
    patterns[position] = (uint8_t)(pattern & SEVEN_SEGMENT_ALL_SEGMENTS_MASK);
    seven_segment_commit_buffer(display, patterns, dot_mask);
}

void seven_segment_set_number(seven_segment_t* display, int32_t value)
{
    seven_segment_number_format_t format;

    if ((display == (seven_segment_t*)0) || (display->initialized == 0u))
    {
        return;
    }

    format.start_digit = 0u;
    format.width = display->config.digit_count;
    format.alignment = SEVEN_SEGMENT_ALIGN_RIGHT;
    format.leading_zero = 0u;
    format.clear_unused = 1u;

    (void)seven_segment_set_number_formatted(display, value, &format);
}

drv_status_t seven_segment_set_number_formatted(seven_segment_t* display,
                                                int32_t value,
                                                const seven_segment_number_format_t* format)
{
    uint8_t patterns[SEVEN_SEGMENT_MAX_DIGITS];
    uint8_t field_symbols[SEVEN_SEGMENT_MAX_DIGITS];
    uint8_t used_positions[SEVEN_SEGMENT_MAX_DIGITS];
    uint8_t dot_mask;
    uint8_t i;
    drv_status_t status;

    if ((display == (seven_segment_t*)0) ||
        (format == (const seven_segment_number_format_t*)0))
    {
        return DRV_STATUS_ERROR;
    }

    status = seven_segment_validate_field(display, format->start_digit, format->width);
    if (status != DRV_STATUS_OK)
    {
        return status;
    }

    status = seven_segment_prepare_number_field(value,
                                                format,
                                                field_symbols,
                                                used_positions);
    if (status != DRV_STATUS_OK)
    {
        return status;
    }

    for (i = 0u; i < display->config.digit_count; i++)
    {
        patterns[i] = display->raw_patterns[i];
    }

    dot_mask = display->dot_mask;

    for (i = 0u; i < format->width; i++)
    {
        uint8_t target;
        uint8_t symbol;

        target = (uint8_t)(format->start_digit + i);
        if (used_positions[i] != 0u)
        {
            symbol = field_symbols[i];
            patterns[target] = seven_segment_encode_symbol(display, symbol);
        }
        else if (format->clear_unused != 0u)
        {
            patterns[target] = 0u;
            dot_mask = (uint8_t)(dot_mask & (uint8_t)(~((uint8_t)1u << target)));
        }
    }

    seven_segment_commit_buffer(display, patterns, dot_mask);
    return DRV_STATUS_OK;
}

drv_status_t seven_segment_set_fixed(seven_segment_t* display,
                                     int32_t scaled_value,
                                     uint8_t decimal_places)
{
    seven_segment_number_format_t format;

    if ((display == (seven_segment_t*)0) || (display->initialized == 0u))
    {
        return DRV_STATUS_ERROR;
    }

    format.start_digit = 0u;
    format.width = display->config.digit_count;
    format.alignment = SEVEN_SEGMENT_ALIGN_RIGHT;
    format.leading_zero = 0u;
    format.clear_unused = 1u;

    return seven_segment_set_fixed_formatted(display,
                                             scaled_value,
                                             decimal_places,
                                             &format);
}

drv_status_t seven_segment_set_fixed_formatted(seven_segment_t* display,
                                               int32_t scaled_value,
                                               uint8_t decimal_places,
                                               const seven_segment_number_format_t* format)
{
    uint8_t patterns[SEVEN_SEGMENT_MAX_DIGITS];
    uint8_t field_symbols[SEVEN_SEGMENT_MAX_DIGITS];
    uint8_t used_positions[SEVEN_SEGMENT_MAX_DIGITS];
    uint8_t dot_mask;
    uint8_t dot_position;
    uint8_t i;
    drv_status_t status;

    if ((display == (seven_segment_t*)0) ||
        (format == (const seven_segment_number_format_t*)0))
    {
        return DRV_STATUS_ERROR;
    }

    if ((decimal_places != 0u) && (display->config.has_dot == 0u))
    {
        return DRV_STATUS_ERROR;
    }

    status = seven_segment_validate_field(display, format->start_digit, format->width);
    if (status != DRV_STATUS_OK)
    {
        return status;
    }

    status = seven_segment_prepare_fixed_field(scaled_value,
                                               decimal_places,
                                               format,
                                               field_symbols,
                                               used_positions,
                                               &dot_position);
    if (status != DRV_STATUS_OK)
    {
        return status;
    }

    for (i = 0u; i < display->config.digit_count; i++)
    {
        patterns[i] = display->raw_patterns[i];
    }

    dot_mask = display->dot_mask;

    for (i = 0u; i < format->width; i++)
    {
        uint8_t target;

        target = (uint8_t)(format->start_digit + i);
        dot_mask = (uint8_t)(dot_mask & (uint8_t)(~((uint8_t)1u << target)));

        if (used_positions[i] != 0u)
        {
            patterns[target] = seven_segment_encode_symbol(display, field_symbols[i]);
        }
        else if (format->clear_unused != 0u)
        {
            patterns[target] = 0u;
        }
    }

    if (dot_position != 0xFFu)
    {
        uint8_t target;

        target = (uint8_t)(format->start_digit + dot_position);
        dot_mask = (uint8_t)(dot_mask | ((uint8_t)1u << target));
    }

    seven_segment_commit_buffer(display, patterns, dot_mask);
    return DRV_STATUS_OK;
}

drv_status_t seven_segment_set_time_hhmm(seven_segment_t* display,
                                         uint8_t start_digit,
                                         uint8_t hours,
                                         uint8_t minutes,
                                         uint8_t leading_zero_hours)
{
    seven_segment_number_format_t hours_format;
    seven_segment_number_format_t minutes_format;
    uint8_t patterns[SEVEN_SEGMENT_MAX_DIGITS];
    uint8_t field_symbols[SEVEN_SEGMENT_MAX_DIGITS];
    uint8_t used_positions[SEVEN_SEGMENT_MAX_DIGITS];
    uint8_t dot_mask;
    uint8_t i;
    drv_status_t status;

    if ((hours > 23u) || (minutes > 59u))
    {
        return DRV_STATUS_ERROR;
    }

    status = seven_segment_validate_field(display, start_digit, 4u);
    if (status != DRV_STATUS_OK)
    {
        return status;
    }

    for (i = 0u; i < display->config.digit_count; i++)
    {
        patterns[i] = display->raw_patterns[i];
    }

    dot_mask = display->dot_mask;

    hours_format.start_digit = start_digit;
    hours_format.width = 2u;
    hours_format.alignment = SEVEN_SEGMENT_ALIGN_RIGHT;
    hours_format.leading_zero = leading_zero_hours;
    hours_format.clear_unused = 1u;

    status = seven_segment_prepare_number_field((int32_t)hours,
                                                &hours_format,
                                                field_symbols,
                                                used_positions);
    if (status != DRV_STATUS_OK)
    {
        return status;
    }

    for (i = 0u; i < hours_format.width; i++)
    {
        uint8_t target;

        target = (uint8_t)(hours_format.start_digit + i);
        if (used_positions[i] != 0u)
        {
            patterns[target] = seven_segment_encode_symbol(display, field_symbols[i]);
        }
        else if (hours_format.clear_unused != 0u)
        {
            patterns[target] = 0u;
            dot_mask = (uint8_t)(dot_mask & (uint8_t)(~((uint8_t)1u << target)));
        }
    }

    minutes_format.start_digit = (uint8_t)(start_digit + 2u);
    minutes_format.width = 2u;
    minutes_format.alignment = SEVEN_SEGMENT_ALIGN_RIGHT;
    minutes_format.leading_zero = 1u;
    minutes_format.clear_unused = 1u;

    status = seven_segment_prepare_number_field((int32_t)minutes,
                                                &minutes_format,
                                                field_symbols,
                                                used_positions);
    if (status != DRV_STATUS_OK)
    {
        return status;
    }

    for (i = 0u; i < minutes_format.width; i++)
    {
        uint8_t target;

        target = (uint8_t)(minutes_format.start_digit + i);
        if (used_positions[i] != 0u)
        {
            patterns[target] = seven_segment_encode_symbol(display, field_symbols[i]);
        }
        else if (minutes_format.clear_unused != 0u)
        {
            patterns[target] = 0u;
            dot_mask = (uint8_t)(dot_mask & (uint8_t)(~((uint8_t)1u << target)));
        }
    }

    seven_segment_commit_buffer(display, patterns, dot_mask);
    return DRV_STATUS_OK;
}

void seven_segment_clear(seven_segment_t* display)
{
    uint8_t patterns[SEVEN_SEGMENT_MAX_DIGITS];
    uint8_t i;

    if ((display == (seven_segment_t*)0) || (display->initialized == 0u))
    {
        return;
    }

    for (i = 0u; i < display->config.digit_count; i++)
    {
        patterns[i] = 0u;
    }

    seven_segment_commit_buffer(display, patterns, 0u);
}

void seven_segment_set_dot(seven_segment_t* display, uint8_t on)
{
    seven_segment_set_dot_at(display, 0u, on);
}

void seven_segment_set_dot_at(seven_segment_t* display,
                              uint8_t position,
                              uint8_t on)
{
    uint8_t dot_mask;

    if ((display == (seven_segment_t*)0) ||
        (display->initialized == 0u) ||
        (position >= display->config.digit_count))
    {
        return;
    }

    dot_mask = display->dot_mask;

    if (on != 0u)
    {
        dot_mask = (uint8_t)(dot_mask | ((uint8_t)1u << position));
    }
    else
    {
        dot_mask = (uint8_t)(dot_mask & (uint8_t)(~((uint8_t)1u << position)));
    }

    seven_segment_commit_buffer(display, display->raw_patterns, dot_mask);
}

void seven_segment_set_brightness(seven_segment_t* display, uint8_t percent)
{
    if ((display == (seven_segment_t*)0) || (display->initialized == 0u))
    {
        return;
    }

    if (percent > 100u)
    {
        percent = 100u;
    }

    display->brightness_percent = percent;
}

void seven_segment_set_blink_mask(seven_segment_t* display, uint16_t digit_mask)
{
    if ((display == (seven_segment_t*)0) || (display->initialized == 0u))
    {
        return;
    }

    display->blink_mask = seven_segment_get_valid_blink_mask(display, digit_mask);

    if (display->blink_mask == 0u)
    {
        display->blink_phase_visible = 1u;
    }
}

void seven_segment_set_blink_period_ms(seven_segment_t* display, uint16_t period_ms)
{
    if ((display == (seven_segment_t*)0) || (display->initialized == 0u))
    {
        return;
    }

    if (period_ms == 0u)
    {
        period_ms = SEVEN_SEGMENT_DEFAULT_BLINK_MS;
    }

    display->blink_period_ms = period_ms;
}

void seven_segment_set_blink_enabled(seven_segment_t* display, uint8_t enabled)
{
    if ((display == (seven_segment_t*)0) || (display->initialized == 0u))
    {
        return;
    }

    display->blink_enabled = (uint8_t)(enabled != 0u ? 1u : 0u);
    if (display->blink_enabled == 0u)
    {
        display->blink_phase_visible = 1u;
    }
}

static void seven_segment_blink_update_core(seven_segment_t* display, uint32_t now_ms)
{
    uint16_t half_period;

    if ((display == (seven_segment_t*)0) || (display->initialized == 0u))
    {
        return;
    }

    if ((display->blink_enabled == 0u) || (display->blink_mask == 0u))
    {
        display->blink_phase_visible = 1u;
        display->blink_last_toggle_ms = now_ms;
        return;
    }

    half_period = display->blink_period_ms;
    if (half_period == 0u)
    {
        half_period = SEVEN_SEGMENT_DEFAULT_BLINK_MS;
    }

    half_period = (uint16_t)(half_period / 2u);
    if (half_period == 0u)
    {
        half_period = 1u;
    }

    while ((uint32_t)(now_ms - display->blink_last_toggle_ms) >= (uint32_t)half_period)
    {
        display->blink_last_toggle_ms += half_period;
        display->blink_phase_visible = (uint8_t)(display->blink_phase_visible == 0u ? 1u : 0u);
    }
}

void seven_segment_blink_update(seven_segment_t* display, uint32_t now_ms)
{
    if ((display == (seven_segment_t*)0) || (display->initialized == 0u))
    {
        return;
    }

    if (display->timer_owned_refresh != 0u)
    {
        return;
    }

    seven_segment_blink_update_core(display, now_ms);
}

void seven_segment_refresh(seven_segment_t* display)
{
    if ((display == (seven_segment_t*)0) || (display->initialized == 0u))
    {
        return;
    }

    if (display->timer_owned_refresh != 0u)
    {
        return;
    }

    seven_segment_refresh_core(display);
}

void seven_segment_process(seven_segment_t* display)
{
    seven_segment_refresh(display);
}

uint8_t seven_segment_is_timer_running(const seven_segment_t* display)
{
    if ((display == (const seven_segment_t*)0) ||
        (display->initialized == 0u) ||
        (display->timer_owned_refresh == 0u))
    {
        return 0u;
    }

    switch (display->config.timer)
    {
        case SEVEN_SEGMENT_TIMER0:
#if SEVEN_SEGMENT_ENABLE_TIMER0
            return (uint8_t)(g_timer0_slot.owner == display ? 1u : 0u);
#else
            return 0u;
#endif

        case SEVEN_SEGMENT_TIMER1:
#if SEVEN_SEGMENT_ENABLE_TIMER1
            return (uint8_t)(g_timer1_slot.owner == display ? 1u : 0u);
#else
            return 0u;
#endif

        case SEVEN_SEGMENT_TIMER2:
#if SEVEN_SEGMENT_ENABLE_TIMER2
            return (uint8_t)(g_timer2_slot.owner == display ? 1u : 0u);
#else
            return 0u;
#endif

        case SEVEN_SEGMENT_TIMER3:
#if SEVEN_SEGMENT_ENABLE_TIMER3
            return (uint8_t)(g_timer3_slot.owner == display ? 1u : 0u);
#else
            return 0u;
#endif

        default:
            return 0u;
    }
}

uint32_t seven_segment_get_refresh_count(const seven_segment_t* display)
{
    uint8_t global_interrupt_state;
    uint32_t refresh_count;

    if (display == (const seven_segment_t*)0)
    {
        return 0u;
    }

    global_interrupt_state = INTCONbits.GIE;
    DRV_INT_DISABLE();
    refresh_count = display->refresh_count;
    if (global_interrupt_state != 0u)
    {
        DRV_INT_ENABLE();
    }

    return refresh_count;
}

seven_segment_init_error_t seven_segment_get_init_error(const seven_segment_t* display)
{
    if (display == (const seven_segment_t*)0)
    {
        return SEVEN_SEGMENT_INIT_ERROR_ARGUMENT;
    }

    return display->init_error;
}

void seven_segment_stop(seven_segment_t* display)
{
    if ((display == (seven_segment_t*)0) || (display->initialized == 0u))
    {
        return;
    }

    seven_segment_release_timer_slot(display);
    seven_segment_disable_all_digits(display);
    seven_segment_apply_pattern(display, 0u, 0u);
}

void seven_segment_deinit(seven_segment_t* display)
{
    if (display == (seven_segment_t*)0)
    {
        return;
    }

    seven_segment_stop(display);
    display->initialized = 0u;
}

void seven_segment_irq_handler(void)
{
#if SEVEN_SEGMENT_ENABLE_TIMER0
    if (g_timer0_slot.owner != (seven_segment_t*)0)
    {
        timer0_irq_handler();
    }
#endif

#if SEVEN_SEGMENT_ENABLE_TIMER1
    if (g_timer1_slot.owner != (seven_segment_t*)0)
    {
        timer1_irq_handler();
    }
#endif

#if SEVEN_SEGMENT_ENABLE_TIMER2
    if (g_timer2_slot.owner != (seven_segment_t*)0)
    {
        /*
         * Let the tested Timer2 driver dispatch its registered callback.
         *
         * Some compiler-specific Timer2 backends may return with TMR2IF
         * still set. Clear it here as a display-library safety guard so
         * the MCU cannot immediately re-enter the ISR forever.
         *
         * Timer2 driver source files remain unchanged.
         */
        timer2_irq_handler();
        PIR1bits.TMR2IF = 0u;
    }
#endif

#if SEVEN_SEGMENT_ENABLE_TIMER3
    if (g_timer3_slot.owner != (seven_segment_t*)0)
    {
        timer3_irq_handler();
    }
#endif
}
