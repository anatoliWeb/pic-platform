/*
 * File: libraries/led/ws2812/ws2812.h
 */

#ifndef LIBRARIES_LED_WS2812_H
#define LIBRARIES_LED_WS2812_H

#include "core/compiler.h"
#include "core/types.h"

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} ws2812_color_t;

typedef struct
{
    volatile uint8_t* port;
    volatile uint8_t* tris;
    uint8_t pin;

    ws2812_color_t* pixels;
    uint16_t led_count;

    /* 255 = full brightness, 0 = off. */
    uint8_t brightness;

    uint8_t initialized;
} ws2812_t;

uint8_t ws2812_init(ws2812_t* strip,
                    volatile uint8_t* port,
                    volatile uint8_t* tris,
                    uint8_t pin,
                    ws2812_color_t* buffer,
                    uint16_t led_count);

void ws2812_set_pixel(ws2812_t* strip, uint16_t index, ws2812_color_t color);
void ws2812_set_all(ws2812_t* strip, ws2812_color_t color);
void ws2812_clear(ws2812_t* strip);
void ws2812_set_brightness(ws2812_t* strip, uint8_t brightness);

void ws2812_show(ws2812_t* strip);

#endif /* LIBRARIES_LED_WS2812_H */