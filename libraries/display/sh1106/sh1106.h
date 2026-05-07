#ifndef LIBRARIES_DISPLAY_SH1106_H
#define LIBRARIES_DISPLAY_SH1106_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/display/graphics/graphics.h"
#include "libraries/display/display_framework/display_framework.h"

typedef enum
{
    SH1106_TRANSPORT_I2C = 0,
    SH1106_TRANSPORT_SPI
} sh1106_transport_t;

typedef struct
{
    sh1106_transport_t transport;
    uint8_t i2c_address;
    uint8_t contrast;
    uint8_t x_offset;
} sh1106_config_t;

typedef struct
{
    sh1106_config_t cfg;
    uint16_t width;
    uint16_t height;
    uint16_t buffer_size;
    uint8_t initialized;

    uint8_t framebuffer[1024];
    graphics_t graphics;
} sh1106_t;

uint8_t sh1106_init(sh1106_t* display, const sh1106_config_t* config);

void sh1106_clear(sh1106_t* display);
void sh1106_update(sh1106_t* display);

void sh1106_draw_pixel(sh1106_t* display, uint16_t x, uint16_t y, uint8_t color);
void sh1106_draw_char(sh1106_t* display, uint16_t x, uint16_t y, char c, uint8_t color);
void sh1106_draw_string(sh1106_t* display, uint16_t x, uint16_t y, const char* str, uint8_t color);

void sh1106_draw_line(sh1106_t* display,
                      uint16_t x0,
                      uint16_t y0,
                      uint16_t x1,
                      uint16_t y1,
                      uint8_t color);

void sh1106_draw_rect(sh1106_t* display,
                      uint16_t x,
                      uint16_t y,
                      uint16_t width,
                      uint16_t height,
                      uint8_t color,
                      uint8_t filled);

void sh1106_get_display_driver(display_driver_t* driver);

#endif /* LIBRARIES_DISPLAY_SH1106_H */
