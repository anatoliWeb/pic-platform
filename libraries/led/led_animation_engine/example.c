/*
 * File: libraries/led/led_animation_engine/example.c
 */

#include "core/compiler.h"
#include "libraries/led/ws2812/ws2812.h"
#include "libraries/led/rgb_utils/rgb_utils.h"
#include "libraries/led/ws2812_matrix/ws2812_matrix.h"
#include "libraries/led/led_segments/led_segments.h"
#include "libraries/led/led_animation_engine/led_animation_engine.h"

#define STRIP_LED_COUNT 64u

static ws2812_color_t g_pixels[STRIP_LED_COUNT];

void main(void)
{
    ws2812_t strip;
    ws2812_matrix_t matrix;
    ws2812_matrix_config_t matrix_cfg;
    led_segment_t seg_a;
    led_segment_t seg_b;
    led_segment_config_t seg_cfg;
    led_animation_engine_t engine;
    led_animation_config_t anim_cfg;

    int8_t h_seg_a;
    int8_t h_seg_b;
    int8_t h_matrix;

    (void)ws2812_init(&strip, &PORTC, &TRISC, 0u, g_pixels, STRIP_LED_COUNT);

    matrix_cfg.strip = &strip;
    matrix_cfg.width = 8u;
    matrix_cfg.height = 8u;
    matrix_cfg.layout = WS2812_MATRIX_LAYOUT_ROW_MAJOR;
    matrix_cfg.serpentine = 1u;
    matrix_cfg.origin = WS2812_MATRIX_ORIGIN_TOP_LEFT;
    matrix_cfg.reverse = 0u;
    (void)ws2812_matrix_init(&matrix, &matrix_cfg);

    seg_cfg.parent_type = LED_SEGMENT_PARENT_STRIP;
    seg_cfg.strip = &strip;
    seg_cfg.matrix = (ws2812_matrix_t*)0;
    seg_cfg.start_index = 0u;
    seg_cfg.length = 16u;
    seg_cfg.reverse = 0u;
    seg_cfg.region_x = 0u;
    seg_cfg.region_y = 0u;
    seg_cfg.region_width = 0u;
    seg_cfg.region_height = 0u;
    (void)led_segment_init(&seg_a, &seg_cfg);

    seg_cfg.start_index = 16u;
    seg_cfg.length = 16u;
    seg_cfg.reverse = 1u;
    (void)led_segment_init(&seg_b, &seg_cfg);

    led_animation_engine_init(&engine);

    anim_cfg.target_type = LED_ANIM_TARGET_SEGMENT;
    anim_cfg.segment = &seg_a;
    anim_cfg.matrix = (ws2812_matrix_t*)0;
    anim_cfg.effect = LED_ANIM_EFFECT_SCANNER;
    anim_cfg.speed_ticks = 2u;
    anim_cfg.color = rgb_color(255u, 32u, 0u);
    anim_cfg.auto_show = 0u;
    anim_cfg.enabled = 1u;
    h_seg_a = led_animation_engine_add(&engine, &anim_cfg);

    anim_cfg.target_type = LED_ANIM_TARGET_SEGMENT;
    anim_cfg.segment = &seg_b;
    anim_cfg.effect = LED_ANIM_EFFECT_PULSE;
    anim_cfg.speed_ticks = 4u;
    anim_cfg.color = rgb_color(0u, 96u, 255u);
    h_seg_b = led_animation_engine_add(&engine, &anim_cfg);

    anim_cfg.target_type = LED_ANIM_TARGET_MATRIX;
    anim_cfg.segment = (led_segment_t*)0;
    anim_cfg.matrix = &matrix;
    anim_cfg.effect = LED_ANIM_EFFECT_RAINBOW;
    anim_cfg.speed_ticks = 3u;
    anim_cfg.color = rgb_color(0u, 0u, 0u);
    h_matrix = led_animation_engine_add(&engine, &anim_cfg);

    while (1)
    {
        /* Non-blocking engine update for all active animations. */
        led_animation_engine_update(&engine);

        /* Single framebuffer push for combined scene. */
        ws2812_show(&strip);

        /* Example of runtime switching and control. */
        if (led_animation_engine_is_running(&engine, (uint8_t)h_seg_a) == 0u)
        {
            (void)led_animation_engine_remove(&engine, (uint8_t)h_seg_a);
        }

        if (h_matrix >= 0)
        {
            /* Keep handle referenced to show multi-animation lifecycle. */
            (void)h_seg_b;
        }
    }
}
