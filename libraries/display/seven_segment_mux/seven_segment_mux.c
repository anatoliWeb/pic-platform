#include "libraries/display/seven_segment_mux/seven_segment_mux.h"

#if defined(DRV_COMPILER_C18)
    #include "../../../C18/libraries/display/seven_segment_mux/seven_segment_mux.c"
#elif defined(DRV_COMPILER_XC8)
    #include "../../../XC8/libraries/display/seven_segment_mux/seven_segment_mux.c"
#else

#include "drivers/gpio/gpio.h"

/* 7-segment patterns: bit0=A ... bit6=G. */
static const uint8_t g_digit_map[10] =
{
    0x3Fu, 0x06u, 0x5Bu, 0x4Fu, 0x66u,
    0x6Du, 0x7Du, 0x07u, 0x7Fu, 0x6Fu
};

/* Hex A-F patterns. */
static const uint8_t g_hex_map[6] =
{
    0x77u, 0x7Cu, 0x39u, 0x5Eu, 0x79u, 0x71u
};

static void seven_segment_mux_write_level(const seven_segment_pin_t* pin, uint8_t on, seven_segment_type_t type)
{
    uint8_t level = on;

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

static void seven_segment_mux_digit_line_set(const seven_segment_mux_t* display, uint8_t index, uint8_t enable)
{
    uint8_t level = (enable != 0u) ? display->config.digit_active_level : (uint8_t)(display->config.digit_active_level == 0u ? 1u : 0u);

    if (level != 0u)
    {
        gpio_write_high(display->config.digit_pins[index].port, display->config.digit_pins[index].bit);
    }
    else
    {
        gpio_write_low(display->config.digit_pins[index].port, display->config.digit_pins[index].bit);
    }
}

static void seven_segment_mux_disable_all_digits(const seven_segment_mux_t* display)
{
    uint8_t i;

    for (i = 0u; i < display->config.digit_count; i++)
    {
        seven_segment_mux_digit_line_set(display, i, 0u);
    }
}

static uint8_t seven_segment_mux_encode(uint8_t value)
{
    if (value <= 9u)
    {
        return g_digit_map[value];
    }

    if (value <= 15u)
    {
        return g_hex_map[value - 10u];
    }

    if (value == SEVEN_SEGMENT_MUX_CHAR_MINUS)
    {
        return 0x40u;
    }

    return 0x00u;
}

static void seven_segment_mux_apply_pattern(const seven_segment_mux_t* display, uint8_t pattern, uint8_t dot_on)
{
    seven_segment_mux_write_level(&display->config.segment_config.seg_a, (uint8_t)((pattern >> 0u) & 0x01u), display->config.segment_config.type);
    seven_segment_mux_write_level(&display->config.segment_config.seg_b, (uint8_t)((pattern >> 1u) & 0x01u), display->config.segment_config.type);
    seven_segment_mux_write_level(&display->config.segment_config.seg_c, (uint8_t)((pattern >> 2u) & 0x01u), display->config.segment_config.type);
    seven_segment_mux_write_level(&display->config.segment_config.seg_d, (uint8_t)((pattern >> 3u) & 0x01u), display->config.segment_config.type);
    seven_segment_mux_write_level(&display->config.segment_config.seg_e, (uint8_t)((pattern >> 4u) & 0x01u), display->config.segment_config.type);
    seven_segment_mux_write_level(&display->config.segment_config.seg_f, (uint8_t)((pattern >> 5u) & 0x01u), display->config.segment_config.type);
    seven_segment_mux_write_level(&display->config.segment_config.seg_g, (uint8_t)((pattern >> 6u) & 0x01u), display->config.segment_config.type);

    if (display->config.segment_config.has_dot != 0u)
    {
        seven_segment_mux_write_level(&display->config.segment_config.dot, dot_on, display->config.segment_config.type);
    }
}

uint8_t seven_segment_mux_init(seven_segment_mux_t* display, const seven_segment_mux_config_t* config)
{
    uint8_t i;

    if ((display == (seven_segment_mux_t*)0) || (config == (const seven_segment_mux_config_t*)0))
    {
        return 0u;
    }

    if ((config->digit_pins == (const seven_segment_pin_t*)0) ||
        (config->digit_count == 0u) ||
        (config->digit_count > SEVEN_SEGMENT_MUX_MAX_DIGITS))
    {
        return 0u;
    }

    display->config = *config;
    display->initialized = 1u;
    display->current_digit = 0u;
    display->dot_mask = 0u;

    for (i = 0u; i < display->config.digit_count; i++)
    {
        gpio_set_output(display->config.digit_pins[i].tris, display->config.digit_pins[i].bit);
        display->values[i] = SEVEN_SEGMENT_MUX_CHAR_BLANK;
    }

    for (i = display->config.digit_count; i < SEVEN_SEGMENT_MUX_MAX_DIGITS; i++)
    {
        display->values[i] = SEVEN_SEGMENT_MUX_CHAR_BLANK;
    }

    gpio_set_output(display->config.segment_config.seg_a.tris, display->config.segment_config.seg_a.bit);
    gpio_set_output(display->config.segment_config.seg_b.tris, display->config.segment_config.seg_b.bit);
    gpio_set_output(display->config.segment_config.seg_c.tris, display->config.segment_config.seg_c.bit);
    gpio_set_output(display->config.segment_config.seg_d.tris, display->config.segment_config.seg_d.bit);
    gpio_set_output(display->config.segment_config.seg_e.tris, display->config.segment_config.seg_e.bit);
    gpio_set_output(display->config.segment_config.seg_f.tris, display->config.segment_config.seg_f.bit);
    gpio_set_output(display->config.segment_config.seg_g.tris, display->config.segment_config.seg_g.bit);

    if (display->config.segment_config.has_dot != 0u)
    {
        gpio_set_output(display->config.segment_config.dot.tris, display->config.segment_config.dot.bit);
    }

    seven_segment_mux_disable_all_digits(display);
    seven_segment_mux_clear(display);

    return 1u;
}

void seven_segment_mux_set_digit(seven_segment_mux_t* display, uint8_t position, uint8_t value)
{
    if ((display == (seven_segment_mux_t*)0) || (display->initialized == 0u))
    {
        return;
    }

    if (position >= display->config.digit_count)
    {
        return;
    }

    display->values[position] = value;
}

void seven_segment_mux_set_number(seven_segment_mux_t* display, int32_t value)
{
    uint8_t i;
    uint8_t is_negative = 0u;
    uint32_t abs_value;

    if ((display == (seven_segment_mux_t*)0) || (display->initialized == 0u))
    {
        return;
    }

    if (value < 0)
    {
        is_negative = 1u;
        abs_value = (uint32_t)(-value);
    }
    else
    {
        abs_value = (uint32_t)value;
    }

    for (i = 0u; i < display->config.digit_count; i++)
    {
        display->values[i] = SEVEN_SEGMENT_MUX_CHAR_BLANK;
    }

    for (i = 0u; i < display->config.digit_count; i++)
    {
        uint8_t pos = (uint8_t)(display->config.digit_count - 1u - i);

        display->values[pos] = (uint8_t)(abs_value % 10u);
        abs_value /= 10u;

        if (abs_value == 0u)
        {
            break;
        }
    }

    if (display->config.suppress_leading_zero != 0u)
    {
        for (i = 0u; i < (uint8_t)(display->config.digit_count - 1u); i++)
        {
            if (display->values[i] == 0u)
            {
                display->values[i] = SEVEN_SEGMENT_MUX_CHAR_BLANK;
            }
            else
            {
                break;
            }
        }
    }

    if (is_negative != 0u)
    {
        for (i = 0u; i < display->config.digit_count; i++)
        {
            if (display->values[i] != SEVEN_SEGMENT_MUX_CHAR_BLANK)
            {
                if (i > 0u)
                {
                    display->values[i - 1u] = SEVEN_SEGMENT_MUX_CHAR_MINUS;
                }
                else
                {
                    display->values[0u] = SEVEN_SEGMENT_MUX_CHAR_MINUS;
                }
                return;
            }
        }

        display->values[0u] = SEVEN_SEGMENT_MUX_CHAR_MINUS;
    }
}

void seven_segment_mux_set_hex(seven_segment_mux_t* display, uint32_t value)
{
    uint8_t i;

    if ((display == (seven_segment_mux_t*)0) || (display->initialized == 0u))
    {
        return;
    }

    for (i = 0u; i < display->config.digit_count; i++)
    {
        uint8_t pos = (uint8_t)(display->config.digit_count - 1u - i);
        display->values[pos] = (uint8_t)(value & 0x0Fu);
        value >>= 4u;
    }
}

void seven_segment_mux_clear(seven_segment_mux_t* display)
{
    uint8_t i;

    if ((display == (seven_segment_mux_t*)0) || (display->initialized == 0u))
    {
        return;
    }

    for (i = 0u; i < display->config.digit_count; i++)
    {
        display->values[i] = SEVEN_SEGMENT_MUX_CHAR_BLANK;
    }

    display->dot_mask = 0u;
}

void seven_segment_mux_set_dot(seven_segment_mux_t* display, uint8_t position, uint8_t on)
{
    uint8_t mask;

    if ((display == (seven_segment_mux_t*)0) || (display->initialized == 0u))
    {
        return;
    }

    if (position >= display->config.digit_count)
    {
        return;
    }

    mask = (uint8_t)(1u << position);

    if (on != 0u)
    {
        display->dot_mask = (uint8_t)(display->dot_mask | mask);
    }
    else
    {
        display->dot_mask = (uint8_t)(display->dot_mask & (uint8_t)(~mask));
    }
}

void seven_segment_mux_update(seven_segment_mux_t* display)
{
    uint8_t idx;
    uint8_t pattern;
    uint8_t dot_on;

    if ((display == (seven_segment_mux_t*)0) || (display->initialized == 0u))
    {
        return;
    }

    idx = display->current_digit;

    /* Disable all digits first to avoid ghosting while segment lines change. */
    seven_segment_mux_disable_all_digits(display);

    pattern = seven_segment_mux_encode(display->values[idx]);
    dot_on = (uint8_t)(((display->dot_mask >> idx) & 0x01u) != 0u ? 1u : 0u);

    seven_segment_mux_apply_pattern(display, pattern, dot_on);
    seven_segment_mux_digit_line_set(display, idx, 1u);

    idx++;
    if (idx >= display->config.digit_count)
    {
        idx = 0u;
    }
    display->current_digit = idx;
}

#endif