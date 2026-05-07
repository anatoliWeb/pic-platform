#include "libraries/display/graphics/graphics.h"

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

static uint8_t graphics_in_bounds(const graphics_t* g, int16_t x, int16_t y)
{
    if ((g == (const graphics_t*)0) || (x < 0) || (y < 0))
    {
        return 0u;
    }

    if (((uint16_t)x >= g->width) || ((uint16_t)y >= g->height))
    {
        return 0u;
    }

    return 1u;
}

static void graphics_set_pixel_fb(graphics_t* g, uint16_t x, uint16_t y, uint8_t color)
{
    uint16_t idx;
    uint8_t mask;

    if ((g == (graphics_t*)0) || (g->framebuffer == (uint8_t*)0) || (g->format != GRAPHICS_FMT_1BPP_VPAGE))
    {
        return;
    }

    idx = (uint16_t)(x + ((y >> 3u) * g->width));
    if (idx >= g->framebuffer_size)
    {
        return;
    }

    mask = (uint8_t)(1u << (y & 0x07u));

    if (color != 0u)
    {
        g->framebuffer[idx] |= mask;
    }
    else
    {
        g->framebuffer[idx] &= (uint8_t)(~mask);
    }
}

uint8_t graphics_init(graphics_t* g,
                      uint16_t width,
                      uint16_t height,
                      uint8_t* framebuffer,
                      uint16_t framebuffer_size,
                      graphics_format_t format,
                      graphics_set_pixel_cb_t set_cb,
                      graphics_get_pixel_cb_t get_cb,
                      graphics_flush_cb_t flush_cb,
                      void* user_ctx)
{
    if (g == (graphics_t*)0)
    {
        return 0u;
    }

    g->width = width;
    g->height = height;
    g->framebuffer = framebuffer;
    g->framebuffer_size = framebuffer_size;
    g->format = format;
    g->set_pixel_cb = set_cb;
    g->get_pixel_cb = get_cb;
    g->flush_cb = flush_cb;
    g->user_ctx = user_ctx;

    if ((g->set_pixel_cb == (graphics_set_pixel_cb_t)0) &&
        !((g->framebuffer != (uint8_t*)0) && (g->format == GRAPHICS_FMT_1BPP_VPAGE)))
    {
        return 0u;
    }

    return 1u;
}

void graphics_draw_pixel(graphics_t* g, int16_t x, int16_t y, uint8_t color)
{
    if (graphics_in_bounds(g, x, y) == 0u)
    {
        return;
    }

    if (g->set_pixel_cb != (graphics_set_pixel_cb_t)0)
    {
        g->set_pixel_cb(g->user_ctx, (uint16_t)x, (uint16_t)y, (uint8_t)(color != 0u ? 1u : 0u));
    }
    else
    {
        graphics_set_pixel_fb(g, (uint16_t)x, (uint16_t)y, color);
    }
}

void graphics_draw_line(graphics_t* g, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color)
{
    int16_t dx;
    int16_t sx;
    int16_t dy;
    int16_t sy;
    int16_t err;
    int16_t e2;

    dx = (int16_t)((x1 > x0) ? (x1 - x0) : (x0 - x1));
    sx = (x0 < x1) ? 1 : -1;
    dy = (int16_t)((y1 > y0) ? (y0 - y1) : (y1 - y0));
    sy = (y0 < y1) ? 1 : -1;
    err = (int16_t)(dx + dy);

    while (1)
    {
        graphics_draw_pixel(g, x0, y0, color);

        if ((x0 == x1) && (y0 == y1))
        {
            break;
        }

        e2 = (int16_t)(2 * err);

        if (e2 >= dy)
        {
            err = (int16_t)(err + dy);
            x0 = (int16_t)(x0 + sx);
        }

        if (e2 <= dx)
        {
            err = (int16_t)(err + dx);
            y0 = (int16_t)(y0 + sy);
        }
    }
}

void graphics_draw_rect(graphics_t* g, int16_t x, int16_t y, int16_t width, int16_t height, uint8_t color)
{
    if ((width <= 0) || (height <= 0))
    {
        return;
    }

    graphics_draw_line(g, x, y, (int16_t)(x + width - 1), y, color);
    graphics_draw_line(g, x, (int16_t)(y + height - 1), (int16_t)(x + width - 1), (int16_t)(y + height - 1), color);
    graphics_draw_line(g, x, y, x, (int16_t)(y + height - 1), color);
    graphics_draw_line(g, (int16_t)(x + width - 1), y, (int16_t)(x + width - 1), (int16_t)(y + height - 1), color);
}

void graphics_fill_rect(graphics_t* g, int16_t x, int16_t y, int16_t width, int16_t height, uint8_t color)
{
    int16_t row;

    if ((width <= 0) || (height <= 0))
    {
        return;
    }

    for (row = 0; row < height; row++)
    {
        graphics_draw_line(g, x, (int16_t)(y + row), (int16_t)(x + width - 1), (int16_t)(y + row), color);
    }
}

void graphics_draw_circle(graphics_t* g, int16_t cx, int16_t cy, int16_t radius, uint8_t color)
{
    int16_t x;
    int16_t y;
    int16_t d;

    if (radius < 0)
    {
        return;
    }

    x = radius;
    y = 0;
    d = 1 - radius;

    while (x >= y)
    {
        graphics_draw_pixel(g, (int16_t)(cx + x), (int16_t)(cy + y), color);
        graphics_draw_pixel(g, (int16_t)(cx + y), (int16_t)(cy + x), color);
        graphics_draw_pixel(g, (int16_t)(cx - y), (int16_t)(cy + x), color);
        graphics_draw_pixel(g, (int16_t)(cx - x), (int16_t)(cy + y), color);
        graphics_draw_pixel(g, (int16_t)(cx - x), (int16_t)(cy - y), color);
        graphics_draw_pixel(g, (int16_t)(cx - y), (int16_t)(cy - x), color);
        graphics_draw_pixel(g, (int16_t)(cx + y), (int16_t)(cy - x), color);
        graphics_draw_pixel(g, (int16_t)(cx + x), (int16_t)(cy - y), color);

        y++;
        if (d <= 0)
        {
            d = (int16_t)(d + (2 * y) + 1);
        }
        else
        {
            x--;
            d = (int16_t)(d + (2 * (y - x)) + 1);
        }
    }
}

void graphics_draw_char(graphics_t* g, int16_t x, int16_t y, char c, uint8_t color)
{
    uint8_t col;
    uint8_t row;
    uint8_t bits;
    uint8_t idx;

    if (g == (graphics_t*)0)
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
                graphics_draw_pixel(g, (int16_t)(x + col), (int16_t)(y + row), color);
            }
        }
    }
}

void graphics_draw_string(graphics_t* g, int16_t x, int16_t y, const char* str, uint8_t color)
{
    int16_t cx;

    if ((g == (graphics_t*)0) || (str == (const char*)0))
    {
        return;
    }

    cx = x;

    while (*str != '\0')
    {
        graphics_draw_char(g, cx, y, *str, color);
        cx = (int16_t)(cx + 6);
        str++;
    }
}

void graphics_draw_triangle(graphics_t* g,
                            int16_t x0,
                            int16_t y0,
                            int16_t x1,
                            int16_t y1,
                            int16_t x2,
                            int16_t y2,
                            uint8_t color)
{
    graphics_draw_line(g, x0, y0, x1, y1, color);
    graphics_draw_line(g, x1, y1, x2, y2, color);
    graphics_draw_line(g, x2, y2, x0, y0, color);
}

void graphics_fill_circle(graphics_t* g, int16_t cx, int16_t cy, int16_t radius, uint8_t color)
{
    int16_t x;
    int16_t y;
    int16_t d;

    if (radius < 0)
    {
        return;
    }

    x = radius;
    y = 0;
    d = 1 - radius;

    while (x >= y)
    {
        graphics_draw_line(g, (int16_t)(cx - x), (int16_t)(cy + y), (int16_t)(cx + x), (int16_t)(cy + y), color);
        graphics_draw_line(g, (int16_t)(cx - x), (int16_t)(cy - y), (int16_t)(cx + x), (int16_t)(cy - y), color);
        graphics_draw_line(g, (int16_t)(cx - y), (int16_t)(cy + x), (int16_t)(cx + y), (int16_t)(cy + x), color);
        graphics_draw_line(g, (int16_t)(cx - y), (int16_t)(cy - x), (int16_t)(cx + y), (int16_t)(cy - x), color);

        y++;
        if (d <= 0)
        {
            d = (int16_t)(d + (2 * y) + 1);
        }
        else
        {
            x--;
            d = (int16_t)(d + (2 * (y - x)) + 1);
        }
    }
}

void graphics_flush(graphics_t* g)
{
    if ((g == (graphics_t*)0) || (g->flush_cb == (graphics_flush_cb_t)0))
    {
        return;
    }

    g->flush_cb(g->user_ctx);
}
