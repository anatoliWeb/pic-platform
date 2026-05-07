#include "core/compiler.h"
#include "libraries/system/uart_debug/uart_debug.h"
#include "libraries/ui/ui_events/ui_events.h"

static ui_events_t g_events;

static void on_ui_event(const ui_event_t* event)
{
    if (event == (const ui_event_t*)0)
    {
        return;
    }

    switch (event->type)
    {
        case UI_EVENT_INPUT:
            DBG_PRINT("UI_EVENT_INPUT\r\n");
            break;

        case UI_EVENT_MENU_SELECTED:
            DBG_PRINT("UI_EVENT_MENU_SELECTED\r\n");
            break;

        case UI_EVENT_WIDGET_CHANGED:
            DBG_PRINT("UI_EVENT_WIDGET_CHANGED\r\n");
            break;

        case UI_EVENT_POPUP_OPEN:
            DBG_PRINT("UI_EVENT_POPUP_OPEN\r\n");
            break;

        case UI_EVENT_NOTIFICATION:
            DBG_PRINT("UI_EVENT_NOTIFICATION\r\n");
            break;

        default:
            break;
    }
}

void main(void)
{
    ui_event_t ev;

    ui_events_init(&g_events);
    (void)ui_events_subscribe(&g_events, on_ui_event);

    /* Button/input event example. */
    ev.type = UI_EVENT_INPUT;
    ev.source = 1u;   /* input_adapter */
    ev.target = 2u;   /* menu_navigation */
    ev.param1 = INPUT_EVENT_SELECT;
    ev.param2 = 0;
    ev.user_data = (void*)0;
    ui_events_push(&g_events, &ev);

    /* Menu selection event example. */
    ev.type = UI_EVENT_MENU_SELECTED;
    ev.source = 2u;
    ev.target = 3u;
    ev.param1 = 1;
    ev.param2 = 0;
    ui_events_push(&g_events, &ev);

    /* Widget change event example. */
    ev.type = UI_EVENT_WIDGET_CHANGED;
    ev.source = 4u;
    ev.target = 0u;
    ev.param1 = 42;
    ev.param2 = 1;
    ui_events_push(&g_events, &ev);

    /* Popup / notification examples. */
    ev.type = UI_EVENT_POPUP_OPEN;
    ev.source = 5u;
    ev.target = 0u;
    ev.param1 = 7;
    ev.param2 = 0;
    ui_events_push(&g_events, &ev);

    ev.type = UI_EVENT_NOTIFICATION;
    ev.source = 6u;
    ev.target = 0u;
    ev.param1 = 100;
    ev.param2 = 1;
    ui_events_push(&g_events, &ev);

    while (1)
    {
        /* Pop + dispatch loop. */
        if (ui_events_pop(&g_events, &ev) != 0u)
        {
            ui_events_dispatch(&g_events, &ev);
        }
    }
}
