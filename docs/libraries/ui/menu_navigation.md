[🇺🇦 Ukrainian version](./menu_navigation.ua.md)

# Menu Navigation Library

## Description

`menu_navigation` is a lightweight controller layer connecting `input_adapter` with `menu_core`.
It translates generic input events into deterministic menu navigation actions.

The module is independent from rendering/display systems.

## Public API

- `menu_navigation_init()`
- `menu_navigation_update()`
- `menu_navigation_set_enabled()` (optional)
- `menu_navigation_reset()` (optional)

## Navigation Flow Notes

`menu_navigation_t` binds:

- `menu_t* menu`
- `input_adapter_t* input`
- `enabled` flag

`menu_navigation_update()` performs:

1. `input_adapter_update()` polling
2. event dequeue loop
3. event-to-menu action mapping

No delays or blocking logic are used.

## Event Mapping Notes

Implemented mapping:

- `INPUT_EVENT_UP` -> `menu_prev()`
- `INPUT_EVENT_DOWN` -> `menu_next()`
- `INPUT_EVENT_SELECT` -> `menu_enter()`
- `INPUT_EVENT_BACK` -> `menu_back()`
- `INPUT_EVENT_HOLD` -> `menu_back()` (default quick-exit behavior)

Optional mapping in current implementation:

- `INPUT_EVENT_LEFT` -> `menu_prev()`
- `INPUT_EVENT_RIGHT` -> `menu_next()`

This allows simple value/toggle navigation patterns while staying backend-agnostic.

## Value / Toggle Handling

Toggle/value logic is executed by `menu_core` through `menu_enter()` on selected item.
This keeps controller layer minimal and reusable.

## Example

`example.c` demonstrates:

- hierarchical menu state
- encoder/button/keypad adapter integration
- toggle and value item behavior
- action callback execution path

## Dependencies

- `core/*`
- `libraries/ui/menu_core`
- `libraries/ui/input_adapter`

## Embedded Constraints

- no dynamic memory
- integer-only logic
- fixed deterministic event processing
