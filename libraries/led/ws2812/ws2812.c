/*
 * File: libraries/led/ws2812/ws2812.c
 */

#include "libraries/led/ws2812/ws2812.h"

#if defined(DRV_COMPILER_C18)
    #include "../../../C18/libraries/led/ws2812/ws2812.c"
#elif defined(DRV_COMPILER_XC8)
    #include "../../../XC8/libraries/led/ws2812/ws2812.c"
#else

#include "core/delay.h"
#include "drivers/gpio/gpio.h"

static void ws2812_write_high(volatile uint8_t* port, uint8_t pin)
{
    *port = (uint8_t)(*port | (uint8_t)(1u << pin));
}

static void ws2812_write_low(volatile uint8_t* port, uint8_t pin)
{
    *port = (uint8_t)(*port & (uint8_t)(~(uint8_t)(1u << pin)));
}

/*
 * Timing placeholders for generic fallback.
 * Exact timing must be calibrated per MCU frequency/compiler.
 */
static void ws2812_delay_t0h(void) { volatile uint8_t i; for (i = 0u; i < 2u; i++) { } }
static void ws2812_delay_t0l(void) { volatile uint8_t i; for (i = 0u; i < 5u; i++) { } }
static void ws2812_delay_t1h(void) { volatile uint8_t i; for (i = 0u; i < 5u; i++) { } }
static void ws2812_delay_t1l(void) { volatile uint8_t i; for (i = 0u; i < 2u; i++) { } }

static void ws2812_send_bit(volatile uint8_t* port, uint8_t pin, uint8_t bit)
{
    if (bit != 0u)
    {
        ws2812_write_high(port, pin);
        ws2812_delay_t1h();
        ws2812_write_low(port, pin);
        ws2812_delay_t1l();
    }
    else
    {
        ws2812_write_high(port, pin);
        ws2812_delay_t0h();
        ws2812_write_low(port, pin);
        ws2812_delay_t0l();
    }
}

static void ws2812_send_byte(volatile uint8_t* port, uint8_t pin, uint8_t value)
{
    uint8_t mask;

    for (mask = 0x80u; mask != 0u; mask >>= 1u)
    {
        ws2812_send_bit(port, pin, (uint8_t)(value & mask));
    }
}

static uint8_t ws2812_scale(uint8_t value, uint8_t brightness)
{
    return (uint8_t)(((uint16_t)value * (uint16_t)brightness) / 255u);
}

uint8_t ws2812_init(ws2812_t* strip,
                    volatile uint8_t* port,
                    volatile uint8_t* tris,
                    uint8_t pin,
                    ws2812_color_t* buffer,
                    uint16_t led_count)
{
    if ((strip == (ws2812_t*)0) ||
        (port == (volatile uint8_t*)0) ||
        (tris == (volatile uint8_t*)0) ||
        (buffer == (ws2812_color_t*)0) ||
        (led_count == 0u))
    {
        return 0u;
    }

    strip->port = port;
    strip->tris = tris;
    strip->pin = pin;
    strip->pixels = buffer;
    strip->led_count = led_count;
    strip->brightness = 255u;

    gpio_set_output(strip->tris, strip->pin);
    gpio_write_low(strip->port, strip->pin);

    strip->initialized = 1u;
    ws2812_clear(strip);
    return 1u;
}

void ws2812_set_pixel(ws2812_t* strip, uint16_t index, ws2812_color_t color)
{
    if ((strip == (ws2812_t*)0) || (strip->initialized == 0u) || (index >= strip->led_count))
    {
        return;
    }

    strip->pixels[index] = color;
}

void ws2812_set_all(ws2812_t* strip, ws2812_color_t color)
{
    uint16_t i;

    if ((strip == (ws2812_t*)0) || (strip->initialized == 0u))
    {
        return;
    }

    for (i = 0u; i < strip->led_count; i++)
    {
        strip->pixels[i] = color;
    }
}

void ws2812_clear(ws2812_t* strip)
{
    ws2812_color_t black = { 0u, 0u, 0u };
    ws2812_set_all(strip, black);
}

void ws2812_set_brightness(ws2812_t* strip, uint8_t brightness)
{
    if ((strip == (ws2812_t*)0) || (strip->initialized == 0u))
    {
        return;
    }

    strip->brightness = brightness;
}

void ws2812_show(ws2812_t* strip)
{
    uint16_t i;

    if ((strip == (ws2812_t*)0) || (strip->initialized == 0u))
    {
        return;
    }

    DRV_INT_DISABLE();

    for (i = 0u; i < strip->led_count; i++)
    {
        uint8_t g = ws2812_scale(strip->pixels[i].g, strip->brightness);
        uint8_t r = ws2812_scale(strip->pixels[i].r, strip->brightness);
        uint8_t b = ws2812_scale(strip->pixels[i].b, strip->brightness);

        /* WS2812 uses GRB order. */
        ws2812_send_byte(strip->port, strip->pin, g);
        ws2812_send_byte(strip->port, strip->pin, r);
        ws2812_send_byte(strip->port, strip->pin, b);
    }

    DRV_INT_ENABLE();

    /* Reset latch time > 50us. */
    DRV_DELAY_US(80u);
}

#endif