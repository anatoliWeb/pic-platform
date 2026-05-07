#include "libraries/display/sh1106/sh1106.h"

#include "drivers/communication/i2c/i2c.h"

static uint8_t sh1106_send_command(const sh1106_t* display, uint8_t cmd)
{
    if ((display == (const sh1106_t*)0) || (display->cfg.transport != SH1106_TRANSPORT_I2C))
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

static void sh1106_send_data_line(const sh1106_t* display, const uint8_t* data, uint8_t len)
{
    uint8_t i;

    if ((display == (const sh1106_t*)0) || (data == (const uint8_t*)0) || (len == 0u))
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

static uint8_t sh1106_hw_init(sh1106_t* display)
{
    if (display == (sh1106_t*)0)
    {
        return 0u;
    }

    if (sh1106_send_command(display, 0xAEu) == 0u) return 0u;
    if (sh1106_send_command(display, 0xD5u) == 0u) return 0u;
    if (sh1106_send_command(display, 0x80u) == 0u) return 0u;
    if (sh1106_send_command(display, 0xA8u) == 0u) return 0u;
    if (sh1106_send_command(display, 0x3Fu) == 0u) return 0u;
    if (sh1106_send_command(display, 0xD3u) == 0u) return 0u;
    if (sh1106_send_command(display, 0x00u) == 0u) return 0u;
    if (sh1106_send_command(display, 0x40u) == 0u) return 0u;
    if (sh1106_send_command(display, 0xADu) == 0u) return 0u;
    if (sh1106_send_command(display, 0x8Bu) == 0u) return 0u;
    if (sh1106_send_command(display, 0xA1u) == 0u) return 0u;
    if (sh1106_send_command(display, 0xC8u) == 0u) return 0u;
    if (sh1106_send_command(display, 0xDAu) == 0u) return 0u;
    if (sh1106_send_command(display, 0x12u) == 0u) return 0u;
    if (sh1106_send_command(display, 0x81u) == 0u) return 0u;
    if (sh1106_send_command(display, display->cfg.contrast) == 0u) return 0u;
    if (sh1106_send_command(display, 0xD9u) == 0u) return 0u;
    if (sh1106_send_command(display, 0x22u) == 0u) return 0u;
    if (sh1106_send_command(display, 0xDBu) == 0u) return 0u;
    if (sh1106_send_command(display, 0x35u) == 0u) return 0u;
    if (sh1106_send_command(display, 0xA4u) == 0u) return 0u;
    if (sh1106_send_command(display, 0xA6u) == 0u) return 0u;
    if (sh1106_send_command(display, 0xAFu) == 0u) return 0u;

    return 1u;
}

uint8_t sh1106_init(sh1106_t* display, const sh1106_config_t* config)
{
    sh1106_config_t cfg;

    if (display == (sh1106_t*)0)
    {
        return 0u;
    }

    if (config == (const sh1106_config_t*)0)
    {
        cfg.transport = SH1106_TRANSPORT_I2C;
        cfg.i2c_address = 0x3Cu;
        cfg.contrast = 0x7Fu;
        cfg.x_offset = 2u;
        config = &cfg;
    }

    display->cfg = *config;
    display->width = 128u;
    display->height = 64u;
    display->buffer_size = 1024u;
    display->initialized = 0u;

    if (display->cfg.transport == SH1106_TRANSPORT_I2C)
    {
        i2c_init(400000u);
    }

    if (graphics_init(&display->graphics,
                      display->width,
                      display->height,
                      display->framebuffer,
                      display->buffer_size,
                      GRAPHICS_FMT_1BPP_VPAGE,
                      (graphics_set_pixel_cb_t)0,
                      (graphics_get_pixel_cb_t)0,
                      (graphics_flush_cb_t)0,
                      (void*)display) == 0u)
    {
        return 0u;
    }

    sh1106_clear(display);

    if (sh1106_hw_init(display) == 0u)
    {
        return 0u;
    }

    sh1106_update(display);
    display->initialized = 1u;

    return 1u;
}

void sh1106_clear(sh1106_t* display)
{
    uint16_t i;

    if (display == (sh1106_t*)0)
    {
        return;
    }

    for (i = 0u; i < display->buffer_size; i++)
    {
        display->framebuffer[i] = 0x00u;
    }
}

void sh1106_update(sh1106_t* display)
{
    uint8_t page;
    uint16_t offset;
    uint8_t col_start;

    if ((display == (sh1106_t*)0) || (display->cfg.transport != SH1106_TRANSPORT_I2C))
    {
        return;
    }

    col_start = display->cfg.x_offset;

    for (page = 0u; page < 8u; page++)
    {
        (void)sh1106_send_command(display, (uint8_t)(0xB0u + page));
        (void)sh1106_send_command(display, (uint8_t)(0x00u + (col_start & 0x0Fu)));
        (void)sh1106_send_command(display, (uint8_t)(0x10u + ((col_start >> 4u) & 0x0Fu)));

        offset = (uint16_t)(page * display->width);
        sh1106_send_data_line(display, &display->framebuffer[offset], (uint8_t)display->width);
    }
}

void sh1106_draw_pixel(sh1106_t* display, uint16_t x, uint16_t y, uint8_t color)
{
    if (display == (sh1106_t*)0)
    {
        return;
    }

    graphics_draw_pixel(&display->graphics, (int16_t)x, (int16_t)y, color);
}

void sh1106_draw_char(sh1106_t* display, uint16_t x, uint16_t y, char c, uint8_t color)
{
    if (display == (sh1106_t*)0)
    {
        return;
    }

    graphics_draw_char(&display->graphics, (int16_t)x, (int16_t)y, c, color);
}

void sh1106_draw_string(sh1106_t* display, uint16_t x, uint16_t y, const char* str, uint8_t color)
{
    if (display == (sh1106_t*)0)
    {
        return;
    }

    graphics_draw_string(&display->graphics, (int16_t)x, (int16_t)y, str, color);
}

void sh1106_draw_line(sh1106_t* display,
                      uint16_t x0,
                      uint16_t y0,
                      uint16_t x1,
                      uint16_t y1,
                      uint8_t color)
{
    if (display == (sh1106_t*)0)
    {
        return;
    }

    graphics_draw_line(&display->graphics, (int16_t)x0, (int16_t)y0, (int16_t)x1, (int16_t)y1, color);
}

void sh1106_draw_rect(sh1106_t* display,
                      uint16_t x,
                      uint16_t y,
                      uint16_t width,
                      uint16_t height,
                      uint8_t color,
                      uint8_t filled)
{
    if (display == (sh1106_t*)0)
    {
        return;
    }

    if (filled != 0u)
    {
        graphics_fill_rect(&display->graphics, (int16_t)x, (int16_t)y, (int16_t)width, (int16_t)height, color);
    }
    else
    {
        graphics_draw_rect(&display->graphics, (int16_t)x, (int16_t)y, (int16_t)width, (int16_t)height, color);
    }
}

static void sh1106_df_clear(void* ctx)
{
    sh1106_clear((sh1106_t*)ctx);
}

static void sh1106_df_update(void* ctx)
{
    sh1106_update((sh1106_t*)ctx);
}

static void sh1106_df_draw_pixel(void* ctx, uint16_t x, uint16_t y, uint8_t color)
{
    sh1106_draw_pixel((sh1106_t*)ctx, x, y, (uint8_t)(color != 0u ? 1u : 0u));
}

static void sh1106_df_draw_char(void* ctx, uint16_t x, uint16_t y, char c, uint8_t color)
{
    sh1106_draw_char((sh1106_t*)ctx, x, y, c, (uint8_t)(color != 0u ? 1u : 0u));
}

void sh1106_get_display_driver(display_driver_t* driver)
{
    if (driver == (display_driver_t*)0)
    {
        return;
    }

    driver->clear = sh1106_df_clear;
    driver->update = sh1106_df_update;
    driver->draw_pixel = sh1106_df_draw_pixel;
    driver->draw_char = sh1106_df_draw_char;
}
