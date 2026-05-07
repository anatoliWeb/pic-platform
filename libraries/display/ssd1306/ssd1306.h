#ifndef LIBRARIES_DISPLAY_SSD1306_H
#define LIBRARIES_DISPLAY_SSD1306_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/display/display_framework/display_framework.h"

typedef enum
{
    SSD1306_SIZE_128X32 = 0,
    SSD1306_SIZE_128X64
} ssd1306_size_t;

typedef enum
{
    SSD1306_TRANSPORT_I2C = 0,
    SSD1306_TRANSPORT_SPI
} ssd1306_transport_t;

typedef struct
{
    ssd1306_size_t size;
    ssd1306_transport_t transport;
    uint8_t i2c_address;
    uint8_t contrast;
} ssd1306_config_t;

typedef struct
{
    ssd1306_config_t cfg;
    uint16_t width;
    uint16_t height;
    uint16_t buffer_size;
    uint8_t initialized;

    /* Max framebuffer for 128x64: 128 * 64 / 8 = 1024 bytes. */
    uint8_t framebuffer[1024];
} ssd1306_t;

uint8_t ssd1306_init(ssd1306_t* display, const ssd1306_config_t* config);

void ssd1306_clear(ssd1306_t* display);
void ssd1306_update(ssd1306_t* display);

void ssd1306_draw_pixel(ssd1306_t* display, uint16_t x, uint16_t y, uint8_t color);
void ssd1306_draw_char(ssd1306_t* display, uint16_t x, uint16_t y, char c, uint8_t color);
void ssd1306_draw_string(ssd1306_t* display, uint16_t x, uint16_t y, const char* str, uint8_t color);

void ssd1306_draw_line(ssd1306_t* display,
                       uint16_t x0,
                       uint16_t y0,
                       uint16_t x1,
                       uint16_t y1,
                       uint8_t color);

void ssd1306_draw_rect(ssd1306_t* display,
                       uint16_t x,
                       uint16_t y,
                       uint16_t width,
                       uint16_t height,
                       uint8_t color,
                       uint8_t filled);

void ssd1306_draw_bitmap(ssd1306_t* display,
                         uint16_t x,
                         uint16_t y,
                         uint16_t width,
                         uint16_t height,
                         const uint8_t* bitmap,
                         uint8_t color);

void ssd1306_get_display_driver(display_driver_t* driver);

#endif /* LIBRARIES_DISPLAY_SSD1306_H */
