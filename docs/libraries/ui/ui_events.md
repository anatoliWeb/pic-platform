[🇺🇦 Ukrainian version](./ui_events.ua.md)

# UI Events Library

## Description

`ui_events` is a reusable lightweight event/message system for embedded UI architectures.
It is hardware-independent and display-independent.

## Public API

- `ui_events_init()`
- `ui_events_push()`
- `ui_events_pop()`
- `ui_events_peek()`
- `ui_events_clear()`
- `ui_events_subscribe()` (optional)
- `ui_events_dispatch()` (optional)

## Event Model Notes

`ui_event_t` fields:

- `type`
- `source`
- `target`
- `param1`
- `param2`
- `user_data`

Supported event types:

- `UI_EVENT_NONE`
- `UI_EVENT_INPUT`
- `UI_EVENT_MENU_CHANGED`
- `UI_EVENT_MENU_SELECTED`
- `UI_EVENT_WIDGET_CHANGED`
- `UI_EVENT_SCREEN_CHANGED`
- `UI_EVENT_POPUP_OPEN`
- `UI_EVENT_POPUP_CLOSE`
- `UI_EVENT_NOTIFICATION`

## Queue / Subscriber Notes

Queue model:

- fixed-size static FIFO (`UI_EVENTS_QUEUE_SIZE`)
- no dynamic allocation
- overflow policy: drop oldest event

Subscriber model:

- fixed-size callback list (`UI_EVENTS_MAX_SUBSCRIBERS`)
- `ui_events_dispatch()` calls all registered subscribers for each event

## Integration Notes

Designed to integrate with:

- `input_adapter`
- `menu_navigation`
- `menu_widgets`
- future screen managers and async services

Event flow pattern:

1. producer pushes events
2. consumer pops events
3. dispatcher notifies subscribers

## Embedded Constraints

- no dynamic memory
- integer-only logic
- deterministic FIFO behavior

## Dependencies

- `core/*`
- `libraries/ui/menu_core`
- `libraries/ui/menu_navigation`
- `libraries/ui/menu_widgets`
- `libraries/ui/input_adapter`
