/*
 * File: libraries/display/st7735/st7735.c
 */

#include "libraries/display/st7735/st7735.h"

#include "core/delay.h"
#include "drivers/communication/spi/spi.h"
#include "drivers/gpio/gpio.h"
#include "libraries/display/graphics/graphics.h"

#define ST7735_CMD_SWRESET 0x01u
#define ST7735_CMD_SLPOUT  0x11u
#define ST7735_CMD_COLMOD  0x3Au
#define ST7735_CMD_MADCTL  0x36u
#define ST7735_CMD_CASET   0x2Au
#define ST7735_CMD_RASET   0x2Bu
#define ST7735_CMD_RAMWR   0x2Cu
#define ST7735_CMD_DISPON  0x29u

#define ST7735_MADCTL_MY   0x80u
#define ST7735_MADCTL_MX   0x40u
#define ST7735_MADCTL_MV   0x20u
#define ST7735_MADCTL_BGR  0x08u

static void st7735_select(const st7735_t* display)
{
    gpio_write_low(display->cfg.cs_port, display->cfg.cs_pin);
}

static void st7735_deselect(const st7735_t* display)
{
    gpio_write_high(display->cfg.cs_port, display->cfg.cs_pin);
}

static void st7735_dc_cmd(const st7735_t* display)
{
    gpio_write_low(display->cfg.dc_port, display->cfg.dc_pin);
}

static void st7735_dc_data(const st7735_t* display)
{
    gpio_write_high(display->cfg.dc_port, display->cfg.dc_pin);
}

static void st7735_write_cmd(const st7735_t* display, uint8_t cmd)
{
    st7735_select(display);
    st7735_dc_cmd(display);
    (void)spi_transfer_byte(cmd);
    st7735_deselect(display);
}

static void st7735_write_data8(const st7735_t* display, uint8_t data)
{
    st7735_select(display);
    st7735_dc_data(display);
    (void)spi_transfer_byte(data);
    st7735_deselect(display);
}

static void st7735_write_data16(const st7735_t* display, uint16_t data)
{
    st7735_select(display);
    st7735_dc_data(display);
    (void)spi_transfer_byte((uint8_t)(data >> 8u));
    (void)spi_transfer_byte((uint8_t)(data & 0xFFu));
    st7735_deselect(display);
}

static void st7735_set_addr_window_raw(const st7735_t* display, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    st7735_write_cmd(display, ST7735_CMD_CASET);
    st7735_write_data16(display, x0);
    st7735_write_data16(display, x1);

    st7735_write_cmd(display, ST7735_CMD_RASET);
    st7735_write_data16(display, y0);
    st7735_write_data16(display, y1);

    st7735_write_cmd(display, ST7735_CMD_RAMWR);
}

static void st7735_reset(const st7735_t* display)
{
    gpio_write_high(display->cfg.rst_port, display->cfg.rst_pin);
    DRV_DELAY_MS(5u);
    gpio_write_low(display->cfg.rst_port, display->cfg.rst_pin);
    DRV_DELAY_MS(10u);
    gpio_write_high(display->cfg.rst_port, display->cfg.rst_pin);
    DRV_DELAY_MS(120u);
}

static void st7735_map_xy(const st7735_t* display, uint16_t* x, uint16_t* y)
{
    uint16_t tx;
    uint16_t ty;

    tx = *x;
    ty = *y;

    switch (display->cfg.rotation)
    {
        case ST7735_ROT_90:
            *x = (uint16_t)(display->cfg.height - 1u - ty);
            *y = tx;
            break;

        case ST7735_ROT_180:
            *x = (uint16_t)(display->cfg.width - 1u - tx);
            *y = (uint16_t)(display->cfg.height - 1u - ty);
            break;

        case ST7735_ROT_270:
            *x = ty;
            *y = (uint16_t)(display->cfg.width - 1u - tx);
            break;

        case ST7735_ROT_0:
        default:
            break;
    }
}

uint8_t st7735_init(st7735_t* display, const st7735_config_t* config)
{
    st7735_config_t cfg;

    if (display == (st7735_t*)0)
    {
        return 0u;
    }

    if (config == (const st7735_config_t*)0)
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
        cfg.width = 128u;
        cfg.height = 160u;
        cfg.rotation = ST7735_ROT_0;
        config = &cfg;
    }

    display->cfg = *config;
    display->width = display->cfg.width;
    display->height = display->cfg.height;
    display->initialized = 0u;

    gpio_set_output(display->cfg.cs_tris, display->cfg.cs_pin);
    gpio_set_output(display->cfg.dc_tris, display->cfg.dc_pin);
    gpio_set_output(display->cfg.rst_tris, display->cfg.rst_pin);

    st7735_deselect(display);
    st7735_dc_data(display);

    spi_init_master(SPI_MODE_0, SPI_CLOCK_FOSC_4);

    st7735_reset(display);

    st7735_write_cmd(display, ST7735_CMD_SWRESET);
    DRV_DELAY_MS(150u);
    st7735_write_cmd(display, ST7735_CMD_SLPOUT);
    DRV_DELAY_MS(120u);

    st7735_write_cmd(display, ST7735_CMD_COLMOD);
    st7735_write_data8(display, 0x05u); /* RGB565 */

    st7735_set_rotation(display, display->cfg.rotation);

    st7735_write_cmd(display, ST7735_CMD_DISPON);
    DRV_DELAY_MS(50u);

    display->initialized = 1u;

    return 1u;
}

void st7735_set_window(st7735_t* display, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    if (display == (st7735_t*)0)
    {
        return;
    }

    st7735_set_addr_window_raw(display, x0, y0, x1, y1);
}

void st7735_clear(st7735_t* display, uint16_t color)
{
    st7735_fill_rect(display, 0u, 0u, display->width, display->height, color);
}

void st7735_update(st7735_t* display)
{
    DRV_UNUSED(display);
    /* Direct-render mode: no full framebuffer flush required. */
}

void st7735_draw_pixel(st7735_t* display, uint16_t x, uint16_t y, uint16_t color)
{
    if ((display == (st7735_t*)0) || (x >= display->width) || (y >= display->height))
    {
        return;
    }

    st7735_map_xy(display, &x, &y);
    st7735_set_addr_window_raw(display, x, y, x, y);

    st7735_select(display);
    st7735_dc_data(display);
    (void)spi_transfer_byte((uint8_t)(color >> 8u));
    (void)spi_transfer_byte((uint8_t)(color & 0xFFu));
    st7735_deselect(display);
}

void st7735_fill_rect(st7735_t* display, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
    uint32_t count;
    uint16_t x1;
    uint16_t y1;

    if ((display == (st7735_t*)0) || (width == 0u) || (height == 0u))
    {
        return;
    }

    if (x >= display->width || y >= display->height)
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

    st7735_set_addr_window_raw(display, x, y, x1, y1);

    count = (uint32_t)width * (uint32_t)height;
    st7735_select(display);
    st7735_dc_data(display);
    while (count-- > 0u)
    {
        (void)spi_transfer_byte((uint8_t)(color >> 8u));
        (void)spi_transfer_byte((uint8_t)(color & 0xFFu));
    }
    st7735_deselect(display);
}

void st7735_set_rotation(st7735_t* display, st7735_rotation_t rotation)
{
    uint8_t madctl;

    if (display == (st7735_t*)0)
    {
        return;
    }

    display->cfg.rotation = rotation;

    switch (rotation)
    {
        case ST7735_ROT_90:
            madctl = (uint8_t)(ST7735_MADCTL_MV | ST7735_MADCTL_MY | ST7735_MADCTL_BGR);
            display->width = display->cfg.height;
            display->height = display->cfg.width;
            break;

        case ST7735_ROT_180:
            madctl = (uint8_t)(ST7735_MADCTL_MX | ST7735_MADCTL_MY | ST7735_MADCTL_BGR);
            display->width = display->cfg.width;
            display->height = display->cfg.height;
            break;

        case ST7735_ROT_270:
            madctl = (uint8_t)(ST7735_MADCTL_MV | ST7735_MADCTL_MX | ST7735_MADCTL_BGR);
            display->width = display->cfg.height;
            display->height = display->cfg.width;
            break;

        case ST7735_ROT_0:
        default:
            madctl = (uint8_t)(ST7735_MADCTL_BGR);
            display->width = display->cfg.width;
            display->height = display->cfg.height;
            break;
    }

    st7735_write_cmd(display, ST7735_CMD_MADCTL);
    st7735_write_data8(display, madctl);
}

void st7735_draw_bitmap(st7735_t* display, uint16_t x, uint16_t y, const bitmap_t* bmp, uint16_t color)
{
    uint16_t ix;
    uint16_t iy;
    uint16_t bpr;
    uint16_t idx;
    uint8_t bit;

    if ((display == (st7735_t*)0) || (bmp == (const bitmap_t*)0) || (bmp->data == (const uint8_t*)0) || (bmp->format != BITMAP_FORMAT_1BPP_ROW))
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
                st7735_draw_pixel(display, (uint16_t)(x + ix), (uint16_t)(y + iy), color);
            }
        }
    }
}

void st7735_blit_sprite(st7735_t* display, uint16_t x, uint16_t y, const sprite_t* sprite, uint16_t color)
{
    uint16_t ix;
    uint16_t iy;
    uint16_t bpr;
    uint16_t idx;
    uint8_t bit;

    if ((display == (st7735_t*)0) || (sprite == (const sprite_t*)0) || (sprite->framebuffer == (const uint8_t*)0) || (sprite->format != SPRITE_FORMAT_1BPP_ROW))
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
                st7735_draw_pixel(display, (uint16_t)(x + ix), (uint16_t)(y + iy), color);
            }
        }
    }
}

static void st7735_df_clear(void* ctx)
{
    st7735_clear((st7735_t*)ctx, ST7735_COLOR_BLACK);
}

static void st7735_df_update(void* ctx)
{
    st7735_update((st7735_t*)ctx);
}

static void st7735_df_draw_pixel(void* ctx, uint16_t x, uint16_t y, uint8_t color)
{
    st7735_draw_pixel((st7735_t*)ctx, x, y, (color == 0u) ? ST7735_COLOR_BLACK : ST7735_COLOR_WHITE);
}

static void st7735_df_draw_char(void* ctx, uint16_t x, uint16_t y, char c, uint8_t color)
{
    st7735_t* display;
    graphics_t g;

    display = (st7735_t*)ctx;
    if ((display == (st7735_t*)0) ||
        (graphics_init(&g,
                       display->width,
                       display->height,
                       (uint8_t*)0,
                       0u,
                       GRAPHICS_FMT_CUSTOM,
                       (graphics_set_pixel_cb_t)st7735_df_draw_pixel,
                       (graphics_get_pixel_cb_t)0,
                       (graphics_flush_cb_t)0,
                       (void*)display) == 0u))
    {
        return;
    }

    graphics_draw_char(&g, (int16_t)x, (int16_t)y, c, (uint8_t)(color != 0u ? 1u : 0u));
}

void st7735_get_display_driver(display_driver_t* driver)
{
    if (driver == (display_driver_t*)0)
    {
        return;
    }

    driver->clear = st7735_df_clear;
    driver->update = st7735_df_update;
    driver->draw_pixel = st7735_df_draw_pixel;
    driver->draw_char = st7735_df_draw_char;
}


