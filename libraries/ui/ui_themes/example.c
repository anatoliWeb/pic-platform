#include "core/compiler.h"
#include "drivers/timers/tick/tick.h"
#include "libraries/display/graphics/graphics.h"
#include "libraries/display/ssd1306/ssd1306.h"
#include "libraries/ui/icon_pack/icon_pack.h"
#include "libraries/ui/ui_themes/ui_themes.h"

static ssd1306_t g_oled;
static graphics_t g_graphics;
static ui_theme_t g_theme;

static void oled_flush(void* ctx)
{
    ssd1306_update((ssd1306_t*)ctx);
}

static void draw_status_panel(const ui_theme_t* theme)
{
    uint8_t fg;
    uint8_t bg;
    uint8_t border;

    fg = (uint8_t)(ui_theme_get_color(theme, UI_COLOR_FOREGROUND) != 0u ? 1u : 0u);
    bg = (uint8_t)(ui_theme_get_color(theme, UI_COLOR_BACKGROUND) != 0u ? 1u : 0u);
    border = (uint8_t)(ui_theme_get_color(theme, UI_COLOR_BORDER) != 0u ? 1u : 0u);

    graphics_fill_rect(&g_graphics, 0, 0, 128, 64, bg);
    graphics_draw_rect(&g_graphics, 0, 0, 128, 64, border);

    graphics_draw_string(&g_graphics, 4, 4, "Theme Demo", fg);

    icon_draw(&g_graphics, UI_ICON_WIFI, 4, 20, fg);
    icon_draw(&g_graphics, UI_ICON_BATTERY, 20, 20, fg);
    icon_draw(&g_graphics, UI_ICON_CLOCK, 36, 20, fg);

    icon_draw_scaled(&g_graphics, UI_ICON_WARNING, 64, 18, 2u, fg);
    icon_draw_inverted(&g_graphics, UI_ICON_INFO, 96, 20, fg);
}

void main(void)
{
    ssd1306_config_t cfg;
    uint8_t state;

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

    ui_theme_apply(&g_theme, ui_theme_get_dark());
    g_theme.monochrome = 1u;

    state = 0u;

    while (1)
    {
        if ((tick_get() % 3000u) == 0u)
        {
            if (state == 0u)
            {
                ui_theme_apply(&g_theme, ui_theme_get_dark());
                state = 1u;
            }
            else
            {
                ui_theme_apply(&g_theme, ui_theme_get_light());
                state = 0u;
            }

            g_theme.monochrome = 1u;
        }

        draw_status_panel(&g_theme);
        graphics_flush(&g_graphics);
    }
}
