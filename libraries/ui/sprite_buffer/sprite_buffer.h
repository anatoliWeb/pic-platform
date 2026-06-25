/*
 * File: libraries/ui/sprite_buffer/sprite_buffer.h
 */

#ifndef LIBRARIES_UI_SPRITE_BUFFER_H
#define LIBRARIES_UI_SPRITE_BUFFER_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/display/graphics/graphics.h"
#include "libraries/display/bitmap/bitmap.h"

typedef enum
{
    SPRITE_FORMAT_1BPP_ROW = 0,
    SPRITE_FORMAT_FUTURE_RGB
} sprite_format_t;

typedef struct
{
    uint16_t width;
    uint16_t height;
    sprite_format_t format;
    uint8_t* framebuffer;
    uint16_t framebuffer_size;
} sprite_t;

uint8_t sprite_init(sprite_t* sprite,
                    uint16_t width,
                    uint16_t height,
                    sprite_format_t format,
                    uint8_t* framebuffer,
                    uint16_t framebuffer_size);

void sprite_clear(sprite_t* sprite, uint8_t color);
void sprite_draw_pixel(sprite_t* sprite, int16_t x, int16_t y, uint8_t color);
void sprite_fill_rect(sprite_t* sprite, int16_t x, int16_t y, int16_t width, int16_t height, uint8_t color);

void sprite_draw_bitmap(sprite_t* sprite, int16_t x, int16_t y, const bitmap_t* bmp, uint8_t color);
void sprite_draw_sprite(sprite_t* dst, int16_t x, int16_t y, const sprite_t* src);
void sprite_copy_region(sprite_t* dst,
                        int16_t dst_x,
                        int16_t dst_y,
                        const sprite_t* src,
                        uint16_t src_x,
                        uint16_t src_y,
                        uint16_t region_width,
                        uint16_t region_height);

void sprite_blit(const sprite_t* sprite, graphics_t* target, int16_t x, int16_t y, uint8_t color);
void sprite_blit_transparent(const sprite_t* sprite,
                             graphics_t* target,
                             int16_t x,
                             int16_t y,
                             uint8_t color,
                             uint8_t transparent_bit);

#endif /* LIBRARIES_UI_SPRITE_BUFFER_H */
