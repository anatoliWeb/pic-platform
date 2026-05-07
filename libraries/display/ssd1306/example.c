#include "core/compiler.h"
#include "drivers/timers/tick/tick.h"
#include "libraries/display/display_framework/display_framework.h"
#include "libraries/display/ssd1306/ssd1306.h"

static ssd1306_t g_oled;

void main(void)
{
    ssd1306_config_t cfg;
    display_t disp;
    display_driver_t drv;
    uint16_t x;
    uint8_t dir;

    cfg.size = SSD1306_SIZE_128X64;
    cfg.transport = SSD1306_TRANSPORT_I2C;
    cfg.i2c_address = 0x3Cu;
    cfg.contrast = 0x7Fu;

    (void)ssd1306_init(&g_oled, &cfg);

    /* Direct drawing API. */
    ssd1306_clear(&g_oled);
    ssd1306_draw_string(&g_oled, 0u, 0u, "SSD1306", 1u);
    ssd1306_draw_rect(&g_oled, 0u, 10u, 60u, 20u, 1u, 0u);
    ssd1306_draw_line(&g_oled, 0u, 63u, 127u, 63u, 1u);
    ssd1306_update(&g_oled);

    /* display_framework adapter demo. */
    ssd1306_get_display_driver(&drv);
    (void)display_init(&disp, &drv, &g_oled, g_oled.width, g_oled.height);

    display_draw_string(&disp, 0u, 24u, "Framework OK");
    display_update(&disp);

    x = 0u;
    dir = 0u;

    while (1)
    {
        ssd1306_draw_rect(&g_oled, 0u, 48u, 128u, 12u, 0u, 1u);
        ssd1306_draw_rect(&g_oled, x, 50u, 8u, 8u, 1u, 1u);
        ssd1306_update(&g_oled);

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
