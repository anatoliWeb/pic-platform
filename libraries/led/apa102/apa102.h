#ifndef LIBRARIES_LED_APA102_H
#define LIBRARIES_LED_APA102_H

#include "core/compiler.h"
#include "core/types.h"
#include "drivers/communication/spi/spi.h"
#include "libraries/led/rgb_utils/rgb_utils.h"

typedef struct
{
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t brightness;
} apa102_pixel_t;

typedef struct
{
    apa102_pixel_t* pixels;
    uint16_t led_count;

    /* Global scale: 255 = full, 0 = off. */
    uint8_t global_brightness;

    /* Default per-pixel brightness (0..31). */
    uint8_t default_pixel_brightness;

    uint8_t initialized;
} apa102_t;

uint8_t apa102_init(apa102_t* strip, apa102_pixel_t* buffer, uint16_t led_count);

void apa102_set_pixel(apa102_t* strip, uint16_t index, rgb_color_t color);
void apa102_set_all(apa102_t* strip, rgb_color_t color);
void apa102_clear(apa102_t* strip);
void apa102_show(apa102_t* strip);

void apa102_set_brightness(apa102_t* strip, uint8_t brightness);

#endif /* LIBRARIES_LED_APA102_H */
