#include "libraries/ui/sprite_buffer/sprite_buffer.h"

static uint16_t sprite_bytes_per_row(const sprite_t* sprite)
{
    if (sprite == (const sprite_t*)0)
    {
        return 0u;
    }

    return (uint16_t)((sprite->width + 7u) >> 3u);
}

static uint8_t sprite_in_bounds(const sprite_t* sprite, int16_t x, int16_t y)
{
    if ((sprite == (const sprite_t*)0) || (x < 0) || (y < 0))
    {
        return 0u;
    }

    if (((uint16_t)x >= sprite->width) || ((uint16_t)y >= sprite->height))
    {
        return 0u;
    }

    return 1u;
}

static uint8_t sprite_get_pixel(const sprite_t* sprite, uint16_t x, uint16_t y)
{
    uint16_t bpr;
    uint16_t idx;
    uint8_t mask;

    if ((sprite == (const sprite_t*)0) || (sprite->framebuffer == (uint8_t*)0) ||
        (sprite->format != SPRITE_FORMAT_1BPP_ROW) || (x >= sprite->width) || (y >= sprite->height))
    {
        return 0u;
    }

    bpr = sprite_bytes_per_row(sprite);
    idx = (uint16_t)(y * bpr + (x >> 3u));
    if (idx >= sprite->framebuffer_size)
    {
        return 0u;
    }

    mask = (uint8_t)(0x80u >> (x & 0x07u));
    return (uint8_t)((sprite->framebuffer[idx] & mask) != 0u ? 1u : 0u);
}

uint8_t sprite_init(sprite_t* sprite,
                    uint16_t width,
                    uint16_t height,
                    sprite_format_t format,
                    uint8_t* framebuffer,
                    uint16_t framebuffer_size)
{
    uint16_t min_size;

    if ((sprite == (sprite_t*)0) || (framebuffer == (uint8_t*)0) || (width == 0u) || (height == 0u))
    {
        return 0u;
    }

    if (format != SPRITE_FORMAT_1BPP_ROW)
    {
        return 0u;
    }

    min_size = (uint16_t)(((width + 7u) >> 3u) * height);
    if (framebuffer_size < min_size)
    {
        return 0u;
    }

    sprite->width = width;
    sprite->height = height;
    sprite->format = format;
    sprite->framebuffer = framebuffer;
    sprite->framebuffer_size = framebuffer_size;

    sprite_clear(sprite, 0u);

    return 1u;
}

void sprite_clear(sprite_t* sprite, uint8_t color)
{
    uint16_t i;
    uint8_t fill;

    if ((sprite == (sprite_t*)0) || (sprite->framebuffer == (uint8_t*)0))
    {
        return;
    }

    fill = (uint8_t)(color != 0u ? 0xFFu : 0x00u);
    for (i = 0u; i < sprite->framebuffer_size; i++)
    {
        sprite->framebuffer[i] = fill;
    }
}

void sprite_draw_pixel(sprite_t* sprite, int16_t x, int16_t y, uint8_t color)
{
    uint16_t bpr;
    uint16_t idx;
    uint8_t mask;

    if (sprite_in_bounds(sprite, x, y) == 0u)
    {
        return;
    }

    bpr = sprite_bytes_per_row(sprite);
    idx = (uint16_t)((uint16_t)y * bpr + ((uint16_t)x >> 3u));
    if (idx >= sprite->framebuffer_size)
    {
        return;
    }

    mask = (uint8_t)(0x80u >> ((uint16_t)x & 0x07u));

    if (color != 0u)
    {
        sprite->framebuffer[idx] |= mask;
    }
    else
    {
        sprite->framebuffer[idx] &= (uint8_t)(~mask);
    }
}

void sprite_fill_rect(sprite_t* sprite, int16_t x, int16_t y, int16_t width, int16_t height, uint8_t color)
{
    int16_t ix;
    int16_t iy;

    if ((sprite == (sprite_t*)0) || (width <= 0) || (height <= 0))
    {
        return;
    }

    for (iy = 0; iy < height; iy++)
    {
        for (ix = 0; ix < width; ix++)
        {
            sprite_draw_pixel(sprite, (int16_t)(x + ix), (int16_t)(y + iy), color);
        }
    }
}

void sprite_draw_bitmap(sprite_t* sprite, int16_t x, int16_t y, const bitmap_t* bmp, uint8_t color)
{
    uint16_t ix;
    uint16_t iy;
    uint16_t bpr;
    uint16_t idx;
    uint8_t bit;

    if ((sprite == (sprite_t*)0) || (bmp == (const bitmap_t*)0) || (bmp->data == (const uint8_t*)0) ||
        (bmp->format != BITMAP_FORMAT_1BPP_ROW))
    {
        return;
    }

    bpr = (uint16_t)((bmp->width + 7u) >> 3u);

    for (iy = 0u; iy < bmp->height; iy++)
    {
        for (ix = 0u; ix < bmp->width; ix++)
        {
            idx = (uint16_t)(iy * bpr + (ix >> 3u));
            bit = (uint8_t)(0x80u >> (ix & 0x07u));

            if ((bmp->data[idx] & bit) != 0u)
            {
                sprite_draw_pixel(sprite, (int16_t)(x + (int16_t)ix), (int16_t)(y + (int16_t)iy), color);
            }
        }
    }
}

void sprite_draw_sprite(sprite_t* dst, int16_t x, int16_t y, const sprite_t* src)
{
    sprite_copy_region(dst, x, y, src, 0u, 0u, src != (const sprite_t*)0 ? src->width : 0u, src != (const sprite_t*)0 ? src->height : 0u);
}

void sprite_copy_region(sprite_t* dst,
                        int16_t dst_x,
                        int16_t dst_y,
                        const sprite_t* src,
                        uint16_t src_x,
                        uint16_t src_y,
                        uint16_t region_width,
                        uint16_t region_height)
{
    uint16_t ix;
    uint16_t iy;
    uint8_t p;

    if ((dst == (sprite_t*)0) || (src == (const sprite_t*)0))
    {
        return;
    }

    for (iy = 0u; iy < region_height; iy++)
    {
        if ((uint16_t)(src_y + iy) >= src->height)
        {
            break;
        }

        for (ix = 0u; ix < region_width; ix++)
        {
            if ((uint16_t)(src_x + ix) >= src->width)
            {
                break;
            }

            p = sprite_get_pixel(src, (uint16_t)(src_x + ix), (uint16_t)(src_y + iy));
            sprite_draw_pixel(dst, (int16_t)(dst_x + (int16_t)ix), (int16_t)(dst_y + (int16_t)iy), p);
        }
    }
}

void sprite_blit(const sprite_t* sprite, graphics_t* target, int16_t x, int16_t y, uint8_t color)
{
    uint16_t ix;
    uint16_t iy;

    if ((sprite == (const sprite_t*)0) || (target == (graphics_t*)0))
    {
        return;
    }

    for (iy = 0u; iy < sprite->height; iy++)
    {
        for (ix = 0u; ix < sprite->width; ix++)
        {
            if (sprite_get_pixel(sprite, ix, iy) != 0u)
            {
                graphics_draw_pixel(target, (int16_t)(x + (int16_t)ix), (int16_t)(y + (int16_t)iy), color);
            }
        }
    }
}

void sprite_blit_transparent(const sprite_t* sprite,
                             graphics_t* target,
                             int16_t x,
                             int16_t y,
                             uint8_t color,
                             uint8_t transparent_bit)
{
    uint16_t ix;
    uint16_t iy;
    uint8_t p;

    if ((sprite == (const sprite_t*)0) || (target == (graphics_t*)0))
    {
        return;
    }

    transparent_bit = (uint8_t)(transparent_bit != 0u ? 1u : 0u);

    for (iy = 0u; iy < sprite->height; iy++)
    {
        for (ix = 0u; ix < sprite->width; ix++)
        {
            p = sprite_get_pixel(sprite, ix, iy);
            if (p == transparent_bit)
            {
                continue;
            }

            if (p != 0u)
            {
                graphics_draw_pixel(target, (int16_t)(x + (int16_t)ix), (int16_t)(y + (int16_t)iy), color);
            }
        }
    }
}
