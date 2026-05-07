#include "libraries/ui/overlay_system/overlay_system.h"

#include "core/tick.h"

static overlay_system_t g_overlay_system;
static graphics_t g_graphics;
static ui_events_t g_events;
static screen_manager_t g_screens;
static popup_manager_t g_popups;
static ui_theme_t g_theme;

/* Status bar overlay draw callback. */
static void status_overlay_draw(overlay_t* overlay, graphics_t* graphics, const ui_theme_t* theme)
{
    uint8_t fg;

    (void)overlay;

    fg = (uint8_t)(ui_theme_get_color(theme, UI_COLOR_FOREGROUND) != 0u ? 1u : 0u);
    graphics_draw_rect(graphics, 0, 0, graphics->width, 10, fg);
    graphics_draw_string(graphics, 2, 1, "STATUS", fg);
}

/* Debug overlay update callback (placeholder for FPS/tick stats). */
static void debug_overlay_update(overlay_t* overlay, uint32_t now_ms)
{
    overlay->user_data = (void*)now_ms;
}

static void debug_overlay_draw(overlay_t* overlay, graphics_t* graphics, const ui_theme_t* theme)
{
    uint8_t fg;

    (void)overlay;

    fg = (uint8_t)(ui_theme_get_color(theme, UI_COLOR_ACCENT) != 0u ? 1u : 0u);
    graphics_draw_string(graphics, 2, 14, "DBG", fg);
}

static overlay_t g_status_overlay =
{
    1u,
    1u,
    10,
    0,
    0,
    0,
    0,
    (void*)0,
    (overlay_on_update_cb_t)0,
    status_overlay_draw,
    (overlay_on_event_cb_t)0
};

static overlay_t g_debug_overlay =
{
    2u,
    1u,
    20,
    0,
    0,
    0,
    0,
    (void*)0,
    debug_overlay_update,
    debug_overlay_draw,
    (overlay_on_event_cb_t)0
};

void overlay_system_example_run(void)
{
    popup_t p;

    ui_events_init(&g_events);
    g_theme = *ui_theme_get_default();

    (void)screen_manager_init(&g_screens, &g_graphics, &g_events);
    (void)popup_manager_init(&g_popups, &g_graphics, &g_theme, &g_events, &g_screens);

    (void)overlay_system_init(&g_overlay_system,
                              &g_graphics,
                              &g_theme,
                              &g_events,
                              &g_screens,
                              &g_popups);

    (void)overlay_add(&g_overlay_system, &g_status_overlay);
    (void)overlay_add(&g_overlay_system, &g_debug_overlay);

    p.type = POPUP_TYPE_INFO;
    p.title = "Overlay";
    p.message = "Popup over overlays";
    p.icon = UI_ICON_INFO;
    p.button_count = 1u;
    p.selected_button = 0u;
    p.buttons[0].label = "OK";
    p.buttons[0].result = POPUP_RESULT_OK;
    p.timeout_ms = 0u;
    p.user_data = (void*)0;

    (void)popup_show(&g_popups, &p);

    while (1)
    {
        overlay_update(&g_overlay_system, tick_get());
        overlay_draw(&g_overlay_system);
        graphics_flush(&g_graphics);
    }
}
