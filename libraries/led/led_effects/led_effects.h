/*
 * File: libraries/led/led_effects/led_effects.h
 */

#ifndef LIBRARIES_LED_LED_EFFECTS_H
#define LIBRARIES_LED_LED_EFFECTS_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/led/ws2812/ws2812.h"
#include "libraries/led/rgb_utils/rgb_utils.h"

typedef enum
{
    LED_EFFECT_MODE_BLINK = 0,
    LED_EFFECT_MODE_BREATHING,
    LED_EFFECT_MODE_RAINBOW_CYCLE,
    LED_EFFECT_MODE_COLOR_WIPE,
    LED_EFFECT_MODE_SCANNER,
    LED_EFFECT_MODE_PULSE
} led_effect_mode_t;

typedef struct
{
    ws2812_t* strip;
    uint16_t led_count;
    uint8_t brightness;
    uint8_t speed;
    rgb_color_t color;
} led_effects_config_t;

typedef struct
{
    led_effects_config_t config;

    uint8_t initialized;
    led_effect_mode_t mode;

    uint8_t speed_counter;
    uint32_t step;

    uint16_t index;
    int8_t direction;
    uint8_t level;
    uint8_t level_dir;

    rgb_color_t color;
} led_effects_t;

uint8_t led_effects_init(led_effects_t* fx, const led_effects_config_t* config);

void led_effects_set_mode(led_effects_t* fx, led_effect_mode_t mode);
void led_effects_set_speed(led_effects_t* fx, uint8_t speed);
void led_effects_set_color(led_effects_t* fx, rgb_color_t color);

void led_effects_update(led_effects_t* fx);

uint8_t led_effects_is_finished(const led_effects_t* fx);

#endif /* LIBRARIES_LED_LED_EFFECTS_H */