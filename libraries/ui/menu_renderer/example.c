/*
 * File: libraries/ui/menu_renderer/example.c
 */

#include "core/compiler.h"
#include "drivers/timers/tick/tick.h"
#include "libraries/display/graphics/graphics.h"
#include "libraries/display/ssd1306/ssd1306.h"
#include "libraries/display/st7735/st7735.h"
#include "libraries/ui/menu_core/menu_core.h"
#include "libraries/ui/menu_renderer/menu_renderer.h"
#include "libraries/ui/ui_themes/ui_themes.h"

static menu_t g_menu;
static menu_renderer_t g_renderer;

static ssd1306_t g_oled;
static graphics_t g_oled_graphics;

static menu_item_t g_root;
static menu_item_t g_settings;
static menu_item_t g_info;
static menu_item_t g_about;
static menu_item_t g_sound;
static menu_item_t g_brightness;

static void oled_flush(void* ctx)
{
    ssd1306_update((ssd1306_t*)ctx);
}

static void setup_menu_tree(void)
{
    g_root.label = "Main";
    g_root.type = MENU_ITEM_SUBMENU;
    g_root.first_child = &g_settings;
    g_root.parent = (menu_item_t*)0;

    g_settings.label = "Settings";
    g_settings.type = MENU_ITEM_SUBMENU;
    g_settings.parent = &g_root;
    g_settings.first_child = &g_sound;
    g_settings.prev = (menu_item_t*)0;
    g_settings.next = &g_info;

    g_info.label = "Info";
    g_info.type = MENU_ITEM_ACTION;
    g_info.parent = &g_root;
    g_info.prev = &g_settings;
    g_info.next = &g_about;

    g_about.label = "About";
    g_about.type = MENU_ITEM_ACTION;
    g_about.parent = &g_root;
    g_about.prev = &g_info;
    g_about.next = (menu_item_t*)0;

    g_sound.label = "Sound";
    g_sound.type = MENU_ITEM_TOGGLE;
    g_sound.parent = &g_settings;
    g_sound.toggle_value = 1u;
    g_sound.prev = (menu_item_t*)0;
    g_sound.next = &g_brightness;

    g_brightness.label = "Brightness";
    g_brightness.type = MENU_ITEM_VALUE;
    g_brightness.parent = &g_settings;
    g_brightness.value = 3;
    g_brightness.min_value = 1;
    g_brightness.max_value = 5;
    g_brightness.step = 1;
    g_brightness.prev = &g_sound;
    g_brightness.next = (menu_item_t*)0;
}

void main(void)
{
    ssd1306_config_t cfg;
    const ui_theme_t* theme;

    setup_menu_tree();

    menu_init(&g_menu);
    menu_set_root(&g_menu, &g_root);

    cfg.size = SSD1306_SIZE_128X64;
    cfg.transport = SSD1306_TRANSPORT_I2C;
    cfg.i2c_address = 0x3Cu;
    cfg.contrast = 0x7Fu;

    (void)ssd1306_init(&g_oled, &cfg);

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

    theme = ui_theme_get_dark();
    (void)menu_renderer_init(&g_renderer, &g_menu, &g_oled_graphics, theme);
    menu_renderer_set_layout(&g_renderer, 10u, 6u, 2u, 0u);

    /* OLED menu rendering loop.
     * For TFT rendering, create graphics callback context for ST7735/ILI9341
     * and reuse this same renderer instance.
     */
    while (1)
    {
        menu_renderer_draw(&g_renderer);
        graphics_flush(&g_oled_graphics);

        if ((tick_get() % 1500u) == 0u)
        {
            menu_next(&g_menu);
        }

        if ((tick_get() % 4000u) == 0u)
        {
            menu_enter(&g_menu);
        }

        if ((tick_get() % 7000u) == 0u)
        {
            menu_back(&g_menu);
            menu_renderer_set_theme(&g_renderer,
                                    (theme == ui_theme_get_dark()) ? ui_theme_get_light() : ui_theme_get_dark());
            theme = (theme == ui_theme_get_dark()) ? ui_theme_get_light() : ui_theme_get_dark();
        }
    }
}
