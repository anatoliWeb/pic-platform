#include "libraries/ui/screen_transitions/screen_transitions.h"

#include "core/tick.h"

static screen_transition_manager_t g_transitions;
static graphics_t g_graphics;
static overlay_system_t g_overlays;
static screen_manager_t g_screens;

static void home_draw(screen_t* screen, graphics_t* g)
{
    (void)screen;

    graphics_fill_rect(g, 0, 0, g->width, g->height, 0u);
    graphics_draw_string(g, 6, 10, "HOME SCREEN", 1u);
}

static void menu_draw(screen_t* screen, graphics_t* g)
{
    (void)screen;

    graphics_fill_rect(g, 0, 0, g->width, g->height, 0u);
    graphics_draw_string(g, 6, 10, "MENU SCREEN", 1u);
}

static screen_t g_home = { 1u, (void*)0, 0, 0, 0, home_draw, 0 };
static screen_t g_menu = { 2u, (void*)0, 0, 0, 0, menu_draw, 0 };

void screen_transitions_example_run(void)
{
    uint32_t t0;

    (void)screen_manager_init(&g_screens, &g_graphics, (ui_events_t*)0);
    (void)overlay_system_init(&g_overlays,
                              &g_graphics,
                              ui_theme_get_default(),
                              (ui_events_t*)0,
                              &g_screens,
                              (popup_manager_t*)0);

    (void)screen_manager_set(&g_screens, &g_home);

    (void)screen_transition_init(&g_transitions, &g_screens, &g_overlays, &g_graphics);

    t0 = tick_get();
    (void)screen_transition_start(&g_transitions,
                                  SCREEN_TRANSITION_FADE,
                                  500u,
                                  &g_menu,
                                  (void*)0,
                                  t0);

    while (1)
    {
        uint32_t now;

        now = tick_get();
        screen_transition_update(&g_transitions, now);
        screen_transition_draw(&g_transitions);
        graphics_flush(&g_graphics);

        if (screen_transition_is_active(&g_transitions) == 0u)
        {
            (void)screen_transition_start(&g_transitions,
                                          SCREEN_TRANSITION_SLIDE_LEFT,
                                          600u,
                                          &g_home,
                                          (void*)0,
                                          now);
        }
    }
}
