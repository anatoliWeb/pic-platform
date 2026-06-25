/*
 * File: libraries/display/ssd1306/ssd1306.c
 */

#include "libraries/display/ssd1306/ssd1306.h"

#include "drivers/communication/i2c/i2c.h"

/* 5x7 font, ASCII 0x20..0x7F. */
static const uint8_t g_font_5x7[96][5] =
{
    {0x00,0x00,0x00,0x00,0x00}, {0x00,0x00,0x5F,0x00,0x00}, {0x00,0x07,0x00,0x07,0x00}, {0x14,0x7F,0x14,0x7F,0x14},
    {0x24,0x2A,0x7F,0x2A,0x12}, {0x23,0x13,0x08,0x64,0x62}, {0x36,0x49,0x55,0x22,0x50}, {0x00,0x05,0x03,0x00,0x00},
    {0x00,0x1C,0x22,0x41,0x00}, {0x00,0x41,0x22,0x1C,0x00}, {0x14,0x08,0x3E,0x08,0x14}, {0x08,0x08,0x3E,0x08,0x08},
    {0x00,0x50,0x30,0x00,0x00}, {0x08,0x08,0x08,0x08,0x08}, {0x00,0x60,0x60,0x00,0x00}, {0x20,0x10,0x08,0x04,0x02},
    {0x3E,0x51,0x49,0x45,0x3E}, {0x00,0x42,0x7F,0x40,0x00}, {0x42,0x61,0x51,0x49,0x46}, {0x21,0x41,0x45,0x4B,0x31},
    {0x18,0x14,0x12,0x7F,0x10}, {0x27,0x45,0x45,0x45,0x39}, {0x3C,0x4A,0x49,0x49,0x30}, {0x01,0x71,0x09,0x05,0x03},
    {0x36,0x49,0x49,0x49,0x36}, {0x06,0x49,0x49,0x29,0x1E}, {0x00,0x36,0x36,0x00,0x00}, {0x00,0x56,0x36,0x00,0x00},
    {0x08,0x14,0x22,0x41,0x00}, {0x14,0x14,0x14,0x14,0x14}, {0x00,0x41,0x22,0x14,0x08}, {0x02,0x01,0x51,0x09,0x06},
    {0x32,0x49,0x79,0x41,0x3E}, {0x7E,0x11,0x11,0x11,0x7E}, {0x7F,0x49,0x49,0x49,0x36}, {0x3E,0x41,0x41,0x41,0x22},
    {0x7F,0x41,0x41,0x22,0x1C}, {0x7F,0x49,0x49,0x49,0x41}, {0x7F,0x09,0x09,0x09,0x01}, {0x3E,0x41,0x49,0x49,0x7A},
    {0x7F,0x08,0x08,0x08,0x7F}, {0x00,0x41,0x7F,0x41,0x00}, {0x20,0x40,0x41,0x3F,0x01}, {0x7F,0x08,0x14,0x22,0x41},
    {0x7F,0x40,0x40,0x40,0x40}, {0x7F,0x02,0x0C,0x02,0x7F}, {0x7F,0x04,0x08,0x10,0x7F}, {0x3E,0x41,0x41,0x41,0x3E},
    {0x7F,0x09,0x09,0x09,0x06}, {0x3E,0x41,0x51,0x21,0x5E}, {0x7F,0x09,0x19,0x29,0x46}, {0x46,0x49,0x49,0x49,0x31},
    {0x01,0x01,0x7F,0x01,0x01}, {0x3F,0x40,0x40,0x40,0x3F}, {0x1F,0x20,0x40,0x20,0x1F}, {0x3F,0x40,0x38,0x40,0x3F},
    {0x63,0x14,0x08,0x14,0x63}, {0x07,0x08,0x70,0x08,0x07}, {0x61,0x51,0x49,0x45,0x43}, {0x00,0x7F,0x41,0x41,0x00},
    {0x02,0x04,0x08,0x10,0x20}, {0x00,0x41,0x41,0x7F,0x00}, {0x04,0x02,0x01,0x02,0x04}, {0x40,0x40,0x40,0x40,0x40},
    {0x00,0x01,0x02,0x04,0x00}, {0x20,0x54,0x54,0x54,0x78}, {0x7F,0x48,0x44,0x44,0x38}, {0x38,0x44,0x44,0x44,0x20},
    {0x38,0x44,0x44,0x48,0x7F}, {0x38,0x54,0x54,0x54,0x18}, {0x08,0x7E,0x09,0x01,0x02}, {0x08,0x14,0x54,0x54,0x3C},
    {0x7F,0x08,0x04,0x04,0x78}, {0x00,0x44,0x7D,0x40,0x00}, {0x20,0x40,0x44,0x3D,0x00}, {0x7F,0x10,0x28,0x44,0x00},
    {0x00,0x41,0x7F,0x40,0x00}, {0x7C,0x04,0x18,0x04,0x78}, {0x7C,0x08,0x04,0x04,0x78}, {0x38,0x44,0x44,0x44,0x38},
    {0x7C,0x14,0x14,0x14,0x08}, {0x08,0x14,0x14,0x18,0x7C}, {0x7C,0x08,0x04,0x04,0x08}, {0x48,0x54,0x54,0x54,0x20},
    {0x04,0x3F,0x44,0x40,0x20}, {0x3C,0x40,0x40,0x20,0x7C}, {0x1C,0x20,0x40,0x20,0x1C}, {0x3C,0x40,0x30,0x40,0x3C},
    {0x44,0x28,0x10,0x28,0x44}, {0x0C,0x50,0x50,0x50,0x3C}, {0x44,0x64,0x54,0x4C,0x44}, {0x00,0x08,0x36,0x41,0x00},
    {0x00,0x00,0x7F,0x00,0x00}, {0x00,0x41,0x36,0x08,0x00}, {0x10,0x08,0x08,0x10,0x08}, {0x00,0x00,0x00,0x00,0x00}
};

static uint8_t ssd1306_send_command(const ssd1306_t* display, uint8_t cmd)
{
    if ((display == (const ssd1306_t*)0) || (display->cfg.transport != SSD1306_TRANSPORT_I2C))
    {
        return 0u;
    }

    i2c_start();
    if (i2c_write_byte((uint8_t)(display->cfg.i2c_address << 1u)) != 0u)
    {
        i2c_stop();
        return 0u;
    }
    if (i2c_write_byte(0x00u) != 0u)
    {
        i2c_stop();
        return 0u;
    }
    if (i2c_write_byte(cmd) != 0u)
    {
        i2c_stop();
        return 0u;
    }
    i2c_stop();

    return 1u;
}

static void ssd1306_send_data_page(const ssd1306_t* display, const uint8_t* data, uint8_t len)
{
    uint8_t i;

    if ((display == (const ssd1306_t*)0) || (data == (const uint8_t*)0) || (len == 0u))
    {
        return;
    }

    i2c_start();
    (void)i2c_write_byte((uint8_t)(display->cfg.i2c_address << 1u));
    (void)i2c_write_byte(0x40u);

    for (i = 0u; i < len; i++)
    {
        (void)i2c_write_byte(data[i]);
    }

    i2c_stop();
}

static uint8_t ssd1306_hw_init(ssd1306_t* display)
{
    uint8_t mux;
    uint8_t compins;

    if (display == (ssd1306_t*)0)
    {
        return 0u;
    }

    mux = (uint8_t)(display->height - 1u);
    compins = (display->height == 64u) ? 0x12u : 0x02u;

    if (ssd1306_send_command(display, 0xAEu) == 0u) return 0u;
    if (ssd1306_send_command(display, 0xD5u) == 0u) return 0u;
    if (ssd1306_send_command(display, 0x80u) == 0u) return 0u;
    if (ssd1306_send_command(display, 0xA8u) == 0u) return 0u;
    if (ssd1306_send_command(display, mux) == 0u) return 0u;
    if (ssd1306_send_command(display, 0xD3u) == 0u) return 0u;
    if (ssd1306_send_command(display, 0x00u) == 0u) return 0u;
    if (ssd1306_send_command(display, 0x40u) == 0u) return 0u;
    if (ssd1306_send_command(display, 0x8Du) == 0u) return 0u;
    if (ssd1306_send_command(display, 0x14u) == 0u) return 0u;
    if (ssd1306_send_command(display, 0x20u) == 0u) return 0u;
    if (ssd1306_send_command(display, 0x00u) == 0u) return 0u;
    if (ssd1306_send_command(display, 0xA1u) == 0u) return 0u;
    if (ssd1306_send_command(display, 0xC8u) == 0u) return 0u;
    if (ssd1306_send_command(display, 0xDAu) == 0u) return 0u;
    if (ssd1306_send_command(display, compins) == 0u) return 0u;
    if (ssd1306_send_command(display, 0x81u) == 0u) return 0u;
    if (ssd1306_send_command(display, display->cfg.contrast) == 0u) return 0u;
    if (ssd1306_send_command(display, 0xD9u) == 0u) return 0u;
    if (ssd1306_send_command(display, 0xF1u) == 0u) return 0u;
    if (ssd1306_send_command(display, 0xDBu) == 0u) return 0u;
    if (ssd1306_send_command(display, 0x40u) == 0u) return 0u;
    if (ssd1306_send_command(display, 0xA4u) == 0u) return 0u;
    if (ssd1306_send_command(display, 0xA6u) == 0u) return 0u;
    if (ssd1306_send_command(display, 0xAFu) == 0u) return 0u;

    return 1u;
}

uint8_t ssd1306_init(ssd1306_t* display, const ssd1306_config_t* config)
{
    ssd1306_config_t cfg;

    if (display == (ssd1306_t*)0)
    {
        return 0u;
    }

    if (config == (const ssd1306_config_t*)0)
    {
        cfg.size = SSD1306_SIZE_128X64;
        cfg.transport = SSD1306_TRANSPORT_I2C;
        cfg.i2c_address = 0x3Cu;
        cfg.contrast = 0x7Fu;
        config = &cfg;
    }

    display->cfg = *config;

    display->width = 128u;
    display->height = (display->cfg.size == SSD1306_SIZE_128X32) ? 32u : 64u;
    display->buffer_size = (uint16_t)(display->width * (display->height / 8u));
    display->initialized = 0u;

    ssd1306_clear(display);

    if (display->cfg.transport == SSD1306_TRANSPORT_I2C)
    {
        i2c_init(400000u);
    }

    if (ssd1306_hw_init(display) == 0u)
    {
        return 0u;
    }

    ssd1306_update(display);
    display->initialized = 1u;

    return 1u;
}

void ssd1306_clear(ssd1306_t* display)
{
    uint16_t i;

    if (display == (ssd1306_t*)0)
    {
        return;
    }

    for (i = 0u; i < 1024u; i++)
    {
        display->framebuffer[i] = 0x00u;
    }
}

void ssd1306_update(ssd1306_t* display)
{
    uint8_t page;
    uint16_t offset;

    if ((display == (ssd1306_t*)0) || (display->cfg.transport != SSD1306_TRANSPORT_I2C))
    {
        return;
    }

    for (page = 0u; page < (uint8_t)(display->height / 8u); page++)
    {
        (void)ssd1306_send_command(display, (uint8_t)(0xB0u + page));
        (void)ssd1306_send_command(display, 0x00u);
        (void)ssd1306_send_command(display, 0x10u);

        offset = (uint16_t)(page * display->width);
        ssd1306_send_data_page(display, &display->framebuffer[offset], (uint8_t)display->width);
    }
}

void ssd1306_draw_pixel(ssd1306_t* display, uint16_t x, uint16_t y, uint8_t color)
{
    uint16_t idx;
    uint8_t mask;

    if ((display == (ssd1306_t*)0) || (x >= display->width) || (y >= display->height))
    {
        return;
    }

    idx = (uint16_t)(x + ((y >> 3u) * display->width));
    mask = (uint8_t)(1u << (y & 0x07u));

    if (color != 0u)
    {
        display->framebuffer[idx] |= mask;
    }
    else
    {
        display->framebuffer[idx] &= (uint8_t)(~mask);
    }
}

void ssd1306_draw_char(ssd1306_t* display, uint16_t x, uint16_t y, char c, uint8_t color)
{
    uint8_t col;
    uint8_t row;
    uint8_t bits;
    uint8_t idx;

    if (display == (ssd1306_t*)0)
    {
        return;
    }

    if ((c < 32) || (c > 127))
    {
        c = '?';
    }

    idx = (uint8_t)(c - 32);

    for (col = 0u; col < 5u; col++)
    {
        bits = g_font_5x7[idx][col];

        for (row = 0u; row < 7u; row++)
        {
            if ((bits & (uint8_t)(1u << row)) != 0u)
            {
                ssd1306_draw_pixel(display, (uint16_t)(x + col), (uint16_t)(y + row), color);
            }
            else
            {
                ssd1306_draw_pixel(display, (uint16_t)(x + col), (uint16_t)(y + row), (uint8_t)(color == 0u ? 1u : 0u));
            }
        }
    }

    /* One empty column as character spacing. */
    for (row = 0u; row < 7u; row++)
    {
        ssd1306_draw_pixel(display, (uint16_t)(x + 5u), (uint16_t)(y + row), (uint8_t)(color == 0u ? 1u : 0u));
    }
}

void ssd1306_draw_string(ssd1306_t* display, uint16_t x, uint16_t y, const char* str, uint8_t color)
{
    uint16_t cx;

    if ((display == (ssd1306_t*)0) || (str == (const char*)0))
    {
        return;
    }

    cx = x;

    while (*str != '\0')
    {
        if ((uint16_t)(cx + 6u) > display->width)
        {
            break;
        }

        ssd1306_draw_char(display, cx, y, *str, color);
        cx = (uint16_t)(cx + 6u);
        str++;
    }
}

void ssd1306_draw_line(ssd1306_t* display,
                       uint16_t x0,
                       uint16_t y0,
                       uint16_t x1,
                       uint16_t y1,
                       uint8_t color)
{
    int16_t dx;
    int16_t sx;
    int16_t dy;
    int16_t sy;
    int16_t err;
    int16_t e2;

    if (display == (ssd1306_t*)0)
    {
        return;
    }

    dx = (int16_t)((x1 > x0) ? (x1 - x0) : (x0 - x1));
    sx = (x0 < x1) ? 1 : -1;
    dy = (int16_t)((y1 > y0) ? (y0 - y1) : (y1 - y0));
    sy = (y0 < y1) ? 1 : -1;
    err = (int16_t)(dx + dy);

    while (1)
    {
        ssd1306_draw_pixel(display, x0, y0, color);

        if ((x0 == x1) && (y0 == y1))
        {
            break;
        }

        e2 = (int16_t)(2 * err);

        if (e2 >= dy)
        {
            err = (int16_t)(err + dy);
            x0 = (uint16_t)((int16_t)x0 + sx);
        }

        if (e2 <= dx)
        {
            err = (int16_t)(err + dx);
            y0 = (uint16_t)((int16_t)y0 + sy);
        }
    }
}

void ssd1306_draw_rect(ssd1306_t* display,
                       uint16_t x,
                       uint16_t y,
                       uint16_t width,
                       uint16_t height,
                       uint8_t color,
                       uint8_t filled)
{
    uint16_t i;

    if ((display == (ssd1306_t*)0) || (width == 0u) || (height == 0u))
    {
        return;
    }

    if (filled != 0u)
    {
        for (i = 0u; i < height; i++)
        {
            ssd1306_draw_line(display, x, (uint16_t)(y + i), (uint16_t)(x + width - 1u), (uint16_t)(y + i), color);
        }
        return;
    }

    ssd1306_draw_line(display, x, y, (uint16_t)(x + width - 1u), y, color);
    ssd1306_draw_line(display, x, (uint16_t)(y + height - 1u), (uint16_t)(x + width - 1u), (uint16_t)(y + height - 1u), color);
    ssd1306_draw_line(display, x, y, x, (uint16_t)(y + height - 1u), color);
    ssd1306_draw_line(display, (uint16_t)(x + width - 1u), y, (uint16_t)(x + width - 1u), (uint16_t)(y + height - 1u), color);
}

void ssd1306_draw_bitmap(ssd1306_t* display,
                         uint16_t x,
                         uint16_t y,
                         uint16_t width,
                         uint16_t height,
                         const uint8_t* bitmap,
                         uint8_t color)
{
    uint16_t ix;
    uint16_t iy;
    uint16_t byte_index;
    uint8_t bit_index;
    uint8_t bit;

    if ((display == (ssd1306_t*)0) || (bitmap == (const uint8_t*)0))
    {
        return;
    }

    for (iy = 0u; iy < height; iy++)
    {
        for (ix = 0u; ix < width; ix++)
        {
            byte_index = (uint16_t)(ix + ((iy >> 3u) * width));
            bit_index = (uint8_t)(iy & 0x07u);
            bit = (uint8_t)((bitmap[byte_index] >> bit_index) & 0x01u);

            if (bit != 0u)
            {
                ssd1306_draw_pixel(display, (uint16_t)(x + ix), (uint16_t)(y + iy), color);
            }
        }
    }
}

static void ssd1306_df_clear(void* ctx)
{
    ssd1306_t* display = (ssd1306_t*)ctx;
    ssd1306_clear(display);
}

static void ssd1306_df_update(void* ctx)
{
    ssd1306_t* display = (ssd1306_t*)ctx;
    ssd1306_update(display);
}

static void ssd1306_df_draw_pixel(void* ctx, uint16_t x, uint16_t y, uint8_t color)
{
    ssd1306_t* display = (ssd1306_t*)ctx;
    ssd1306_draw_pixel(display, x, y, (uint8_t)(color != 0u ? 1u : 0u));
}

static void ssd1306_df_draw_char(void* ctx, uint16_t x, uint16_t y, char c, uint8_t color)
{
    ssd1306_t* display = (ssd1306_t*)ctx;
    ssd1306_draw_char(display, x, y, c, (uint8_t)(color != 0u ? 1u : 0u));
}

void ssd1306_get_display_driver(display_driver_t* driver)
{
    if (driver == (display_driver_t*)0)
    {
        return;
    }

    driver->clear = ssd1306_df_clear;
    driver->update = ssd1306_df_update;
    driver->draw_pixel = ssd1306_df_draw_pixel;
    driver->draw_char = ssd1306_df_draw_char;
}
