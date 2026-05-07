#include "core/compiler.h"
#include "drivers/timers/tick/tick.h"
#include "libraries/display/bitmap/bitmap.h"
#include "libraries/display/display_framework/display_framework.h"
#include "libraries/display/sh1106/sh1106.h"

static sh1106_t g_display;

static const uint8_t g_logo_16x16[] =
{
    0x00,0x00,
    0x03,0xC0,
    0x0C,0x30,
    0x10,0x08,
    0x23,0xC4,
    0x27,0xE4,
    0x4F,0xF2,
    0x5F,0xFA,
    0x5F,0xFA,
    0x4F,0xF2,
    0x27,0xE4,
    0x23,0xC4,
    0x10,0x08,
    0x0C,0x30,
    0x03,0xC0,
    0x00,0x00
};

void main(void)
{
    sh1106_config_t cfg;
    display_t disp;
    display_driver_t drv;
    bitmap_t logo;
    uint16_t x;
    uint8_t dir;

    cfg.transport = SH1106_TRANSPORT_I2C;
    cfg.i2c_address = 0x3Cu;
    cfg.contrast = 0x7Fu;
    cfg.x_offset = 2u;

    (void)sh1106_init(&g_display, &cfg);

    /* Text and primitives. */
    sh1106_clear(&g_display);
    sh1106_draw_string(&g_display, 0u, 0u, "SH1106", 1u);
    sh1106_draw_rect(&g_display, 0u, 10u, 40u, 20u, 1u, 0u);
    sh1106_draw_line(&g_display, 0u, 63u, 127u, 63u, 1u);

    /* Bitmap rendering via shared bitmap + graphics core. */
    logo.width = 16u;
    logo.height = 16u;
    logo.format = BITMAP_FORMAT_1BPP_ROW;
    logo.data = g_logo_16x16;

    bitmap_draw(&g_display.graphics, 50, 10, &logo, 1u);

    sh1106_update(&g_display);

    /* display_framework adapter demo. */
    sh1106_get_display_driver(&drv);
    (void)display_init(&disp, &drv, &g_display, 128u, 64u);
    display_draw_string(&disp, 0u, 34u, "Framework OK");
    display_update(&disp);

    x = 0u;
    dir = 0u;

    while (1)
    {
        sh1106_draw_rect(&g_display, 0u, 54u, 128u, 10u, 0u, 1u);
        sh1106_draw_rect(&g_display, x, 56u, 8u, 6u, 1u, 1u);
        sh1106_update(&g_display);

        if ((tick_get() % 20u) == 0u)
        {
            if (dir == 0u)
            {
                if (x < 120u)
                {
                    x++;
                }
                else
                {
                    dir = 1u;
                }
            }
            else
            {
                if (x > 0u)
                {
                    x--;
                }
                else
                {
                    dir = 0u;
                }
            }
        }
    }
}
