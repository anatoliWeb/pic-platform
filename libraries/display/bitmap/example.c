/*
 * File: libraries/display/bitmap/example.c
 */

#include "core/compiler.h"
#include "drivers/timers/tick/tick.h"
#include "libraries/display/bitmap/bitmap.h"
#include "libraries/display/graphics/graphics.h"
#include "libraries/display/ssd1306/ssd1306.h"

static ssd1306_t g_oled;
static graphics_t g_graphics;

/* 16x16 row-based 1bpp icon (smiley-like). */
static const uint8_t g_icon_16x16[] =
{
    0x00,0x00,
    0x03,0xC0,
    0x0C,0x30,
    0x10,0x08,
    0x27,0xE4,
    0x20,0x04,
    0x40,0x02,
    0x44,0x22,
    0x44,0x22,
    0x40,0x02,
    0x20,0x04,
    0x27,0xE4,
    0x10,0x08,
    0x0C,0x30,
    0x03,0xC0,
    0x00,0x00
};

/* 24x8 sprite sheet, 3 frames of 8x8 each. */
static const uint8_t g_sprite_24x8[] =
{
    0x18,0x3C,0x7E,
    0x24,0x42,0x81,
    0x42,0x81,0x81,
    0x81,0x81,0x81,
    0x81,0x81,0x81,
    0x42,0x81,0x81,
    0x24,0x42,0x81,
    0x18,0x3C,0x7E
};

static void oled_flush(void* ctx)
{
    ssd1306_update((ssd1306_t*)ctx);
}

void main(void)
{
    ssd1306_config_t cfg;
    bitmap_t icon;
    bitmap_t sprite;
    uint16_t frame;
    int16_t x;
    int8_t dir;

    cfg.size = SSD1306_SIZE_128X64;
    cfg.transport = SSD1306_TRANSPORT_I2C;
    cfg.i2c_address = 0x3Cu;
    cfg.contrast = 0x7Fu;

    (void)ssd1306_init(&g_oled, &cfg);

    (void)graphics_init(&g_graphics,
                        g_oled.width,
                        g_oled.height,
                        g_oled.framebuffer,
                        g_oled.buffer_size,
                        GRAPHICS_FMT_1BPP_VPAGE,
                        (graphics_set_pixel_cb_t)0,
                        (graphics_get_pixel_cb_t)0,
                        oled_flush,
                        &g_oled);

    icon.width = 16u;
    icon.height = 16u;
    icon.format = BITMAP_FORMAT_1BPP_ROW;
    icon.data = g_icon_16x16;

    sprite.width = 24u;
    sprite.height = 8u;
    sprite.format = BITMAP_FORMAT_1BPP_ROW;
    sprite.data = g_sprite_24x8;

    graphics_fill_rect(&g_graphics, 0, 0, 128, 64, 0u);

    /* Basic icon rendering. */
    bitmap_draw(&g_graphics, 2, 2, &icon, 1u);

    /* Transparent rendering over text. */
    graphics_draw_string(&g_graphics, 30, 4, "Bitmap", 1u);
    bitmap_draw_transparent(&g_graphics, 28, 2, &icon, 1u, 0u);

    /* Clipped region rendering from sprite sheet. */
    bitmap_draw_region(&g_graphics, 2, 24, &sprite, 8u, 0u, 8u, 8u, 1u);

    /* Scaled and inverted demo. */
    bitmap_draw_scaled(&g_graphics, 20, 24, &icon, 2u, 1u);
    bitmap_draw_inverted(&g_graphics, 70, 24, &icon, 1u);

    graphics_flush(&g_graphics);

    frame = 0u;
    x = 0;
    dir = 1;

    while (1)
    {
        if ((tick_get() % 30u) == 0u)
        {
            graphics_fill_rect(&g_graphics, 0, 52, 128, 12, 0u);

            /* Simple sprite animation using region selection. */
            bitmap_draw_region(&g_graphics,
                               x,
                               54,
                               &sprite,
                               (uint16_t)((frame % 3u) * 8u),
                               0u,
                               8u,
                               8u,
                               1u);

            frame++;
            x = (int16_t)(x + dir);
            if (x <= 0)
            {
                x = 0;
                dir = 1;
            }
            else if (x >= 120)
            {
                x = 120;
                dir = -1;
            }

            graphics_flush(&g_graphics);
        }
    }
}
