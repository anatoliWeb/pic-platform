#include "libraries/input/segment_keys/segment_keys.h"
#include "drivers/gpio/gpio.h"

static void segment_keys_write_level(const seven_segment_pin_t* pin,
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

static const seven_segment_pin_t* segment_keys_get_segment_pin(const seven_segment_t* display,
                                                               uint8_t segment_mask)
{
    if ((display == (const seven_segment_t*)0) || (segment_mask == 0u))
    {
        return (const seven_segment_pin_t*)0;
    }

    switch (segment_mask)
    {
        case SEVEN_SEGMENT_MASK_A: return &display->config.seg_a;
        case SEVEN_SEGMENT_MASK_B: return &display->config.seg_b;
        case SEVEN_SEGMENT_MASK_C: return &display->config.seg_c;
        case SEVEN_SEGMENT_MASK_D: return &display->config.seg_d;
        case SEVEN_SEGMENT_MASK_E: return &display->config.seg_e;
        case SEVEN_SEGMENT_MASK_F: return &display->config.seg_f;
        case SEVEN_SEGMENT_MASK_G: return &display->config.seg_g;
        case SEVEN_SEGMENT_MASK_DP: return &display->config.dot;
        default: return (const seven_segment_pin_t*)0;
    }
}

static void segment_keys_set_segment_inputs(const seven_segment_t* display)
{
    gpio_set_input(display->config.seg_a.tris, display->config.seg_a.bit);
    gpio_set_input(display->config.seg_b.tris, display->config.seg_b.bit);
    gpio_set_input(display->config.seg_c.tris, display->config.seg_c.bit);
    gpio_set_input(display->config.seg_d.tris, display->config.seg_d.bit);
    gpio_set_input(display->config.seg_e.tris, display->config.seg_e.bit);
    gpio_set_input(display->config.seg_f.tris, display->config.seg_f.bit);
    gpio_set_input(display->config.seg_g.tris, display->config.seg_g.bit);

    if (display->config.has_dot != 0u)
    {
        gpio_set_input(display->config.dot.tris, display->config.dot.bit);
    }
}

static void segment_keys_restore_segment_outputs(const seven_segment_t* display)
{
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
}

static void segment_keys_restore_display_state(const seven_segment_t* display)
{
    uint8_t digit_index;
    uint8_t pattern;
    uint8_t dot_on;
    uint8_t level;

    if ((display == (const seven_segment_t*)0) ||
        (display->config.digit_count == 0u) ||
        (display->config.digit_pins == (const seven_segment_pin_t*)0))
    {
        return;
    }

    if ((display->brightness_percent == 0u) ||
        (display->brightness_phase >= display->brightness_percent))
    {
        return;
    }

    digit_index = display->current_digit;
    if (digit_index == 0u)
    {
        digit_index = (uint8_t)(display->config.digit_count - 1u);
    }
    else
    {
        digit_index--;
    }

    pattern = display->raw_patterns[digit_index];
    dot_on = (uint8_t)(((display->dot_mask >> digit_index) & 0x01u) != 0u ? 1u : 0u);

    segment_keys_write_level(&display->config.seg_a,
                             (uint8_t)((pattern & SEVEN_SEGMENT_MASK_A) != 0u),
                             display->config.type);
    segment_keys_write_level(&display->config.seg_b,
                             (uint8_t)((pattern & SEVEN_SEGMENT_MASK_B) != 0u),
                             display->config.type);
    segment_keys_write_level(&display->config.seg_c,
                             (uint8_t)((pattern & SEVEN_SEGMENT_MASK_C) != 0u),
                             display->config.type);
    segment_keys_write_level(&display->config.seg_d,
                             (uint8_t)((pattern & SEVEN_SEGMENT_MASK_D) != 0u),
                             display->config.type);
    segment_keys_write_level(&display->config.seg_e,
                             (uint8_t)((pattern & SEVEN_SEGMENT_MASK_E) != 0u),
                             display->config.type);
    segment_keys_write_level(&display->config.seg_f,
                             (uint8_t)((pattern & SEVEN_SEGMENT_MASK_F) != 0u),
                             display->config.type);
    segment_keys_write_level(&display->config.seg_g,
                             (uint8_t)((pattern & SEVEN_SEGMENT_MASK_G) != 0u),
                             display->config.type);

    if (display->config.has_dot != 0u)
    {
        segment_keys_write_level(&display->config.dot, dot_on, display->config.type);
    }

    level = display->config.digit_active_level;
    if (level != 0u)
    {
        gpio_write_high(display->config.digit_pins[digit_index].port,
                        display->config.digit_pins[digit_index].bit);
    }
    else
    {
        gpio_write_low(display->config.digit_pins[digit_index].port,
                       display->config.digit_pins[digit_index].bit);
    }
}

static void segment_keys_disable_all_digits(const seven_segment_t* display)
{
    uint8_t i;
    uint8_t level;

    if ((display == (const seven_segment_t*)0) ||
        (display->config.digit_pins == (const seven_segment_pin_t*)0))
    {
        return;
    }

    level = (uint8_t)(display->config.digit_active_level == 0u ? 1u : 0u);

    for (i = 0u; i < display->config.digit_count; i++)
    {
        if (level != 0u)
        {
            gpio_write_high(display->config.digit_pins[i].port,
                            display->config.digit_pins[i].bit);
        }
        else
        {
            gpio_write_low(display->config.digit_pins[i].port,
                           display->config.digit_pins[i].bit);
        }
    }
}

static uint8_t segment_keys_sample_mask(const segment_keys_t* ctx)
{
    const seven_segment_t* display;
    uint8_t mask;
    uint8_t bit_mask;
    const seven_segment_pin_t* pin;

    display = ctx->config.display;
    mask = 0u;

    segment_keys_disable_all_digits(display);
    segment_keys_set_segment_inputs(display);

    for (bit_mask = 1u; bit_mask != 0u; bit_mask <<= 1u)
    {
        if ((bit_mask == SEVEN_SEGMENT_MASK_DP) &&
            (display->config.has_dot == 0u))
        {
            continue;
        }

        pin = segment_keys_get_segment_pin(display, bit_mask);
        if ((pin == (const seven_segment_pin_t*)0) ||
            (pin->port == (volatile uint8_t*)0))
        {
            continue;
        }

        if (gpio_read(pin->port, pin->bit) == ctx->config.active_level)
        {
            mask = (uint8_t)(mask | bit_mask);
        }

        if (bit_mask == SEVEN_SEGMENT_MASK_DP)
        {
            break;
        }
    }

    segment_keys_restore_segment_outputs(display);
    segment_keys_restore_display_state(display);
    return mask;
}

uint8_t segment_keys_init(segment_keys_t* ctx,
                          const segment_keys_config_t* config)
{
    uint8_t i;
    uint8_t j;

    if ((ctx == (segment_keys_t*)0) ||
        (config == (const segment_keys_config_t*)0) ||
        (config->display == (seven_segment_t*)0) ||
        (config->keys == (const segment_keys_entry_t*)0) ||
        (config->key_count == 0u) ||
        (config->key_count > SEGMENT_KEYS_MAX_KEYS))
    {
        return 0u;
    }

    for (i = 0u; i < config->key_count; i++)
    {
        if ((config->keys[i].segment_mask == 0u) ||
            ((config->keys[i].segment_mask &
              (uint8_t)(~(SEVEN_SEGMENT_ALL_SEGMENTS_MASK | SEVEN_SEGMENT_MASK_DP))) != 0u))
        {
            return 0u;
        }

        if (((config->keys[i].segment_mask & SEVEN_SEGMENT_MASK_DP) != 0u) &&
            (config->display->config.has_dot == 0u))
        {
            return 0u;
        }

        for (j = (uint8_t)(i + 1u); j < config->key_count; j++)
        {
            if (config->keys[i].segment_mask == config->keys[j].segment_mask)
            {
                return 0u;
            }
        }
    }

    ctx->config = *config;
    ctx->initialized = 1u;
    ctx->raw_mask = 0u;
    ctx->unknown_state = 0u;
    ctx->last_key = '\0';

    for (i = 0u; i < SEGMENT_KEYS_MAX_KEYS; i++)
    {
        ctx->pressed_state[i] = 0u;
    }

    return 1u;
}

void segment_keys_update(segment_keys_t* ctx)
{
    uint8_t i;
    uint8_t raw_mask;
    uint8_t matched_count;
    uint8_t matched_index;
    uint8_t pressed;

    if ((ctx == (segment_keys_t*)0) || (ctx->initialized == 0u))
    {
        return;
    }

    raw_mask = segment_keys_sample_mask(ctx);
    ctx->raw_mask = raw_mask;
    ctx->unknown_state = 0u;
    ctx->last_key = '\0';
    matched_count = 0u;
    matched_index = 0xFFu;

    for (i = 0u; i < ctx->config.key_count; i++)
    {
        if (raw_mask == ctx->config.keys[i].segment_mask)
        {
            matched_count++;
            matched_index = i;
        }
    }

    if ((raw_mask != 0u) && (matched_count != 1u))
    {
        ctx->unknown_state = 1u;
    }

    for (i = 0u; i < ctx->config.key_count; i++)
    {
        pressed = (uint8_t)((matched_count == 1u) && (matched_index == i) ? 1u : 0u);
        ctx->pressed_state[i] = pressed;

        if ((pressed != 0u) && (ctx->last_key == '\0'))
        {
            ctx->last_key = ctx->config.keys[i].key;
        }

        if (ctx->config.keys[i].button != (button_t*)0)
        {
            button_set_raw_state(ctx->config.keys[i].button,
                                 (uint8_t)(pressed != 0u ? 0u : 1u));
            button_update(ctx->config.keys[i].button);
        }
    }
}

uint8_t segment_keys_is_pressed(const segment_keys_t* ctx, uint8_t key_index)
{
    if ((ctx == (const segment_keys_t*)0) ||
        (ctx->initialized == 0u) ||
        (key_index >= ctx->config.key_count))
    {
        return 0u;
    }

    return ctx->pressed_state[key_index];
}

uint8_t segment_keys_get_raw_mask(const segment_keys_t* ctx)
{
    if ((ctx == (const segment_keys_t*)0) || (ctx->initialized == 0u))
    {
        return 0u;
    }

    return ctx->raw_mask;
}

uint8_t segment_keys_is_unknown(const segment_keys_t* ctx)
{
    if ((ctx == (const segment_keys_t*)0) || (ctx->initialized == 0u))
    {
        return 0u;
    }

    return ctx->unknown_state;
}

char segment_keys_get_key(segment_keys_t* ctx)
{
    char key;

    if ((ctx == (segment_keys_t*)0) || (ctx->initialized == 0u))
    {
        return '\0';
    }

    key = ctx->last_key;
    ctx->last_key = '\0';
    return key;
}
