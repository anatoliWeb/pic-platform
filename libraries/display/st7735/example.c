/*
 * File: libraries/display/st7735/example.c
 */

#include "core/compiler.h"
#include "drivers/timers/tick/tick.h"
#include "libraries/display/bitmap/bitmap.h"
#include "libraries/display/display_framework/display_framework.h"
#include "libraries/display/st7735/st7735.h"
#include "libraries/ui/sprite_buffer/sprite_buffer.h"

static st7735_t g_tft;
static sprite_t g_sprite;
static uint8_t g_sprite_buf[16u * 16u / 8u];

static const uint8_t g_logo_8x8[] =
{
    0x3C,
    0x42,
    0xA9,
    0x85,
    0xA9,
    0x91,
    0x42,
    0x3C
};

void main(void)
{
    st7735_config_t cfg;
    bitmap_t logo;
    display_t df;
    display_driver_t drv;
    uint16_t x;
    uint8_t rot;

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

    (void)st7735_init(&g_tft, &cfg);
    st7735_clear(&g_tft, ST7735_COLOR_BLACK);

    /* Color primitives. */
    st7735_fill_rect(&g_tft, 0u, 0u, 128u, 20u, ST7735_COLOR_BLUE);
    st7735_fill_rect(&g_tft, 0u, 20u, 128u, 20u, ST7735_COLOR_GREEN);
    st7735_fill_rect(&g_tft, 0u, 40u, 128u, 20u, ST7735_COLOR_RED);

    /* Bitmap render. */
    logo.width = 8u;
    logo.height = 8u;
    logo.format = BITMAP_FORMAT_1BPP_ROW;
    logo.data = g_logo_8x8;
    st7735_draw_bitmap(&g_tft, 4u, 4u, &logo, ST7735_COLOR_WHITE);

    /* Sprite render. */
    (void)sprite_init(&g_sprite, 16u, 16u, SPRITE_FORMAT_1BPP_ROW, g_sprite_buf, sizeof(g_sprite_buf));
    sprite_clear(&g_sprite, 0u);
    sprite_draw_bitmap(&g_sprite, 4, 4, &logo, 1u);
    st7735_blit_sprite(&g_tft, 20u, 4u, &g_sprite, ST7735_COLOR_YELLOW);

    /* display_framework integration. */
    st7735_get_display_driver(&drv);
    (void)display_init(&df, &drv, &g_tft, g_tft.width, g_tft.height);
    display_draw_string(&df, 40u, 4u, "TFT");

    x = 0u;
    rot = 0u;

    while (1)
    {
        st7735_fill_rect(&g_tft, 0u, 80u, g_tft.width, 20u, ST7735_COLOR_BLACK);
        st7735_fill_rect(&g_tft, x, 84u, 12u, 12u, ST7735_COLOR_CYAN);

        if ((tick_get() % 30u) == 0u)
        {
            x = (uint16_t)((x + 2u) % (g_tft.width - 12u));
        }

        if ((tick_get() % 3000u) == 0u)
        {
            rot = (uint8_t)((rot + 1u) & 0x03u);
            st7735_set_rotation(&g_tft, (st7735_rotation_t)rot);
        }
    }
}
