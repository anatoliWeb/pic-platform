/*
 * File: libraries/display/display_framework/display_framework.h
 */

#ifndef LIBRARIES_DISPLAY_DISPLAY_FRAMEWORK_H
#define LIBRARIES_DISPLAY_DISPLAY_FRAMEWORK_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/display/lcd_hd44780/lcd.h"
#include "libraries/display/seven_segment/seven_segment.h"
#include "libraries/led/ws2812_matrix/ws2812_matrix.h"
#include "libraries/led/rgb_utils/rgb_utils.h"

typedef struct
{
    void (*clear)(void* ctx);
    void (*update)(void* ctx);
    void (*draw_pixel)(void* ctx, uint16_t x, uint16_t y, uint8_t color);
    void (*draw_char)(void* ctx, uint16_t x, uint16_t y, char c, uint8_t color);
} display_driver_t;

typedef struct
{
    display_driver_t driver;
    void* ctx;
    uint16_t width;
    uint16_t height;
    uint8_t initialized;
} display_t;

uint8_t display_init(display_t* display,
                     const display_driver_t* driver,
                     void* ctx,
                     uint16_t width,
                     uint16_t height);

void display_clear(display_t* display);
void display_update(display_t* display);

void display_draw_pixel(display_t* display, uint16_t x, uint16_t y, uint8_t color);
void display_draw_char(display_t* display, uint16_t x, uint16_t y, char c);
void display_draw_string(display_t* display, uint16_t x, uint16_t y, const char* str);
void display_draw_number(display_t* display, uint16_t x, uint16_t y, int32_t value);

void display_draw_line(display_t* display,
                       uint16_t x0,
                       uint16_t y0,
                       uint16_t x1,
                       uint16_t y1,
                       uint8_t color);

void display_draw_rect(display_t* display,
                       uint16_t x,
                       uint16_t y,
                       uint16_t width,
                       uint16_t height,
                       uint8_t color,
                       uint8_t filled);

void display_backend_lcd_hd44780(display_driver_t* driver);
void display_backend_seven_segment(display_driver_t* driver);
void display_backend_ws2812_matrix(display_driver_t* driver);

#endif /* LIBRARIES_DISPLAY_DISPLAY_FRAMEWORK_H */
