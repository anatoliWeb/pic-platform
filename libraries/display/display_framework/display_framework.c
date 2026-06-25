/*
 * File: libraries/display/display_framework/display_framework.c
 */

#include "libraries/display/display_framework/display_framework.h"

typedef struct
{
    char symbol;
    uint8_t rows[7];
} display_glyph_5x7_t;

static const display_glyph_5x7_t g_basic_5x7[] =
{
    {' ', {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u}},
    {'-', {0x00u, 0x00u, 0x00u, 0x1Fu, 0x00u, 0x00u, 0x00u}},
    {'0', {0x0Eu, 0x11u, 0x13u, 0x15u, 0x19u, 0x11u, 0x0Eu}},
    {'1', {0x04u, 0x0Cu, 0x04u, 0x04u, 0x04u, 0x04u, 0x0Eu}},
    {'2', {0x0Eu, 0x11u, 0x01u, 0x02u, 0x04u, 0x08u, 0x1Fu}},
    {'3', {0x1Eu, 0x01u, 0x01u, 0x0Eu, 0x01u, 0x01u, 0x1Eu}},
    {'4', {0x02u, 0x06u, 0x0Au, 0x12u, 0x1Fu, 0x02u, 0x02u}},
    {'5', {0x1Fu, 0x10u, 0x10u, 0x1Eu, 0x01u, 0x01u, 0x1Eu}},
    {'6', {0x06u, 0x08u, 0x10u, 0x1Eu, 0x11u, 0x11u, 0x0Eu}},
    {'7', {0x1Fu, 0x01u, 0x02u, 0x04u, 0x08u, 0x08u, 0x08u}},
    {'8', {0x0Eu, 0x11u, 0x11u, 0x0Eu, 0x11u, 0x11u, 0x0Eu}},
    {'9', {0x0Eu, 0x11u, 0x11u, 0x0Fu, 0x01u, 0x02u, 0x1Cu}},
    {'A', {0x0Eu, 0x11u, 0x11u, 0x1Fu, 0x11u, 0x11u, 0x11u}},
    {'B', {0x1Eu, 0x11u, 0x11u, 0x1Eu, 0x11u, 0x11u, 0x1Eu}},
    {'C', {0x0Eu, 0x11u, 0x10u, 0x10u, 0x10u, 0x11u, 0x0Eu}},
    {'D', {0x1Cu, 0x12u, 0x11u, 0x11u, 0x11u, 0x12u, 0x1Cu}},
    {'E', {0x1Fu, 0x10u, 0x10u, 0x1Eu, 0x10u, 0x10u, 0x1Fu}},
    {'F', {0x1Fu, 0x10u, 0x10u, 0x1Eu, 0x10u, 0x10u, 0x10u}}
};

static const uint8_t* display_find_glyph(char c)
{
    uint16_t i;
    char uc;

    uc = c;
    if ((uc >= 'a') && (uc <= 'z'))
    {
        uc = (char)(uc - 'a' + 'A');
    }

    for (i = 0u; i < (uint16_t)(sizeof(g_basic_5x7) / sizeof(g_basic_5x7[0])); i++)
    {
        if (g_basic_5x7[i].symbol == uc)
        {
            return g_basic_5x7[i].rows;
        }
    }

    return g_basic_5x7[0].rows;
}

uint8_t display_init(display_t* display,
                     const display_driver_t* driver,
                     void* ctx,
                     uint16_t width,
                     uint16_t height)
{
    if ((display == (display_t*)0) || (driver == (const display_driver_t*)0))
    {
        return 0u;
    }

    display->driver = *driver;
    display->ctx = ctx;
    display->width = width;
    display->height = height;
    display->initialized = 1u;
    return 1u;
}

void display_clear(display_t* display)
{
    if ((display == (display_t*)0) || (display->initialized == 0u))
    {
        return;
    }

    if (display->driver.clear != (void (*)(void*))0)
    {
        display->driver.clear(display->ctx);
    }
}

void display_update(display_t* display)
{
    if ((display == (display_t*)0) || (display->initialized == 0u))
    {
        return;
    }

    if (display->driver.update != (void (*)(void*))0)
    {
        display->driver.update(display->ctx);
    }
}

void display_draw_pixel(display_t* display, uint16_t x, uint16_t y, uint8_t color)
{
    if ((display == (display_t*)0) || (display->initialized == 0u))
    {
        return;
    }

    if (display->driver.draw_pixel != (void (*)(void*, uint16_t, uint16_t, uint8_t))0)
    {
        display->driver.draw_pixel(display->ctx, x, y, color);
    }
}

void display_draw_char(display_t* display, uint16_t x, uint16_t y, char c)
{
    if ((display == (display_t*)0) || (display->initialized == 0u))
    {
        return;
    }

    if (display->driver.draw_char != (void (*)(void*, uint16_t, uint16_t, char, uint8_t))0)
    {
        display->driver.draw_char(display->ctx, x, y, c, 255u);
    }
}

void display_draw_string(display_t* display, uint16_t x, uint16_t y, const char* str)
{
    uint16_t cx;
    uint16_t cy;
    char c;

    if ((display == (display_t*)0) || (display->initialized == 0u) || (str == (const char*)0))
    {
        return;
    }

    cx = x;
    cy = y;

    while (*str != '\0')
    {
        c = *str++;

        if (c == '\n')
        {
            cx = x;
            cy = (uint16_t)(cy + 1u);
            continue;
        }

        display_draw_char(display, cx, cy, c);
        cx = (uint16_t)(cx + 1u);
    }
}

void display_draw_number(display_t* display, uint16_t x, uint16_t y, int32_t value)
{
    char buf[12];
    uint8_t i;
    uint8_t start;
    uint32_t v;

    if ((display == (display_t*)0) || (display->initialized == 0u))
    {
        return;
    }

    for (i = 0u; i < (uint8_t)sizeof(buf); i++)
    {
        buf[i] = '\0';
    }

    i = 0u;
    if (value < 0)
    {
        v = (uint32_t)(-value);
    }
    else
    {
        v = (uint32_t)value;
    }

    do
    {
        buf[i++] = (char)('0' + (char)(v % 10u));
        v /= 10u;
    } while ((v > 0u) && (i < (uint8_t)(sizeof(buf) - 1u)));

    if ((value < 0) && (i < (uint8_t)(sizeof(buf) - 1u)))
    {
        buf[i++] = '-';
    }

    start = 0u;
    while (start < (uint8_t)(i / 2u))
    {
        char t;
        t = buf[start];
        buf[start] = buf[(uint8_t)(i - 1u - start)];
        buf[(uint8_t)(i - 1u - start)] = t;
        start++;
    }

    display_draw_string(display, x, y, buf);
}

void display_draw_line(display_t* display,
                       uint16_t x0,
                       uint16_t y0,
                       uint16_t x1,
                       uint16_t y1,
                       uint8_t color)
{
    int16_t dx;
    int16_t dy;
    int16_t sx;
    int16_t sy;
    int16_t err;
    int16_t e2;
    int16_t ix0;
    int16_t iy0;
    int16_t ix1;
    int16_t iy1;

    ix0 = (int16_t)x0;
    iy0 = (int16_t)y0;
    ix1 = (int16_t)x1;
    iy1 = (int16_t)y1;

    dx = (int16_t)(ix1 > ix0 ? (ix1 - ix0) : (ix0 - ix1));
    dy = (int16_t)(iy1 > iy0 ? (iy0 - iy1) : (iy1 - iy0));
    sx = (int16_t)(ix0 < ix1 ? 1 : -1);
    sy = (int16_t)(iy0 < iy1 ? 1 : -1);
    err = (int16_t)(dx + dy);

    while (1)
    {
        if ((ix0 >= 0) && (iy0 >= 0))
        {
            display_draw_pixel(display, (uint16_t)ix0, (uint16_t)iy0, color);
        }

        if ((ix0 == ix1) && (iy0 == iy1))
        {
            break;
        }

        e2 = (int16_t)(err << 1);
        if (e2 >= dy)
        {
            err = (int16_t)(err + dy);
            ix0 = (int16_t)(ix0 + sx);
        }
        if (e2 <= dx)
        {
            err = (int16_t)(err + dx);
            iy0 = (int16_t)(iy0 + sy);
        }
    }
}

void display_draw_rect(display_t* display,
                       uint16_t x,
                       uint16_t y,
                       uint16_t width,
                       uint16_t height,
                       uint8_t color,
                       uint8_t filled)
{
    uint16_t i;
    uint16_t j;

    if ((width == 0u) || (height == 0u))
    {
        return;
    }

    if (filled != 0u)
    {
        for (j = 0u; j < height; j++)
        {
            for (i = 0u; i < width; i++)
            {
                display_draw_pixel(display, (uint16_t)(x + i), (uint16_t)(y + j), color);
            }
        }
        return;
    }

    for (i = 0u; i < width; i++)
    {
        display_draw_pixel(display, (uint16_t)(x + i), y, color);
        display_draw_pixel(display, (uint16_t)(x + i), (uint16_t)(y + height - 1u), color);
    }

    for (j = 0u; j < height; j++)
    {
        display_draw_pixel(display, x, (uint16_t)(y + j), color);
        display_draw_pixel(display, (uint16_t)(x + width - 1u), (uint16_t)(y + j), color);
    }
}

static void display_lcd_clear(void* ctx)
{
    (void)ctx;
    lcd_clear();
}

static void display_lcd_update(void* ctx)
{
    (void)ctx;
}

static void display_lcd_draw_pixel(void* ctx, uint16_t x, uint16_t y, uint8_t color)
{
    (void)ctx;
    (void)x;
    (void)y;
    (void)color;
}

static void display_lcd_draw_char(void* ctx, uint16_t x, uint16_t y, char c, uint8_t color)
{
    (void)ctx;
    (void)color;
    lcd_set_cursor((uint8_t)y, (uint8_t)x);
    lcd_write_char(c);
}

void display_backend_lcd_hd44780(display_driver_t* driver)
{
    if (driver == (display_driver_t*)0)
    {
        return;
    }

    driver->clear = display_lcd_clear;
    driver->update = display_lcd_update;
    driver->draw_pixel = display_lcd_draw_pixel;
    driver->draw_char = display_lcd_draw_char;
}

static void display_sevenseg_clear(void* ctx)
{
    seven_segment_t* display;
    display = (seven_segment_t*)ctx;
    seven_segment_clear(display);
}

static void display_sevenseg_update(void* ctx)
{
    (void)ctx;
}

static void display_sevenseg_draw_pixel(void* ctx, uint16_t x, uint16_t y, uint8_t color)
{
    (void)ctx;
    (void)x;
    (void)y;
    (void)color;
}

static void display_sevenseg_draw_char(void* ctx, uint16_t x, uint16_t y, char c, uint8_t color)
{
    seven_segment_t* display;
    (void)x;
    (void)y;
    (void)color;

    display = (seven_segment_t*)ctx;
    if ((c >= '0') && (c <= '9'))
    {
        seven_segment_set_digit(display, (uint8_t)(c - '0'));
    }
    else if ((c >= 'A') && (c <= 'F'))
    {
        seven_segment_set_hex(display, (uint8_t)(10u + (uint8_t)(c - 'A')));
    }
    else if ((c >= 'a') && (c <= 'f'))
    {
        seven_segment_set_hex(display, (uint8_t)(10u + (uint8_t)(c - 'a')));
    }
    else if (c == '-')
    {
        seven_segment_set_raw(display, 0x40u);
    }
    else
    {
        seven_segment_clear(display);
    }
}

void display_backend_seven_segment(display_driver_t* driver)
{
    if (driver == (display_driver_t*)0)
    {
        return;
    }

    driver->clear = display_sevenseg_clear;
    driver->update = display_sevenseg_update;
    driver->draw_pixel = display_sevenseg_draw_pixel;
    driver->draw_char = display_sevenseg_draw_char;
}

static void display_matrix_clear(void* ctx)
{
    ws2812_matrix_t* matrix;
    matrix = (ws2812_matrix_t*)ctx;
    ws2812_matrix_clear(matrix);
}

static void display_matrix_update(void* ctx)
{
    ws2812_matrix_t* matrix;
    matrix = (ws2812_matrix_t*)ctx;
    ws2812_matrix_show(matrix);
}

static void display_matrix_draw_pixel(void* ctx, uint16_t x, uint16_t y, uint8_t color)
{
    ws2812_matrix_t* matrix;
    matrix = (ws2812_matrix_t*)ctx;
    (void)ws2812_matrix_set_pixel(matrix, x, y, rgb_color(color, color, color));
}

static void display_matrix_draw_char(void* ctx, uint16_t x, uint16_t y, char c, uint8_t color)
{
    ws2812_matrix_t* matrix;
    const uint8_t* glyph;
    uint8_t row;
    uint8_t col;

    matrix = (ws2812_matrix_t*)ctx;
    glyph = display_find_glyph(c);

    for (row = 0u; row < 7u; row++)
    {
        for (col = 0u; col < 5u; col++)
        {
            if ((glyph[row] & (uint8_t)(1u << (4u - col))) != 0u)
            {
                (void)ws2812_matrix_set_pixel(matrix,
                                              (uint16_t)(x + col),
                                              (uint16_t)(y + row),
                                              rgb_color(color, color, color));
            }
        }
    }
}

void display_backend_ws2812_matrix(display_driver_t* driver)
{
    if (driver == (display_driver_t*)0)
    {
        return;
    }

    driver->clear = display_matrix_clear;
    driver->update = display_matrix_update;
    driver->draw_pixel = display_matrix_draw_pixel;
    driver->draw_char = display_matrix_draw_char;
}
