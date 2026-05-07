#include "core/compiler.h"
#include "drivers/timers/tick/tick.h"
#include "libraries/display/graphics/graphics.h"
#include "libraries/display/ssd1306/ssd1306.h"
#include "libraries/ui/popup_manager/popup_manager.h"

static ssd1306_t g_oled;
static graphics_t g_graphics;
static ui_events_t g_events;
static screen_manager_t g_screens;
static popup_manager_t g_popups;

static void oled_flush(void* ctx)
{
    ssd1306_update((ssd1306_t*)ctx);
}

static popup_t make_info_popup(void)
{
    popup_t p;

    p.type = POPUP_TYPE_INFO;
    p.title = "Info";
    p.message = "Operation Done";
    p.icon = UI_ICON_INFO;
    p.button_count = 1u;
    p.selected_button = 0u;
    p.buttons[0].label = "OK";
    p.buttons[0].result = POPUP_RESULT_OK;
    p.result = POPUP_RESULT_NONE;
    p.timeout_ms = 0u;
    p.started_ms = 0u;
    p.user_data = (void*)0;

    return p;
}

static popup_t make_warning_popup(void)
{
    popup_t p;

    p.type = POPUP_TYPE_WARNING;
    p.title = "Warning";
    p.message = "High Temp";
    p.icon = UI_ICON_WARNING;
    p.button_count = 1u;
    p.selected_button = 0u;
    p.buttons[0].label = "OK";
    p.buttons[0].result = POPUP_RESULT_OK;
    p.result = POPUP_RESULT_NONE;
    p.timeout_ms = 3000u;
    p.started_ms = 0u;
    p.user_data = (void*)0;

    return p;
}

static popup_t make_confirm_popup(void)
{
    popup_t p;

    p.type = POPUP_TYPE_CONFIRM;
    p.title = "Confirm";
    p.message = "Apply?";
    p.icon = UI_ICON_SETTINGS;
    p.button_count = 2u;
    p.selected_button = 0u;
    p.buttons[0].label = "YES";
    p.buttons[0].result = POPUP_RESULT_YES;
    p.buttons[1].label = "NO";
    p.buttons[1].result = POPUP_RESULT_NO;
    p.result = POPUP_RESULT_NONE;
    p.timeout_ms = 0u;
    p.started_ms = 0u;
    p.user_data = (void*)0;

    return p;
}

int main(void)
{
    ssd1306_config_t cfg;
    popup_t p;
    ui_event_t ev;

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

    ui_events_init(&g_events);
    (void)screen_manager_init(&g_screens, &g_graphics, &g_events);

    (void)popup_manager_init(&g_popups,
                             &g_graphics,
                             ui_theme_get_dark(),
                             &g_events,
                             &g_screens);

    /* Show info popup. */
    p = make_info_popup();
    (void)popup_show(&g_popups, &p);

    while (1)
    {
        popup_update(&g_popups, tick_get());

        if (popup_is_active(&g_popups) != 0u)
        {
            popup_draw(&g_popups);
            graphics_flush(&g_graphics);
        }

        /* Demo event injection. */
        if ((tick_get() % 2500u) == 0u)
        {
            ev.type = UI_EVENT_INPUT;
            ev.source = 1u;
            ev.target = 0u;
            ev.param1 = INPUT_EVENT_SELECT;
            ev.param2 = 0;
            ev.user_data = (void*)0;
            ui_events_push(&g_events, &ev);
        }

        if ((tick_get() % 7000u) == 0u)
        {
            p = make_warning_popup();
            (void)popup_show(&g_popups, &p);
        }

        if ((tick_get() % 11000u) == 0u)
        {
            p = make_confirm_popup();
            (void)popup_show(&g_popups, &p);
        }
    }
}
