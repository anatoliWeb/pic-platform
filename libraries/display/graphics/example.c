/*
 * File: libraries/display/graphics/example.c
 */

#include "core/compiler.h"
#include "drivers/timers/tick/tick.h"
#include "libraries/display/graphics/graphics.h"
#include "libraries/display/ssd1306/ssd1306.h"

static ssd1306_t g_oled;
static graphics_t g_graphics;

static void ssd1306_flush_cb(void* user_ctx)
{
    ssd1306_t* oled = (ssd1306_t*)user_ctx;
    ssd1306_update(oled);
}

void main(void)
{
    ssd1306_config_t cfg;
    int16_t x;
    int8_t dx;

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
                        ssd1306_flush_cb,
                        &g_oled);

    ssd1306_clear(&g_oled);
    graphics_draw_string(&g_graphics, 0, 0, "Graphics Core", 1u);
    graphics_draw_rect(&g_graphics, 0, 10, 80, 20, 1u);
    graphics_fill_rect(&g_graphics, 90, 10, 20, 10, 1u);
    graphics_draw_circle(&g_graphics, 64, 45, 12, 1u);
    graphics_flush(&g_graphics);

    x = 0;
    dx = 1;

    while (1)
    {
        if ((tick_get() % 15u) == 0u)
        {
            graphics_fill_rect(&g_graphics, 0, 54, 128, 10, 0u);
            graphics_draw_rect(&g_graphics, x, 56, 8, 6, 1u);
            graphics_flush(&g_graphics);

            x = (int16_t)(x + dx);
            if (x <= 0)
            {
                x = 0;
                dx = 1;
            }
            else if (x >= 120)
            {
                x = 120;
                dx = -1;
            }
        }
    }
}
