[🇺🇦 Ukrainian version](./screen_manager.ua.md)

# Screen Manager Library

## Description

`screen_manager` is a reusable lightweight screen/page lifecycle manager for embedded UI applications.
It is display-agnostic and input-agnostic.

The module manages:

- active screen
- modal push/pop stack behavior
- update/draw lifecycle callbacks
- event forwarding to active screen

## Public API

- `screen_manager_init()`
- `screen_manager_set()`
- `screen_manager_push()`
- `screen_manager_pop()`
- `screen_manager_update()`
- `screen_manager_draw()`
- `screen_manager_send_event()` (optional)
- `screen_manager_get_current()` (optional)

## Screen Lifecycle Notes

`screen_t` fields:

- `id`
- `user_data`
- `on_enter()`
- `on_exit()`
- `on_update()`
- `on_draw()`
- `on_event()`

Lifecycle flow:

- `set`: exit current -> replace stack root -> enter new
- `push`: push modal screen -> enter new top
- `pop`: exit top -> resume previous top (`on_enter` call)
- `update`: active screen update + queued event forwarding
- `draw`: active screen draw callback

## Stack / Event Flow Notes

Stack model:

- fixed-size static stack (`SCREEN_MANAGER_STACK_SIZE`)
- deterministic push/pop behavior
- no dynamic allocation

Event model integration:

- optional `ui_events_t*` queue binding in manager
- `screen_manager_update()` pops queued events and forwards to active screen
- `screen_manager_send_event()` allows direct event injection

## Modal Behavior

- `push` is intended for popup/dialog overlays
- `pop` returns to previous screen context
- architecture prepared for popup managers and overlay layering

## Dependencies

- `core/*`
- `libraries/ui/ui_events`
- `libraries/ui/menu_renderer`
- `libraries/display/graphics`
- `libraries/display/display_framework`

## Embedded Constraints

- no dynamic memory
- integer-only logic
- deterministic callback execution
