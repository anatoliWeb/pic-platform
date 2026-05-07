#ifndef LIBRARIES_DISPLAY_BITMAP_H
#define LIBRARIES_DISPLAY_BITMAP_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/display/graphics/graphics.h"

typedef enum
{
    BITMAP_FORMAT_1BPP_ROW = 0,
    BITMAP_FORMAT_UNKNOWN
} bitmap_format_t;

typedef struct
{
    uint16_t width;
    uint16_t height;
    bitmap_format_t format;
    const uint8_t* data;
} bitmap_t;

void bitmap_draw(graphics_t* g, int16_t x, int16_t y, const bitmap_t* bmp, uint8_t color);

void bitmap_draw_transparent(graphics_t* g,
                             int16_t x,
                             int16_t y,
                             const bitmap_t* bmp,
                             uint8_t color,
                             uint8_t transparent_bit);

void bitmap_draw_region(graphics_t* g,
                        int16_t x,
                        int16_t y,
                        const bitmap_t* bmp,
                        uint16_t src_x,
                        uint16_t src_y,
                        uint16_t region_width,
                        uint16_t region_height,
                        uint8_t color);

void bitmap_draw_scaled(graphics_t* g,
                        int16_t x,
                        int16_t y,
                        const bitmap_t* bmp,
                        uint8_t scale,
                        uint8_t color);

void bitmap_draw_inverted(graphics_t* g, int16_t x, int16_t y, const bitmap_t* bmp, uint8_t color);

#endif /* LIBRARIES_DISPLAY_BITMAP_H */
