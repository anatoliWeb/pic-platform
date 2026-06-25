/*
 * File: libraries/input/shared_segment_buttons/shared_segment_buttons.c
 */

#include "libraries/input/shared_segment_buttons/shared_segment_buttons.h"

#if defined(DRV_COMPILER_C18)
    #include "../../../C18/libraries/input/shared_segment_buttons/shared_segment_buttons.c"
#elif defined(DRV_COMPILER_XC8)
    #include "../../../XC8/libraries/input/shared_segment_buttons/shared_segment_buttons.c"
#else

#include "drivers/gpio/gpio.h"

static const seven_segment_pin_t* shared_segment_buttons_get_segment_pin(const seven_segment_mux_t* display,
                                                                          shared_segment_id_t segment)
{
    switch (segment)
    {
        case SHARED_SEG_SEG_A: return &display->config.segment_config.seg_a;
        case SHARED_SEG_SEG_B: return &display->config.segment_config.seg_b;
        case SHARED_SEG_SEG_C: return &display->config.segment_config.seg_c;
        case SHARED_SEG_SEG_D: return &display->config.segment_config.seg_d;
        case SHARED_SEG_SEG_E: return &display->config.segment_config.seg_e;
        case SHARED_SEG_SEG_F: return &display->config.segment_config.seg_f;
        case SHARED_SEG_SEG_G: return &display->config.segment_config.seg_g;
        case SHARED_SEG_SEG_DOT: return &display->config.segment_config.dot;
        default: return &display->config.segment_config.seg_a;
    }
}

static void shared_segment_buttons_set_digit_line(const seven_segment_mux_t* display, uint8_t index, uint8_t enable)
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

static void shared_segment_buttons_disable_all_digits(const seven_segment_mux_t* display)
{
    uint8_t i;

    for (i = 0u; i < display->config.digit_count; i++)
    {
        shared_segment_buttons_set_digit_line(display, i, 0u);
    }
}

static uint8_t shared_segment_buttons_sample(const shared_segment_buttons_t* ctx,
                                             const shared_segment_button_map_t* map)
{
    const seven_segment_mux_t* display = ctx->config.display;
    const seven_segment_pin_t* seg_pin = shared_segment_buttons_get_segment_pin(display, map->segment);
    uint8_t raw;

    /* Keep display dark for a very short sampling phase to avoid ghosting. */
    shared_segment_buttons_disable_all_digits(display);

    /* Release shared segment line, then select target digit and sample the line. */
    gpio_set_input(seg_pin->tris, seg_pin->bit);
    shared_segment_buttons_set_digit_line(display, map->digit_index, 1u);
    raw = gpio_read(seg_pin->port, seg_pin->bit);

    /* Restore line to output for next display refresh phase. */
    shared_segment_buttons_set_digit_line(display, map->digit_index, 0u);
    gpio_set_output(seg_pin->tris, seg_pin->bit);

    return (uint8_t)(raw == ctx->config.active_level ? 1u : 0u);
}

uint8_t shared_segment_buttons_init(shared_segment_buttons_t* ctx,
                                    const shared_segment_buttons_config_t* config)
{
    uint8_t i;

    if ((ctx == (shared_segment_buttons_t*)0) || (config == (const shared_segment_buttons_config_t*)0))
    {
        return 0u;
    }

    if ((config->display == (const seven_segment_mux_t*)0) ||
        (config->map == (const shared_segment_button_map_t*)0) ||
        (config->button_count == 0u) ||
        (config->button_count > SHARED_SEGMENT_BUTTONS_MAX))
    {
        return 0u;
    }

    ctx->config = *config;

    if (ctx->config.debounce_scans == 0u)
    {
        ctx->config.debounce_scans = 2u;
    }

    for (i = 0u; i < ctx->config.button_count; i++)
    {
        if (ctx->config.map[i].digit_index >= ctx->config.display->config.digit_count)
        {
            return 0u;
        }

        if ((ctx->config.map[i].segment == SHARED_SEG_SEG_DOT) &&
            (ctx->config.display->config.segment_config.has_dot == 0u))
        {
            return 0u;
        }

        ctx->stable_state[i] = 0u;
        ctx->debounce_count[i] = 0u;
    }

    for (i = ctx->config.button_count; i < SHARED_SEGMENT_BUTTONS_MAX; i++)
    {
        ctx->stable_state[i] = 0u;
        ctx->debounce_count[i] = 0u;
    }

    ctx->scan_index = 0u;
    ctx->last_key = '\0';
    ctx->initialized = 1u;

    return 1u;
}

void shared_segment_buttons_update(shared_segment_buttons_t* ctx)
{
    uint8_t idx;
    uint8_t sample;

    if ((ctx == (shared_segment_buttons_t*)0) || (ctx->initialized == 0u))
    {
        return;
    }

    idx = ctx->scan_index;
    sample = shared_segment_buttons_sample(ctx, &ctx->config.map[idx]);

    if (sample == ctx->stable_state[idx])
    {
        ctx->debounce_count[idx] = 0u;
    }
    else
    {
        if (ctx->debounce_count[idx] < 255u)
        {
            ctx->debounce_count[idx]++;
        }

        if (ctx->debounce_count[idx] >= ctx->config.debounce_scans)
        {
            ctx->stable_state[idx] = sample;
            ctx->debounce_count[idx] = 0u;

            if (sample != 0u)
            {
                ctx->last_key = ctx->config.map[idx].key;
            }
        }
    }

    idx++;
    if (idx >= ctx->config.button_count)
    {
        idx = 0u;
    }
    ctx->scan_index = idx;
}

uint8_t shared_segment_buttons_is_pressed(const shared_segment_buttons_t* ctx, uint8_t button_index)
{
    if ((ctx == (const shared_segment_buttons_t*)0) || (ctx->initialized == 0u))
    {
        return 0u;
    }

    if (button_index >= ctx->config.button_count)
    {
        return 0u;
    }

    return ctx->stable_state[button_index];
}

char shared_segment_buttons_get_key(shared_segment_buttons_t* ctx)
{
    char key;

    if ((ctx == (shared_segment_buttons_t*)0) || (ctx->initialized == 0u))
    {
        return '\0';
    }

    key = ctx->last_key;
    ctx->last_key = '\0';
    return key;
}

#endif