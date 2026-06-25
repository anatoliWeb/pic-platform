/*
 * File: libraries/display/st7735/st7735.h
 */

#ifndef LIBRARIES_DISPLAY_ST7735_H
#define LIBRARIES_DISPLAY_ST7735_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/display/display_framework/display_framework.h"
#include "libraries/display/bitmap/bitmap.h"
#include "libraries/ui/sprite_buffer/sprite_buffer.h"

typedef enum
{
    ST7735_ROT_0 = 0,
    ST7735_ROT_90,
    ST7735_ROT_180,
    ST7735_ROT_270
} st7735_rotation_t;

typedef struct
{
    volatile uint8_t* cs_port;
    volatile uint8_t* cs_tris;
    uint8_t cs_pin;

    volatile uint8_t* dc_port;
    volatile uint8_t* dc_tris;
    uint8_t dc_pin;

    volatile uint8_t* rst_port;
    volatile uint8_t* rst_tris;
    uint8_t rst_pin;

    uint16_t width;
    uint16_t height;
    st7735_rotation_t rotation;
} st7735_config_t;

typedef struct
{
    st7735_config_t cfg;
    uint16_t width;
    uint16_t height;
    uint8_t initialized;
} st7735_t;

#define ST7735_COLOR565(r,g,b) (uint16_t)((((uint16_t)(r) & 0xF8u) << 8) | (((uint16_t)(g) & 0xFCu) << 3) | (((uint16_t)(b) & 0xF8u) >> 3))

#define ST7735_COLOR_BLACK   0x0000u
#define ST7735_COLOR_WHITE   0xFFFFu
#define ST7735_COLOR_RED     0xF800u
#define ST7735_COLOR_GREEN   0x07E0u
#define ST7735_COLOR_BLUE    0x001Fu
#define ST7735_COLOR_YELLOW  0xFFE0u
#define ST7735_COLOR_CYAN    0x07FFu
#define ST7735_COLOR_MAGENTA 0xF81Fu

uint8_t st7735_init(st7735_t* display, const st7735_config_t* config);

void st7735_clear(st7735_t* display, uint16_t color);
void st7735_update(st7735_t* display);

void st7735_draw_pixel(st7735_t* display, uint16_t x, uint16_t y, uint16_t color);
void st7735_fill_rect(st7735_t* display, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void st7735_set_rotation(st7735_t* display, st7735_rotation_t rotation);

void st7735_draw_bitmap(st7735_t* display, uint16_t x, uint16_t y, const bitmap_t* bmp, uint16_t color);
void st7735_blit_sprite(st7735_t* display, uint16_t x, uint16_t y, const sprite_t* sprite, uint16_t color);
void st7735_set_window(st7735_t* display, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

void st7735_get_display_driver(display_driver_t* driver);

#endif /* LIBRARIES_DISPLAY_ST7735_H */
