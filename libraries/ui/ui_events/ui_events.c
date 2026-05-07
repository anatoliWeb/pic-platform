#include "libraries/ui/ui_events/ui_events.h"

static uint8_t ui_events_next_index(uint8_t idx)
{
    return (uint8_t)((idx + 1u) % UI_EVENTS_QUEUE_SIZE);
}

void ui_events_init(ui_events_t* events)
{
    uint8_t i;

    if (events == (ui_events_t*)0)
    {
        return;
    }

    events->head = 0u;
    events->tail = 0u;
    events->subscriber_count = 0u;

    for (i = 0u; i < UI_EVENTS_QUEUE_SIZE; i++)
    {
        events->queue[i].type = UI_EVENT_NONE;
        events->queue[i].source = 0u;
        events->queue[i].target = 0u;
        events->queue[i].param1 = 0;
        events->queue[i].param2 = 0;
        events->queue[i].user_data = (void*)0;
    }

    for (i = 0u; i < UI_EVENTS_MAX_SUBSCRIBERS; i++)
    {
        events->subscribers[i] = (ui_event_cb_t)0;
    }
}

void ui_events_push(ui_events_t* events, const ui_event_t* event)
{
    uint8_t next;

    if ((events == (ui_events_t*)0) || (event == (const ui_event_t*)0))
    {
        return;
    }

    next = ui_events_next_index(events->head);

    if (next == events->tail)
    {
        /* Overflow policy: drop oldest event. */
        events->tail = ui_events_next_index(events->tail);
    }

    events->queue[events->head] = *event;
    events->head = next;
}

uint8_t ui_events_pop(ui_events_t* events, ui_event_t* out_event)
{
    if ((events == (ui_events_t*)0) || (out_event == (ui_event_t*)0) || (events->head == events->tail))
    {
        return 0u;
    }

    *out_event = events->queue[events->tail];
    events->tail = ui_events_next_index(events->tail);

    return 1u;
}

uint8_t ui_events_peek(const ui_events_t* events, ui_event_t* out_event)
{
    if ((events == (const ui_events_t*)0) || (out_event == (ui_event_t*)0) || (events->head == events->tail))
    {
        return 0u;
    }

    *out_event = events->queue[events->tail];
    return 1u;
}

void ui_events_clear(ui_events_t* events)
{
    if (events == (ui_events_t*)0)
    {
        return;
    }

    events->head = 0u;
    events->tail = 0u;
}

uint8_t ui_events_subscribe(ui_events_t* events, ui_event_cb_t cb)
{
    if ((events == (ui_events_t*)0) || (cb == (ui_event_cb_t)0))
    {
        return 0u;
    }

    if (events->subscriber_count >= UI_EVENTS_MAX_SUBSCRIBERS)
    {
        return 0u;
    }

    events->subscribers[events->subscriber_count] = cb;
    events->subscriber_count++;

    return 1u;
}

void ui_events_dispatch(ui_events_t* events, const ui_event_t* event)
{
    uint8_t i;

    if ((events == (ui_events_t*)0) || (event == (const ui_event_t*)0))
    {
        return;
    }

    for (i = 0u; i < events->subscriber_count; i++)
    {
        if (events->subscribers[i] != (ui_event_cb_t)0)
        {
            events->subscribers[i](event);
        }
    }
}
