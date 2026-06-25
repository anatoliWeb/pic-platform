/*
 * File: libraries/display/ili9341/example.c
 */

#include "core/compiler.h"
#include "drivers/timers/tick/tick.h"
#include "libraries/display/bitmap/bitmap.h"
#include "libraries/display/display_framework/display_framework.h"
#include "libraries/display/ili9341/ili9341.h"
#include "libraries/ui/sprite_buffer/sprite_buffer.h"

static ili9341_t g_tft;
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
    ili9341_config_t cfg;
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
    cfg.width = 240u;
    cfg.height = 320u;
    cfg.rotation = ILI9341_ROT_0;

    (void)ili9341_init(&g_tft, &cfg);
    ili9341_clear(&g_tft, ILI9341_COLOR_BLACK);

    /* Color primitives. */
    ili9341_fill_rect(&g_tft, 0u, 0u, 240u, 30u, ILI9341_COLOR_BLUE);
    ili9341_fill_rect(&g_tft, 0u, 30u, 240u, 30u, ILI9341_COLOR_GREEN);
    ili9341_fill_rect(&g_tft, 0u, 60u, 240u, 30u, ILI9341_COLOR_RED);

    /* Bitmap render. */
    logo.width = 8u;
    logo.height = 8u;
    logo.format = BITMAP_FORMAT_1BPP_ROW;
    logo.data = g_logo_8x8;
    ili9341_draw_bitmap(&g_tft, 4u, 4u, &logo, ILI9341_COLOR_WHITE);

    /* Sprite render. */
    (void)sprite_init(&g_sprite, 16u, 16u, SPRITE_FORMAT_1BPP_ROW, g_sprite_buf, sizeof(g_sprite_buf));
    sprite_clear(&g_sprite, 0u);
    sprite_draw_bitmap(&g_sprite, 4, 4, &logo, 1u);
    ili9341_blit_sprite(&g_tft, 20u, 4u, &g_sprite, ILI9341_COLOR_YELLOW);

    /* display_framework integration. */
    ili9341_get_display_driver(&drv);
    (void)display_init(&df, &drv, &g_tft, g_tft.width, g_tft.height);
    display_draw_string(&df, 50u, 4u, "ILI9341");

    x = 0u;
    rot = 0u;

    while (1)
    {
        ili9341_fill_rect(&g_tft, 0u, 120u, g_tft.width, 20u, ILI9341_COLOR_BLACK);
        ili9341_fill_rect(&g_tft, x, 124u, 16u, 12u, ILI9341_COLOR_CYAN);

        if ((tick_get() % 25u) == 0u)
        {
            x = (uint16_t)((x + 3u) % (g_tft.width - 16u));
        }

        if ((tick_get() % 4000u) == 0u)
        {
            rot = (uint8_t)((rot + 1u) & 0x03u);
            ili9341_set_rotation(&g_tft, (ili9341_rotation_t)rot);
        }
    }
}
