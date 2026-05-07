#ifndef LIBRARIES_LED_LED_ANIMATION_ENGINE_H
#define LIBRARIES_LED_LED_ANIMATION_ENGINE_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/led/rgb_utils/rgb_utils.h"
#include "libraries/led/led_effects/led_effects.h"
#include "libraries/led/led_segments/led_segments.h"
#include "libraries/led/ws2812_matrix/ws2812_matrix.h"

#ifndef LED_ANIM_ENGINE_MAX_ANIMATIONS
#define LED_ANIM_ENGINE_MAX_ANIMATIONS 8u
#endif

typedef enum
{
    LED_ANIM_TARGET_NONE = 0,
    LED_ANIM_TARGET_SEGMENT,
    LED_ANIM_TARGET_MATRIX
} led_anim_target_t;

typedef enum
{
    LED_ANIM_EFFECT_BLINK = 0,
    LED_ANIM_EFFECT_BREATHING,
    LED_ANIM_EFFECT_RAINBOW,
    LED_ANIM_EFFECT_COLOR_WIPE,
    LED_ANIM_EFFECT_SCANNER,
    LED_ANIM_EFFECT_PULSE
} led_anim_effect_t;

typedef struct
{
    led_anim_target_t target_type;
    led_segment_t* segment;
    ws2812_matrix_t* matrix;
    led_anim_effect_t effect;
    uint16_t speed_ticks;
    rgb_color_t color;
    uint8_t auto_show;
    uint8_t enabled;
} led_animation_config_t;

typedef struct
{
    uint8_t used;
    uint8_t enabled;
    uint8_t paused;

    led_anim_target_t target_type;
    led_segment_t* segment;
    ws2812_matrix_t* matrix;

    led_anim_effect_t effect;
    uint16_t speed_ticks;
    uint16_t tick_counter;
    uint8_t auto_show;

    rgb_color_t color;

    uint16_t index;
    int8_t direction;
    uint8_t level;
    uint8_t level_dir;
    uint16_t hue_step;
    uint8_t state_toggle;
} led_animation_t;

typedef struct
{
    led_animation_t pool[LED_ANIM_ENGINE_MAX_ANIMATIONS];
    uint8_t paused;
} led_animation_engine_t;

void led_animation_engine_init(led_animation_engine_t* engine);

int8_t led_animation_engine_add(led_animation_engine_t* engine, const led_animation_config_t* cfg);
uint8_t led_animation_engine_remove(led_animation_engine_t* engine, uint8_t handle);

void led_animation_engine_update(led_animation_engine_t* engine);

void led_animation_engine_pause(led_animation_engine_t* engine);
void led_animation_engine_resume(led_animation_engine_t* engine);

uint8_t led_animation_engine_is_running(const led_animation_engine_t* engine, uint8_t handle);

#endif /* LIBRARIES_LED_LED_ANIMATION_ENGINE_H */
