#include "core/compiler.h"
#include "drivers/timers/tick/tick.h"
#include "libraries/display/graphics/graphics.h"
#include "libraries/display/ssd1306/ssd1306.h"
#include "libraries/display/st7735/st7735.h"
#include "libraries/ui/icon_pack/icon_pack.h"

static ssd1306_t g_oled;
static st7735_t g_tft;
static graphics_t g_oled_graphics;

static void oled_flush(void* ctx)
{
    ssd1306_update((ssd1306_t*)ctx);
}

void main(void)
{
    ssd1306_config_t oled_cfg;
    st7735_config_t tft_cfg;
    uint8_t i;
    uint8_t frame;

    oled_cfg.size = SSD1306_SIZE_128X64;
    oled_cfg.transport = SSD1306_TRANSPORT_I2C;
    oled_cfg.i2c_address = 0x3Cu;
    oled_cfg.contrast = 0x7Fu;

    (void)ssd1306_init(&g_oled, &oled_cfg);

    (void)graphics_init(&g_oled_graphics,
                        g_oled.width,
                        g_oled.height,
                        g_oled.framebuffer,
                        g_oled.buffer_size,
                        GRAPHICS_FMT_1BPP_VPAGE,
                        (graphics_set_pixel_cb_t)0,
                        (graphics_get_pixel_cb_t)0,
                        oled_flush,
                        &g_oled);

    /* OLED icon demo grid. */
    graphics_fill_rect(&g_oled_graphics, 0, 0, 128, 64, 0u);
    for (i = 0u; i < (uint8_t)UI_ICON_COUNT && i < 16u; i++)
    {
        icon_draw(&g_oled_graphics,
                  (ui_icon_id_t)i,
                  (int16_t)((i % 8u) * 14u),
                  (int16_t)((i / 8u) * 14u),
                  1u);
    }
    graphics_flush(&g_oled_graphics);

    /* TFT icon demo. */
    tft_cfg.cs_port = &PORTC;
    tft_cfg.cs_tris = &TRISC;
    tft_cfg.cs_pin = 2u;
    tft_cfg.dc_port = &PORTC;
    tft_cfg.dc_tris = &TRISC;
    tft_cfg.dc_pin = 1u;
    tft_cfg.rst_port = &PORTC;
    tft_cfg.rst_tris = &TRISC;
    tft_cfg.rst_pin = 0u;
    tft_cfg.width = 128u;
    tft_cfg.height = 160u;
    tft_cfg.rotation = ST7735_ROT_0;

    (void)st7735_init(&g_tft, &tft_cfg);
    st7735_clear(&g_tft, ST7735_COLOR_BLACK);

    for (i = 0u; i < (uint8_t)UI_ICON_COUNT && i < 12u; i++)
    {
        const ui_icon_t* icon;
        icon = icon_get((ui_icon_id_t)i);
        if (icon != (const ui_icon_t*)0)
        {
            bitmap_t bmp;
            bmp.width = icon->width;
            bmp.height = icon->height;
            bmp.format = icon->format;
            bmp.data = icon->data;
            st7735_draw_bitmap(&g_tft,
                               (uint16_t)((i % 6u) * 20u + 4u),
                               (uint16_t)((i / 6u) * 20u + 4u),
                               &bmp,
                               ST7735_COLOR_CYAN);
        }
    }

    frame = 0u;

    while (1)
    {
        /* Icon animation demo: blink warning/info on OLED. */
        if ((tick_get() % 250u) == 0u)
        {
            frame ^= 1u;
            icon_draw_inverted(&g_oled_graphics, frame != 0u ? UI_ICON_WARNING : UI_ICON_INFO, 112, 54, 1u);
            graphics_flush(&g_oled_graphics);
        }
    }
}
