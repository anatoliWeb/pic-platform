#include "libraries/led/led_effects/led_effects.h"

#if defined(DRV_COMPILER_C18)
    #include "../../../C18/libraries/led/led_effects/led_effects.c"
#elif defined(DRV_COMPILER_XC8)
    #include "../../../XC8/libraries/led/led_effects/led_effects.c"
#else

/* Convert generic RGB utility color to WS2812 pixel color. */
static ws2812_color_t led_effects_to_ws_color(rgb_color_t color)
{
    ws2812_color_t ws_color;
    ws_color.r = color.r;
    ws_color.g = color.g;
    ws_color.b = color.b;
    return ws_color;
}

/* Apply configured brightness to the strip before rendering. */
static void led_effects_apply_brightness(led_effects_t* fx)
{
    ws2812_set_brightness(fx->config.strip, fx->config.brightness);
}

/* Reset runtime animation state when mode changes. */
static void led_effects_mode_reset_state(led_effects_t* fx)
{
    fx->speed_counter = 0u;
    fx->step = 0u;
    fx->index = 0u;
    fx->direction = 1;
    fx->level = 0u;
    fx->level_dir = 1u;
}

uint8_t led_effects_init(led_effects_t* fx, const led_effects_config_t* config)
{
    if ((fx == (led_effects_t*)0) || (config == (const led_effects_config_t*)0))
    {
        return 0u;
    }

    if ((config->strip == (ws2812_t*)0) || (config->led_count == 0u))
    {
        return 0u;
    }

    fx->config = *config;
    fx->mode = LED_EFFECT_MODE_BLINK;
    fx->color = config->color;
    fx->initialized = 1u;

    if (fx->config.speed == 0u)
    {
        fx->config.speed = 1u;
    }

    led_effects_mode_reset_state(fx);
    led_effects_apply_brightness(fx);
    ws2812_clear(fx->config.strip);
    ws2812_show(fx->config.strip);

    return 1u;
}

void led_effects_set_mode(led_effects_t* fx, led_effect_mode_t mode)
{
    if ((fx == (led_effects_t*)0) || (fx->initialized == 0u))
    {
        return;
    }

    fx->mode = mode;
    led_effects_mode_reset_state(fx);
}

void led_effects_set_speed(led_effects_t* fx, uint8_t speed)
{
    if ((fx == (led_effects_t*)0) || (fx->initialized == 0u))
    {
        return;
    }

    fx->config.speed = (uint8_t)(speed == 0u ? 1u : speed);
}

void led_effects_set_color(led_effects_t* fx, rgb_color_t color)
{
    if ((fx == (led_effects_t*)0) || (fx->initialized == 0u))
    {
        return;
    }

    fx->color = color;
}

static void led_effects_run_blink(led_effects_t* fx)
{
    if ((fx->step & 0x01u) == 0u)
    {
        ws2812_set_all(fx->config.strip, led_effects_to_ws_color(fx->color));
    }
    else
    {
        ws2812_clear(fx->config.strip);
    }
    ws2812_show(fx->config.strip);
}

static void led_effects_run_breathing(led_effects_t* fx)
{
    uint8_t level;
    rgb_color_t c;

    if (fx->level_dir != 0u)
    {
        if (fx->level < 250u) { fx->level = (uint8_t)(fx->level + 5u); }
        else { fx->level = 255u; fx->level_dir = 0u; }
    }
    else
    {
        if (fx->level > 5u) { fx->level = (uint8_t)(fx->level - 5u); }
        else { fx->level = 0u; fx->level_dir = 1u; }
    }

    level = fx->level;
    c = rgb_scale(fx->color, level);
    ws2812_set_all(fx->config.strip, led_effects_to_ws_color(c));
    ws2812_show(fx->config.strip);
}

static void led_effects_run_rainbow(led_effects_t* fx)
{
    uint16_t i;
    hsv_color_t hsv;

    for (i = 0u; i < fx->config.led_count; i++)
    {
        hsv.h = (uint16_t)((fx->step + (uint32_t)(i * 360u / fx->config.led_count)) % 360u);
        hsv.s = 255u;
        hsv.v = 255u;
        ws2812_set_pixel(fx->config.strip, i, led_effects_to_ws_color(hsv_to_rgb(hsv)));
    }

    ws2812_show(fx->config.strip);
    fx->step = (uint32_t)((fx->step + 3u) % 360u);
}

static void led_effects_run_color_wipe(led_effects_t* fx)
{
    uint16_t i;

    if (fx->index == 0u)
    {
        ws2812_clear(fx->config.strip);
    }

    for (i = 0u; i < fx->index; i++)
    {
        ws2812_set_pixel(fx->config.strip, i, led_effects_to_ws_color(fx->color));
    }

    ws2812_show(fx->config.strip);

    fx->index++;
    if (fx->index > fx->config.led_count)
    {
        fx->index = 0u;
    }
}

static void led_effects_run_scanner(led_effects_t* fx)
{
    uint16_t pos;

    ws2812_clear(fx->config.strip);
    pos = fx->index;
    ws2812_set_pixel(fx->config.strip, pos, led_effects_to_ws_color(fx->color));

    if (pos > 0u)
    {
        ws2812_set_pixel(fx->config.strip, (uint16_t)(pos - 1u), led_effects_to_ws_color(rgb_scale(fx->color, 64u)));
    }
    if ((pos + 1u) < fx->config.led_count)
    {
        ws2812_set_pixel(fx->config.strip, (uint16_t)(pos + 1u), led_effects_to_ws_color(rgb_scale(fx->color, 64u)));
    }

    ws2812_show(fx->config.strip);

    if (fx->direction > 0)
    {
        if (fx->index + 1u >= fx->config.led_count)
        {
            fx->direction = -1;
        }
        else
        {
            fx->index++;
        }
    }
    else
    {
        if (fx->index == 0u)
        {
            fx->direction = 1;
        }
        else
        {
            fx->index--;
        }
    }
}

static void led_effects_run_pulse(led_effects_t* fx)
{
    rgb_color_t c;

    if (fx->level_dir != 0u)
    {
        if (fx->level < 240u) { fx->level = (uint8_t)(fx->level + 15u); }
        else { fx->level = 255u; fx->level_dir = 0u; }
    }
    else
    {
        if (fx->level > 15u) { fx->level = (uint8_t)(fx->level - 15u); }
        else { fx->level = 0u; fx->level_dir = 1u; }
    }

    c = rgb_scale(fx->color, fx->level);
    ws2812_set_all(fx->config.strip, led_effects_to_ws_color(c));
    ws2812_show(fx->config.strip);
}

void led_effects_update(led_effects_t* fx)
{
    if ((fx == (led_effects_t*)0) || (fx->initialized == 0u))
    {
        return;
    }

    fx->speed_counter++;
    if (fx->speed_counter < fx->config.speed)
    {
        return;
    }
    fx->speed_counter = 0u;

    led_effects_apply_brightness(fx);

    switch (fx->mode)
    {
        case LED_EFFECT_MODE_BLINK:
            led_effects_run_blink(fx);
            fx->step++;
            break;

        case LED_EFFECT_MODE_BREATHING:
            led_effects_run_breathing(fx);
            break;

        case LED_EFFECT_MODE_RAINBOW_CYCLE:
            led_effects_run_rainbow(fx);
            break;

        case LED_EFFECT_MODE_COLOR_WIPE:
            led_effects_run_color_wipe(fx);
            break;

        case LED_EFFECT_MODE_SCANNER:
            led_effects_run_scanner(fx);
            break;

        case LED_EFFECT_MODE_PULSE:
        default:
            led_effects_run_pulse(fx);
            break;
    }
}

uint8_t led_effects_is_finished(const led_effects_t* fx)
{
    if ((fx == (const led_effects_t*)0) || (fx->initialized == 0u))
    {
        return 0u;
    }

    if (fx->mode == LED_EFFECT_MODE_COLOR_WIPE)
    {
        return (uint8_t)(fx->index == 0u ? 1u : 0u);
    }

    return 0u;
}

#endif
