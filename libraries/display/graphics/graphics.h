#ifndef LIBRARIES_DISPLAY_GRAPHICS_H
#define LIBRARIES_DISPLAY_GRAPHICS_H

#include "core/compiler.h"
#include "core/types.h"

typedef struct graphics_s graphics_t;

typedef void (*graphics_set_pixel_cb_t)(void* user_ctx, uint16_t x, uint16_t y, uint8_t color);
typedef uint8_t (*graphics_get_pixel_cb_t)(void* user_ctx, uint16_t x, uint16_t y);
typedef void (*graphics_flush_cb_t)(void* user_ctx);

typedef enum
{
    GRAPHICS_FMT_1BPP_VPAGE = 0,
    GRAPHICS_FMT_CUSTOM
} graphics_format_t;

struct graphics_s
{
    uint16_t width;
    uint16_t height;

    uint8_t* framebuffer;
    uint16_t framebuffer_size;
    graphics_format_t format;

    graphics_set_pixel_cb_t set_pixel_cb;
    graphics_get_pixel_cb_t get_pixel_cb;
    graphics_flush_cb_t flush_cb;

    void* user_ctx;
};

uint8_t graphics_init(graphics_t* g,
                      uint16_t width,
                      uint16_t height,
                      uint8_t* framebuffer,
                      uint16_t framebuffer_size,
                      graphics_format_t format,
                      graphics_set_pixel_cb_t set_cb,
                      graphics_get_pixel_cb_t get_cb,
                      graphics_flush_cb_t flush_cb,
                      void* user_ctx);

void graphics_draw_pixel(graphics_t* g, int16_t x, int16_t y, uint8_t color);
void graphics_draw_line(graphics_t* g, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color);
void graphics_draw_rect(graphics_t* g, int16_t x, int16_t y, int16_t width, int16_t height, uint8_t color);
void graphics_fill_rect(graphics_t* g, int16_t x, int16_t y, int16_t width, int16_t height, uint8_t color);
void graphics_draw_circle(graphics_t* g, int16_t cx, int16_t cy, int16_t radius, uint8_t color);

void graphics_draw_char(graphics_t* g, int16_t x, int16_t y, char c, uint8_t color);
void graphics_draw_string(graphics_t* g, int16_t x, int16_t y, const char* str, uint8_t color);

void graphics_draw_triangle(graphics_t* g,
                            int16_t x0,
                            int16_t y0,
                            int16_t x1,
                            int16_t y1,
                            int16_t x2,
                            int16_t y2,
                            uint8_t color);
void graphics_fill_circle(graphics_t* g, int16_t cx, int16_t cy, int16_t radius, uint8_t color);

void graphics_flush(graphics_t* g);

#endif /* LIBRARIES_DISPLAY_GRAPHICS_H */
