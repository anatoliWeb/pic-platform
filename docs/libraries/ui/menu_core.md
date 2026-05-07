[🇺🇦 Ukrainian version](./menu_core.ua.md)

# Menu Core Library

## Description

`menu_core` is a reusable embedded menu tree/navigation core.
It is fully independent from rendering and input hardware modules.

Core responsibilities:

- menu structure
- selection state
- navigation state
- action callbacks

## Public API

- `menu_init()`
- `menu_set_root()`
- `menu_get_current()`
- `menu_next()`
- `menu_prev()`
- `menu_enter()`
- `menu_back()`
- `menu_select()` (optional)
- `menu_reset()` (optional)

## Menu Structure Notes

`menu_item_t` supports:

- `parent`
- `first_child`
- `next` / `prev` sibling links
- `label`
- `flags`
- `action` callback + `action_ctx`
- item data fields for toggle/value types

Supported item types:

- `MENU_ITEM_ACTION`
- `MENU_ITEM_SUBMENU`
- `MENU_ITEM_TOGGLE`
- `MENU_ITEM_VALUE`

## Navigation Model Notes

Menu state (`menu_t`) keeps:

- `root`
- `current` (active submenu/root context)
- `selected` (currently highlighted item)

Navigation behavior:

- `menu_next/menu_prev` move inside sibling list
- `menu_enter` opens submenu or executes/toggles action/value
- `menu_back` returns to parent submenu and restores parent selection context

## Static Allocation Model

Menu tree is intended for static allocation:

- menu items are declared as static/global structs
- links are assigned at startup
- no dynamic allocation

## Callback Model

Action callback type:

- `typedef void (*menu_action_cb_t)(void* ctx);`

Supports UI event hooks and application actions without coupling to display/input layers.

## Example

`example.c` demonstrates:

- hierarchical menu tree
- submenu entry/back navigation
- toggle/value item behavior
- action callback usage

## Dependencies

- `core/*`

## Embedded Constraints

- no dynamic memory
- integer-only logic
- deterministic pointer-based traversal
