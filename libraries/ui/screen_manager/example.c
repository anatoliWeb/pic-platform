/*
 * File: libraries/ui/screen_manager/example.c
 */

#include "core/compiler.h"
#include "drivers/timers/tick/tick.h"
#include "libraries/display/graphics/graphics.h"
#include "libraries/display/ssd1306/ssd1306.h"
#include "libraries/system/uart_debug/uart_debug.h"
#include "libraries/ui/screen_manager/screen_manager.h"

static ssd1306_t g_oled;
static graphics_t g_graphics;
static ui_events_t g_events;
static screen_manager_t g_mgr;

static screen_t g_home;
static screen_t g_settings;
static screen_t g_about;
static screen_t g_popup;

static void oled_flush(void* ctx)
{
    ssd1306_update((ssd1306_t*)ctx);
}

static void clear_and_title(graphics_t* g, const char* title)
{
    if (g == (graphics_t*)0)
    {
        return;
    }

    graphics_fill_rect(g, 0, 0, g->width, g->height, 0u);
    graphics_draw_string(g, 2, 2, title, 1u);
}

static void home_enter(screen_t* screen, void* prev_user_data)
{
    DRV_UNUSED(screen);
    DRV_UNUSED(prev_user_data);
    DBG_PRINT("Home enter\r\n");
}

static void home_draw(screen_t* screen, graphics_t* g)
{
    DRV_UNUSED(screen);
    clear_and_title(g, "HOME");
    graphics_draw_string(g, 2, 14, "# -> Settings", 1u);
    graphics_draw_string(g, 2, 24, "* -> Popup", 1u);
    graphics_flush(g);
}

static void settings_draw(screen_t* screen, graphics_t* g)
{
    DRV_UNUSED(screen);
    clear_and_title(g, "SETTINGS");
    graphics_draw_string(g, 2, 14, "# -> About", 1u);
    graphics_draw_string(g, 2, 24, "* -> Back", 1u);
    graphics_flush(g);
}

static void about_draw(screen_t* screen, graphics_t* g)
{
    DRV_UNUSED(screen);
    clear_and_title(g, "ABOUT");
    graphics_draw_string(g, 2, 14, "PIC Platform", 1u);
    graphics_draw_string(g, 2, 24, "* -> Back", 1u);
    graphics_flush(g);
}

static void popup_draw(screen_t* screen, graphics_t* g)
{
    DRV_UNUSED(screen);
    clear_and_title(g, "POPUP");
    graphics_draw_rect(g, 10, 16, 108, 28, 1u);
    graphics_draw_string(g, 18, 26, "Notification", 1u);
    graphics_flush(g);
}

static void common_event_handler(screen_manager_t* mgr, screen_t* screen, const ui_event_t* event)
{
    DRV_UNUSED(screen);

    if ((mgr == (screen_manager_t*)0) || (event == (const ui_event_t*)0))
    {
        return;
    }

    if ((event->type == UI_EVENT_INPUT) && (event->param1 == INPUT_EVENT_BACK))
    {
        (void)screen_manager_pop(mgr);
    }
}

static void home_event(screen_t* screen, const ui_event_t* event)
{
    DRV_UNUSED(screen);

    if (event == (const ui_event_t*)0)
    {
        return;
    }

    if ((event->type == UI_EVENT_INPUT) && (event->param1 == INPUT_EVENT_SELECT))
    {
        (void)screen_manager_set(&g_mgr, &g_settings);
    }
    else if ((event->type == UI_EVENT_POPUP_OPEN) ||
             ((event->type == UI_EVENT_INPUT) && (event->param1 == INPUT_EVENT_BACK)))
    {
        (void)screen_manager_push(&g_mgr, &g_popup);
    }
}

static void settings_event(screen_t* screen, const ui_event_t* event)
{
    if (event == (const ui_event_t*)0)
    {
        return;
    }

    if ((event->type == UI_EVENT_INPUT) && (event->param1 == INPUT_EVENT_SELECT))
    {
        (void)screen_manager_push(&g_mgr, &g_about);
    }
    else
    {
        common_event_handler(&g_mgr, screen, event);
    }
}

static void about_event(screen_t* screen, const ui_event_t* event)
{
    common_event_handler(&g_mgr, screen, event);
}

static void popup_event(screen_t* screen, const ui_event_t* event)
{
    common_event_handler(&g_mgr, screen, event);
}

static void setup_screens(void)
{
    g_home.id = 1u;
    g_home.user_data = (void*)0;
    g_home.on_enter = home_enter;
    g_home.on_exit = (screen_on_exit_cb_t)0;
    g_home.on_update = (screen_on_update_cb_t)0;
    g_home.on_draw = home_draw;
    g_home.on_event = home_event;

    g_settings.id = 2u;
    g_settings.user_data = (void*)0;
    g_settings.on_enter = (screen_on_enter_cb_t)0;
    g_settings.on_exit = (screen_on_exit_cb_t)0;
    g_settings.on_update = (screen_on_update_cb_t)0;
    g_settings.on_draw = settings_draw;
    g_settings.on_event = settings_event;

    g_about.id = 3u;
    g_about.user_data = (void*)0;
    g_about.on_enter = (screen_on_enter_cb_t)0;
    g_about.on_exit = (screen_on_exit_cb_t)0;
    g_about.on_update = (screen_on_update_cb_t)0;
    g_about.on_draw = about_draw;
    g_about.on_event = about_event;

    g_popup.id = 4u;
    g_popup.user_data = (void*)0;
    g_popup.on_enter = (screen_on_enter_cb_t)0;
    g_popup.on_exit = (screen_on_exit_cb_t)0;
    g_popup.on_update = (screen_on_update_cb_t)0;
    g_popup.on_draw = popup_draw;
    g_popup.on_event = popup_event;
}

void main(void)
{
    ssd1306_config_t cfg;
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
    setup_screens();

    (void)screen_manager_init(&g_mgr, &g_graphics, &g_events);
    (void)screen_manager_set(&g_mgr, &g_home);

    while (1)
    {
        /* Demo event injection for flow testing. */
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
            ev.type = UI_EVENT_POPUP_OPEN;
            ev.source = 2u;
            ev.target = 0u;
            ev.param1 = 0;
            ev.param2 = 0;
            ev.user_data = (void*)0;
            ui_events_push(&g_events, &ev);
        }

        if ((tick_get() % 9500u) == 0u)
        {
            ev.type = UI_EVENT_INPUT;
            ev.source = 1u;
            ev.target = 0u;
            ev.param1 = INPUT_EVENT_BACK;
            ev.param2 = 0;
            ev.user_data = (void*)0;
            ui_events_push(&g_events, &ev);
        }

        screen_manager_update(&g_mgr);
        screen_manager_draw(&g_mgr);
    }
}
