/*
 * File: libraries/ui/sprite_buffer/example.c
 */

#include "core/compiler.h"
#include "drivers/timers/tick/tick.h"
#include "libraries/display/bitmap/bitmap.h"
#include "libraries/display/graphics/graphics.h"
#include "libraries/display/ssd1306/ssd1306.h"
#include "libraries/ui/sprite_buffer/sprite_buffer.h"

static ssd1306_t g_oled;
static graphics_t g_graphics;

static sprite_t g_sprite_a;
static sprite_t g_sprite_b;

static uint8_t g_sprite_a_buf[16u * 16u / 8u];
static uint8_t g_sprite_b_buf[16u * 16u / 8u];

static const uint8_t g_icon_8x8[] =
{
    0x3C,
    0x42,
    0xA5,
    0x81,
    0xA5,
    0x99,
    0x42,
    0x3C
};

static void oled_flush(void* ctx)
{
    ssd1306_update((ssd1306_t*)ctx);
}

void main(void)
{
    ssd1306_config_t cfg;
    bitmap_t icon;
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

    (void)sprite_init(&g_sprite_a, 16u, 16u, SPRITE_FORMAT_1BPP_ROW, g_sprite_a_buf, sizeof(g_sprite_a_buf));
    (void)sprite_init(&g_sprite_b, 16u, 16u, SPRITE_FORMAT_1BPP_ROW, g_sprite_b_buf, sizeof(g_sprite_b_buf));

    icon.width = 8u;
    icon.height = 8u;
    icon.format = BITMAP_FORMAT_1BPP_ROW;
    icon.data = g_icon_8x8;

    /* Offscreen render into sprite A. */
    sprite_clear(&g_sprite_a, 0u);
    sprite_fill_rect(&g_sprite_a, 0, 0, 16, 16, 0u);
    sprite_draw_bitmap(&g_sprite_a, 4, 4, &icon, 1u);

    /* Compose sprite B from sprite A region + extra pixel art. */
    sprite_clear(&g_sprite_b, 0u);
    sprite_copy_region(&g_sprite_b, 2, 2, &g_sprite_a, 0u, 0u, 12u, 12u);
    sprite_draw_pixel(&g_sprite_b, 0, 0, 1u);
    sprite_draw_pixel(&g_sprite_b, 15, 15, 1u);

    x = 0;
    dir = 1;

    while (1)
    {
        graphics_fill_rect(&g_graphics, 0, 0, 128, 64, 0u);
        graphics_draw_string(&g_graphics, 0, 0, "Sprite Buffer", 1u);

        /* Basic blit. */
        sprite_blit(&g_sprite_a, &g_graphics, x, 20, 1u);

        /* Transparent blit demo (skip zeros). */
        sprite_blit_transparent(&g_sprite_b, &g_graphics, 40, 20, 1u, 0u);

        graphics_flush(&g_graphics);

        if ((tick_get() % 20u) == 0u)
        {
            x = (int16_t)(x + dir);
            if (x <= 0)
            {
                x = 0;
                dir = 1;
            }
            else if (x >= 112)
            {
                x = 112;
                dir = -1;
            }
        }
    }
}
