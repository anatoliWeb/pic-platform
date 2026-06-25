/*
 * File: libraries/display/bitmap/bitmap.c
 */

#include "libraries/display/bitmap/bitmap.h"

static uint8_t bitmap_get_1bpp_row_pixel(const bitmap_t* bmp, uint16_t x, uint16_t y)
{
    uint16_t bytes_per_row;
    uint16_t index;
    uint8_t bit;

    if ((bmp == (const bitmap_t*)0) || (bmp->data == (const uint8_t*)0))
    {
        return 0u;
    }

    if ((x >= bmp->width) || (y >= bmp->height))
    {
        return 0u;
    }

    bytes_per_row = (uint16_t)((bmp->width + 7u) >> 3u);
    index = (uint16_t)(y * bytes_per_row + (x >> 3u));
    bit = (uint8_t)(0x80u >> (x & 0x07u));

    return (uint8_t)((bmp->data[index] & bit) != 0u ? 1u : 0u);
}

static void bitmap_draw_internal(graphics_t* g,
                                 int16_t x,
                                 int16_t y,
                                 const bitmap_t* bmp,
                                 uint16_t src_x,
                                 uint16_t src_y,
                                 uint16_t region_width,
                                 uint16_t region_height,
                                 uint8_t color,
                                 uint8_t use_transparency,
                                 uint8_t transparent_bit,
                                 uint8_t invert_bits,
                                 uint8_t scale)
{
    uint16_t ix;
    uint16_t iy;
    uint8_t pixel;
    uint16_t sx;
    uint16_t sy;
    uint8_t dx;
    uint8_t dy;

    if ((g == (graphics_t*)0) || (bmp == (const bitmap_t*)0) || (bmp->data == (const uint8_t*)0))
    {
        return;
    }

    if (bmp->format != BITMAP_FORMAT_1BPP_ROW)
    {
        return;
    }

    if (scale == 0u)
    {
        scale = 1u;
    }

    for (iy = 0u; iy < region_height; iy++)
    {
        sy = (uint16_t)(src_y + iy);
        if (sy >= bmp->height)
        {
            break;
        }

        for (ix = 0u; ix < region_width; ix++)
        {
            sx = (uint16_t)(src_x + ix);
            if (sx >= bmp->width)
            {
                break;
            }

            pixel = bitmap_get_1bpp_row_pixel(bmp, sx, sy);

            if (invert_bits != 0u)
            {
                pixel = (uint8_t)(pixel == 0u ? 1u : 0u);
            }

            if ((use_transparency != 0u) && (pixel == transparent_bit))
            {
                continue;
            }

            if (pixel != 0u)
            {
                for (dy = 0u; dy < scale; dy++)
                {
                    for (dx = 0u; dx < scale; dx++)
                    {
                        graphics_draw_pixel(g,
                                            (int16_t)(x + (int16_t)(ix * scale + dx)),
                                            (int16_t)(y + (int16_t)(iy * scale + dy)),
                                            color);
                    }
                }
            }
        }
    }
}

void bitmap_draw(graphics_t* g, int16_t x, int16_t y, const bitmap_t* bmp, uint8_t color)
{
    if (bmp == (const bitmap_t*)0)
    {
        return;
    }

    bitmap_draw_internal(g,
                         x,
                         y,
                         bmp,
                         0u,
                         0u,
                         bmp->width,
                         bmp->height,
                         color,
                         0u,
                         0u,
                         0u,
                         1u);
}

void bitmap_draw_transparent(graphics_t* g,
                             int16_t x,
                             int16_t y,
                             const bitmap_t* bmp,
                             uint8_t color,
                             uint8_t transparent_bit)
{
    if (bmp == (const bitmap_t*)0)
    {
        return;
    }

    bitmap_draw_internal(g,
                         x,
                         y,
                         bmp,
                         0u,
                         0u,
                         bmp->width,
                         bmp->height,
                         color,
                         1u,
                         (uint8_t)(transparent_bit != 0u ? 1u : 0u),
                         0u,
                         1u);
}

void bitmap_draw_region(graphics_t* g,
                        int16_t x,
                        int16_t y,
                        const bitmap_t* bmp,
                        uint16_t src_x,
                        uint16_t src_y,
                        uint16_t region_width,
                        uint16_t region_height,
                        uint8_t color)
{
    bitmap_draw_internal(g,
                         x,
                         y,
                         bmp,
                         src_x,
                         src_y,
                         region_width,
                         region_height,
                         color,
                         0u,
                         0u,
                         0u,
                         1u);
}

void bitmap_draw_scaled(graphics_t* g,
                        int16_t x,
                        int16_t y,
                        const bitmap_t* bmp,
                        uint8_t scale,
                        uint8_t color)
{
    if (bmp == (const bitmap_t*)0)
    {
        return;
    }

    bitmap_draw_internal(g,
                         x,
                         y,
                         bmp,
                         0u,
                         0u,
                         bmp->width,
                         bmp->height,
                         color,
                         0u,
                         0u,
                         0u,
                         scale);
}

void bitmap_draw_inverted(graphics_t* g, int16_t x, int16_t y, const bitmap_t* bmp, uint8_t color)
{
    if (bmp == (const bitmap_t*)0)
    {
        return;
    }

    bitmap_draw_internal(g,
                         x,
                         y,
                         bmp,
                         0u,
                         0u,
                         bmp->width,
                         bmp->height,
                         color,
                         0u,
                         0u,
                         1u,
                         1u);
}
