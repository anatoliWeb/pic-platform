#include "libraries/ui/overlay_system/overlay_system.h"

/* Find overlay by id in the local fixed-size overlay list. */
static int16_t overlay_find_index(const overlay_system_t* sys, uint8_t overlay_id)
{
    uint8_t i;

    if (sys == (const overlay_system_t*)0)
    {
        return -1;
    }

    for (i = 0u; i < sys->count; i++)
    {
        if ((sys->overlays[i] != (overlay_t*)0) && (sys->overlays[i]->id == overlay_id))
        {
            return (int16_t)i;
        }
    }

    return -1;
}

/* Keep overlays ordered by z-index using a lightweight insertion sort. */
static void overlay_sort_z(overlay_system_t* sys)
{
    uint8_t i;

    if (sys == (overlay_system_t*)0)
    {
        return;
    }

    for (i = 1u; i < sys->count; i++)
    {
        overlay_t* key;
        int16_t j;

        key = sys->overlays[i];
        j = (int16_t)i - 1;

        while ((j >= 0) && (sys->overlays[j] != (overlay_t*)0) && (key != (overlay_t*)0) &&
               (sys->overlays[j]->z_index > key->z_index))
        {
            sys->overlays[j + 1] = sys->overlays[j];
            j--;
        }

        sys->overlays[j + 1] = key;
    }
}

uint8_t overlay_system_init(overlay_system_t* sys,
                            graphics_t* graphics,
                            const ui_theme_t* theme,
                            ui_events_t* events,
                            screen_manager_t* screens,
                            popup_manager_t* popups)
{
    uint8_t i;

    if ((sys == (overlay_system_t*)0) || (graphics == (graphics_t*)0) || (theme == (const ui_theme_t*)0))
    {
        return 0u;
    }

    sys->count = 0u;
    sys->graphics = graphics;
    sys->theme = theme;
    sys->events = events;
    sys->screens = screens;
    sys->popups = popups;

    for (i = 0u; i < OVERLAY_SYSTEM_MAX_OVERLAYS; i++)
    {
        sys->overlays[i] = (overlay_t*)0;
    }

    return 1u;
}

uint8_t overlay_add(overlay_system_t* sys, overlay_t* overlay)
{
    if ((sys == (overlay_system_t*)0) || (overlay == (overlay_t*)0))
    {
        return 0u;
    }

    if (sys->count >= OVERLAY_SYSTEM_MAX_OVERLAYS)
    {
        return 0u;
    }

    if (overlay_find_index(sys, overlay->id) >= 0)
    {
        return 0u;
    }

    sys->overlays[sys->count] = overlay;
    sys->count++;
    overlay_sort_z(sys);

    return 1u;
}

uint8_t overlay_remove(overlay_system_t* sys, uint8_t overlay_id)
{
    int16_t idx;
    uint8_t i;

    if (sys == (overlay_system_t*)0)
    {
        return 0u;
    }

    idx = overlay_find_index(sys, overlay_id);
    if (idx < 0)
    {
        return 0u;
    }

    for (i = (uint8_t)idx; i + 1u < sys->count; i++)
    {
        sys->overlays[i] = sys->overlays[i + 1u];
    }

    sys->count--;
    sys->overlays[sys->count] = (overlay_t*)0;

    return 1u;
}

void overlay_update(overlay_system_t* sys, uint32_t now_ms)
{
    uint8_t i;

    if (sys == (overlay_system_t*)0)
    {
        return;
    }

    if (sys->screens != (screen_manager_t*)0)
    {
        screen_manager_update(sys->screens);
    }

    for (i = 0u; i < sys->count; i++)
    {
        overlay_t* overlay;

        overlay = sys->overlays[i];
        if ((overlay == (overlay_t*)0) || (overlay->visible == 0u))
        {
            continue;
        }

        if (overlay->on_update != (overlay_on_update_cb_t)0)
        {
            overlay->on_update(overlay, now_ms);
        }
    }

    if ((sys->events != (ui_events_t*)0) && (sys->count > 0u))
    {
        ui_event_t ev;

        if (ui_events_peek(sys->events, &ev) != 0u)
        {
            for (i = 0u; i < sys->count; i++)
            {
                overlay_t* overlay;

                overlay = sys->overlays[i];
                if ((overlay == (overlay_t*)0) || (overlay->visible == 0u))
                {
                    continue;
                }

                if (overlay->on_event != (overlay_on_event_cb_t)0)
                {
                    overlay->on_event(overlay, &ev);
                }
            }
        }
    }

    if (sys->popups != (popup_manager_t*)0)
    {
        popup_update(sys->popups, now_ms);
    }
}

void overlay_draw(overlay_system_t* sys)
{
    uint8_t i;

    if ((sys == (overlay_system_t*)0) || (sys->graphics == (graphics_t*)0))
    {
        return;
    }

    /* Layer 1: active screen content. */
    if (sys->screens != (screen_manager_t*)0)
    {
        screen_manager_draw(sys->screens);
    }

    /* Layer 2: all visible overlays, sorted by z-index. */
    for (i = 0u; i < sys->count; i++)
    {
        overlay_t* overlay;

        overlay = sys->overlays[i];
        if ((overlay == (overlay_t*)0) || (overlay->visible == 0u))
        {
            continue;
        }

        if (overlay->on_draw != (overlay_on_draw_cb_t)0)
        {
            overlay->on_draw(overlay, sys->graphics, sys->theme);
        }
    }

    /* Layer 3: popup manager above overlays. */
    if (sys->popups != (popup_manager_t*)0)
    {
        popup_draw(sys->popups);
    }
}

uint8_t overlay_set_visible(overlay_system_t* sys, uint8_t overlay_id, uint8_t visible)
{
    int16_t idx;

    if (sys == (overlay_system_t*)0)
    {
        return 0u;
    }

    idx = overlay_find_index(sys, overlay_id);
    if (idx < 0)
    {
        return 0u;
    }

    sys->overlays[idx]->visible = (uint8_t)(visible != 0u ? 1u : 0u);
    return 1u;
}

uint8_t overlay_bring_to_front(overlay_system_t* sys, uint8_t overlay_id)
{
    int16_t idx;
    int16_t max_z;
    uint8_t i;

    if (sys == (overlay_system_t*)0)
    {
        return 0u;
    }

    idx = overlay_find_index(sys, overlay_id);
    if (idx < 0)
    {
        return 0u;
    }

    max_z = sys->overlays[0]->z_index;
    for (i = 1u; i < sys->count; i++)
    {
        if (sys->overlays[i]->z_index > max_z)
        {
            max_z = sys->overlays[i]->z_index;
        }
    }

    sys->overlays[idx]->z_index = (int16_t)(max_z + 1);
    overlay_sort_z(sys);
    return 1u;
}
