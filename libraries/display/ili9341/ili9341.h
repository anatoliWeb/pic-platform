/*
 * File: libraries/display/ili9341/ili9341.h
 */

#ifndef LIBRARIES_DISPLAY_ILI9341_H
#define LIBRARIES_DISPLAY_ILI9341_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/display/display_framework/display_framework.h"
#include "libraries/display/bitmap/bitmap.h"
#include "libraries/ui/sprite_buffer/sprite_buffer.h"

typedef enum
{
    ILI9341_ROT_0 = 0,
    ILI9341_ROT_90,
    ILI9341_ROT_180,
    ILI9341_ROT_270
} ili9341_rotation_t;

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
    ili9341_rotation_t rotation;
} ili9341_config_t;

typedef struct
{
    ili9341_config_t cfg;
    uint16_t width;
    uint16_t height;
    uint8_t initialized;
} ili9341_t;

#define ILI9341_COLOR565(r,g,b) (uint16_t)((((uint16_t)(r) & 0xF8u) << 8) | (((uint16_t)(g) & 0xFCu) << 3) | (((uint16_t)(b) & 0xF8u) >> 3))

#define ILI9341_COLOR_BLACK   0x0000u
#define ILI9341_COLOR_WHITE   0xFFFFu
#define ILI9341_COLOR_RED     0xF800u
#define ILI9341_COLOR_GREEN   0x07E0u
#define ILI9341_COLOR_BLUE    0x001Fu
#define ILI9341_COLOR_YELLOW  0xFFE0u
#define ILI9341_COLOR_CYAN    0x07FFu
#define ILI9341_COLOR_MAGENTA 0xF81Fu

uint8_t ili9341_init(ili9341_t* display, const ili9341_config_t* config);

void ili9341_clear(ili9341_t* display, uint16_t color);
void ili9341_update(ili9341_t* display);

void ili9341_draw_pixel(ili9341_t* display, uint16_t x, uint16_t y, uint16_t color);
void ili9341_fill_rect(ili9341_t* display, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void ili9341_set_rotation(ili9341_t* display, ili9341_rotation_t rotation);

void ili9341_draw_bitmap(ili9341_t* display, uint16_t x, uint16_t y, const bitmap_t* bmp, uint16_t color);
void ili9341_blit_sprite(ili9341_t* display, uint16_t x, uint16_t y, const sprite_t* sprite, uint16_t color);
void ili9341_set_window(ili9341_t* display, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

void ili9341_get_display_driver(display_driver_t* driver);

#endif /* LIBRARIES_DISPLAY_ILI9341_H */
