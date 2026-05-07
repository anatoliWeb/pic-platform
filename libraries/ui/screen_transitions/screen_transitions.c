#include "libraries/ui/screen_transitions/screen_transitions.h"

/* Draw current transition marker without large framebuffers.
 * This keeps rendering lightweight for low-RAM PIC targets. */
static void screen_transition_draw_marker(const screen_transition_manager_t* mgr)
{
    int16_t w;
    int16_t h;
    int16_t pos;

    if ((mgr == (const screen_transition_manager_t*)0) || (mgr->graphics == (graphics_t*)0))
    {
        return;
    }

    w = (int16_t)mgr->graphics->width;
    h = (int16_t)mgr->graphics->height;

    switch (mgr->transition.type)
    {
        case SCREEN_TRANSITION_FADE:
            /* Fade hint: draw progress bar at bottom. */
            pos = (int16_t)(((uint32_t)(w - 2) * mgr->transition.progress) / 1000u);
            graphics_draw_rect(mgr->graphics, 0, (int16_t)(h - 3), w, 3, 1u);
            graphics_fill_rect(mgr->graphics, 1, (int16_t)(h - 2), pos, 1, 1u);
            break;

        case SCREEN_TRANSITION_SLIDE_LEFT:
            pos = (int16_t)(((uint32_t)w * mgr->transition.progress) / 1000u);
            graphics_draw_line(mgr->graphics, (int16_t)(w - 1 - pos), 0, (int16_t)(w - 1 - pos), (int16_t)(h - 1), 1u);
            break;

        case SCREEN_TRANSITION_SLIDE_RIGHT:
            pos = (int16_t)(((uint32_t)w * mgr->transition.progress) / 1000u);
            graphics_draw_line(mgr->graphics, pos, 0, pos, (int16_t)(h - 1), 1u);
            break;

        case SCREEN_TRANSITION_SLIDE_UP:
            pos = (int16_t)(((uint32_t)h * mgr->transition.progress) / 1000u);
            graphics_draw_line(mgr->graphics, 0, (int16_t)(h - 1 - pos), (int16_t)(w - 1), (int16_t)(h - 1 - pos), 1u);
            break;

        case SCREEN_TRANSITION_SLIDE_DOWN:
            pos = (int16_t)(((uint32_t)h * mgr->transition.progress) / 1000u);
            graphics_draw_line(mgr->graphics, 0, pos, (int16_t)(w - 1), pos, 1u);
            break;

        default:
            break;
    }
}

uint8_t screen_transition_init(screen_transition_manager_t* mgr,
                               screen_manager_t* screens,
                               overlay_system_t* overlays,
                               graphics_t* graphics)
{
    if ((mgr == (screen_transition_manager_t*)0) || (screens == (screen_manager_t*)0) || (graphics == (graphics_t*)0))
    {
        return 0u;
    }

    mgr->screens = screens;
    mgr->overlays = overlays;
    mgr->graphics = graphics;

    mgr->speed_percent = 100u;

    mgr->transition.type = SCREEN_TRANSITION_FADE;
    mgr->transition.duration_ms = 0u;
    mgr->transition.progress = 0u;
    mgr->transition.active = 0u;
    mgr->transition.started_ms = 0u;
    mgr->transition.source_screen = (screen_t*)0;
    mgr->transition.target_screen = (screen_t*)0;
    mgr->transition.user_data = (void*)0;

    return 1u;
}

uint8_t screen_transition_start(screen_transition_manager_t* mgr,
                                screen_transition_type_t type,
                                uint32_t duration_ms,
                                screen_t* target_screen,
                                void* user_data,
                                uint32_t now_ms)
{
    if ((mgr == (screen_transition_manager_t*)0) || (target_screen == (screen_t*)0))
    {
        return 0u;
    }

    mgr->transition.type = type;
    mgr->transition.duration_ms = (duration_ms == 0u) ? 1u : duration_ms;
    mgr->transition.progress = 0u;
    mgr->transition.active = 1u;
    mgr->transition.started_ms = now_ms;
    mgr->transition.source_screen = screen_manager_get_current(mgr->screens);
    mgr->transition.target_screen = target_screen;
    mgr->transition.user_data = user_data;

    return 1u;
}

void screen_transition_update(screen_transition_manager_t* mgr, uint32_t now_ms)
{
    uint32_t elapsed;
    uint32_t scaled_elapsed;
    uint32_t denom;

    if ((mgr == (screen_transition_manager_t*)0) || (mgr->transition.active == 0u))
    {
        return;
    }

    elapsed = (uint32_t)(now_ms - mgr->transition.started_ms);

    /* Speed scaling: 100% = normal, 200% = twice faster. */
    scaled_elapsed = (uint32_t)(elapsed * mgr->speed_percent);
    denom = (uint32_t)(mgr->transition.duration_ms * 100u);

    if (denom == 0u)
    {
        mgr->transition.progress = 1000u;
    }
    else
    {
        uint32_t p;
        p = (uint32_t)((scaled_elapsed * 1000u) / denom);
        mgr->transition.progress = (uint16_t)((p > 1000u) ? 1000u : p);
    }

    if (mgr->transition.progress >= 1000u)
    {
        /* Finalize by switching to the target screen. */
        (void)screen_manager_set(mgr->screens, mgr->transition.target_screen);

        mgr->transition.active = 0u;
        mgr->transition.progress = 1000u;
    }
}

void screen_transition_draw(screen_transition_manager_t* mgr)
{
    if (mgr == (screen_transition_manager_t*)0)
    {
        return;
    }

    if (mgr->transition.active == 0u)
    {
        if (mgr->overlays != (overlay_system_t*)0)
        {
            overlay_draw(mgr->overlays);
        }
        else
        {
            screen_manager_draw(mgr->screens);
        }
        return;
    }

    /* Render source/target in two phases to avoid full off-screen framebuffers. */
    if ((mgr->transition.progress < 500u) && (mgr->transition.source_screen != (screen_t*)0))
    {
        if (mgr->transition.source_screen->on_draw != (screen_on_draw_cb_t)0)
        {
            mgr->transition.source_screen->on_draw(mgr->transition.source_screen, mgr->graphics);
        }
    }
    else if (mgr->transition.target_screen != (screen_t*)0)
    {
        if (mgr->transition.target_screen->on_draw != (screen_on_draw_cb_t)0)
        {
            mgr->transition.target_screen->on_draw(mgr->transition.target_screen, mgr->graphics);
        }
    }

    screen_transition_draw_marker(mgr);
}

uint8_t screen_transition_is_active(const screen_transition_manager_t* mgr)
{
    if (mgr == (const screen_transition_manager_t*)0)
    {
        return 0u;
    }

    return mgr->transition.active;
}

void screen_transition_cancel(screen_transition_manager_t* mgr)
{
    if (mgr == (screen_transition_manager_t*)0)
    {
        return;
    }

    mgr->transition.active = 0u;
    mgr->transition.progress = 0u;
}

void screen_transition_set_speed(screen_transition_manager_t* mgr, uint16_t speed_percent)
{
    if (mgr == (screen_transition_manager_t*)0)
    {
        return;
    }

    if (speed_percent == 0u)
    {
        speed_percent = 1u;
    }

    mgr->speed_percent = speed_percent;
}
