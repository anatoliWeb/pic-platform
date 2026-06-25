/*
 * File: libraries/led/rgb_utils/example.c
 */

#include "core/compiler.h"
#include "libraries/led/rgb_utils/rgb_utils.h"

void main(void)
{
    rgb_color_t red;
    rgb_color_t blue;
    rgb_color_t blend_50;
    rgb_color_t dimmed;
    rgb_color_t rainbow[6];
    hsv_color_t hsv;
    uint8_t i;

    red = rgb_color(255u, 0u, 0u);
    blue = rgb_color(0u, 0u, 255u);

    /* Linear blend: 50% red + 50% blue. */
    blend_50 = rgb_blend(red, blue, 128u);

    /* Global brightness scaling to 25%. */
    dimmed = rgb_scale(blend_50, 64u);

    /* Generate simple HSV rainbow samples. */
    for (i = 0u; i < 6u; i++)
    {
        hsv.h = (uint16_t)(i * 60u);
        hsv.s = 255u;
        hsv.v = 255u;
        rainbow[i] = hsv_to_rgb(hsv);
    }

    /* Keep variables alive for debugger inspection. */
    while (1)
    {
        DRV_UNUSED(dimmed);
        DRV_UNUSED(rainbow[0]);
    }
}