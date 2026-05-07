#include "libraries/ui/popup_manager/popup_manager.h"

static uint8_t popup_queue_next(uint8_t idx)
{
    return (uint8_t)((idx + 1u) % POPUP_QUEUE_SIZE);
}

static void popup_emit_event(popup_manager_t* mgr, ui_event_type_t type, int16_t p1, int16_t p2, void* user_data)
{
    ui_event_t ev;

    if (mgr == (popup_manager_t*)0 || mgr->events == (ui_events_t*)0)
    {
        return;
    }

    ev.type = type;
    ev.source = 10u; /* popup manager source id */
    ev.target = 0u;
    ev.param1 = p1;
    ev.param2 = p2;
    ev.user_data = user_data;

    ui_events_push(mgr->events, &ev);
}

static uint8_t popup_queue_pop(popup_manager_t* mgr, popup_t* out)
{
    if ((mgr == (popup_manager_t*)0) || (out == (popup_t*)0) || (mgr->q_head == mgr->q_tail))
    {
        return 0u;
    }

    *out = mgr->queue[mgr->q_tail];
    mgr->q_tail = popup_queue_next(mgr->q_tail);
    return 1u;
}

uint8_t popup_manager_init(popup_manager_t* mgr,
                           graphics_t* graphics,
                           const ui_theme_t* theme,
                           ui_events_t* events,
                           screen_manager_t* screens)
{
    uint8_t i;

    if ((mgr == (popup_manager_t*)0) || (graphics == (graphics_t*)0) || (theme == (const ui_theme_t*)0))
    {
        return 0u;
    }

    mgr->graphics = graphics;
    mgr->theme = theme;
    mgr->events = events;
    mgr->screens = screens;

    mgr->active_valid = 0u;
    mgr->q_head = 0u;
    mgr->q_tail = 0u;
    mgr->dim_background = 1u;

    for (i = 0u; i < POPUP_QUEUE_SIZE; i++)
    {
        mgr->queue[i].type = POPUP_TYPE_INFO;
        mgr->queue[i].title = (const char*)0;
        mgr->queue[i].message = (const char*)0;
        mgr->queue[i].icon = UI_ICON_INFO;
        mgr->queue[i].button_count = 0u;
        mgr->queue[i].selected_button = 0u;
        mgr->queue[i].result = POPUP_RESULT_NONE;
        mgr->queue[i].timeout_ms = 0u;
        mgr->queue[i].started_ms = 0u;
        mgr->queue[i].user_data = (void*)0;
    }

    return 1u;
}

uint8_t popup_show(popup_manager_t* mgr, const popup_t* popup)
{
    uint8_t next;

    if ((mgr == (popup_manager_t*)0) || (popup == (const popup_t*)0))
    {
        return 0u;
    }

    if (mgr->active_valid == 0u)
    {
        mgr->active = *popup;
        mgr->active.result = POPUP_RESULT_NONE;
        mgr->active.started_ms = 0u;
        mgr->active_valid = 1u;

        popup_emit_event(mgr, UI_EVENT_POPUP_OPEN, (int16_t)popup->type, 0, popup->user_data);
        return 1u;
    }

    /* Queue mode: one active + pending queue. */
    next = popup_queue_next(mgr->q_head);
    if (next == mgr->q_tail)
    {
        return 0u;
    }

    mgr->queue[mgr->q_head] = *popup;
    mgr->queue[mgr->q_head].result = POPUP_RESULT_NONE;
    mgr->queue[mgr->q_head].started_ms = 0u;
    mgr->q_head = next;

    return 1u;
}

void popup_close(popup_manager_t* mgr, popup_result_t result)
{
    popup_t next;

    if ((mgr == (popup_manager_t*)0) || (mgr->active_valid == 0u))
    {
        return;
    }

    mgr->active.result = result;
    popup_emit_event(mgr,
                     UI_EVENT_POPUP_CLOSE,
                     (int16_t)mgr->active.type,
                     (int16_t)result,
                     mgr->active.user_data);

    mgr->active_valid = 0u;

    if (popup_queue_pop(mgr, &next) != 0u)
    {
        mgr->active = next;
        mgr->active_valid = 1u;
        popup_emit_event(mgr, UI_EVENT_POPUP_OPEN, (int16_t)next.type, 0, next.user_data);
    }
}

void popup_update(popup_manager_t* mgr, uint32_t now_ms)
{
    ui_event_t ev;

    if ((mgr == (popup_manager_t*)0) || (mgr->active_valid == 0u))
    {
        return;
    }

    if (mgr->active.started_ms == 0u)
    {
        mgr->active.started_ms = now_ms;
    }

    if ((mgr->active.timeout_ms > 0u) && ((uint32_t)(now_ms - mgr->active.started_ms) >= mgr->active.timeout_ms))
    {
        popup_close(mgr, POPUP_RESULT_TIMEOUT);
        return;
    }

    if (mgr->events != (ui_events_t*)0)
    {
        if (ui_events_peek(mgr->events, &ev) != 0u)
        {
            if (ev.type == UI_EVENT_INPUT)
            {
                /* Map generic input events to popup buttons. */
                if (ev.param1 == INPUT_EVENT_LEFT)
                {
                    if (mgr->active.selected_button > 0u)
                    {
                        mgr->active.selected_button--;
                    }
                }
                else if (ev.param1 == INPUT_EVENT_RIGHT)
                {
                    if ((mgr->active.button_count > 0u) && (mgr->active.selected_button + 1u < mgr->active.button_count))
                    {
                        mgr->active.selected_button++;
                    }
                }
                else if (ev.param1 == INPUT_EVENT_SELECT)
                {
                    if ((mgr->active.button_count > 0u) && (mgr->active.selected_button < mgr->active.button_count))
                    {
                        popup_close(mgr, mgr->active.buttons[mgr->active.selected_button].result);
                    }
                    else
                    {
                        popup_close(mgr, POPUP_RESULT_OK);
                    }
                }
                else if ((ev.param1 == INPUT_EVENT_BACK) || (ev.param1 == INPUT_EVENT_HOLD))
                {
                    popup_close(mgr, POPUP_RESULT_CANCEL);
                }
            }
        }
    }
}

void popup_draw(popup_manager_t* mgr)
{
    uint8_t fg;
    uint8_t bg;
    uint8_t ac;
    int16_t px;
    int16_t py;
    int16_t pw;
    int16_t ph;
    uint8_t i;

    if ((mgr == (popup_manager_t*)0) || (mgr->active_valid == 0u) || (mgr->graphics == (graphics_t*)0) || (mgr->theme == (const ui_theme_t*)0))
    {
        return;
    }

    fg = (uint8_t)(ui_theme_get_color(mgr->theme, UI_COLOR_FOREGROUND) != 0u ? 1u : 0u);
    bg = (uint8_t)(ui_theme_get_color(mgr->theme, UI_COLOR_BACKGROUND) != 0u ? 1u : 0u);
    ac = (uint8_t)(ui_theme_get_color(mgr->theme, UI_COLOR_ACCENT) != 0u ? 1u : 0u);

    if (mgr->dim_background != 0u)
    {
        graphics_fill_rect(mgr->graphics, 0, 0, mgr->graphics->width, mgr->graphics->height, bg);
    }

    pw = (int16_t)(mgr->graphics->width - 12u);
    ph = (int16_t)(mgr->graphics->height - 20u);
    px = 6;
    py = 10;

    graphics_draw_rect(mgr->graphics, px, py, pw, ph, fg);

    if (mgr->active.title != (const char*)0)
    {
        graphics_draw_string(mgr->graphics, (int16_t)(px + 2), (int16_t)(py + 2), mgr->active.title, fg);
    }

    icon_draw(mgr->graphics, mgr->active.icon, (int16_t)(px + 2), (int16_t)(py + 14), fg);

    if (mgr->active.message != (const char*)0)
    {
        graphics_draw_string(mgr->graphics, (int16_t)(px + 14), (int16_t)(py + 14), mgr->active.message, fg);
    }

    for (i = 0u; i < mgr->active.button_count; i++)
    {
        int16_t bx;
        int16_t by;
        int16_t bw;

        bw = (int16_t)((pw - 6) / (mgr->active.button_count == 0u ? 1u : mgr->active.button_count));
        bx = (int16_t)(px + 3 + (i * bw));
        by = (int16_t)(py + ph - 12);

        graphics_draw_rect(mgr->graphics, bx, by, (int16_t)(bw - 2), 10, (uint8_t)(i == mgr->active.selected_button ? ac : fg));
        if (mgr->active.buttons[i].label != (const char*)0)
        {
            graphics_draw_string(mgr->graphics, (int16_t)(bx + 2), (int16_t)(by + 1), mgr->active.buttons[i].label, fg);
        }
    }
}

uint8_t popup_is_active(const popup_manager_t* mgr)
{
    if (mgr == (const popup_manager_t*)0)
    {
        return 0u;
    }

    return mgr->active_valid;
}

popup_result_t popup_get_result(const popup_manager_t* mgr)
{
    if ((mgr == (const popup_manager_t*)0) || (mgr->active_valid != 0u))
    {
        return POPUP_RESULT_NONE;
    }

    return mgr->active.result;
}
