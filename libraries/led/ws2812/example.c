/*
 * File: libraries/led/ws2812/example.c
 */

#include "core/compiler.h"
#include "core/delay.h"
#include "libraries/led/ws2812/ws2812.h"

#define WS2812_LED_COUNT 8u

static ws2812_color_t g_pixels[WS2812_LED_COUNT];

static ws2812_color_t make_color(uint8_t r, uint8_t g, uint8_t b)
{
    ws2812_color_t c;
    c.r = r;
    c.g = g;
    c.b = b;
    return c;
}

void main(void)
{
    ws2812_t strip;
    uint8_t i;

    (void)ws2812_init(&strip, &PORTC, &TRISC, 0u, g_pixels, WS2812_LED_COUNT);
    ws2812_set_brightness(&strip, 96u);

    while (1)
    {
        /* Color wipe: red. */
        ws2812_clear(&strip);
        for (i = 0u; i < WS2812_LED_COUNT; i++)
        {
            ws2812_set_pixel(&strip, i, make_color(255u, 0u, 0u));
            ws2812_show(&strip);
            DRV_DELAY_MS(80u);
        }

        /* Blinking all blue. */
        ws2812_set_all(&strip, make_color(0u, 0u, 255u));
        ws2812_show(&strip);
        DRV_DELAY_MS(200u);
        ws2812_clear(&strip);
        ws2812_show(&strip);
        DRV_DELAY_MS(200u);

        /* Simple rainbow cycle over 3 colors. */
        ws2812_set_all(&strip, make_color(255u, 0u, 0u));
        ws2812_show(&strip);
        DRV_DELAY_MS(150u);
        ws2812_set_all(&strip, make_color(0u, 255u, 0u));
        ws2812_show(&strip);
        DRV_DELAY_MS(150u);
        ws2812_set_all(&strip, make_color(0u, 0u, 255u));
        ws2812_show(&strip);
        DRV_DELAY_MS(150u);
    }
}