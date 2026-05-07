#include "libraries/led/led_animation_engine/led_animation_engine.h"

static uint16_t led_anim_target_length(const led_animation_t* anim)
{
    if (anim->target_type == LED_ANIM_TARGET_SEGMENT)
    {
        return anim->segment->length;
    }

    if (anim->target_type == LED_ANIM_TARGET_MATRIX)
    {
        return (uint16_t)(anim->matrix->width * anim->matrix->height);
    }

    return 0u;
}

static void led_anim_target_set_pixel(led_animation_t* anim, uint16_t index, rgb_color_t color)
{
    uint16_t x;
    uint16_t y;
    uint16_t width;

    if (anim->target_type == LED_ANIM_TARGET_SEGMENT)
    {
        (void)led_segment_set_pixel(anim->segment, index, color);
        return;
    }

    width = anim->matrix->width;
    x = (uint16_t)(index % width);
    y = (uint16_t)(index / width);
    (void)ws2812_matrix_set_pixel(anim->matrix, x, y, color);
}

static void led_anim_target_clear(led_animation_t* anim)
{
    if (anim->target_type == LED_ANIM_TARGET_SEGMENT)
    {
        led_segment_clear(anim->segment);
    }
    else if (anim->target_type == LED_ANIM_TARGET_MATRIX)
    {
        ws2812_matrix_clear(anim->matrix);
    }
}

static void led_anim_target_fill(led_animation_t* anim, rgb_color_t color)
{
    if (anim->target_type == LED_ANIM_TARGET_SEGMENT)
    {
        led_segment_fill(anim->segment, color);
    }
    else if (anim->target_type == LED_ANIM_TARGET_MATRIX)
    {
        ws2812_matrix_fill(anim->matrix, color);
    }
}

static void led_anim_target_show(led_animation_t* anim)
{
    if (anim->auto_show == 0u)
    {
        return;
    }

    if (anim->target_type == LED_ANIM_TARGET_SEGMENT)
    {
        led_segment_show(anim->segment);
    }
    else if (anim->target_type == LED_ANIM_TARGET_MATRIX)
    {
        ws2812_matrix_show(anim->matrix);
    }
}

static void led_anim_step_blink(led_animation_t* anim)
{
    if (anim->state_toggle == 0u)
    {
        led_anim_target_fill(anim, anim->color);
        anim->state_toggle = 1u;
    }
    else
    {
        led_anim_target_clear(anim);
        anim->state_toggle = 0u;
    }
}

static void led_anim_step_breathing(led_animation_t* anim)
{
    rgb_color_t c;

    if (anim->level_dir != 0u)
    {
        if (anim->level < 250u) { anim->level = (uint8_t)(anim->level + 5u); }
        else { anim->level = 255u; anim->level_dir = 0u; }
    }
    else
    {
        if (anim->level > 5u) { anim->level = (uint8_t)(anim->level - 5u); }
        else { anim->level = 0u; anim->level_dir = 1u; }
    }

    c = rgb_scale(anim->color, anim->level);
    led_anim_target_fill(anim, c);
}

static void led_anim_step_rainbow(led_animation_t* anim)
{
    uint16_t i;
    uint16_t len;
    hsv_color_t hsv;

    len = led_anim_target_length(anim);
    if (len == 0u)
    {
        return;
    }

    for (i = 0u; i < len; i++)
    {
        hsv.h = (uint16_t)((anim->hue_step + (uint16_t)((uint32_t)i * 360u / len)) % 360u);
        hsv.s = 255u;
        hsv.v = 255u;
        led_anim_target_set_pixel(anim, i, hsv_to_rgb(hsv));
    }

    anim->hue_step = (uint16_t)((anim->hue_step + 3u) % 360u);
}

static void led_anim_step_color_wipe(led_animation_t* anim)
{
    uint16_t len;
    uint16_t i;

    len = led_anim_target_length(anim);
    if (len == 0u)
    {
        return;
    }

    if (anim->index == 0u)
    {
        led_anim_target_clear(anim);
    }

    for (i = 0u; i < anim->index && i < len; i++)
    {
        led_anim_target_set_pixel(anim, i, anim->color);
    }

    anim->index++;
    if (anim->index > len)
    {
        anim->index = 0u;
    }
}

static void led_anim_step_scanner(led_animation_t* anim)
{
    uint16_t len;
    uint16_t pos;

    len = led_anim_target_length(anim);
    if (len == 0u)
    {
        return;
    }

    if (anim->index >= len)
    {
        anim->index = 0u;
    }

    led_anim_target_clear(anim);
    pos = anim->index;
    led_anim_target_set_pixel(anim, pos, anim->color);

    if (pos > 0u)
    {
        led_anim_target_set_pixel(anim, (uint16_t)(pos - 1u), rgb_scale(anim->color, 64u));
    }
    if ((pos + 1u) < len)
    {
        led_anim_target_set_pixel(anim, (uint16_t)(pos + 1u), rgb_scale(anim->color, 64u));
    }

    if (anim->direction > 0)
    {
        if ((anim->index + 1u) >= len)
        {
            anim->direction = -1;
        }
        else
        {
            anim->index++;
        }
    }
    else
    {
        if (anim->index == 0u)
        {
            anim->direction = 1;
        }
        else
        {
            anim->index--;
        }
    }
}

static void led_anim_step_pulse(led_animation_t* anim)
{
    rgb_color_t c;

    if (anim->level_dir != 0u)
    {
        if (anim->level < 240u) { anim->level = (uint8_t)(anim->level + 15u); }
        else { anim->level = 255u; anim->level_dir = 0u; }
    }
    else
    {
        if (anim->level > 15u) { anim->level = (uint8_t)(anim->level - 15u); }
        else { anim->level = 0u; anim->level_dir = 1u; }
    }

    c = rgb_scale(anim->color, anim->level);
    led_anim_target_fill(anim, c);
}

void led_animation_engine_init(led_animation_engine_t* engine)
{
    uint8_t i;

    if (engine == (led_animation_engine_t*)0)
    {
        return;
    }

    engine->paused = 0u;

    for (i = 0u; i < LED_ANIM_ENGINE_MAX_ANIMATIONS; i++)
    {
        engine->pool[i].used = 0u;
        engine->pool[i].enabled = 0u;
        engine->pool[i].paused = 0u;
    }
}

int8_t led_animation_engine_add(led_animation_engine_t* engine, const led_animation_config_t* cfg)
{
    uint8_t i;
    led_animation_t* anim;

    if ((engine == (led_animation_engine_t*)0) || (cfg == (const led_animation_config_t*)0))
    {
        return -1;
    }

    for (i = 0u; i < LED_ANIM_ENGINE_MAX_ANIMATIONS; i++)
    {
        if (engine->pool[i].used == 0u)
        {
            anim = &engine->pool[i];

            if (cfg->target_type == LED_ANIM_TARGET_SEGMENT)
            {
                if (cfg->segment == (led_segment_t*)0)
                {
                    return -1;
                }
            }
            else if (cfg->target_type == LED_ANIM_TARGET_MATRIX)
            {
                if (cfg->matrix == (ws2812_matrix_t*)0)
                {
                    return -1;
                }
            }
            else
            {
                return -1;
            }

            anim->used = 1u;
            anim->enabled = (uint8_t)(cfg->enabled != 0u ? 1u : 0u);
            anim->paused = 0u;
            anim->target_type = cfg->target_type;
            anim->segment = cfg->segment;
            anim->matrix = cfg->matrix;
            anim->effect = cfg->effect;
            anim->speed_ticks = (uint16_t)(cfg->speed_ticks == 0u ? 1u : cfg->speed_ticks);
            anim->tick_counter = 0u;
            anim->auto_show = (uint8_t)(cfg->auto_show != 0u ? 1u : 0u);
            anim->color = cfg->color;
            anim->index = 0u;
            anim->direction = 1;
            anim->level = 0u;
            anim->level_dir = 1u;
            anim->hue_step = 0u;
            anim->state_toggle = 0u;

            return (int8_t)i;
        }
    }

    return -1;
}

uint8_t led_animation_engine_remove(led_animation_engine_t* engine, uint8_t handle)
{
    if (engine == (led_animation_engine_t*)0)
    {
        return 0u;
    }

    if (handle >= LED_ANIM_ENGINE_MAX_ANIMATIONS)
    {
        return 0u;
    }

    engine->pool[handle].used = 0u;
    engine->pool[handle].enabled = 0u;
    engine->pool[handle].paused = 0u;
    return 1u;
}

void led_animation_engine_update(led_animation_engine_t* engine)
{
    uint8_t i;
    led_animation_t* anim;

    if (engine == (led_animation_engine_t*)0)
    {
        return;
    }

    if (engine->paused != 0u)
    {
        return;
    }

    for (i = 0u; i < LED_ANIM_ENGINE_MAX_ANIMATIONS; i++)
    {
        anim = &engine->pool[i];

        if ((anim->used == 0u) || (anim->enabled == 0u) || (anim->paused != 0u))
        {
            continue;
        }

        anim->tick_counter++;
        if (anim->tick_counter < anim->speed_ticks)
        {
            continue;
        }
        anim->tick_counter = 0u;

        switch (anim->effect)
        {
            case LED_ANIM_EFFECT_BLINK:
                led_anim_step_blink(anim);
                break;
            case LED_ANIM_EFFECT_BREATHING:
                led_anim_step_breathing(anim);
                break;
            case LED_ANIM_EFFECT_RAINBOW:
                led_anim_step_rainbow(anim);
                break;
            case LED_ANIM_EFFECT_COLOR_WIPE:
                led_anim_step_color_wipe(anim);
                break;
            case LED_ANIM_EFFECT_SCANNER:
                led_anim_step_scanner(anim);
                break;
            case LED_ANIM_EFFECT_PULSE:
            default:
                led_anim_step_pulse(anim);
                break;
        }

        led_anim_target_show(anim);
    }
}

void led_animation_engine_pause(led_animation_engine_t* engine)
{
    if (engine == (led_animation_engine_t*)0)
    {
        return;
    }

    engine->paused = 1u;
}

void led_animation_engine_resume(led_animation_engine_t* engine)
{
    if (engine == (led_animation_engine_t*)0)
    {
        return;
    }

    engine->paused = 0u;
}

uint8_t led_animation_engine_is_running(const led_animation_engine_t* engine, uint8_t handle)
{
    if (engine == (const led_animation_engine_t*)0)
    {
        return 0u;
    }

    if (handle >= LED_ANIM_ENGINE_MAX_ANIMATIONS)
    {
        return 0u;
    }

    if ((engine->paused != 0u) ||
        (engine->pool[handle].used == 0u) ||
        (engine->pool[handle].enabled == 0u) ||
        (engine->pool[handle].paused != 0u))
    {
        return 0u;
    }

    return 1u;
}
