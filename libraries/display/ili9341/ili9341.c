/*
 * File: libraries/display/ili9341/ili9341.c
 */

#include "libraries/display/ili9341/ili9341.h"

#include "core/delay.h"
#include "drivers/communication/spi/spi.h"
#include "drivers/gpio/gpio.h"
#include "libraries/display/graphics/graphics.h"

#define ILI9341_CMD_SWRESET 0x01u
#define ILI9341_CMD_SLPOUT  0x11u
#define ILI9341_CMD_DISPON  0x29u
#define ILI9341_CMD_CASET   0x2Au
#define ILI9341_CMD_PASET   0x2Bu
#define ILI9341_CMD_RAMWR   0x2Cu
#define ILI9341_CMD_MADCTL  0x36u
#define ILI9341_CMD_PIXFMT  0x3Au

#define ILI9341_MADCTL_MY   0x80u
#define ILI9341_MADCTL_MX   0x40u
#define ILI9341_MADCTL_MV   0x20u
#define ILI9341_MADCTL_BGR  0x08u

static void ili9341_df_draw_pixel(void* ctx, uint16_t x, uint16_t y, uint8_t color);

static void ili9341_select(const ili9341_t* display)
{
    gpio_write_low(display->cfg.cs_port, display->cfg.cs_pin);
}

static void ili9341_deselect(const ili9341_t* display)
{
    gpio_write_high(display->cfg.cs_port, display->cfg.cs_pin);
}

static void ili9341_dc_cmd(const ili9341_t* display)
{
    gpio_write_low(display->cfg.dc_port, display->cfg.dc_pin);
}

static void ili9341_dc_data(const ili9341_t* display)
{
    gpio_write_high(display->cfg.dc_port, display->cfg.dc_pin);
}

static void ili9341_write_cmd(const ili9341_t* display, uint8_t cmd)
{
    ili9341_select(display);
    ili9341_dc_cmd(display);
    (void)spi_transfer_byte(cmd);
    ili9341_deselect(display);
}

static void ili9341_write_data8(const ili9341_t* display, uint8_t data)
{
    ili9341_select(display);
    ili9341_dc_data(display);
    (void)spi_transfer_byte(data);
    ili9341_deselect(display);
}

static void ili9341_write_data16(const ili9341_t* display, uint16_t data)
{
    ili9341_select(display);
    ili9341_dc_data(display);
    (void)spi_transfer_byte((uint8_t)(data >> 8u));
    (void)spi_transfer_byte((uint8_t)(data & 0xFFu));
    ili9341_deselect(display);
}

static void ili9341_set_addr_window_raw(const ili9341_t* display, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    ili9341_write_cmd(display, ILI9341_CMD_CASET);
    ili9341_write_data16(display, x0);
    ili9341_write_data16(display, x1);

    ili9341_write_cmd(display, ILI9341_CMD_PASET);
    ili9341_write_data16(display, y0);
    ili9341_write_data16(display, y1);

    ili9341_write_cmd(display, ILI9341_CMD_RAMWR);
}

static void ili9341_reset(const ili9341_t* display)
{
    gpio_write_high(display->cfg.rst_port, display->cfg.rst_pin);
    DRV_DELAY_MS(5u);
    gpio_write_low(display->cfg.rst_port, display->cfg.rst_pin);
    DRV_DELAY_MS(10u);
    gpio_write_high(display->cfg.rst_port, display->cfg.rst_pin);
    DRV_DELAY_MS(120u);
}

static void ili9341_map_xy(const ili9341_t* display, uint16_t* x, uint16_t* y)
{
    uint16_t tx;
    uint16_t ty;

    tx = *x;
    ty = *y;

    switch (display->cfg.rotation)
    {
        case ILI9341_ROT_90:
            *x = (uint16_t)(display->cfg.height - 1u - ty);
            *y = tx;
            break;

        case ILI9341_ROT_180:
            *x = (uint16_t)(display->cfg.width - 1u - tx);
            *y = (uint16_t)(display->cfg.height - 1u - ty);
            break;

        case ILI9341_ROT_270:
            *x = ty;
            *y = (uint16_t)(display->cfg.width - 1u - tx);
            break;

        case ILI9341_ROT_0:
        default:
            break;
    }
}

uint8_t ili9341_init(ili9341_t* display, const ili9341_config_t* config)
{
    ili9341_config_t cfg;

    if (display == (ili9341_t*)0)
    {
        return 0u;
    }

    if (config == (const ili9341_config_t*)0)
    {
        cfg.cs_port = &PORTC;
        cfg.cs_tris = &TRISC;
        cfg.cs_pin = 2u;
        cfg.dc_port = &PORTC;
        cfg.dc_tris = &TRISC;
        cfg.dc_pin = 1u;
        cfg.rst_port = &PORTC;
        cfg.rst_tris = &TRISC;
        cfg.rst_pin = 0u;
        cfg.width = 240u;
        cfg.height = 320u;
        cfg.rotation = ILI9341_ROT_0;
        config = &cfg;
    }

    display->cfg = *config;
    display->width = display->cfg.width;
    display->height = display->cfg.height;
    display->initialized = 0u;

    gpio_set_output(display->cfg.cs_tris, display->cfg.cs_pin);
    gpio_set_output(display->cfg.dc_tris, display->cfg.dc_pin);
    gpio_set_output(display->cfg.rst_tris, display->cfg.rst_pin);

    ili9341_deselect(display);
    ili9341_dc_data(display);

    spi_init_master(SPI_MODE_0, SPI_CLOCK_FOSC_4);

    ili9341_reset(display);

    ili9341_write_cmd(display, ILI9341_CMD_SWRESET);
    DRV_DELAY_MS(150u);
    ili9341_write_cmd(display, ILI9341_CMD_SLPOUT);
    DRV_DELAY_MS(120u);

    ili9341_write_cmd(display, ILI9341_CMD_PIXFMT);
    ili9341_write_data8(display, 0x55u); /* 16-bit RGB565 */

    ili9341_set_rotation(display, display->cfg.rotation);

    ili9341_write_cmd(display, ILI9341_CMD_DISPON);
    DRV_DELAY_MS(50u);

    display->initialized = 1u;

    return 1u;
}

void ili9341_set_window(ili9341_t* display, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    if (display == (ili9341_t*)0)
    {
        return;
    }

    ili9341_set_addr_window_raw(display, x0, y0, x1, y1);
}

void ili9341_clear(ili9341_t* display, uint16_t color)
{
    ili9341_fill_rect(display, 0u, 0u, display->width, display->height, color);
}

void ili9341_update(ili9341_t* display)
{
    DRV_UNUSED(display);
    /* Direct-render mode: no full framebuffer flush required. */
}

void ili9341_draw_pixel(ili9341_t* display, uint16_t x, uint16_t y, uint16_t color)
{
    if ((display == (ili9341_t*)0) || (x >= display->width) || (y >= display->height))
    {
        return;
    }

    ili9341_map_xy(display, &x, &y);
    ili9341_set_addr_window_raw(display, x, y, x, y);

    ili9341_select(display);
    ili9341_dc_data(display);
    (void)spi_transfer_byte((uint8_t)(color >> 8u));
    (void)spi_transfer_byte((uint8_t)(color & 0xFFu));
    ili9341_deselect(display);
}

void ili9341_fill_rect(ili9341_t* display, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
    uint32_t count;
    uint16_t x1;
    uint16_t y1;

    if ((display == (ili9341_t*)0) || (width == 0u) || (height == 0u))
    {
        return;
    }

    if ((x >= display->width) || (y >= display->height))
    {
        return;
    }

    if ((uint32_t)x + width > display->width)
    {
        width = (uint16_t)(display->width - x);
    }

    if ((uint32_t)y + height > display->height)
    {
        height = (uint16_t)(display->height - y);
    }

    x1 = (uint16_t)(x + width - 1u);
    y1 = (uint16_t)(y + height - 1u);

    ili9341_set_addr_window_raw(display, x, y, x1, y1);

    count = (uint32_t)width * (uint32_t)height;
    ili9341_select(display);
    ili9341_dc_data(display);
    while (count-- > 0u)
    {
        (void)spi_transfer_byte((uint8_t)(color >> 8u));
        (void)spi_transfer_byte((uint8_t)(color & 0xFFu));
    }
    ili9341_deselect(display);
}

void ili9341_set_rotation(ili9341_t* display, ili9341_rotation_t rotation)
{
    uint8_t madctl;

    if (display == (ili9341_t*)0)
    {
        return;
    }

    display->cfg.rotation = rotation;

    switch (rotation)
    {
        case ILI9341_ROT_90:
            madctl = (uint8_t)(ILI9341_MADCTL_MV | ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR);
            display->width = display->cfg.height;
            display->height = display->cfg.width;
            break;

        case ILI9341_ROT_180:
            madctl = (uint8_t)(ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR);
            display->width = display->cfg.width;
            display->height = display->cfg.height;
            break;

        case ILI9341_ROT_270:
            madctl = (uint8_t)(ILI9341_MADCTL_MV | ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR);
            display->width = display->cfg.height;
            display->height = display->cfg.width;
            break;

        case ILI9341_ROT_0:
        default:
            madctl = (uint8_t)(ILI9341_MADCTL_BGR);
            display->width = display->cfg.width;
            display->height = display->cfg.height;
            break;
    }

    ili9341_write_cmd(display, ILI9341_CMD_MADCTL);
    ili9341_write_data8(display, madctl);
}

void ili9341_draw_bitmap(ili9341_t* display, uint16_t x, uint16_t y, const bitmap_t* bmp, uint16_t color)
{
    uint16_t ix;
    uint16_t iy;
    uint16_t bpr;
    uint16_t idx;
    uint8_t bit;

    if ((display == (ili9341_t*)0) || (bmp == (const bitmap_t*)0) || (bmp->data == (const uint8_t*)0) || (bmp->format != BITMAP_FORMAT_1BPP_ROW))
    {
        return;
    }

    bpr = (uint16_t)((bmp->width + 7u) >> 3u);

    for (iy = 0u; iy < bmp->height; iy++)
    {
        for (ix = 0u; ix < bmp->width; ix++)
        {
            idx = (uint16_t)(iy * bpr + (ix >> 3u));
            bit = (uint8_t)(0x80u >> (ix & 0x07u));
            if ((bmp->data[idx] & bit) != 0u)
            {
                ili9341_draw_pixel(display, (uint16_t)(x + ix), (uint16_t)(y + iy), color);
            }
        }
    }
}

void ili9341_blit_sprite(ili9341_t* display, uint16_t x, uint16_t y, const sprite_t* sprite, uint16_t color)
{
    uint16_t ix;
    uint16_t iy;
    uint16_t bpr;
    uint16_t idx;
    uint8_t bit;

    if ((display == (ili9341_t*)0) || (sprite == (const sprite_t*)0) || (sprite->framebuffer == (const uint8_t*)0) || (sprite->format != SPRITE_FORMAT_1BPP_ROW))
    {
        return;
    }

    bpr = (uint16_t)((sprite->width + 7u) >> 3u);

    for (iy = 0u; iy < sprite->height; iy++)
    {
        for (ix = 0u; ix < sprite->width; ix++)
        {
            idx = (uint16_t)(iy * bpr + (ix >> 3u));
            bit = (uint8_t)(0x80u >> (ix & 0x07u));
            if ((sprite->framebuffer[idx] & bit) != 0u)
            {
                ili9341_draw_pixel(display, (uint16_t)(x + ix), (uint16_t)(y + iy), color);
            }
        }
    }
}

static void ili9341_df_clear(void* ctx)
{
    ili9341_clear((ili9341_t*)ctx, ILI9341_COLOR_BLACK);
}

static void ili9341_df_update(void* ctx)
{
    ili9341_update((ili9341_t*)ctx);
}

static void ili9341_df_draw_pixel(void* ctx, uint16_t x, uint16_t y, uint8_t color)
{
    ili9341_draw_pixel((ili9341_t*)ctx, x, y, (color == 0u) ? ILI9341_COLOR_BLACK : ILI9341_COLOR_WHITE);
}

static void ili9341_df_draw_char(void* ctx, uint16_t x, uint16_t y, char c, uint8_t color)
{
    ili9341_t* display;
    graphics_t g;

    display = (ili9341_t*)ctx;
    if ((display == (ili9341_t*)0) ||
        (graphics_init(&g,
                       display->width,
                       display->height,
                       (uint8_t*)0,
                       0u,
                       GRAPHICS_FMT_CUSTOM,
                       (graphics_set_pixel_cb_t)ili9341_df_draw_pixel,
                       (graphics_get_pixel_cb_t)0,
                       (graphics_flush_cb_t)0,
                       (void*)display) == 0u))
    {
        return;
    }

    graphics_draw_char(&g, (int16_t)x, (int16_t)y, c, (uint8_t)(color != 0u ? 1u : 0u));
}

void ili9341_get_display_driver(display_driver_t* driver)
{
    if (driver == (display_driver_t*)0)
    {
        return;
    }

    driver->clear = ili9341_df_clear;
    driver->update = ili9341_df_update;
    driver->draw_pixel = ili9341_df_draw_pixel;
    driver->draw_char = ili9341_df_draw_char;
}
