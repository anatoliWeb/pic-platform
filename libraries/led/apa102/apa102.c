/*
 * File: libraries/led/apa102/apa102.c
 */

#include "libraries/led/apa102/apa102.h"

static uint8_t apa102_scale_u8(uint8_t value, uint8_t scale)
{
    uint16_t v;
    v = (uint16_t)value * (uint16_t)scale;
    return (uint8_t)(v / 255u);
}

static uint8_t apa102_clamp_brightness_5bit(uint8_t brightness)
{
    if (brightness > 31u)
    {
        return 31u;
    }
    return brightness;
}

uint8_t apa102_init(apa102_t* strip, apa102_pixel_t* buffer, uint16_t led_count)
{
    uint16_t i;

    if ((strip == (apa102_t*)0) || (buffer == (apa102_pixel_t*)0) || (led_count == 0u))
    {
        return 0u;
    }

    strip->pixels = buffer;
    strip->led_count = led_count;
    strip->global_brightness = 255u;
    strip->default_pixel_brightness = 31u;
    strip->initialized = 1u;

    for (i = 0u; i < led_count; i++)
    {
        strip->pixels[i].r = 0u;
        strip->pixels[i].g = 0u;
        strip->pixels[i].b = 0u;
        strip->pixels[i].brightness = strip->default_pixel_brightness;
    }

    return 1u;
}

void apa102_set_pixel(apa102_t* strip, uint16_t index, rgb_color_t color)
{
    if ((strip == (apa102_t*)0) || (strip->initialized == 0u))
    {
        return;
    }

    if (index >= strip->led_count)
    {
        return;
    }

    strip->pixels[index].r = color.r;
    strip->pixels[index].g = color.g;
    strip->pixels[index].b = color.b;
    strip->pixels[index].brightness = strip->default_pixel_brightness;
}

void apa102_set_all(apa102_t* strip, rgb_color_t color)
{
    uint16_t i;

    if ((strip == (apa102_t*)0) || (strip->initialized == 0u))
    {
        return;
    }

    for (i = 0u; i < strip->led_count; i++)
    {
        strip->pixels[i].r = color.r;
        strip->pixels[i].g = color.g;
        strip->pixels[i].b = color.b;
        strip->pixels[i].brightness = strip->default_pixel_brightness;
    }
}

void apa102_clear(apa102_t* strip)
{
    uint16_t i;

    if ((strip == (apa102_t*)0) || (strip->initialized == 0u))
    {
        return;
    }

    for (i = 0u; i < strip->led_count; i++)
    {
        strip->pixels[i].r = 0u;
        strip->pixels[i].g = 0u;
        strip->pixels[i].b = 0u;
    }
}

void apa102_set_brightness(apa102_t* strip, uint8_t brightness)
{
    if ((strip == (apa102_t*)0) || (strip->initialized == 0u))
    {
        return;
    }

    strip->global_brightness = brightness;
}

void apa102_show(apa102_t* strip)
{
    uint16_t i;
    uint16_t end_bytes;
    uint8_t frame_brightness;
    uint8_t scaled_r;
    uint8_t scaled_g;
    uint8_t scaled_b;

    if ((strip == (apa102_t*)0) || (strip->initialized == 0u))
    {
        return;
    }

    /* Start frame: 32 zero bits. */
    spi_transfer_byte(0x00u);
    spi_transfer_byte(0x00u);
    spi_transfer_byte(0x00u);
    spi_transfer_byte(0x00u);

    for (i = 0u; i < strip->led_count; i++)
    {
        frame_brightness = apa102_clamp_brightness_5bit(strip->pixels[i].brightness);
        scaled_r = apa102_scale_u8(strip->pixels[i].r, strip->global_brightness);
        scaled_g = apa102_scale_u8(strip->pixels[i].g, strip->global_brightness);
        scaled_b = apa102_scale_u8(strip->pixels[i].b, strip->global_brightness);

        /* LED frame: 0b111xxxxx + B + G + R. */
        spi_transfer_byte((uint8_t)(0xE0u | frame_brightness));
        spi_transfer_byte(scaled_b);
        spi_transfer_byte(scaled_g);
        spi_transfer_byte(scaled_r);
    }

    /*
     * End frame: at least (led_count / 2) clock bits.
     * Sending (led_count + 15) / 16 bytes covers common APA102 chains.
     */
    end_bytes = (uint16_t)((strip->led_count + 15u) / 16u);
    for (i = 0u; i < end_bytes; i++)
    {
        spi_transfer_byte(0xFFu);
    }
}
