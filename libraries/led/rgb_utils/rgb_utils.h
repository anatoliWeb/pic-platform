/*
 * File: libraries/led/rgb_utils/rgb_utils.h
 */

#ifndef LIBRARIES_LED_RGB_UTILS_H
#define LIBRARIES_LED_RGB_UTILS_H

#include "core/compiler.h"
#include "core/types.h"

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb_color_t;

typedef struct
{
    uint16_t h; /* 0..359 */
    uint8_t s;  /* 0..255 */
    uint8_t v;  /* 0..255 */
} hsv_color_t;

rgb_color_t rgb_color(uint8_t r, uint8_t g, uint8_t b);

rgb_color_t rgb_scale(rgb_color_t color, uint8_t scale);

rgb_color_t rgb_blend(rgb_color_t a, rgb_color_t b, uint8_t alpha);

rgb_color_t rgb_add(rgb_color_t a, rgb_color_t b);
rgb_color_t rgb_subtract(rgb_color_t a, rgb_color_t b);

rgb_color_t hsv_to_rgb(hsv_color_t hsv);

#endif /* LIBRARIES_LED_RGB_UTILS_H */