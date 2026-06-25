/*
 * File: C18/libraries/led/rgb_utils/rgb_utils.c
 */

#include "libraries/led/rgb_utils/rgb_utils.h"

static uint8_t rgb_scale_u8(uint8_t value, uint8_t scale)
{
    return (uint8_t)(((uint16_t)value * (uint16_t)scale) / 255u);
}

rgb_color_t rgb_color(uint8_t r, uint8_t g, uint8_t b)
{
    rgb_color_t c;
    c.r = r;
    c.g = g;
    c.b = b;
    return c;
}

rgb_color_t rgb_scale(rgb_color_t color, uint8_t scale)
{
    color.r = rgb_scale_u8(color.r, scale);
    color.g = rgb_scale_u8(color.g, scale);
    color.b = rgb_scale_u8(color.b, scale);
    return color;
}

rgb_color_t rgb_blend(rgb_color_t a, rgb_color_t b, uint8_t alpha)
{
    /* alpha: 0 -> a, 255 -> b */
    uint16_t inv = (uint16_t)(255u - alpha);
    rgb_color_t out;

    out.r = (uint8_t)((((uint16_t)a.r * inv) + ((uint16_t)b.r * alpha)) / 255u);
    out.g = (uint8_t)((((uint16_t)a.g * inv) + ((uint16_t)b.g * alpha)) / 255u);
    out.b = (uint8_t)((((uint16_t)a.b * inv) + ((uint16_t)b.b * alpha)) / 255u);

    return out;
}

rgb_color_t rgb_add(rgb_color_t a, rgb_color_t b)
{
    uint16_t r = (uint16_t)a.r + (uint16_t)b.r;
    uint16_t g = (uint16_t)a.g + (uint16_t)b.g;
    uint16_t bl = (uint16_t)a.b + (uint16_t)b.b;
    rgb_color_t out;

    out.r = (uint8_t)(r > 255u ? 255u : r);
    out.g = (uint8_t)(g > 255u ? 255u : g);
    out.b = (uint8_t)(bl > 255u ? 255u : bl);

    return out;
}

rgb_color_t rgb_subtract(rgb_color_t a, rgb_color_t b)
{
    rgb_color_t out;

    out.r = (uint8_t)(a.r > b.r ? (a.r - b.r) : 0u);
    out.g = (uint8_t)(a.g > b.g ? (a.g - b.g) : 0u);
    out.b = (uint8_t)(a.b > b.b ? (a.b - b.b) : 0u);

    return out;
}

rgb_color_t hsv_to_rgb(hsv_color_t hsv)
{
    rgb_color_t out;
    uint16_t h;
    uint8_t s;
    uint8_t v;

    /* Clamp hue into 0..359 for stable sector math. */
    h = (uint16_t)(hsv.h % 360u);
    s = hsv.s;
    v = hsv.v;

    if (s == 0u)
    {
        out.r = v;
        out.g = v;
        out.b = v;
        return out;
    }

    {
        uint8_t sector = (uint8_t)(h / 60u);
        uint16_t remainder = (uint16_t)(h % 60u);

        uint8_t p = (uint8_t)(((uint16_t)v * (uint16_t)(255u - s)) / 255u);
        uint8_t q = (uint8_t)(((uint16_t)v * (uint16_t)(255u - ((uint16_t)s * remainder) / 60u)) / 255u);
        uint8_t t = (uint8_t)(((uint16_t)v * (uint16_t)(255u - ((uint16_t)s * (60u - remainder)) / 60u)) / 255u);

        switch (sector)
        {
            case 0u:
                out.r = v;
                out.g = t;
                out.b = p;
                break;

            case 1u:
                out.r = q;
                out.g = v;
                out.b = p;
                break;

            case 2u:
                out.r = p;
                out.g = v;
                out.b = t;
                break;

            case 3u:
                out.r = p;
                out.g = q;
                out.b = v;
                break;

            case 4u:
                out.r = t;
                out.g = p;
                out.b = v;
                break;

            default:
                out.r = v;
                out.g = p;
                out.b = q;
                break;
        }
    }

    return out;
}