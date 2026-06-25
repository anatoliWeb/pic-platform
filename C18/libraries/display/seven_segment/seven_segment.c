/*
 * File: C18/libraries/display/seven_segment/seven_segment.c
 */

#include "libraries/display/seven_segment/seven_segment.h"
#include "drivers/gpio/gpio.h"

/* Bit order: 0=A, 1=B, 2=C, 3=D, 4=E, 5=F, 6=G. */
static const uint8_t g_digit_map[10] =
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

/* Hex digits A-F in uppercase style. */
static const uint8_t g_hex_map[6] =
{
    0x77u, /* A */
    0x7Cu, /* b */
    0x39u, /* C */
    0x5Eu, /* d */
    0x79u, /* E */
    0x71u  /* F */
};

static void seven_segment_write_level(const seven_segment_pin_t* pin, uint8_t on, seven_segment_type_t type)
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

static void seven_segment_apply_pattern(seven_segment_t* display, uint8_t pattern)
{
    seven_segment_write_level(&display->config.seg_a, (uint8_t)((pattern >> 0u) & 0x01u), display->config.type);
    seven_segment_write_level(&display->config.seg_b, (uint8_t)((pattern >> 1u) & 0x01u), display->config.type);
    seven_segment_write_level(&display->config.seg_c, (uint8_t)((pattern >> 2u) & 0x01u), display->config.type);
    seven_segment_write_level(&display->config.seg_d, (uint8_t)((pattern >> 3u) & 0x01u), display->config.type);
    seven_segment_write_level(&display->config.seg_e, (uint8_t)((pattern >> 4u) & 0x01u), display->config.type);
    seven_segment_write_level(&display->config.seg_f, (uint8_t)((pattern >> 5u) & 0x01u), display->config.type);
    seven_segment_write_level(&display->config.seg_g, (uint8_t)((pattern >> 6u) & 0x01u), display->config.type);
}

uint8_t seven_segment_init(seven_segment_t* display, const seven_segment_config_t* config)
{
    if ((display == (seven_segment_t*)0) || (config == (const seven_segment_config_t*)0))
    {
        return 0u;
    }

    display->config = *config;

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

    display->initialized = 1u;
    seven_segment_clear(display);
    seven_segment_set_dot(display, 0u);

    return 1u;
}

void seven_segment_set_raw(seven_segment_t* display, uint8_t pattern)
{
    if ((display == (seven_segment_t*)0) || (display->initialized == 0u))
    {
        return;
    }

    seven_segment_apply_pattern(display, pattern);
}

void seven_segment_set_digit(seven_segment_t* display, uint8_t digit)
{
    if (digit > 9u)
    {
        seven_segment_clear(display);
        return;
    }

    seven_segment_set_raw(display, g_digit_map[digit]);
}

void seven_segment_set_hex(seven_segment_t* display, uint8_t value)
{
    if (value <= 9u)
    {
        seven_segment_set_digit(display, value);
        return;
    }

    if (value <= 15u)
    {
        seven_segment_set_raw(display, g_hex_map[value - 10u]);
        return;
    }

    /* Out-of-range values are shown as minus sign. */
    seven_segment_set_raw(display, 0x40u);
}

void seven_segment_clear(seven_segment_t* display)
{
    seven_segment_set_raw(display, 0x00u);
}

void seven_segment_set_dot(seven_segment_t* display, uint8_t on)
{
    if ((display == (seven_segment_t*)0) || (display->initialized == 0u))
    {
        return;
    }

    if (display->config.has_dot == 0u)
    {
        return;
    }

    seven_segment_write_level(&display->config.dot, (uint8_t)(on != 0u ? 1u : 0u), display->config.type);
}
