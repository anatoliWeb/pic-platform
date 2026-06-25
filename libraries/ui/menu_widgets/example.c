/*
 * File: libraries/ui/menu_widgets/example.c
 */

#include "core/compiler.h"
#include "drivers/timers/tick/tick.h"
#include "libraries/display/graphics/graphics.h"
#include "libraries/display/ssd1306/ssd1306.h"
#include "libraries/ui/menu_core/menu_core.h"
#include "libraries/ui/menu_renderer/menu_renderer.h"
#include "libraries/ui/menu_widgets/menu_widgets.h"
#include "libraries/ui/ui_themes/ui_themes.h"

static ssd1306_t g_oled;
static graphics_t g_graphics;

static menu_t g_menu;
static menu_renderer_t g_renderer;

static menu_item_t g_root;
static menu_item_t g_item_widgets;

static int16_t g_checkbox = 1;
static int16_t g_toggle = 0;
static int16_t g_progress = 40;
static int16_t g_number = 12;
static int16_t g_option = 1;

static const char* g_options[] = {"LOW", "MED", "HIGH"};

static menu_widget_t w_checkbox;
static menu_widget_t w_toggle;
static menu_widget_t w_progress;
static menu_widget_t w_numeric;
static menu_widget_t w_option;
static menu_widget_t w_slider;

static void oled_flush(void* ctx)
{
    ssd1306_update((ssd1306_t*)ctx);
}

static void setup_menu(void)
{
    g_root.label = "Root";
    g_root.type = MENU_ITEM_SUBMENU;
    g_root.parent = (menu_item_t*)0;
    g_root.first_child = &g_item_widgets;

    g_item_widgets.label = "Widgets";
    g_item_widgets.type = MENU_ITEM_SUBMENU;
    g_item_widgets.parent = &g_root;
    g_item_widgets.first_child = (menu_item_t*)0;
}

void main(void)
{
    ssd1306_config_t cfg;
    const ui_theme_t* theme;

    setup_menu();

    menu_init(&g_menu);
    menu_set_root(&g_menu, &g_root);

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

    theme = ui_theme_get_dark();
    (void)menu_renderer_init(&g_renderer, &g_menu, &g_graphics, theme);
    menu_renderer_set_icons(&g_renderer, 1u);

    menu_widget_init(&w_checkbox, MENU_WIDGET_CHECKBOX, 2, 2, 10, 8, &g_checkbox, 0, 1, 1);
    menu_widget_init(&w_toggle, MENU_WIDGET_TOGGLE, 16, 2, 20, 8, &g_toggle, 0, 1, 1);
    menu_widget_init(&w_progress, MENU_WIDGET_PROGRESS, 2, 14, 60, 8, &g_progress, 0, 100, 5);
    menu_widget_init(&w_numeric, MENU_WIDGET_NUMERIC, 66, 14, 24, 8, &g_number, 0, 99, 1);
    menu_widget_init(&w_option, MENU_WIDGET_OPTION, 2, 26, 40, 8, &g_option, 0, 2, 1);
    w_option.options = g_options;
    w_option.option_count = 3u;

    menu_widget_init(&w_slider, MENU_WIDGET_SLIDER, 2, 38, 88, 8, &g_progress, 0, 100, 5);

    while (1)
    {
        /* Demo animation/update values. */
        if ((tick_get() % 200u) == 0u)
        {
            g_progress += 2;
            if (g_progress > 100)
            {
                g_progress = 0;
            }

            g_toggle ^= 1;
            g_checkbox ^= 1;
            g_number = (int16_t)((g_number + 1) % 100);
            g_option = (int16_t)((g_option + 1) % 3);
        }

        /* Themed menu background/list. */
        menu_renderer_draw(&g_renderer);

        /* Widgets overlay demo area. */
        menu_widget_draw(&w_checkbox, &g_graphics, theme, 0u);
        menu_widget_draw(&w_toggle, &g_graphics, theme, 1u);
        menu_widget_draw(&w_progress, &g_graphics, theme, 0u);
        menu_widget_draw(&w_numeric, &g_graphics, theme, 0u);
        menu_widget_draw(&w_option, &g_graphics, theme, 0u);
        menu_widget_draw(&w_slider, &g_graphics, theme, 0u);

        graphics_flush(&g_graphics);
    }
}
